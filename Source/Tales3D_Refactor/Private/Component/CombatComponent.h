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
	
	/*----------
	 * BasicAttack FX
	 ----------*/
	UPROPERTY(EditDefaultsOnly, Category="Combat|FX")
	TObjectPtr<class UNiagaraSystem> BasicHitFX = nullptr;
	// if you want to attach FX to a certain Socket
	UPROPERTY(EditDefaultsOnly, Category="Combat|FX")
	FName BasicHitFXSocket = NAME_None;
	// Z offset for FX
	UPROPERTY(EditDefaultsOnly, Category="Combat|FX")
	float BasicHitFX_ZOffset = 0.f;
	
	/*----------
	 Sound
	 ----------*/
	UPROPERTY(EditDefaultsOnly, Category="Combat|SFX")
	TObjectPtr<class USoundBase> BasicSwooshSFX = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|SFX")
	TObjectPtr<class USoundBase> BasicHitSFX = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|SFX")
	float BasicSwooshVolume = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Combat|SFX")
	float BasicHitVolume = 1.0f;
	
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
	
	/*----------
	 BasicAttack FX
	 ----------*/
	FVector GetActiveHitPoint() const;
	FRotator GetHitFacingRotation() const;
	
	/*----------
	 Sound
	 ----------*/
	void PlayBasicSwoosh() const;
	void PlayBasicHitSoundAtPoint(const FVector& Point) const;
	
	ACoreCharacter* GetOwnerCharacter() const;
};
