// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class ACoreCharacter;
class ACoreEnemy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	
	// AnimNotify calls
	UFUNCTION(BlueprintCallable, Category="Combat")
	void NotifyBasicHit();
	// plays BP_PlayBasicAttackSection(CoreCharacter)
	UFUNCTION(BlueprintCallable, Category="Combat")
	void PlayBasicSection(ACoreEnemy* Target, int32 SectionIndex);
		
protected:
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float BasicDamage = 10.f;
	
private:
	UPROPERTY()
	TObjectPtr<ACoreEnemy> CurrentTarget;
	
	ACoreCharacter* GetOwnerCharacter() const;
};
