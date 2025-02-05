// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FlyPlayerController.generated.h"

class UInputMappingContext; // IMC 관련 전방 선언
class UInputAction; // IA 관련 전방 선언

/**
 * 
 */
UCLASS()
class PAWN_PRACTICE_API AFlyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AFlyPlayerController();

	// 에디터에서 세팅할 IMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	// IA_Move를 지정할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	// IA_Look를 지정할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	// IA_Rotate를 지정할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* RotateAction;
	// IA_UpDown를 지정할 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* UpDownAction;

	virtual void BeginPlay() override;
};
