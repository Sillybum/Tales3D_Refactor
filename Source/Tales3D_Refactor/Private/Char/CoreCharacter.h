// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoreCharacter.generated.h"


UCLASS()
class ACoreCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACoreCharacter();
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
	
	// Combat
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Combat|Anim")
	void BP_PlayBasicAttack();
	// Skills
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Skill|Anim")
	void BP_PlaySkillMontage(UAnimMontage* Montage);

protected:
	/*----------
	 *Components
	 ----------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<class UCameraComponent> TopDownCamera;
};
