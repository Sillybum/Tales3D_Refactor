// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoreCharacter.generated.h"

class UHealthComponent;
class UCoreSkillData;

UCLASS()
class ACoreCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACoreCharacter();
	
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
protected:
	virtual void BeginPlay() override;
	
public:
	/*----------
	 *Custom Components
	 ----------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combat")
	TObjectPtr<class UCombatComponent> Combat;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Skill")
	TObjectPtr<class USkillComponent> Skills;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<class UInventoryComponent> Inventory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Equip")
	TObjectPtr<class UEquipmentComponent> Equipment;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Combo")
	TObjectPtr<class UComboComponent> Combo;
	
	// Combat
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Combat|Anim")
	void BP_PlayBasicAttack();
	// Skills
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Skill|Anim")
	void BP_PlaySkillMontage(UAnimMontage* Montage);
	// EffectFX
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Skill|FX")
	void BP_SpawnSkillEffectFX(UCoreSkillData* SkillData, AActor* TargetActor);
	// AfterImageFX
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Skill|FX")
	void BP_SpawnSkillAfterImageFX(UCoreSkillData* SkillData);
	// Combo / plays basic attack section
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Combat|Anim")
	void BP_PlayBasicAttackSection(int32 SectionIndex);
	
	// Health
	UFUNCTION(BlueprintPure, Category="Health")
	UHealthComponent* GetHealth() const { return Health; }

protected:
	/*----------
	 *Components
	 ----------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<class UCameraComponent> TopDownCamera;
	
	// Health
	UFUNCTION()
	void HandleHealthChanged(float NewHP, float InMaxHP);
	
private:
	// Health Component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UHealthComponent> Health;
};
