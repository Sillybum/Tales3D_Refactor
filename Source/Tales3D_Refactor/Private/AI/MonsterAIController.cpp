// TalesWeaver Clone Project (GitHub:Sillybum)


#include "AI/MonsterAIController.h"

#include "NavigationSystem.h"
#include "Char/CoreEnemy.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AMonsterAIController::AMonsterAIController()
{
	// Creates Perception Component
	PerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("PerceptionComp"));
	SetPerceptionComponent(*PerceptionComp);
	
	// Sets Sight
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 500.f;
	SightConfig->LoseSightRadius = 1000.f;
	SightConfig->PeripheralVisionAngleDegrees = 180.f;
	SightConfig->SetMaxAge(2.0f);
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals	= true;
	
	PerceptionComp->ConfigureSense(*SightConfig);
	PerceptionComp->SetDominantSense(UAISense_Sight::StaticClass());
}

void AMonsterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (InPawn)
	{
		HomeLocation = InPawn->GetActorLocation();
	}
	
	StartPatrolIdle();
}

void AMonsterAIController::BeginPlay()
{
	Super::BeginPlay();

	if (PerceptionComp)
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AMonsterAIController::OnTargetPerception);
	}
	
	EnsureAggreTimer();
}

void AMonsterAIController::OnTargetPerception(AActor* Actor, struct FAIStimulus Stimulus)
{
	// sets only players as chase target
	APawn* CharPawn = Cast<APawn>(Actor);
	if (!CharPawn || !CharPawn->IsPlayerControlled())
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		StartChase(Actor);
	}
	else
	{
		if (State == EMonsterAIState::Chase)
		{
			GetWorld()->GetTimerManager().ClearTimer(Timer_LoseSight);
			GetWorld()->GetTimerManager().SetTimer(
				Timer_LoseSight,
				this,
				&AMonsterAIController::OnLoseSightExpired,
				LoseSightDelay,
				false
				);
		}
	}
}

void AMonsterAIController::StartPatrolIdle()
{
	// makes it not look target
	ClearFocus(EAIFocusPriority::Gameplay);
	
	State = EMonsterAIState::PatrolIdle;
	CurrentTarget = nullptr;
	
	// cleans up chase timer if it remains
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_LoseSight);
		W->GetTimerManager().ClearTimer(Timer_Idle);
	}
	
	SetMoveSpeed(WalkSpeed);
	
	// moves after stopping a while
	const float IdleTime = FMath::FRandRange(IdleMin, IdleMax);
	GetWorld()->GetTimerManager().SetTimer(
		Timer_Idle,
		this,
		&AMonsterAIController::StartPatrolMove,
		IdleTime,
		false
		);
	EnsureAggreTimer();
}

void AMonsterAIController::StartPatrolMove()
{
	ClearFocus(EAIFocusPriority::Gameplay);
	
	EnsureAggreTimer();
	
	State = EMonsterAIState::PatrolMove;
	
	// finds random spot on navi
	UNavigationSystemV1* Nav = UNavigationSystemV1::GetCurrent(GetWorld());
	if (!Nav || !GetPawn())
	{
		StartPatrolIdle();
		return;
	}
	
	FNavLocation RandomLoc;
	const bool bFound = Nav->GetRandomReachablePointInRadius(HomeLocation, WanderRadius, RandomLoc);
	if (!bFound)
	{
		StartPatrolIdle();
		return;
	}
	
	MoveToLocation(RandomLoc.Location, 10.f, true);
}

void AMonsterAIController::StartChase(AActor* TargetActor)
{
	if (!TargetActor) return;
	
	State = EMonsterAIState::Chase;
	CurrentTarget = TargetActor;
	
	// Cleans up Patrol Timer
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_Idle);
	}
	
	SetMoveSpeed(RunSpeed);
	
	MoveToActor(TargetActor, AttackAcceptRadius, true);
	
	// when chasing, resets MoveToActor periodically (bc target is moving)
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_Repath);
		W->GetTimerManager().SetTimer(
			Timer_Repath,
			this,
			&AMonsterAIController::RepathToTarget,
			RepathInterval,
			true
			);
	}
}

void AMonsterAIController::StopChase()
{
	CurrentTarget = nullptr;

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_Repath);
		W->GetTimerManager().ClearTimer(Timer_ChaseExpire);
		W->GetTimerManager().ClearTimer(Timer_Attack);
	}
	
	StopMovement();
	StartPatrolIdle();
}

void AMonsterAIController::SetMoveSpeed(float NewSpeed) const
{
	const ACharacter* Char = Cast<ACharacter>(GetPawn());
	if (!Char) return;

	if (UCharacterMovementComponent* Move = Char->GetCharacterMovement())
	{
		Move->MaxWalkSpeed = NewSpeed;
	}
}

void AMonsterAIController::OnMoveCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{
	Super::OnMoveCompleted(RequestID, Result);

	if (State == EMonsterAIState::PatrolMove)
	{
		StartPatrolIdle();
	}
}

void AMonsterAIController::OnLoseSightExpired()
{
	StopChase();
}

