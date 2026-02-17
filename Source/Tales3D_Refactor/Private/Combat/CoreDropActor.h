// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CoreDropActor.generated.h"

class UUserWidget;

UCLASS()
class ACoreDropActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoreDropActor();
	
protected:
	virtual void BeginPlay() override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Drop")
	FText ItemName = FText::FromString(TEXT("Item"));

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop")
	TObjectPtr<class UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop")
	TObjectPtr<class UWidgetComponent> NameWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop")
	TSubclassOf<UUserWidget> NameWidgetClass;
	
	UFUNCTION(BlueprintCallable, Category="Drop")
	void ApplyItemNameToWidget();

};
