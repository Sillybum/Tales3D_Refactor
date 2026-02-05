// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Component/HealthComponent.h"

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	
	HP = FMath::Clamp(HP, 0.f, MaxHP);
	if (HP <= 0.f)
	{
		HP = MaxHP;
	}
	
	OnHealthChanged.Broadcast(HP, MaxHP);
}

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::ApplyDamage(float Amount)
{
	if (Amount <= 0.f) return;
	
	HP = FMath::Clamp(HP - Amount, 0.f, MaxHP);
	OnHealthChanged.Broadcast(HP, MaxHP);
}