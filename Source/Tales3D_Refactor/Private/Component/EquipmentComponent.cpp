// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Component/EquipmentComponent.h"

#include "InventoryComponent.h"
#include "Char/CoreCharacter.h"
#include "Data/CoreItemData.h"
#include "Data/Item/CoreWeaponItemData.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);	
}

bool UEquipmentComponent::EquipWeaponByItemId(FPrimaryAssetId ItemId)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->Inventory || !ItemId.IsValid()) return false;
	
	// ID -> DataAsset Sync load
	UCoreItemData* BaseData = OwnerChar->Inventory->GetItemDataSync(ItemId);
	UCoreWeaponItemData* WeaponData = Cast<UCoreWeaponItemData>(BaseData);
	if (!WeaponData) return false;
	
	return EquipWeaponData(WeaponData);
}

void UEquipmentComponent::UnequipWeapon()
{
	if (EquippedWeaponActor)
	{
		EquippedWeaponActor->Destroy();
		EquippedWeaponActor = nullptr;
	}
	
	EquippedWeaponId = FPrimaryAssetId();
}

ACoreCharacter* UEquipmentComponent::GetOwnerCharacter() const
{
	return Cast<ACoreCharacter>(GetOwner());
}

bool UEquipmentComponent::EquipWeaponData(UCoreWeaponItemData* WeaponData)
{
	if (!WeaponData || !WeaponData->EquippedActorClass) return false;
	
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return false;
	
	UnequipWeapon();
	UWorld* W = GetWorld();
	if (!W) return false;
	
	FActorSpawnParameters Params;
	Params.Owner = OwnerChar;
	Params.Instigator = OwnerChar;
	AActor* NewWeapon = W->SpawnActor<AActor>(WeaponData->EquippedActorClass, Params);
	if (!NewWeapon) return false;
	
	// Tag (easy to attach FX or other system)
	NewWeapon->Tags.Add(TEXT("EquippedWeapon"));
	AttachWeaponActor(NewWeapon, WeaponData);
	
	EquippedWeaponActor = NewWeapon;
	EquippedWeaponId = WeaponData->GetPrimaryAssetId();
	return true;
}

void UEquipmentComponent::AttachWeaponActor(AActor* WeaponActor, const UCoreWeaponItemData* WeaponData)
{
	if (!WeaponActor || !WeaponData) return;
	
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar) return;
	
	USkeletalMeshComponent* Mesh = OwnerChar->GetMesh();
	if (!Mesh) return;
	
	// snaps to socket
	WeaponActor->AttachToComponent(
		Mesh,
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		WeaponData->AttachSocket
		);
	WeaponActor->SetActorRelativeTransform(WeaponData->AttachOffset);
}

