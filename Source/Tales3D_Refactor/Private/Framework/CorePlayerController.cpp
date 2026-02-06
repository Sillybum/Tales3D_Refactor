// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Framework/CorePlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Char/CoreCharacter.h"
#include "Char/CoreEnemy.h"
#include "Component/CombatComponent.h"
#include "Component/EquipmentComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/SkillComponent.h"

ACorePlayerController::ACorePlayerController()
{
	// For Left Click Hold
	PrimaryActorTick.bCanEverTick = true;
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
}

void ACorePlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// Registers IMC to Local Player
	if (ULocalPlayer* LP = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (IMC_Core)
			{
				Subsystem->AddMappingContext(IMC_Core, 0);
			}
		}
	}
}

void ACorePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// Binds Actions
	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		if (IA_MoveHold)
		{
			// On LB pushed
			EIC->BindAction(IA_MoveHold, ETriggerEvent::Started, this, &ACorePlayerController::OnMoveHoldStarted);
			// On LB being holded
			EIC->BindAction(IA_MoveHold, ETriggerEvent::Triggered, this, &ACorePlayerController::OnMoveHoldTriggered);
			// On LB released
			EIC->BindAction(IA_MoveHold, ETriggerEvent::Completed, this, &ACorePlayerController::OnMoveHoldCompleted);
			// When things such as window losing focus happen
			EIC->BindAction(IA_MoveHold, ETriggerEvent::Canceled, this, &ACorePlayerController::OnMoveHoldCompleted);
		}
		if (IA_BasicAttack)
		{
			EIC->BindAction(IA_BasicAttack, ETriggerEvent::Started, this, &ACorePlayerController::OnBasicAttackStarted);
		}
		if (IA_Skill1)
		{
			EIC->BindAction(IA_Skill1, ETriggerEvent::Started, this, &ACorePlayerController::OnSkill1Started);
		}
		// Debug (to be deleted)
		if (IA_DebugInventory)
		{
			EIC->BindAction(IA_DebugInventory, ETriggerEvent::Started, this, &ACorePlayerController::OnDebugInventoryStarted);
		}
		if (IA_EquipTest)
		{
			EIC->BindAction(IA_EquipTest, ETriggerEvent::Started, this, &ACorePlayerController::OnEquipTest);
		}
		if (IA_UnequipTest)
		{
			EIC->BindAction(IA_UnequipTest, ETriggerEvent::Started, this, &ACorePlayerController::OnUnequipTest);
		}
	}
}

void ACorePlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (bMoveHeld)
	{
		UpdateDestinationAndMove();
	}
}

void ACorePlayerController::OnMoveHoldStarted()
{
	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);

	if (bHit && Hit.bBlockingHit)
	{
		// if enemy clicked
		if (ACoreEnemy* Enemy = Cast<ACoreEnemy>(Hit.GetActor()))
		{
			SelectEnemy(Enemy);
			bMoveHeld = false;
			return;
		}
	}
	
	// if not enemy clicked
	ClearSelection();
	bMoveHeld = true;
	UpdateDestinationAndMove();
}

void ACorePlayerController::OnMoveHoldTriggered()
{
	if (!bMoveHeld) return;
	
	UpdateDestinationAndMove();
}

void ACorePlayerController::OnMoveHoldCompleted()
{
	bMoveHeld = false;
}

void ACorePlayerController::OnBasicAttackStarted()
{
	if (!SelectedEnemy) return;

	if (ACoreCharacter* C = Cast<ACoreCharacter>(GetPawn()))
	{
		if (C->Combat)
		{
			C->Combat->RequestBasicAttack(SelectedEnemy);
		}
		
	}
}

void ACorePlayerController::OnSkill1Started()
{
	if (!SelectedEnemy) return;

	if (ACoreCharacter* C = Cast<ACoreCharacter>(GetPawn()))
	{
		if (C->Skills)
		{
			C->Skills->RequestUseSkill(TEXT("Skill1"), SelectedEnemy);
		}
	}
}

void ACorePlayerController::OnDebugInventoryStarted()
{
	if (ACoreCharacter* C = Cast<ACoreCharacter>(GetPawn()))
	{
		if (C->Inventory)
		{
			C->Inventory->DebugPrintInventory();
		}
	}
}

void ACorePlayerController::OnEquipTest()
{
	if (ACoreCharacter* C = Cast<ACoreCharacter>(GetPawn()))
	{
		if (C->Equipment)
		{
			const FPrimaryAssetId CassandraId(FPrimaryAssetType(TEXT("Item")), FName(TEXT("DA_Rapier_Cassandra")));
			C->Equipment->EquipWeaponByItemId(CassandraId);
		}
	}
}

void ACorePlayerController::OnUnequipTest()
{
	if (ACoreCharacter* C = Cast<ACoreCharacter>(GetPawn()))
	{
		if (C->Equipment)
		{
			C->Equipment->UnequipWeapon();
		}
	}
}

void ACorePlayerController::UpdateDestinationAndMove()
{
	FHitResult Hit;
	const bool bHit = GetHitResultUnderCursor(ECC_Visibility, false, Hit);
	if (!bHit || !Hit.bBlockingHit) return;
	
	CachedDestination = Hit.ImpactPoint;
	// NavMesh oriented Move
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
}

void ACorePlayerController::SelectEnemy(ACoreEnemy* NewEnemy)
{
	if (SelectedEnemy == NewEnemy) return;

	if (SelectedEnemy)
	{
		SelectedEnemy->SetSelected(false);
	}
	SelectedEnemy = NewEnemy;
	if (SelectedEnemy)
	{
		SelectedEnemy->SetSelected(true);
	}
}

void ACorePlayerController::ClearSelection()
{
	if (SelectedEnemy)
	{
		SelectedEnemy->SetSelected(false);
		SelectedEnemy = nullptr;
	}
}
