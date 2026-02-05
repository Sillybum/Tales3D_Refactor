// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Component/CombatComponent.h"

#include "HealthComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Char/CoreCharacter.h"
#include "Char/CoreEnemy.h"
#include "Components/CapsuleComponent.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::RequestBasicAttack(ACoreEnemy* Target)
{
	if (bAttacking) return;
	
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !Target) return;
	
	CurrentTarget = Target;

	if (IsInRange())
	{
		StartBasicAttackNow();
		return;
	}
	// if enemy out of range
	if (AController* C = OwnerChar->GetController())
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(C, CurrentTarget);
	}
	// Keeps checking distance until enemy in range
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_CheckRange);
		W->GetTimerManager().SetTimer(Timer_CheckRange, this, &UCombatComponent::CheckRangeAndAttack, 0.05f, true);
	}
}

void UCombatComponent::NotifyBasicHit()
{
	if (!bAttacking || !CurrentTarget) return;

	if (UHealthComponent* H = CurrentTarget->GetHealth())
	{
		H->ApplyDamage(BasicDamage);
	}
}

void UCombatComponent::NotifyBasicAttackEnded()
{
	bAttacking = false;
}

ACoreCharacter* UCombatComponent::GetOwnerCharacter() const
{
	return Cast<ACoreCharacter>(GetOwner());
}

void UCombatComponent::StartBasicAttackNow()
{
	if (bAttacking) return;
	
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return;
	
	// if character moving
	if (AController* C = OwnerChar->GetController())
	{
		C->StopMovement();
	}
	
	FaceTargetNow();
	bAttacking = true;
	OwnerChar->BP_PlayBasicAttack();
}

void UCombatComponent::CheckRangeAndAttack()
{
	if (!CurrentTarget)
	{
		if (UWorld* W = GetWorld())
		{
			W->GetTimerManager().ClearTimer(Timer_CheckRange);
		}
		return;
	}
	
	if (IsInRange())
	{
		if (UWorld* W = GetWorld())
		{
			W->GetTimerManager().ClearTimer(Timer_CheckRange);
		}
		StartBasicAttackNow();
	}
}

bool UCombatComponent::IsInRange() const
{
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !CurrentTarget) return false;
	
	const float Dist = FVector::Dist(OwnerActor->GetActorLocation(), CurrentTarget->GetActorLocation());
	
	return Dist <= AttackRange;
}

void UCombatComponent::FaceTargetNow() const
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !CurrentTarget) return;
	
	const FVector OwnerLoc = OwnerChar->GetActorLocation();
	FVector TargetLoc = GetTargetAimPoint();
	
	TargetLoc.Z = OwnerLoc.Z;
	
	const FVector Dir = TargetLoc - OwnerLoc;
	if (Dir.IsNearlyZero()) return;
	
	const FRotator LookRot = Dir.Rotation();
	
	OwnerChar->SetActorRotation(FRotator(0.f, LookRot.Yaw, 0.f));
}

FVector UCombatComponent::GetTargetAimPoint() const
{
	if (!CurrentTarget) return FVector::ZeroVector;
	
	FVector Aim = CurrentTarget->GetActorLocation();
	return Aim;
}
