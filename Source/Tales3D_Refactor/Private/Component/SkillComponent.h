// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"


class ACoreCharacter;
class ACoreEnemy;
class UCoreSkillData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USkillComponent();
protected:
	virtual void BeginPlay() override;
	
public:
	// Auto-register Skills on list on BP at BeginPlay
	UPROPERTY(EditDefaultsOnly, Category="Skill")
	TArray<TObjectPtr<UCoreSkillData>> DefaultSkills;
	// called by Input
	UFUNCTION(BlueprintCallable, Category="Skill")
	void RequestUseSkill(FName SkillId, ACoreEnemy* Target);
	// Applies Damage (Called by AnimNotify)
	UFUNCTION(BlueprintCallable, Category="Skill")
	void NotifyActiveSkillHit();
	// Called on Montage ended
	UFUNCTION(BlueprintCallable, Category="Skill")
	void NotifyActiveSkillEnded();
	
	UFUNCTION(BlueprintPure, Category="Skill")
	bool IsSkillActive() const { return bSkillActive; }
	
	UFUNCTION(BlueprintPure, Category="Skill")
	UCoreSkillData* GetActiveSkill() const { return ActiveSkill; }
	UFUNCTION(BlueprintPure, Category="Skill")
	ACoreEnemy* GetActiveTarget() const { return CurrentTarget; }
	
	/*-----
	 * FX
	 -----*/
	UFUNCTION(BlueprintCallable, Category="Skill|FX")
	void NotifySkillEffectFX();
	UFUNCTION(BlueprintCallable, Category="Skill|FX")
	void NotifySkillAfterImageFX();
	
private:
	// Registered Skills
	UPROPERTY()
	TMap<FName, TObjectPtr<UCoreSkillData>> SkillMap;
	// Cooldown
	UPROPERTY()
	TMap<FName, float> NextReadyTime;
	// Present State
	UPROPERTY()
	TObjectPtr<ACoreEnemy> CurrentTarget;
	UPROPERTY()
	TObjectPtr<UCoreSkillData> ActiveSkill;
	
	bool bSkillActive = false;
	
	FTimerHandle Timer_CheckRange;
	
	ACoreCharacter* GetOwnerCharacter() const;
	
	bool CanUseSkill(UCoreSkillData* Skill, ACoreEnemy* Target) const;
	bool IsInRange(UCoreSkillData* Skill, ACoreEnemy* Target) const;
	
	void StartSkillNow(UCoreSkillData* Skill, ACoreEnemy* Target);
	void CheckRangeAndStartSkill();
	void FaceTargetNow() const;
	float GetWorldTime() const;
};
