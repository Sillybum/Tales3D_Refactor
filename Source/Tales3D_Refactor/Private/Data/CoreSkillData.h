// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CoreSkillData.generated.h"

class UNiagaraSystem;

UCLASS()
class UCoreSkillData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// Skill ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skill")
	FName SkillId = NAME_None;
	// Montage to play
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skill")
	TObjectPtr<class UAnimMontage> Montage = nullptr;
	// Skill Range
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skill")
	float Range = 200.f;
	// Damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skill")
	float Damage = 25.f;
	// Cooldown
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skill")
	float Cooldown = 1.0f;
	
	/*-----
	 * FX
	 -----*/
	// ----------
	// Cool effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|Effect")
	TObjectPtr<UNiagaraSystem> EffectFX = nullptr;
	// true: attach to owner | false: attach to world
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|Effect")
	bool bAttachEffectFXToOwner = true;
	// socket to attach | None: just attach to mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|Effect", meta=(EditCondition="bAttachEffectFXToOwner"))
	FName EffectFXSocket = NAME_None;
	// if bAttachEffectFXToOwner=false, whether to spawn on target
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|Effect", meta=(EditCondition="!bAttachEffectFXToOwner"))
	bool bSpawnEffectFXAtTarget = true;
	// -----------
	// AfterImage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|AfterImage")
	bool bUseAfterImage = false;
	// Color of AfterImage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|AfterImage", meta=(EditCondition="bUseAfterImage"))
	FLinearColor AfterImageTint = FLinearColor(0.f, 0.6f, 1.f, 1.f);
	// AfterImageLifeTime
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|AfterImage", meta=(EditCondition="bUseAfterImage"))
	float AfterImageLifeTime = 0.25f;
	// Start Opacity
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|AfterImage", meta=(EditCondition="bUseAfterImage"))
	float AfterImageStartOpacity = 0.35f;
	// AfterImage Actor | if null, uses default on BP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|AfterImage", meta=(EditCondition="bUseAfterImage"))
	TSubclassOf<AActor> AfterImageActorClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="FX|AfterImage", meta=(EditCondition="bUseAfterImage"))
	TSubclassOf<AActor> AfterImageWeaponActorClass;
	
	//==========
	//   SFX
	//==========
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SFX")
	TObjectPtr<class USoundBase> SkillStartSwooshSFX = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SFX")
	float SkillStartSwooshVolume = 1.0f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SFX")
	TObjectPtr<class USoundBase> SkillHitSFX = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SFX")
	float SkillHitVolume = 1.0f;
	
	// Sets socket as sound location(if None, capsule)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SFX")
	FName SkillHitSFXSocket = NAME_None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SFX")
	float SkillHitSFX_ZOffset = 0.f;
};
