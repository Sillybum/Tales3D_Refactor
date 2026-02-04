// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "CorePlayerController.generated.h"

class ACoreEnemy;
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
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputAction> IA_BasicAttack;
	
	// Selected Enemy
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Targeting", meta=(AllowPrivateAccess="true"))
	TObjectPtr<ACoreEnemy> SelectedEnemy;
	
	// States
	bool bMoveHeld = false;
	FVector CachedDestination;
	
	// Input Callbacks
	void OnMoveHoldStarted();
	void OnMoveHoldTriggered();
	void OnMoveHoldCompleted();
	void OnBasicAttackStarted();
	// Actual Move Update
	void UpdateDestinationAndMove();
	
	// Enemy
	void SelectEnemy(ACoreEnemy* NewEnemy);
	void ClearSelection();
};
