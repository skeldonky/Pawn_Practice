// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FlyPlayerController.generated.h"

class UInputMappingContext; // IMC ���� ���� ����
class UInputAction; // IA ���� ���� ����

/**
 * 
 */
UCLASS()
class PAWN_PRACTICE_API AFlyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	AFlyPlayerController();

	// �����Ϳ��� ������ IMC
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;
	// IA_Move�� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;
	// IA_Look�� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;
	// IA_Rotate�� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* RotateAction;
	// IA_UpDown�� ������ ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* UpDownAction;

	virtual void BeginPlay() override;
};
