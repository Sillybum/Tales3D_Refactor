// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ComboComponent.generated.h"

class ACoreEnemy;

UENUM(BlueprintType)
enum class EComboState : uint8
{
	Idle,
	Basic_Attacking,
	Basic_Window,
	Skill_Attacking,
	Skill_Window
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnComboCountChanged, int32, NewCount);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UComboComponent();

protected:
	virtual void BeginPlay() override;

public:
	// Input (called by controller)
	UFUNCTION(BlueprintCallable, Category="Combo")
	void InputBasic(ACoreEnemy* Target);
	UFUNCTION(BlueprintCallable, Category="Combo")
	void InputSkill(FName SkillId, ACoreEnemy* Target);
	
	// Called from AnimNotify
	UFUNCTION(BlueprintCallable, Category="Combo|Notify")
	void NotifyBasicWindowOpen();
	// Basic attack finished without connected combo attack
	UFUNCTION(BlueprintCallable, Category="Combo|Notify")
	void NotifyBasicSectionFinished();
	// Notifies skill montage ended(skill cannot be cancelled
	UFUNCTION(BlueprintCallable, Category="Combo|Notify")
	void NotifySkillMontageEnded();
	// returns combo count for debug| can be connected to UI later
	UFUNCTION(BlueprintPure, Category="Combo")
	int32 GetTotalComboCount() const { return TotalComboCount; }
	
	// When target disappeared(death, unselect, etc)
	UFUNCTION(BlueprintCallable, Category="Combo")
	void ForceEndCombo();
	
	/*
	 * Combo Widget
	 */
	UPROPERTY(BlueprintAssignable, Category="Combo")
	FOnComboCountChanged OnComboCountChanged;
	
private:
	// Rules
	UPROPERTY(EditDefaultsOnly, Category="Combo|Rule")
	int32 MaxBasicChain = 5;
	UPROPERTY(EditDefaultsOnly, Category="Combo|Rule")
	float SkillWindowDuration = 0.6f;
	
	// State
	UPROPERTY()
	TObjectPtr<ACoreEnemy> CurrentTarget;
	
	EComboState State = EComboState::Idle;
	// if skill used, basic attack starts from A1 again
	int32 BasicChainIndex = 0;
	int32 TotalComboCount = 0;
	
	// Input buffer
	bool bBasicBuffered = false;
	bool bSkillBuffered = false;
	FName BufferedSkillId = NAME_None;
	
	FTimerHandle Timer_SkillWindowExpire;
	
	// Inside Logic
	class ACoreCharacter* GetOwnerCharacter() const;
	void StartBasic(int32 NextIndex);
	void StartSkill(FName SkillId);
	
	void ConsumeOnBasicWindow();	// Consumes buffer in basic window(Skill first)
	void ConsumeOnSkillWindow();	// Consumes basic attack buffer in skill window
	
	void EndCombo();
	void DebugPrint(const FString& Msg) const;
	
	void OnSkillWindowExpired();
};
