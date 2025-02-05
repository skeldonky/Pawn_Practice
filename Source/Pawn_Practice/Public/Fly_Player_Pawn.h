// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Fly_Player_Pawn.generated.h"

class UCapsuleComponent;	//ĸ�� ������Ʈ
class USkeletalMeshComponent;
class USpringArmComponent;	//������ ��
class UCameraComponent;		//ī�޶�
class UFloatingPawnMovement;//�̵� ������ ���� Ŭ����
// Enhanced Input���� �׼� ���� ���� �� ����ϴ� ����ü
struct FInputActionValue;

UCLASS()
class PAWN_PRACTICE_API AFly_Player_Pawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AFly_Player_Pawn();

protected:
	// ĸ�� ������Ʈ (�浹��)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* CapsuleComp;
	// ĳ���� ��
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USkeletalMeshComponent* MeshComponent;
	// ������ �� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;
	// ī�޶� ������Ʈ
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	//�̵�
	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MoveSpeed = 1000.0f;		// �⺻ �̵� �ӵ�
	UPROPERTY(EditAnywhere, Category = "Movement")
	float RotateSpeed = 5.0f;		// �⺻ �̵� �ӵ�

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//�ٴ����� Ȯ���ϴ� �Լ�
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
