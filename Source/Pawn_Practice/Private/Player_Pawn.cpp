// Fill out your copyright notice in the Description page of Project Settings.


#include "Player_Pawn.h"
#include "PawnPlayerController.h"
// 카메라, 스프링 암 include
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

    // 루트 컴포넌트를 캡슐 컴포넌트로 설정
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    RootComponent = CapsuleComp;
    CapsuleComp->InitCapsuleSize(42.0f, 96.0f); // 크기 설정
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CapsuleComp->SetCollisionObjectType(ECC_Pawn);
    CapsuleComp->SetCollisionResponseToAllChannels(ECR_Block);

    // 스켈레탈 메시 추가 (RootComponent에 Attach)
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CapsuleComp);  // 캡슐에 부착
    // 캐릭터가 땅을 바라보도록 Y축 회전 (캐릭터 모델이 Z축을 따라 정렬되도록)
    MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -96.0f));  // 캡슐 아래 정렬
    MeshComponent->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));  // 캐릭터가 정면을 바라보도록 회전

    // 스프링 암 생성 및 설정
    SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArmComp->SetupAttachment(CapsuleComp); // 캡슐 컴포넌트에 부착
    SpringArmComp->TargetArmLength = 300.0f;       // 카메라 거리 설정
    SpringArmComp->bUsePawnControlRotation = true; // 컨트롤 회전 적용

    // 카메라 생성 및 설정
    CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    CameraComp->SetupAttachment(SpringArmComp, USpringArmComponent::SocketName);
    CameraComp->bUsePawnControlRotation = false; // 카메라 자체 회전 방지

    // 폰이 플레이어 입력을 받을 수 있도록 설정
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    //이동 컴포넌트
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

    // Enhanced InputComponent로 캐스팅
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // IA를 가져오기 위해 현재 소유 중인 Controller를 APawnPlayerController로 캐스팅
        if (APawnPlayerController* PlayerController = Cast<APawnPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                // IA_Move 액션 키를 "키를 누르고 있는 동안" Move() 호출
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &APlayer_Pawn::Move);
            }

            if (PlayerController->LookAction)
            {
                // IA_Look 액션 마우스가 "움직일 때" Look() 호출
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &APlayer_Pawn::Look);
            }

            if (PlayerController->JumpAction)
            {
                //IA_Jump 액션 키를 누르고있는동안 StartJump() 호출
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Triggered, this, &APlayer_Pawn::StartJump);

                //IA_Jump 액션 키를 때면은 StopJump() 호출
                EnhancedInput->BindAction(PlayerController->JumpAction, ETriggerEvent::Completed, this, &APlayer_Pawn::StopJump);
            }
        }
    }

    // 컨트롤러 회전 활성화
    bUseControllerRotationYaw = true;
}

//이동
void APlayer_Pawn::Move(const FInputActionValue& value)
{
    if (!Controller || !MovementComponent) return;

    FVector2D MoveInput = value.Get<FVector2D>();

    if (APlayerController* PC = Cast<APlayerController>(Controller))
    {
        // 현재 컨트롤러의 회전 값 가져오기
        FRotator ControlRotation = PC->GetControlRotation();
        FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f); // Pitch와 Roll을 제거하여 Yaw만 사용

        if (!FMath::IsNearlyZero(MoveInput.X))
        {
            // 카메라 방향을 기준으로 전진/후진 방향 설정
            FVector Direction = GetActorForwardVector();
            MovementComponent->AddInputVector(Direction * MoveInput.X * MoveSpeed);
        }

        if (!FMath::IsNearlyZero(MoveInput.Y))
        {
            // 카메라 방향을 기준으로 좌우 이동 방향 설정
            FVector Direction = GetActorRightVector();
            MovementComponent->AddInputVector(Direction * MoveInput.Y * MoveSpeed);
        }
    }
}

//시점 이동
void APlayer_Pawn::Look(const FInputActionValue& value)
{
    // 마우스의 X, Y 움직임을 2D 축으로 가져옴
    FVector2D LookInput = value.Get<FVector2D>();

    // X는 좌우 회전 (Yaw), Y는 상하 회전 (Pitch)
    // 좌우 회전
    AddControllerYawInput(LookInput.X);
    // 상하 회전
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

