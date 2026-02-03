// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CorePlayerController.generated.h"

class UInputAction;
class UInputMappingContext;

UCLASS()
class ACorePlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	ACorePlayerController();
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	// For mousehold update
	virtual void PlayerTick(float DeltaTime) override;
	
private:
	// Enhanced Input Assets (be assigned on BP)
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputMappingContext> IMC_Core;
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_MoveHold;
	
	// States
	bool bMoveHeld = false;
	FVector CachedDestination;
	
	// Input Callbacks
	void OnMoveHoldStarted();
	void OnMoveHoldTriggered();
	void OnMoveHoldCompleted();
	
	// Actual Move Update
	void UpdateDestinationAndMove();
};
