// TalesWeaver Clone Project (GitHub:Sillybum)


#include "Char/CoreCharacter.h"

#include "Camera/CameraComponent.h"
#include "Component/CombatComponent.h"
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
	CameraBoom->TargetArmLength = 800.f;
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
	

}
