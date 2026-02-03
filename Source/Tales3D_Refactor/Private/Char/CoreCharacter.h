// TalesWeaver Clone Project (GitHub:Sillybum)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CoreCharacter.generated.h"

UCLASS()
class ACoreCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ACoreCharacter();

protected:
	/*----------
	 *Components
	 ----------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<class USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Camera")
	TObjectPtr<class UCameraComponent> TopDownCamera;
};
