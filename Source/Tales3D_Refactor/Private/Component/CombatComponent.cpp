// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Component/CombatComponent.h"

#include "ComboComponent.h"
#include "HealthComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Char/CoreCharacter.h"
#include "Char/CoreEnemy.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::NotifyBasicHit()
{
	// if (!bAttacking || !CurrentTarget) return;
	if (!ActiveTarget) return;
	// Applies Damage
	if (UHealthComponent* H = ActiveTarget->GetHealth())
	{
		H->ApplyDamage(BasicDamage);
	}
	
	// Hit FX
	if (BasicHitFX)
	{
		const FVector SpawnLoc = GetActiveHitPoint();
		const FRotator SpawnRot = GetHitFacingRotation();
		
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BasicHitFX,
			SpawnLoc,
			SpawnRot
			);
	}
	// Plays sound
	PlayBasicHitSoundAtPoint(GetActiveHitPoint());
}

void UCombatComponent::PlayBasicSection(ACoreEnemy* Target, int32 SectionIndex)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !Target) return;
	
	PendingTarget = Target;
	PendingSectionIndex = SectionIndex;

	if (IsInRange())
	{
		if (AController* C = OwnerChar->GetController())
		{
			C->StopMovement();
		}
		
		FaceTargetNow();
		
		// Target confirmed that's referred to by HitNotify
		ActiveTarget = PendingTarget;
		// Plays swoosh sound
		PlayBasicSwoosh();
		OwnerChar->BP_PlayBasicAttackSection(PendingSectionIndex);
		ClearPending();
		return;
	}
	
	StartMoveToTarget();

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(Timer_CheckRange);
		World->GetTimerManager().SetTimer(
			Timer_CheckRange,
			this,
			&UCombatComponent::CheckRangeAndStart,
			0.05f,
			true
			);
	}
}

void UCombatComponent::NotifyBasicSectionFinished()
{
	ClearActive();
}

void UCombatComponent::StartMoveToTarget()
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !PendingTarget) return;

	if (AController* C = OwnerChar->GetController())
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(C, PendingTarget);
	}
}

void UCombatComponent::CheckRangeAndStart()
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar)
	{
		ClearPending();
		return;
	}

	if (!PendingTarget || PendingSectionIndex <= 0)
	{
		ClearPending();

		if (OwnerChar->Combo)
		{
			OwnerChar->Combo->ForceEndCombo();
		}
		return;
	}

	if (IsInRange())
	{
		if (AController* C = OwnerChar->GetController())
		{
			C->StopMovement();
		}
		
		FaceTargetNow();
		
		ActiveTarget = PendingTarget;
		
		// Plays sound
		PlayBasicSwoosh();
		
		OwnerChar->BP_PlayBasicAttackSection(PendingSectionIndex);
		ClearPending();
	}
}

bool UCombatComponent::IsInRange() const
{
	if (!PendingTarget || !GetOwner()) return false;
	
	const float Dist = FVector::Dist(GetOwner()->GetActorLocation(), PendingTarget->GetActorLocation());
	return Dist <= BasicAttackRange;
}

void UCombatComponent::FaceTargetNow() const
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !PendingTarget) return;
	
	const FVector OwnerLoc = OwnerChar->GetActorLocation();
	FVector TargetLoc = GetTargetAimPoint();
	
	TargetLoc.Z = OwnerLoc.Z;
	
	const FVector Dir = TargetLoc - OwnerLoc;
	if (Dir.IsNearlyZero()) return;
	
	const float Yaw = Dir.Rotation().Yaw;
	OwnerChar->SetActorRotation(FRotator(0.f, Yaw, 0.f));
}

FVector UCombatComponent::GetTargetAimPoint() const
{
	if (!PendingTarget) return FVector::ZeroVector;
	
	FVector Aim = PendingTarget->GetActorLocation();

	return Aim;
}

void UCombatComponent::ClearPending()
{
	PendingTarget = nullptr;
	PendingSectionIndex = 0;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(Timer_CheckRange);
	}
}

void UCombatComponent::ClearActive()
{
	ActiveTarget = nullptr;
}

FVector UCombatComponent::GetActiveHitPoint() const
{
	if (!ActiveTarget) return FVector::ZeroVector;
	
	// if there's socket info, gets its location
	if (BasicHitFXSocket != NAME_None)
	{
		if (USkeletalMeshComponent* Mesh = ActiveTarget->FindComponentByClass<USkeletalMeshComponent>())
		{
			if (Mesh->DoesSocketExist(BasicHitFXSocket))
			{
				FVector P = Mesh->GetSocketLocation(BasicHitFXSocket);
				P.Z += BasicHitFX_ZOffset;
				return P;
			}
		}
	}
	
	// if no socket info, uses enemy capsule
	FVector P = ActiveTarget->GetActorLocation();
	if (UCapsuleComponent* Cap = ActiveTarget->GetCapsuleComponent())
	{
		P.Z += Cap->GetScaledCapsuleHalfHeight();
	}
	P.Z += BasicHitFX_ZOffset;
	return P;
}

FRotator UCombatComponent::GetHitFacingRotation() const
{
	// if FX needs directional info
	if (!ActiveTarget || !GetOwner()) return FRotator::ZeroRotator;
	
	FVector From = GetOwner()->GetActorLocation();
	FVector To = ActiveTarget->GetActorLocation();
	To.Z = From.Z;
	
	const FVector Dir = To - From;
	if (Dir.IsNearlyZero()) return FRotator::ZeroRotator;
	
	return Dir.Rotation();
}

void UCombatComponent::PlayBasicSwoosh() const
{
	if (!BasicSwooshSFX) return;

	if (const AActor* OwnerActor = GetOwner())
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			BasicSwooshSFX,
			OwnerActor->GetActorLocation(),
			BasicSwooshVolume
			);
	}
}

void UCombatComponent::PlayBasicHitSoundAtPoint(const FVector& Point) const
{
	if (!BasicHitSFX) return;
	
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		BasicHitSFX,
		Point,
		BasicHitVolume
		);
}

ACoreCharacter* UCombatComponent::GetOwnerCharacter() const
{
	return Cast<ACoreCharacter>(GetOwner());
}
