// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Char/CoreEnemy.h"

#include "AIController.h"
#include "Component/HealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/EnemyHPBarWidget.h"

ACoreEnemy::ACoreEnemy()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// HP Widget (Attaches on Root)
	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBarWidget"));
	HPBarWidget->SetupAttachment(RootComponent);
	HPBarWidget->SetRelativeLocation(FVector(0.f, 0.f, -200.f));
	// Makes it face screen
	HPBarWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HPBarWidget->SetDrawSize(FVector2D(120.f, 16.f));
	// Hidden until enemy selected
	HPBarWidget->SetVisibility(false);
	
	// Health Component
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

void ACoreEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (HPBarWidget)
	{
		HPBarWidget->InitWidget();
	}
	
	if (Health)
	{
		Health->OnHealthChanged.AddDynamic(this, &ACoreEnemy::HandleHealthChanged);
	}
}

void ACoreEnemy::SetSelected(bool bSelected)
{
	if (bIsSelected == bSelected) return;
	
	bIsSelected = bSelected;
	UpdateHPBarVisibility();
}

void ACoreEnemy::HandleHealthChanged(float NewHP, float MaxHP)
{
	if (UEnemyHPBarWidget* W = GetHPBarWidget())
	{
		W->SetHP(NewHP, MaxHP);
	}
	
	if (!bIsDead && NewHP <= 0.f)
	{
		bIsDead = true;
		SetSelected(false);
		SetActorEnableCollision(false);
		
		if (UCharacterMovementComponent* Move = GetCharacterMovement())
		{
			Move->StopMovementImmediately();
			Move->DisableMovement();
		}
		if (AAIController* AI = Cast<AAIController>(GetController()))
		{
			AI->StopMovement();
		}
		SpawnDeathDrops();
		BP_PlayMonsterDeath();

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(DeathTimerHandle, this, &ACoreEnemy::DestroyAfterDeath, DeathDestroyDelay, false);
		}
	}
}

UEnemyHPBarWidget* ACoreEnemy::GetHPBarWidget() const
{
	if (!HPBarWidget) return nullptr;
	
	return Cast<UEnemyHPBarWidget>(HPBarWidget->GetUserWidgetObject());
}

void ACoreEnemy::UpdateHPBarVisibility()
{
	if (!HPBarWidget) return;

	if (HPBarWidgetClass && HPBarWidget->GetWidgetClass() != HPBarWidgetClass)
	{
		HPBarWidget->SetWidgetClass(HPBarWidgetClass);
	}
	HPBarWidget->SetVisibility(bIsSelected);
}

void ACoreEnemy::DestroyAfterDeath()
{
	Destroy();
}

void ACoreEnemy::SpawnDeathDrops()
{
	if (!DropItemActorClass) return;
	if (!GetWorld()) return;

	const int32 MinCount = FMath::Max(0, DropCountMin);
	const int32 MaxCount = FMath::Max(MinCount, DropCountMax);
	const int32 DropCount = FMath::RandRange(MinCount, MaxCount);
	if (DropCount <= 0) return;

	FVector SpawnCenter = GetActorLocation();
	if (const UCapsuleComponent* Cap = GetCapsuleComponent())
	{
		SpawnCenter.Z += Cap->GetScaledCapsuleHalfHeight() * 0.5f;
	}
	SpawnCenter.Z += DropSpawnHeightOffset;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < DropCount; ++i)
	{
		FVector HorizontalDir = FMath::VRand();
		HorizontalDir.Z = 0.f;
		if (HorizontalDir.IsNearlyZero())
		{
			HorizontalDir = FVector::ForwardVector;
		}
		HorizontalDir.Normalize();

		const float SpawnDist = FMath::FRandRange(0.f, DropSpawnRadius);
		const FVector SpawnLoc = SpawnCenter + HorizontalDir * SpawnDist;
		AActor* DropActor = GetWorld()->SpawnActor<AActor>(DropItemActorClass, SpawnLoc, FRotator::ZeroRotator, Params);
		if (!DropActor) continue;

		if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(DropActor->GetRootComponent()))
		{
			if (!Prim->IsSimulatingPhysics())
			{
				Prim->SetSimulatePhysics(true);
			}

			const float HorizontalSpeed = FMath::FRandRange(
				DropLaunchHorizontalSpeedMin,
				FMath::Max(DropLaunchHorizontalSpeedMin, DropLaunchHorizontalSpeedMax));
			const float UpwardSpeed = FMath::FRandRange(
				DropLaunchUpwardSpeedMin,
				FMath::Max(DropLaunchUpwardSpeedMin, DropLaunchUpwardSpeedMax));
			const FVector LaunchVel = HorizontalDir * HorizontalSpeed + FVector::UpVector * UpwardSpeed;

			Prim->SetPhysicsLinearVelocity(LaunchVel);
		}
	}
}
