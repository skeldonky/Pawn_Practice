// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Fly_Player_Pawn.generated.h"

class UCapsuleComponent;	//캡슐 컴포넌트
class USkeletalMeshComponent;
class USpringArmComponent;	//스프링 암
class UCameraComponent;		//카메라
class UFloatingPawnMovement;//이동 구현을 위한 클래스
// Enhanced Input에서 액션 값을 받을 때 사용하는 구조체
struct FInputActionValue;

UCLASS()
class PAWN_PRACTICE_API AFly_Player_Pawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFly_Player_Pawn();

protected:
	// 캡슐 컴포넌트 (충돌용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComp;
	// 캐릭터 모델
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USkeletalMeshComponent* MeshComponent;
	// 스프링 암 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	// 카메라 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	//이동
	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 1000.0f;		// 기본 이동 속도
	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotateSpeed = 5.0f;		// 기본 이동 속도

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//바닥인지 확인하는 함수
	bool IsOnGround();

	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void Rotate(const FInputActionValue& value);
	UFUNCTION()
	void UpDown(const FInputActionValue& value);
};
