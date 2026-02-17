// TalesWeaver Clone Project (GitHub:Sillybum)


#include "UI/PickupMessageWidget.h"

#include "Components/Border.h"
#include "Components/TextBlock.h"

void UPickupMessageWidget::InitPickupMessage(const FText& InMessage, float InX, float InStartY, float InTargetY)
{
	ElapsedTime = 0.f;
	PosX = InX;
	StartY = InStartY;
	TargetY = InTargetY;

	if (Text_Message)
	{
		Text_Message->SetText(InMessage);
		Text_Message->SetColorAndOpacity(FSlateColor(FLinearColor::White));
	}
	if (Border_Background)
	{
		Border_Background->SetBrushColor(FLinearColor(0.08f, 0.22f, 0.75f, 0.85f));
	}

	SetAlignmentInViewport(FVector2D(0.f, 0.5f));
	SetPositionInViewport(FVector2D(PosX, StartY), true);
	SetRenderOpacity(1.f);
}

void UPickupMessageWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	ElapsedTime += InDeltaTime;

	const float MoveEnd = MoveDuration;
	const float StayEnd = MoveEnd + StayDuration;
	const float FadeEnd = StayEnd + FadeDuration;

	if (ElapsedTime <= MoveEnd)
	{
		const float Alpha = FMath::Clamp(ElapsedTime / MoveDuration, 0.f, 1.f);
		const float Y = FMath::Lerp(StartY, TargetY, Alpha);
		SetPositionInViewport(FVector2D(PosX, Y), true);
		SetRenderOpacity(1.f);
		return;
	}

	SetPositionInViewport(FVector2D(PosX, TargetY), true);

	if (ElapsedTime <= StayEnd)
	{
		SetRenderOpacity(1.f);
		return;
	}

	if (ElapsedTime <= FadeEnd)
	{
		const float FadeAlpha = FMath::Clamp((ElapsedTime - StayEnd) / FadeDuration, 0.f, 1.f);
		SetRenderOpacity(1.f - FadeAlpha);
		return;
	}

	RemoveFromParent();
}
