// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, NewHP, float, MaxHP);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()
protected:
	virtual void BeginPlay() override;

public:	
	UHealthComponent();
	
	UFUNCTION(BlueprintCallable, Category="Health")
	void ApplyDamage(float Amount);
	UFUNCTION(BlueprintPure, Category="Health")
	float GetHP() const { return HP; }
	UFUNCTION(BlueprintPure, Category="Health")
	float GetMaxHP() const { return MaxHP; }
	
	UPROPERTY(BlueprintAssignable, Category="Health")
	FOnHealthChanged OnHealthChanged;
		
protected:
	UPROPERTY(EditDefaultsOnly, Category="Health")
	float MaxHP = 100.f;
	UPROPERTY(VisibleAnywhere, Category="Health")
	float HP = 100.f;
};
