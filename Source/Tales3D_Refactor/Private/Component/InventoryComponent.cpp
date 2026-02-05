// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Component/InventoryComponent.h"

#include "Data/CoreItemData.h"
#include "Engine/AssetManager.h"
#include "Net/UnrealNetwork.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, Items);
}

void UInventoryComponent::AddItemById(FPrimaryAssetId ItemId, int32 Quantity)
{
	if (!ItemId.IsValid() || Quantity <= 0) return;
	
	const int32 Index = FindStackIndex(ItemId);
	if (Index == INDEX_NONE)
	{
		FInventoryStack NewStack;
		NewStack.ItemId = ItemId;
		NewStack.Quantity = Quantity;
		Items.Add(NewStack);
	}
	else
	{
		Items[Index].Quantity += Quantity;
	}
}

void UInventoryComponent::AddItemData(UCoreItemData* ItemData, int32 Quantity)
{
	if (!ItemData || Quantity <= 0) return;
	
	AddItemById(ItemData->GetPrimaryAssetId(), Quantity);
}

bool UInventoryComponent::RemoveItemById(FPrimaryAssetId ItemId, int32 Quantity)
{
	if (!ItemId.IsValid() || Quantity <= 0) return false;
	
	const int32 Index = FindStackIndex(ItemId);
	if (Index ==  INDEX_NONE) return false;
	
	Items[Index].Quantity -= Quantity;
	if (Items[Index].Quantity <= 0)
	{
		Items.RemoveAt(Index);
	}
	return true;
}

UCoreItemData* UInventoryComponent::GetItemDataSync(FPrimaryAssetId ItemId)
{
	if (!ItemId.IsValid()) return nullptr;
	
	UAssetManager& AM = UAssetManager::Get();
	// if already loaded, returns it
	if (UObject* Obj = AM.GetPrimaryAssetObject(ItemId))
	{
		return Cast<UCoreItemData>(Obj);
	}
	// if not loaded yet, loads it sync
	TSharedPtr<FStreamableHandle> Handle = AM.LoadPrimaryAsset(ItemId);
	if (Handle.IsValid())
	{
		Handle->WaitUntilComplete();
	}
	
	return Cast<UCoreItemData>(AM.GetPrimaryAssetObject(ItemId));
}

void UInventoryComponent::RequestItemDataAsync(FPrimaryAssetId ItemId)
{
	if (!ItemId.IsValid()) return;
	
	UAssetManager& AM = UAssetManager::Get();
	
	// if already loaded
	if (UObject* Obj = AM.GetPrimaryAssetObject(ItemId))
	{
		if (UCoreItemData* Data = Cast<UCoreItemData>(Obj))
		{
			OnItemDataLoaded.Broadcast(ItemId, Data);
			return;
		}
	}
	// if getting loaded
	if (PendingLoads.Contains(ItemId)) return;
	
	// loads async -> callbacks
	TSharedPtr<FStreamableHandle> Handle = AM.LoadPrimaryAsset(
		ItemId, TArray<FName>{}, FStreamableDelegate::CreateUObject(this, &UInventoryComponent::HandleItemLoaded, ItemId));
	
	PendingLoads.Add(ItemId, Handle);
}

void UInventoryComponent::DebugPrintInventory()
{
	if (!GEngine) return;

	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, TEXT("--- Inventory ---"));

	for (const FInventoryStack& S : Items)
	{
		UCoreItemData* Data = GetItemDataSync(S.ItemId);
		
		const FString NameStr = Data ? Data->DisplayName.ToString() : S.ItemId.ToString();
		const FString Line = FString::Printf(TEXT("%s x%d"), *NameStr, S.Quantity);
		
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, Line);
	}
}

int32 UInventoryComponent::FindStackIndex(FPrimaryAssetId ItemId) const
{
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].ItemId == ItemId)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

void UInventoryComponent::HandleItemLoaded(FPrimaryAssetId ItemId)
{
	PendingLoads.Remove(ItemId);
	
	UAssetManager& AM = UAssetManager::Get();
	if (UCoreItemData* Data = Cast<UCoreItemData>(AM.GetPrimaryAssetObject(ItemId)))
	{
		OnItemDataLoaded.Broadcast(ItemId, Data);
	}
}