void AMonsterAIController::UpdateAggro()
{
	EnsureAggreTimer();
	
	APawn* SelfPawn = GetPawn();
	APawn* PlayerPawn = GetPlayerPawn();
	if (!SelfPawn || !PlayerPawn || !GetWorld()) return;
	
	const FVector SelfLoc =  SelfPawn->GetActorLocation();
	const FVector PlayerLoc = PlayerPawn->GetActorLocation();
	
	const float DistSq = FVector::DistSquared(SelfLoc, PlayerLoc);
	const float AggroSq = FMath::Square(AggroRadius);
	const float LoseSq = FMath::Square(AggroLoseRadius);
	
	// if too far from post, it returns
	if ((State == EMonsterAIState::Chase || State == EMonsterAIState::Attack) && MaxChaseDistanceFromHome > 0.f)
	{
		const float HomeDistSq = FVector::DistSquared(SelfLoc, HomeLocation);
		if (HomeDistSq > FMath::Square(MaxChaseDistanceFromHome))
		{
			StopChase();
			return;
		}
	}
	
	auto& TM = GetWorld()->GetTimerManager();

	if (DistSq <= AggroSq)
	{
		TM.ClearTimer(Timer_ChaseExpire);

		if (State != EMonsterAIState::Chase && State != EMonsterAIState::Attack)
		{
			StartChase(PlayerPawn);
		}

		if (IsInAttackRange(PlayerPawn))
		{
			EnterAttack(PlayerPawn);
		}
		else
		{
			if (State == EMonsterAIState::Attack)
			{
				ExitAttackToChase();
			}
		}
		return;
	}

	if ((State == EMonsterAIState::Chase || State == EMonsterAIState::Attack) && DistSq <= LoseSq)
	{
		TM.ClearTimer(Timer_ChaseExpire);

		if (IsInAttackRange(PlayerPawn))
		{
			EnterAttack(PlayerPawn);
		}
		else
		{
			if (State == EMonsterAIState::Attack)
			{
				ExitAttackToChase();
			}
		}
		return;
	}

	if (State == EMonsterAIState::Attack)
	{
		ExitAttackToChase();
		// ExitAttackToChase could go patrol if on target, in which case, exits here
		if (State != EMonsterAIState::Chase)
		{
			return;
		}
	}

	if (State == EMonsterAIState::Chase)
	{
		if (!TM.IsTimerActive(Timer_ChaseExpire))
		{
			TM.SetTimer(
				Timer_ChaseExpire,
				this,
				&AMonsterAIController::StopChase,
				ChasePersistSeconds,
				false
				);
		}
	}
}


void AMonsterAIController::RepathToTarget()
{
	if (State != EMonsterAIState::Chase) return;
	if (!CurrentTarget) return;
	
	MoveToActor(CurrentTarget, AttackAcceptRadius, true);
}

APawn* AMonsterAIController::GetPlayerPawn() const
{
	return UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
}

void AMonsterAIController::EnsureAggreTimer()
{
	if (!GetWorld()) return;
	
	auto& TM = GetWorld()->GetTimerManager();
	
	if (TM.IsTimerActive(Timer_AggroCheck))
	{
		return;
	}
	
	TM.SetTimer(
		Timer_AggroCheck,
		this,
		&AMonsterAIController::UpdateAggro,
		AggroCheckInterval,
		true
		);
	
	UpdateAggro();
	
}

void AMonsterAIController::EnterAttack(APawn* PlayerPawn)
{
	if (!PlayerPawn) return;
	
	if (State == EMonsterAIState::Attack) return;
	
	State = EMonsterAIState::Attack;
	CurrentTarget = PlayerPawn;

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_Repath);
		W->GetTimerManager().ClearTimer(Timer_ChaseExpire);
	}
	
	StopMovement();
	SetFocus(PlayerPawn); // face player

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_Attack);
		W->GetTimerManager().SetTimer(
			Timer_Attack,
			this,
			&AMonsterAIController::PerformAttackTick,
			AttackInterval,
			true,
			0.0f
			);
	}
}

void AMonsterAIController::ExitAttackToChase()
{
	if (State != EMonsterAIState::Attack) return;

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_Attack);
	}
	
	State = EMonsterAIState::Chase;

	if (CurrentTarget)
	{
		SetMoveSpeed(RunSpeed);
		SetFocus(CurrentTarget);
		
		MoveToActor(CurrentTarget, AttackAcceptRadius, true);

		if (UWorld* W = GetWorld())
		{
			W->GetTimerManager().ClearTimer(Timer_Repath);
			W->GetTimerManager().SetTimer(
				Timer_Repath,
				this,
				&AMonsterAIController::RepathToTarget,
				RepathInterval,
				true
				);
		}
	}
	else
	{
		ClearFocus(EAIFocusPriority::Gameplay);
		StartPatrolIdle();
	}
}

void AMonsterAIController::PerformAttackTick()
{
	APawn* SelfPawn = GetPawn();
	APawn* PlayerPawn = Cast<APawn>(CurrentTarget.Get());

	if (!SelfPawn || !PlayerPawn)
	{
		ExitAttackToChase();
		return;
	}

	if (!IsInAttackRange(PlayerPawn))
	{
		ExitAttackToChase();
		return;
	}
	
	SetFocus(PlayerPawn);

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.8f, FColor::Red, TEXT("Monster attack!"));
	}

	if (ACoreEnemy* E = Cast<ACoreEnemy>(SelfPawn))
	{
		E->BP_PlayMonsterAttack();
	}
	
	UGameplayStatics::ApplyDamage(
		PlayerPawn,
		AttackDamage,
		this,
		SelfPawn,
		UDamageType::StaticClass()
		);
}

bool AMonsterAIController::IsInAttackRange(APawn* PlayerPawn) const
{
	if (!GetPawn() || !PlayerPawn) return false;
	
	const float Dist = FVector::Dist(GetPawn()->GetActorLocation(), PlayerPawn->GetActorLocation());
	return Dist <= AttackRange;
}
