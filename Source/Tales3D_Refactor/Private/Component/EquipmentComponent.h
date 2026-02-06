// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EquipmentComponent.generated.h"


class UCoreWeaponItemData;
class ACoreCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEquipmentComponent();

	// Tries equipping item in inventory
	UFUNCTION(BlueprintCallable, Category="Equip")
	bool EquipWeaponByItemId(FPrimaryAssetId ItemId);
	UFUNCTION(BlueprintCallable, Category="Equip")
	void UnequipWeapon();
	UFUNCTION(BlueprintPure, Category="Equip")
	bool HasWeaponEquipped() const { return EquippedWeaponActor != nullptr; }
	UFUNCTION(BlueprintPure, Category="Equip")
	FPrimaryAssetId GetEquippedWeaponId() const { return EquippedWeaponId; }
	
private:
	UPROPERTY()
	FPrimaryAssetId EquippedWeaponId;
	UPROPERTY()
	TObjectPtr<AActor> EquippedWeaponActor;
	
	ACoreCharacter* GetOwnerCharacter() const;
	
	bool EquipWeaponData(UCoreWeaponItemData* WeaponData);
	void AttachWeaponActor(AActor* WeaponActor, const UCoreWeaponItemData* WeaponData);
};
