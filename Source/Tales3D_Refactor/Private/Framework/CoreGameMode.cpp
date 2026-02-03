// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/CoreGameMode.h"

#include "CorePlayerController.h"
#include "Char/CoreCharacter.h"

ACoreGameMode::ACoreGameMode()
{
	PlayerControllerClass	= ACorePlayerController::StaticClass();
	DefaultPawnClass		= ACoreCharacter::StaticClass();
}
