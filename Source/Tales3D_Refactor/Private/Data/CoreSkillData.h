// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CoreSkillData.generated.h"

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
	// FX | Sound
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skill")
	TObjectPtr<class UNiagaraSystem> FX = nullptr;
};
