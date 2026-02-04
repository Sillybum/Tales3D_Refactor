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
	
	UFUNCTION(BlueprintCallable, Category="Combat")
	void RequestBasicAttack(ACoreEnemy* Target);
	// AnimNotify calls
	UFUNCTION(BlueprintCallable, Category="Combat")
	void NotifyBasicHit();
	// Called on Montage ended
	UFUNCTION(BlueprintCallable, Category="Combat")
	void NotifyBasicAttackEnded();
	UFUNCTION(BlueprintPure, Category="Combat")
	bool IsAttacking() const { return bAttacking; }
		
protected:
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float AttackRange = 200.f;
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float BasicDamage = 10.f;
	
private:
	UPROPERTY()
	TObjectPtr<ACoreEnemy> CurrentTarget;
	
	bool bAttacking = false;
	
	FTimerHandle Timer_CheckRange;
	
	ACoreCharacter* GetOwnerCharacter() const;
	void StartBasicAttackNow();
	void CheckRangeAndAttack();
	bool IsInRange() const;
	// Faces Target
	void FaceTargetNow() const;
	FVector GetTargetAimPoint() const;
};
