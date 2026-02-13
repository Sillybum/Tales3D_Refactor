// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoreEnemy.generated.h"

class UEnemyHPBarWidget;
class UHealthComponent;

UCLASS()
class ACoreEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	ACoreEnemy();
	
protected:
	virtual void BeginPlay() override;
	
public:
	// Selects enemy (called from Controller)
	void SetSelected(bool bSelected);
	bool IsSelected() const { return bIsSelected; }
	// Health Component
	UFUNCTION(BlueprintPure, Category="Health")
	UHealthComponent* GetHealth() const { return Health; }
	
	// Plays Attack Montage
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="AI|Combat")
	void BP_PlayMonsterAttack();
	
protected:
	/*----------
	 *Components
	 ----------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="UI")
	TObjectPtr<class UWidgetComponent> HPBarWidget;
	// For Assigning on BP
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> HPBarWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Health")
	TObjectPtr<UHealthComponent> Health;
	// For refreshing HP on BP
	UFUNCTION()
	void HandleHealthChanged(float NewHP, float MaxHP);
	
	
private:
	bool bIsSelected = false;
	void UpdateHPBarVisibility();
	// Gets HP Bar Widget
	UEnemyHPBarWidget* GetHPBarWidget() const;
};
