// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CoreItemData.generated.h"

/**
 * 
 */
UCLASS()
class UCoreItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	/*----------
	 *For UI
	 ----------*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	FText DisplayName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item", meta=(MultiLine=true))
	FText Description;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item")
	TObjectPtr<class UTexture2D> Icon = nullptr;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		static const FPrimaryAssetType ItemType(TEXT("Item"));
		return FPrimaryAssetId(ItemType, GetFName());
	}
};
