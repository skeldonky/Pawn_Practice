// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyGameMode.h"
#include "Fly_Player_Pawn.h"
#include "FlyPlayerController.h"

AFlyGameMode::AFlyGameMode()
{
	DefaultPawnClass = AFly_Player_Pawn::StaticClass();
	PlayerControllerClass = AFlyPlayerController::StaticClass();
}
