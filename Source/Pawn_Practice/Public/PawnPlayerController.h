// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PawnPlayerController.generated.h"

class UInputMappingContext; // IMC 관련 전방 선언
class UInputAction; // IA 관련 전방 선언

/**
 * 
 */
UCLASS()
class PAWN_PRACTICE_API APawnPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	APawnPlayerController();

	// 에디터에서 세팅할 IMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	// IA_Move를 지정할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	// IA_Look를 지정할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* JumpAction;

	virtual void BeginPlay() override;
};
