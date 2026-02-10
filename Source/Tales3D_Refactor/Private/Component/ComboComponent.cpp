// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Component/ComboComponent.h"

#include "CombatComponent.h"
#include "SkillComponent.h"
#include "Char/CoreCharacter.h"

UComboComponent::UComboComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UComboComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UComboComponent::InputBasic(ACoreEnemy* Target)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->Combat) return;
	if (!Target) return;
	
	CurrentTarget = Target;
	
	if (bBasicPlaying && WindowType == EComboWindowType::None)
	{
		bBasicBuffered = true;
		DebugPrint(TEXT("Basic buffered (no window yet)"));
		return;
	}

	// if skill is playing
	if (OwnerChar->Skills && OwnerChar->Skills->IsSkillActive())
	{
		bBasicBuffered = true;
		DebugPrint(TEXT("Basic buffered (during skill"));
		return;
	}
	
	// if in skill window, can use basic attack immediately
	if (WindowType == EComboWindowType::Skill)
	{
		bBasicBuffered = false;
		StartBasic(1);	// from A1
		return;
	}
	
	// if in Basic window, can cancel basic attack
	if (WindowType == EComboWindowType::Basic)
	{
		bBasicBuffered = false;
		const int32 Next = FMath::Clamp(BasicChainIndex + 1, 1, MaxBasicChain);
		if (Next > MaxBasicChain)
		{
			EndCombo();
			return;
		}
		StartBasic(Next);
		return;
	}
	// if no window
	StartBasic(1);
}

void UComboComponent::InputSkill(FName SkillId, ACoreEnemy* Target)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->Skills) return;
	if (!Target || SkillId.IsNone()) return;
	
	CurrentTarget = Target;

	if (OwnerChar->Skills->IsSkillActive())
	{
		DebugPrint(TEXT("Skill input ignored (skill is active"));
		return;
	}
	if (WindowType == EComboWindowType::Skill)
	{
		DebugPrint(TEXT("Skill input ignored (skill window: basic only"));
		return;
	}
	
	bSkillBuffered = true;
	BufferedSkillId = SkillId;

	if (WindowType == EComboWindowType::Basic)
	{
		ConsumeOnBasicWindow();
	}
	else
	{
		DebugPrint(TEXT("Skill buffered (will consume on basic window"));
	}
}

void UComboComponent::NotifyBasicWindowOpen()
{
	// notify A1~A4
	WindowType = EComboWindowType::Basic;
	DebugPrint(TEXT("Basic window Open"));
	
	ConsumeOnBasicWindow();
}

void UComboComponent::NotifyBasicSectionFinished()
{
	// when basic attack combo finished with no further input
	DebugPrint(FString::Printf(TEXT("Basic section FINISHED A%d"), BasicChainIndex));
	EndCombo();
}

void UComboComponent::NotifySkillMontageEnded()
{
	// Skill cannot be canceled, it opens window when its montage is over
	DebugPrint(TEXT("Skill montage ENDED -> Skill window OPEN"));
	OpenSkillWindow();
	// if basic attacked already buffered, consumes
	ConsumeOnSkillWindow();
}

class ACoreCharacter* UComboComponent::GetOwnerCharacter() const
{
	return Cast<ACoreCharacter>(GetOwner());
}

void UComboComponent::StartBasic(int32 NextIndex)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->Combat || !CurrentTarget) return;
	
	BasicChainIndex = NextIndex;
	TotalComboCount++;
	bBasicPlaying = true;
	// if new attack, closes window
	CloseWindow();
	OwnerChar->Combat->PlayBasicSection(CurrentTarget, BasicChainIndex);
	
	DebugPrint(FString::Printf(TEXT("Start Basic A%d | TotalCombo=%d"), BasicChainIndex, TotalComboCount));
}

void UComboComponent::StartSkill(FName SkillId)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->Skills || !CurrentTarget) return;
	
	BasicChainIndex = 0;
	TotalComboCount++;
	bBasicPlaying = false;
	CloseWindow();
	
	const bool bAccepted = OwnerChar->Skills->RequestUseSkill(SkillId, CurrentTarget);
	if (!bAccepted)
	{
		DebugPrint(TEXT("Skill rejected (cooldown or invalid"));
		TotalComboCount--;
		return;
	}
	DebugPrint(FString::Printf(TEXT("Start SKILL %s | Total Combo=%d"), *SkillId.ToString(), TotalComboCount));
}

void UComboComponent::ConsumeOnBasicWindow()
{
	// Skillbuffer First
	if (bSkillBuffered && !BufferedSkillId.IsNone())
	{
		// if last basic attack, cannot skill combo
		if (BasicChainIndex >= MaxBasicChain)
		{
			DebugPrint(TEXT("Skill blocked (already at last basic)"));
			bSkillBuffered = false;
			BufferedSkillId = NAME_None;
		}
		else
		{
			// consumes skill
			bSkillBuffered = false;
			const FName UseId = BufferedSkillId;
			BufferedSkillId = NAME_None;
			
			StartSkill(UseId);
			return;
		}
	}
	
	// if no skill buffer
	if (bBasicBuffered)
	{
		bBasicBuffered = false;
		
		const int32 Next = BasicChainIndex + 1;
		if (Next > MaxBasicChain)
		{
			EndCombo();
			return;
		}
		
		StartBasic(Next);
	}
}

void UComboComponent::ConsumeOnSkillWindow()
{
	if (bBasicBuffered)
	{
		bBasicBuffered = false;
		StartBasic(1);
	}
}

void UComboComponent::OpenSkillWindow()
{
}

void UComboComponent::CloseWindow()
{
	WindowType = EComboWindowType::None;
}

void UComboComponent::EndCombo()
{
	WindowType = EComboWindowType::None;
	BasicChainIndex = 0;
	TotalComboCount = 0;
	bBasicPlaying = false;
	
	bBasicBuffered = false;
	bSkillBuffered = false;
	BufferedSkillId = NAME_None;
	
	CurrentTarget = nullptr;

	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_SkillWindowExpire);
	}
	DebugPrint(TEXT("Combo End (reset)"));
}

void UComboComponent::DebugPrint(const FString& Msg) const
{
	if (!GEngine) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 1.5f, FColor::Yellow, Msg);
}

void UComboComponent::OnSkillWindowExpired()
{
	// if skill window ends with no further attack, combo ends
	if (WindowType == EComboWindowType::Skill)
	{
		DebugPrint(TEXT("Skill window EXPIRED -> Combo ends"));
		EndCombo();
	}
}