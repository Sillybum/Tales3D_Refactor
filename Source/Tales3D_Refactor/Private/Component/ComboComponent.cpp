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

	switch (State)
	{
	case EComboState::Skill_Attacking:
		// unable to cancel while skill
		bBasicBuffered = true;
		DebugPrint(TEXT("Basic buffered (during skill"));
		return;

	case EComboState::Skill_Window:
		bBasicBuffered = false;
		StartBasic(1);
		return;
		
	case EComboState::Basic_Attacking:
		bBasicBuffered = true;
		DebugPrint(TEXT("Basic buffered (no window yet)"));
		return;
		
	case EComboState::Basic_Window:
		bBasicBuffered = false;
	{
		const int32 Next = BasicChainIndex + 1;
		if (Next > MaxBasicChain)
		{
			EndCombo();
			return;
		}
		StartBasic(Next);
		return;
	}
	case EComboState::Idle:
	default:
		StartBasic(1);
		return;
	}
}

void UComboComponent::InputSkill(FName SkillId, ACoreEnemy* Target)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->Skills) return;
	if (!Target || SkillId.IsNone()) return;
	
	CurrentTarget = Target;

	if (State == EComboState::Skill_Attacking)
	{
		DebugPrint(TEXT("Skill ignored (skill is active)"));
		return;
	}
	if (State == EComboState::Skill_Window)
	{
		DebugPrint(TEXT("Skill ignored (skill window: basic only)"));
		return;
	}
	
	bSkillBuffered = true;
	BufferedSkillId = SkillId;

	if (State == EComboState::Basic_Window)
	{
		ConsumeOnBasicWindow();
	}
	else if (State == EComboState::Basic_Attacking)
	{
		DebugPrint(TEXT("Skill buffered (waits basic window)"));
	}
	else
	{
		DebugPrint(TEXT("Skill buffered (need basic window to consume)"));	
	}
}

void UComboComponent::NotifyBasicWindowOpen()
{
	if (State == EComboState::Basic_Attacking || State == EComboState::Basic_Window)
	{
		State = EComboState::Basic_Window;
		DebugPrint(TEXT("Basic window OPEN"));
		
		ConsumeOnBasicWindow();
	}
}

void UComboComponent::NotifyBasicSectionFinished()
{
	DebugPrint(FString::Printf(TEXT("Basic section FINISHED (A%d) -> EndCombo"), BasicChainIndex));
	EndCombo();
}

void UComboComponent::NotifySkillMontageEnded()
{
	DebugPrint(TEXT("Skill montage ENDED -> Skill window OPEN"));
	
	State = EComboState::Skill_Window;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(Timer_SkillWindowExpire);
		World->GetTimerManager().SetTimer(
			Timer_SkillWindowExpire,
			this,
			&UComboComponent::OnSkillWindowExpired,
			SkillWindowDuration,
			false
			);
	}
	
	ConsumeOnSkillWindow();
}

void UComboComponent::ForceEndCombo()
{
	EndCombo();
}

class ACoreCharacter* UComboComponent::GetOwnerCharacter() const
{
	return Cast<ACoreCharacter>(GetOwner());
}

void UComboComponent::StartBasic(int32 NextIndex)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->Combat || !CurrentTarget) return;

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(Timer_SkillWindowExpire);
	}
	
	BasicChainIndex = NextIndex;
	TotalComboCount++;
	
	OnComboCountChanged.Broadcast(TotalComboCount);
	
	State = EComboState::Basic_Attacking;
	
	OwnerChar->Combat->PlayBasicSection(CurrentTarget, BasicChainIndex);
	
	DebugPrint(FString::Printf(TEXT("Start Basic A%d | TotalCombo=%d"), BasicChainIndex, TotalComboCount));
}

void UComboComponent::StartSkill(FName SkillId)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !OwnerChar->Skills || !CurrentTarget) return;

	if (BasicChainIndex >= MaxBasicChain)
	{
		DebugPrint(TEXT("Skill blocked (already at last basic"));
		return;
	}
	
	BasicChainIndex = 0;
	TotalComboCount++;
	State = EComboState::Skill_Attacking;
	const bool bAccepted = OwnerChar->Skills->RequestUseSkill(SkillId, CurrentTarget);
	if (!bAccepted)
	{
		DebugPrint(TEXT("Skill rejected (cooldown/invalid"));
		TotalComboCount--;
		
		State = EComboState::Basic_Window;
		return;
	}
	OnComboCountChanged.Broadcast(TotalComboCount);
	DebugPrint(FString::Printf(TEXT("Start Skill %s | TotalCombo=%d"), *SkillId.ToString(), TotalComboCount));
}

void UComboComponent::ConsumeOnBasicWindow()
{
	if (State != EComboState::Basic_Window) return;

	if (bSkillBuffered && !BufferedSkillId.IsNone())
	{
		const FName UseId = BufferedSkillId;
		
		bSkillBuffered = false;
		BufferedSkillId = NAME_None;
		
		StartSkill(UseId);
		return;
	}

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
	if (State != EComboState::Skill_Window) return;

	if (bBasicBuffered)
	{
		bBasicBuffered = false;
		StartBasic(1);
	}
}

void UComboComponent::EndCombo()
{
	State = EComboState::Idle;
	
	BasicChainIndex = 0;
	TotalComboCount = 0;
	
	OnComboCountChanged.Broadcast(TotalComboCount);
	
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
	if (State == EComboState::Skill_Window)
	{
		DebugPrint(TEXT("Skill window EXPIRED -> EndCombo"));
		EndCombo();
	}
}