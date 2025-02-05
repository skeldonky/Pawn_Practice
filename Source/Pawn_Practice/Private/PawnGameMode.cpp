// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnGameMode.h"
#include "Player_Pawn.h"
#include "PawnPlayerController.h"

APawnGameMode::APawnGameMode()
{
	DefaultPawnClass = APlayer_Pawn::StaticClass();
	PlayerControllerClass = APawnPlayerController::StaticClass();
}
