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

void UCombatComponent::NotifyBasicHit()
{
	// if (!bAttacking || !CurrentTarget) return;
	if (!CurrentTarget) return;

	if (UHealthComponent* H = CurrentTarget->GetHealth())
	{
		H->ApplyDamage(BasicDamage);
	}
}

void UCombatComponent::PlayBasicSection(ACoreEnemy* Target, int32 SectionIndex)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !Target) return;
	
	CurrentTarget = Target;
	OwnerChar->BP_PlayBasicAttackSection(SectionIndex);
}

ACoreCharacter* UCombatComponent::GetOwnerCharacter() const
{
	return Cast<ACoreCharacter>(GetOwner());
}