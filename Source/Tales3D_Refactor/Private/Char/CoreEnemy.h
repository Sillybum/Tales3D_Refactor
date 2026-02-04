// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoreEnemy.generated.h"

UCLASS()
class ACoreEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ACoreEnemy();

	// Selects enemy (called from Controller)
	void SetSelected(bool bSelected);
	bool IsSelected() const { return bIsSelected; }
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<class UWidgetComponent> HPBarWidget;
	// For Assigning on BP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<class UUserWidget> HPBarWidgetClass;
	
private:
	bool bIsSelected = false;
	void UpdateHPBarVisibility();
};
