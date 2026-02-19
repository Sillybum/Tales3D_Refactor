// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Char/CoreCharacter.h"

#include "Camera/CameraComponent.h"
#include "Component/CombatComponent.h"
#include "Component/ComboComponent.h"
#include "Component/EquipmentComponent.h"
#include "Component/HealthComponent.h"
#include "Component/InventoryComponent.h"
#include "Component/SkillComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

ACoreCharacter::ACoreCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	
	/*----------
	 *TopDown Settings
	 ----------*/
	bUseControllerRotationPitch	= false;
	bUseControllerRotationRoll	= false;
	bUseControllerRotationYaw	= false;
	// Character heads to where it sees
	if (UCharacterMovementComponent* Move = GetCharacterMovement())
	{
		Move->bOrientRotationToMovement = true;
		Move->RotationRate = FRotator(0.f, 720.f, 0.f);
	}
	// Camera Boom
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 1100.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch	= false;
	CameraBoom->bInheritRoll	= false;
	CameraBoom->bInheritYaw		= false;
	// Camera
	TopDownCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCamera->SetupAttachment(CameraBoom);
	TopDownCamera->bUsePawnControlRotation = false;
	/*----------
	 * Custom Components
	 ----------*/
	Combat = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	Skills = CreateDefaultSubobject<USkillComponent>(TEXT("Skill"));
	Inventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	Equipment = CreateDefaultSubobject<UEquipmentComponent>(TEXT("Equipment"));
	Combo = CreateDefaultSubobject<UComboComponent>(TEXT("Combo"));
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));
}

float ACoreCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (Health && DamageAmount > 0.f)
	{
		Health->ApplyDamage(DamageAmount);
	}
	
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ACoreCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// Test Debug
	if (Health)
	{
		LastHP = Health->GetHP();
		Health->OnHealthChanged.AddDynamic(this, &ACoreCharacter::HandleHealthChanged);
	}
}

void ACoreCharacter::HandleHealthChanged(float NewHP, float InMaxHP)
{
	UE_LOG(LogTemp, Warning, TEXT("[PlayerHP] %.0f / %.0f"), NewHP, InMaxHP);
	
	if (NewHP < LastHP)
	{
		BP_PlayHitEffect();
	}
	LastHP = NewHP;

	if (!bIsDead && NewHP <= 0.f)
	{
		bIsDead = true;
		CachedController = GetController();
		SetActorEnableCollision(false);

		if (UCharacterMovementComponent* Move = GetCharacterMovement())
		{
			Move->StopMovementImmediately();
			Move->DisableMovement();
		}
		if (AController* C = GetController())
		{
			C->StopMovement();
		}

		BP_PlayDeath();

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(DeathTimerHandle, this, &ACoreCharacter::RespawnAfterDeath, DeathDestroyDelay, false);
		}
	}
}

void ACoreCharacter::RespawnAfterDeath()
{
	Destroy();
}
