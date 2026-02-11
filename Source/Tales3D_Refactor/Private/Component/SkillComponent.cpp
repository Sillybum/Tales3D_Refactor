// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Component/SkillComponent.h"

#include "HealthComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Char/CoreCharacter.h"
#include "Char/CoreEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Data/CoreSkillData.h"
#include "Kismet/GameplayStatics.h"

USkillComponent::USkillComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USkillComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register skills on SkillMap
	for (UCoreSkillData* Skill : DefaultSkills)
	{
		if (!Skill || Skill->SkillId.IsNone())
		{
			continue;
		}
		SkillMap.Add(Skill->SkillId, Skill);
		// Sets Cooldown 0 that skill can be used right away
		NextReadyTime.FindOrAdd(Skill->SkillId) = 0.f;
	}
}

bool USkillComponent::RequestUseSkill(FName SkillId, ACoreEnemy* Target)
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || SkillId.IsNone() || !Target) return false;
	
	UCoreSkillData* Skill = SkillMap.Contains(SkillId) ? SkillMap[SkillId] : nullptr;
	if (!CanUseSkill(Skill, Target)) return false;
	
	// For use in NotifyHit
	ActiveSkill = Skill;
	CurrentTarget = Target;

	if (IsInRange(Skill, Target))
	{
		StartSkillNow(Skill, Target);
		return true;
	}
	// if out of range, moves close
	if (AController* C = OwnerChar->GetController())
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(C, Target);
	}
	// Keeps checking distance until in range
	if (UWorld* W = GetWorld())
	{
		W->GetTimerManager().ClearTimer(Timer_CheckRange);
		W->GetTimerManager().SetTimer(Timer_CheckRange, this, &USkillComponent::CheckRangeAndStartSkill, 0.05f, true);
	}
	return true;
}

void USkillComponent::NotifyActiveSkillHit()
{
	if (!bSkillActive || !ActiveSkill || !CurrentTarget) return;

	if (UHealthComponent* H = CurrentTarget->GetHealth())
	{
		H->ApplyDamage(ActiveSkill->Damage);
	}
}

void USkillComponent::NotifyActiveSkillEnded()
{
	bSkillActive = false;
	ActiveSkill = nullptr;
	CurrentTarget = nullptr;
}

void USkillComponent::NotifySkillEffectFX()
{
	if (!bSkillActive || !ActiveSkill) return;

	if (ACoreCharacter* OwnerChar = GetOwnerCharacter())
	{
		OwnerChar->BP_SpawnSkillEffectFX(ActiveSkill, CurrentTarget);
	}
}

void USkillComponent::NotifySkillAfterImageFX()
{
	if (!bSkillActive || !ActiveSkill) return;

	if (ACoreCharacter* OwnerChar = GetOwnerCharacter())
	{
		OwnerChar->BP_SpawnSkillAfterImageFX(ActiveSkill);
	}
}

void USkillComponent::NotifySkillHitSFX()
{
	if (!ActiveSkill || !ActiveSkill->SkillHitSFX) return;
	if (!CurrentTarget) return;
	
	const FVector HitPoint = GetSkillHitPoint();
	
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		ActiveSkill->SkillHitSFX,
		HitPoint,
		ActiveSkill->SkillHitVolume
		);
}

void USkillComponent::PlaySkillStartSFX() const
{
	if (!ActiveSkill || !ActiveSkill->SkillStartSwooshSFX) return;
	
	const AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;
	
	UGameplayStatics::PlaySoundAtLocation(
		GetWorld(),
		ActiveSkill->SkillStartSwooshSFX,
		OwnerActor->GetActorLocation(),
		ActiveSkill->SkillStartSwooshVolume
		);
}

FVector USkillComponent::GetSkillHitPoint() const
{
	if (!CurrentTarget) return FVector::ZeroVector;
	
	// if socket info, plays sound at socket
	if (ActiveSkill && ActiveSkill->SkillHitSFXSocket != NAME_None)
	{
		if (USkeletalMeshComponent* Mesh = CurrentTarget->FindComponentByClass<USkeletalMeshComponent>())
		{
			if (Mesh->DoesSocketExist(ActiveSkill->SkillHitSFXSocket))
			{
				FVector P = Mesh->GetSocketLocation(ActiveSkill->SkillHitSFXSocket);
				P.Z += ActiveSkill->SkillHitSFX_ZOffset;
				return P;
			}
		}
	}
	
	// if no socket info
	FVector P = CurrentTarget->GetActorLocation();
	if (UCapsuleComponent* Cap = CurrentTarget->GetCapsuleComponent())
	{
		P.Z += Cap->GetScaledCapsuleHalfHeight();
	}

	if (ActiveSkill)
	{
		P.Z += ActiveSkill->SkillHitSFX_ZOffset;
	}
	return P;
}

ACoreCharacter* USkillComponent::GetOwnerCharacter() const
{
	return Cast<ACoreCharacter>(GetOwner());
}

bool USkillComponent::CanUseSkill(UCoreSkillData* Skill, ACoreEnemy* Target) const
{
	if (!Skill || !Target) return false;
	if (bSkillActive) return false;
	
	// Checks cooldown
	const float Now = GetWorldTime();
	const float ReadyTime = NextReadyTime.Contains(Skill->SkillId) ? NextReadyTime[Skill->SkillId] : 0.f;
	if (Now < ReadyTime) return false;
	
	return true;
}

bool USkillComponent::IsInRange(UCoreSkillData* Skill, ACoreEnemy* Target) const
{
	if (!Skill || !Target || !GetOwner()) return false;
	
	const float Dist = FVector::Dist(GetOwner()->GetActorLocation(), Target->GetActorLocation());
	return Dist <= Skill->Range;
}

void USkillComponent::StartSkillNow(UCoreSkillData* Skill, ACoreEnemy* Target)
{
	if (!Skill || !Target) return;
	
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar)	return;
	
	// makes char stop if it is moving
	if (AController* C = OwnerChar->GetController())
	{
		C->StopMovement();
	}
	FaceTargetNow();
	bSkillActive = true;
	NextReadyTime[Skill->SkillId] = GetWorldTime() + Skill->Cooldown;
	OwnerChar->BP_PlaySkillMontage(Skill->Montage);
	PlaySkillStartSFX();
}

void USkillComponent::CheckRangeAndStartSkill()
{
	if (!ActiveSkill || !CurrentTarget)
	{
		if (UWorld* W = GetWorld())
		{
			W->GetTimerManager().ClearTimer(Timer_CheckRange);
		}
		return;
	}
	if (IsInRange(ActiveSkill, CurrentTarget))
	{
		if (UWorld* W = GetWorld())
		{
			W->GetTimerManager().ClearTimer(Timer_CheckRange);
		}
		StartSkillNow(ActiveSkill, CurrentTarget);
	}
}

void USkillComponent::FaceTargetNow() const
{
	ACoreCharacter* OwnerChar = GetOwnerCharacter();
	if (!OwnerChar || !CurrentTarget) return;
	
	FVector TargetLocation = CurrentTarget->GetActorLocation();
	const FVector OwnerLocation = OwnerChar->GetActorLocation();
	TargetLocation.Z = OwnerLocation.Z;
	
	const FVector Dir = (TargetLocation - OwnerLocation);
	if (Dir.IsNearlyZero()) return;
	
	const float Yaw = Dir.Rotation().Yaw;
	OwnerChar->SetActorRotation(FRotator(0.f, Yaw, 0.f));
}

float USkillComponent::GetWorldTime() const
{
	if (const UWorld* W = GetWorld())
	{
		return W->GetTimeSeconds();
	}
	return 0.f;
}

