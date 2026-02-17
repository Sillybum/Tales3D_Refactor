// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DropItemNameWidget.generated.h"

class UTextBlock;

UCLASS()
class UDropItemNameWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Drop")
	void SetItemName(const FText& NewItemName);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_ItemName;
};
