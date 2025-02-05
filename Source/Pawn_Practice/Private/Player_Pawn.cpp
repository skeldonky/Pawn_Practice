// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Pawn.h"
#include "PawnPlayerController.h"
// ī�޶�, ������ �� include
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
// Enhanced Input
#include "EnhancedInputComponent.h"
// FloatingPawnMovement
#include "GameFramework/FloatingPawnMovement.h"

// Sets default values
APlayer_Pawn::APlayer_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // ��Ʈ ������Ʈ�� ĸ�� ������Ʈ�� ����
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    RootComponent = CapsuleComp;
    CapsuleComp->InitCapsuleSize(42.0f, 96.0f); // ũ�� ����
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComp->SetCollisionObjectType(ECC_Pawn);
    CapsuleComp->SetCollisionResponseToAllChannels(ECR_Block);

    // ���̷�Ż �޽� �߰� (RootComponent�� Attach)
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CapsuleComp);  // ĸ���� ����
    // ĳ���Ͱ� ���� �ٶ󺸵��� Y�� ȸ�� (ĳ���� ���� Z���� ���� ���ĵǵ���)
    MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));  // ĸ�� �Ʒ� ����
    MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // ĳ���Ͱ� ������ �ٶ󺸵��� ȸ��

    // ������ �� ���� �� ����
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(CapsuleComp); // ĸ�� ������Ʈ�� ����
    SpringArmComp->TargetArmLength = 300.0f;       // ī�޶� �Ÿ� ����
    SpringArmComp->bUsePawnControlRotation = true; // ��Ʈ�� ȸ�� ����

    // ī�޶� ���� �� ����
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false; // ī�޶� ��ü ȸ�� ����

    // ���� �÷��̾� �Է��� ���� �� �ֵ��� ����
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    //�̵� ������Ʈ
    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
}

void APlayer_Pawn::ApplyGravity(float DeltaTime)
{
    if (!bIsGrounded)
    {
        Velocity.Z += Gravity * DeltaTime;
        FVector NewLocation = GetActorLocation() + Velocity * DeltaTime;

        FHitResult Hit;
        SetActorLocation(NewLocation, true, &Hit);
        if (Hit.IsValidBlockingHit())
        {
            bIsGrounded = true;
            Velocity.Z = 0.0f;
        }
    }
    else
    {
        bIsGrounded = CheckGroundCollision();
    }
}

bool APlayer_Pawn::CheckGroundCollision()
{
    FHitResult Hit;
    FVector Start = GetActorLocation();
    FVector End = Start + FVector(0.0f, 0.0f, -5.0f);
    FCollisionQueryParams Params;
    Params.AddIgnoredActor(this);

    bool bHit = GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params);
    return bHit;
}

// Called when the game starts or when spawned
void APlayer_Pawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APlayer_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

    ApplyGravity(DeltaTime);
}

// Called to bind functionality to input
void APlayer_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced InputComponent�� ĳ����
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // IA�� �������� ���� ���� ���� ���� Controller�� APawnPlayerController�� ĳ����
        if (APawnPlayerController* PlayerController = Cast<APawnPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                // IA_Move �׼� Ű�� "Ű�� ������ �ִ� ����" Move() ȣ��
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &APlayer_Pawn::Move);
            }

            if (PlayerController->LookAction)
            {
                // IA_Look �׼� ���콺�� "������ ��" Look() ȣ��
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &APlayer_Pawn::Look);
            }

            if (PlayerController->JumpAction)
            {
                //IA_Jump �׼� Ű�� �������ִµ��� StartJump() ȣ��
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &APlayer_Pawn::StartJump);

                //IA_Jump �׼� Ű�� ������ StopJump() ȣ��
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &APlayer_Pawn::StopJump);
            }
        }
    }

    // ��Ʈ�ѷ� ȸ�� Ȱ��ȭ
    bUseControllerRotationYaw = true;
}

//�̵�
void APlayer_Pawn::Move(const FInputActionValue& value)
{
    if (!Controller || !MovementComponent) return;

    FVector2D MoveInput = value.Get<FVector2D>();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        // ���� ��Ʈ�ѷ��� ȸ�� �� ��������
        FRotator ControlRotation = PC->GetControlRotation();
        FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f); // Pitch�� Roll�� �����Ͽ� Yaw�� ���

        if (!FMath::IsNearlyZero(MoveInput.X))
        {
            // ī�޶� ������ �������� ����/���� ���� ����
            FVector Direction = GetActorForwardVector();
            MovementComponent->AddInputVector(Direction * MoveInput.X * MoveSpeed);
        }

        if (!FMath::IsNearlyZero(MoveInput.Y))
        {
            // ī�޶� ������ �������� �¿� �̵� ���� ����
            FVector Direction = GetActorRightVector();
            MovementComponent->AddInputVector(Direction * MoveInput.Y * MoveSpeed);
        }
    }
}

//���� �̵�
void APlayer_Pawn::Look(const FInputActionValue& value)
{
    // ���콺�� X, Y �������� 2D ������ ������
    FVector2D LookInput = value.Get<FVector2D>();

    // X�� �¿� ȸ�� (Yaw), Y�� ���� ȸ�� (Pitch)
    // �¿� ȸ��
    AddControllerYawInput(LookInput.X);
    // ���� ȸ��
    AddControllerPitchInput(LookInput.Y);
}

void APlayer_Pawn::StartJump(const FInputActionValue& value)
{
    if (!bIsJumping)
    {
        FVector JumpVelocity = FVector(0.0f, 0.0f, JumpForce);
        MovementComponent->Velocity = JumpVelocity;
        bIsJumping = true;
    }
}

void APlayer_Pawn::StopJump(const FInputActionValue& value)
{
    bIsJumping = false;
}

