// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"
#include "CoreDropActor.generated.h"

class UUserWidget;
class UPrimitiveComponent;
class USoundBase;
struct FHitResult;

UCLASS()
class ACoreDropActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ACoreDropActor();
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Drop")
	FText ItemName = FText::FromString(TEXT("Item"));
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop")
	float NameWidgetHeight = 70.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop")
	TObjectPtr<class UStaticMeshComponent> Mesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Drop")
	TObjectPtr<class UWidgetComponent> NameWidget;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop")
	TSubclassOf<UUserWidget> NameWidgetClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop|Pickup")
	TObjectPtr<class USoundBase> PickupSound = nullptr;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop|Arc", meta=(ClampMin="0.0"))
	float GravityEnableDelay = 0.1f;
	
	UFUNCTION(BlueprintCallable, Category="Drop")
	void ApplyItemNameToWidget();
	UFUNCTION(BlueprintCallable, Category="Drop")
	void UpdateNameWidgetWorldLocation();
	UFUNCTION(BlueprintCallable, Category="Drop|Arc")
	void LaunchInArc(const FVector& LaunchVelocity);
	
private:
	UFUNCTION()
	void OnMeshBeginOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	void EnableGravityAfterLaunch();

	bool bPickedUp = false;
	FTimerHandle Timer_EnableGravity;

};
