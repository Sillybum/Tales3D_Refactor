// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickupMessageWidget.generated.h"

class UTextBlock;
class UBorder;

UCLASS()
class UPickupMessageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="PickupUI")
	void InitPickupMessage(const FText& InMessage, float InX, float InStartY, float InTargetY);

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_Message;
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UBorder> Border_Background;

private:
	float ElapsedTime = 0.f;
	float PosX = 24.f;
	float StartY = 0.f;
	float TargetY = 0.f;
	float MoveDuration = 0.9f;
	float StayDuration = 1.0f;
	float FadeDuration = 0.35f;
};
