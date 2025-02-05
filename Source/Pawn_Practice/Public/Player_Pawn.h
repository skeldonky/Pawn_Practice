// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player_Pawn.generated.h"

class UCapsuleComponent;	//ĸ�� ������Ʈ
class USkeletalMeshComponent;
class USpringArmComponent;	//������ ��
class UCameraComponent;		//ī�޶�
class UFloatingPawnMovement;//�̵� ������ ���� Ŭ����
// Enhanced Input���� �׼� ���� ���� �� ����ϴ� ����ü
struct FInputActionValue;

UCLASS()
class PAWN_PRACTICE_API APlayer_Pawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayer_Pawn();

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
	float MoveSpeed = 500.0f;		// �⺻ �̵� �ӵ�

	//�߷� Ȯ���Ұ� ������ ����
	UPROPERTY(EditAnywhere, Category = "Movement")
	float JumpForce = 2400.0f;

	//���� üũ
	bool bIsJumping = false;
	//�߷� �� �ٴ� Ž��
	FVector Velocity;
	const float Gravity = -980.0f;
	bool bIsGrounded;

	void ApplyGravity(float DeltaTime);
	bool CheckGroundCollision();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	UFUNCTION()
	void Move(const FInputActionValue& value);
	UFUNCTION()
	void Look(const FInputActionValue& value);
	UFUNCTION()
	void StartJump(const FInputActionValue& value);
	UFUNCTION()
	void StopJump(const FInputActionValue& value);
};
