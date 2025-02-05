// Fill out your copyright notice in the Description page of Project Settings.


#include "Fly_Player_Pawn.h"
#include "FlyPlayerController.h"
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
AFly_Player_Pawn::AFly_Player_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // ��Ʈ ������Ʈ�� ĸ�� ������Ʈ�� ����
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    RootComponent = CapsuleComp;
    CapsuleComp->InitCapsuleSize(50.0f, 50.0f); // ũ�� ����
    CapsuleComp->SetCollisionObjectType(ECC_Pawn);
    CapsuleComp->SetCollisionResponseToAllChannels(ECR_Block);
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // ������ �浹 ���� Query�� ����

    // ���̷�Ż �޽� �߰� (RootComponent�� Attach)
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CapsuleComp);  // ĸ���� ����
    // ĳ���Ͱ� ���� �ٶ󺸵��� Y�� ȸ�� (ĳ���� ���� Z���� ���� ���ĵǵ���)
    MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -6.0f));  // ĸ�� �Ʒ� ����
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

// Called when the game starts or when spawned
void AFly_Player_Pawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AFly_Player_Pawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AFly_Player_Pawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Enhanced InputComponent�� ĳ����
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // IA�� �������� ���� ���� ���� ���� Controller�� AFlyPlayerController�� ĳ����
        if (AFlyPlayerController* PlayerController = Cast<AFlyPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                // IA_Move �׼� Ű�� "Ű�� ������ �ִ� ����" Move() ȣ��
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AFly_Player_Pawn::Move);
            }

            if (PlayerController->LookAction)
            {
                // IA_Look �׼� ���콺�� "������ ��" Look() ȣ��
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AFly_Player_Pawn::Look);
            }

            if (PlayerController->RotateAction)
            {
                // IA_Rotate �׼� Q, E ��ư�� ������ ���� ������ ȸ�� Rotate() ȣ��
                EnhancedInput->BindAction(PlayerController->RotateAction, ETriggerEvent::Triggered, this, &AFly_Player_Pawn::Rotate);
            }

            if (PlayerController->UpDownAction)
            {
                // IA_UpDown �׼� Space Bar, Shift ������ ���� �ö󰡰� �Ʒ��� ������ UpDown() ȣ��
                EnhancedInput->BindAction(PlayerController->UpDownAction, ETriggerEvent::Triggered, this, &AFly_Player_Pawn::UpDown);
            }
        }
    }

    // ��Ʈ�ѷ� ȸ�� Ȱ��ȭ
    bUseControllerRotationYaw = false;
}

bool AFly_Player_Pawn::IsOnGround()
{
    FVector Start = GetActorLocation();
    float TraceDistance = CapsuleComp ? CapsuleComp->GetScaledCapsuleHalfHeight() + 10.f : 50.f;
    FVector End = Start + FVector(0.f, 0.f, -TraceDistance);

    FHitResult HitResult;
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(this);

    return GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, CollisionParams);
}

void AFly_Player_Pawn::Move(const FInputActionValue& value)
{
    if (!Controller || !MovementComponent) return;

    FVector2D MoveInput = value.Get<FVector2D>();

    //�ٴ��� �ƴϸ�(�����϶� ���� �ٴ� ��� X) �ӵ� ����
    float CurrentMoveSpeed = IsOnGround() ? MoveSpeed : MoveSpeed * 0.5f;

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        // ���� ������ �������� ����/����
        FVector Direction = GetActorForwardVector();
        MovementComponent->AddInputVector(Direction * MoveInput.X * CurrentMoveSpeed);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        // ���� ������ ������ �������� �¿� �̵�
        FVector Direction = GetActorRightVector();
        MovementComponent->AddInputVector(Direction * MoveInput.Y * CurrentMoveSpeed);
    }
}

void AFly_Player_Pawn::Look(const FInputActionValue& value)
{
    FVector2D LookInput = value.Get<FVector2D>();

    if (!FMath::IsNearlyZero(LookInput.X))
    {
        AddControllerYawInput(LookInput.X);
    }

    if (!FMath::IsNearlyZero(LookInput.Y))
    {
        AddControllerPitchInput(LookInput.Y);
    }
}

void AFly_Player_Pawn::Rotate(const FInputActionValue& value)
{
    float RotateInput = value.Get<float>();

    if (!FMath::IsNearlyZero(RotateInput))
    {
        // ���� Pawn�� ȸ������ ������
        FRotator NewRotation = GetActorRotation();

        // Yaw ���� �Է°��� ���� ����
        NewRotation.Yaw += RotateInput * RotateSpeed;

        // Pawn ��ü�� ȸ�� ����
        SetActorRotation(NewRotation);
    }
}

void AFly_Player_Pawn::UpDown(const FInputActionValue& value)
{
    if (!MovementComponent) return;

    float UpDownInput = value.Get<float>();

    // �Է��̶� �ٴڿ� ��Ҵ��� üũ
    if (UpDownInput < 0 && IsOnGround())
    {
        return;
    }

    if (!FMath::IsNearlyZero(UpDownInput))
    {
        FVector Direction = FVector(0.f, 0.f, 1.f);
        FVector MovementVector = Direction * UpDownInput * MoveSpeed * GetWorld()->GetDeltaSeconds();
        SetActorLocation(GetActorLocation() + MovementVector, true);
    }
}

