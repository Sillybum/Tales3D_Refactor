// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

struct FStreamableHandle;
class UCoreItemData;

USTRUCT(BlueprintType)
struct FInventoryStack
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FPrimaryAssetId ItemId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Quantity = 0;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnItemDataLoaded, FPrimaryAssetId, ItemId, UCoreItemData*, ItemData);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	
	// Basic functions
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddItemById(FPrimaryAssetId ItemId, int32 Quantity);
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddItemData(UCoreItemData* ItemData, int32 Quantity);
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveItemById(FPrimaryAssetId ItemId, int32 Quantity);
	
	UFUNCTION(BlueprintPure, Category="Inventory")
	const TArray<FInventoryStack>& GetItems() const { return Items;	}
	
	// Data Load
	UFUNCTION(BlueprintCallable, Category="Inventory|Data")
	UCoreItemData* GetItemDataSync(FPrimaryAssetId ItemId);
	UFUNCTION(BlueprintCallable, Category="Inventory|Data")
	void RequestItemDataAsync(FPrimaryAssetId ItemId);
	
	UPROPERTY(BlueprintAssignable, Category="Inventory|Data")
	FOnItemDataLoaded OnItemDataLoaded;
	
	// Debug (to be deleted when UI created)
	UFUNCTION(BlueprintCallable, Category="Inventory|Debug")
	void DebugPrintInventory();
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	TArray<FInventoryStack> Items;
	
	TMap<FPrimaryAssetId, TSharedPtr<FStreamableHandle>> PendingLoads;
	int32 FindStackIndex(FPrimaryAssetId ItemId) const;
	void HandleItemLoaded(FPrimaryAssetId ItemId);
};
