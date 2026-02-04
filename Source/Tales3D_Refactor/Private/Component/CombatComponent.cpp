// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Component/CombatComponent.h"

#include "HealthComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Char/CoreCharacter.h"
#include "Char/CoreEnemy.h"

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