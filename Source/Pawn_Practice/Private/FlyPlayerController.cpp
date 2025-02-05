// Fill out your copyright notice in the Description page of Project Settings.


#include "FlyPlayerController.h"
// Enhanced Input System�� Local Player Subsystem�� ����ϱ� ���� ����
#include "EnhancedInputSubsystems.h"

AFlyPlayerController::AFlyPlayerController()
	: InputMappingContext(nullptr), 
	  MoveAction(nullptr), 
	  LookAction(nullptr), 
	  RotateAction(nullptr), 
	  UpDownAction(nullptr)
{
}

void AFlyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// ���� PlayerController�� ����� Local Player ��ü�� ������    
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		// Local Player���� EnhancedInputLocalPlayerSubsystem�� ȹ��
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				// Subsystem�� ���� �츮�� �Ҵ��� IMC�� Ȱ��ȭ
				// �켱����(Priority)�� 0�� ���� ���� �켱����
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}
