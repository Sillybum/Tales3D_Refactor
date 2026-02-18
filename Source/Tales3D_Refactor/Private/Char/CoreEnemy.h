// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
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
	UFUNCTION(BlueprintPure, Category="Health")
	bool IsDead() const { return bIsDead; }
	UFUNCTION(BlueprintPure, Category="AI|Combat")
	float GetAttackDamage() const { return AttackDamage; }
	
	// Plays Attack Montage
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="AI|Combat")
	void BP_PlayMonsterAttack();
	// Plays Death Montage
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="AI|Combat")
	void BP_PlayMonsterDeath();
	
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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Death")
	float DeathDestroyDelay = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI|Combat")
	float AttackDamage = 10.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop")
	TSubclassOf<AActor> DropItemActorClass;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0"))
	int32 DropCountMin = 1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0"))
	int32 DropCountMax = 2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0.0"))
	float DropSpawnRadius = 30.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0.0"))
	float DropSpawnHeightOffset = 50.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0.0"))
	float DropLaunchHorizontalSpeedMin = 180.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0.0"))
	float DropLaunchHorizontalSpeedMax = 320.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0.0"))
	float DropLaunchUpwardSpeedMin = 280.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Drop", meta=(ClampMin="0.0"))
	float DropLaunchUpwardSpeedMax = 420.f;
	// For refreshing HP on BP
	UFUNCTION()
	void HandleHealthChanged(float NewHP, float MaxHP);
	
	
private:
	bool bIsSelected = false;
	bool bIsDead = false;
	FTimerHandle DeathTimerHandle;
	void UpdateHPBarVisibility();
	void DestroyAfterDeath();
	void SpawnDeathDrops();
	// Gets HP Bar Widget
	UEnemyHPBarWidget* GetHPBarWidget() const;
};
