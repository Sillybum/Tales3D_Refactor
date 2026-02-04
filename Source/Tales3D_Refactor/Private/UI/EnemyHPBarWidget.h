// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyHPBarWidget.generated.h"

class UProgressBar;

UCLASS()
class UEnemyHPBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	// Called from CoreEnemy
	UFUNCTION(BlueprintCallable, Category="HP")
	void SetHP(float NewHP, float NewMaxHP);
	
protected:
	// If WBP has ProgressBar named ProgressBar_HP, connects automatically
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_HP;
};
