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
	UFUNCTION(BlueprintCallable, Category="Combat")
	void NotifyBasicSectionFinished();
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|Basic")
	float BasicAttackRange = 250.f;
		
protected:
	UPROPERTY(EditDefaultsOnly, Category="Combat")
	float BasicDamage = 10.f;
	
private:
	/*-----
	 *Approach Logic
	 -----*/
	// Target far away(when approaching)
	UPROPERTY()
	TObjectPtr<ACoreEnemy> PendingTarget;
	// Target HitNotify refers to, while actually attacking
	UPROPERTY()
	TObjectPtr<ACoreEnemy> ActiveTarget;
	
	int32 PendingSectionIndex = 0;
	
	FTimerHandle Timer_CheckRange;
	
	void StartMoveToTarget()	;
	void CheckRangeAndStart();
	bool IsInRange() const;
	
	void FaceTargetNow() const;
	FVector GetTargetAimPoint() const;
	void ClearPending();
	void ClearActive();
	
	
	ACoreCharacter* GetOwnerCharacter() const;
};
