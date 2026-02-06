// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Data/CoreItemData.h"
#include "CoreWeaponItemData.generated.h"

/**
 * 
 */
UCLASS()
class UCoreWeaponItemData : public UCoreItemData
{
	GENERATED_BODY()
	
public:
	// Weapon to equip & spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equip")
	TSubclassOf<AActor> EquippedActorClass;
	// Socket name to attach
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equip")
	FName AttachSocket = TEXT("weapon_r");
	// Socket offset
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Equip")
	FTransform AttachOffset;
};
