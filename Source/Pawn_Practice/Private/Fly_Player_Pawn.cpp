// Fill out your copyright notice in the Description page of Project Settings.


#include "Fly_Player_Pawn.h"
#include "FlyPlayerController.h"
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
AFly_Player_Pawn::AFly_Player_Pawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

    // 루트 컴포넌트를 캡슐 컴포넌트로 설정
    CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
    RootComponent = CapsuleComp;
    CapsuleComp->InitCapsuleSize(50.0f, 50.0f); // 크기 설정
    CapsuleComp->SetCollisionObjectType(ECC_Pawn);
    CapsuleComp->SetCollisionResponseToAllChannels(ECR_Block);
    CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);  // 물리적 충돌 없이 Query만 가능

    // 스켈레탈 메시 추가 (RootComponent에 Attach)
    MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
    MeshComponent->SetupAttachment(CapsuleComp);  // 캡슐에 부착
    // 캐릭터가 땅을 바라보도록 Y축 회전 (캐릭터 모델이 Z축을 따라 정렬되도록)
    MeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, -6.0f));  // 캡슐 아래 정렬
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

    // Enhanced InputComponent로 캐스팅
    if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        // IA를 가져오기 위해 현재 소유 중인 Controller를 AFlyPlayerController로 캐스팅
        if (AFlyPlayerController* PlayerController = Cast<AFlyPlayerController>(GetController()))
        {
            if (PlayerController->MoveAction)
            {
                // IA_Move 액션 키를 "키를 누르고 있는 동안" Move() 호출
                EnhancedInput->BindAction(PlayerController->MoveAction, ETriggerEvent::Triggered, this, &AFly_Player_Pawn::Move);
            }

            if (PlayerController->LookAction)
            {
                // IA_Look 액션 마우스가 "움직일 때" Look() 호출
                EnhancedInput->BindAction(PlayerController->LookAction, ETriggerEvent::Triggered, this, &AFly_Player_Pawn::Look);
            }

            if (PlayerController->RotateAction)
            {
                // IA_Rotate 액션 Q, E 버튼을 누르면 왼쪽 오른쪽 회전 Rotate() 호출
                EnhancedInput->BindAction(PlayerController->RotateAction, ETriggerEvent::Triggered, this, &AFly_Player_Pawn::Rotate);
            }

            if (PlayerController->UpDownAction)
            {
                // IA_UpDown 액션 Space Bar, Shift 누르면 위로 올라가고 아래로 내려감 UpDown() 호출
                EnhancedInput->BindAction(PlayerController->UpDownAction, ETriggerEvent::Triggered, this, &AFly_Player_Pawn::UpDown);
            }
        }
    }

    // 컨트롤러 회전 활성화
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

    //바닥이 아니면(공중일때 기준 바닥 통과 X) 속도 감소
    float CurrentMoveSpeed = IsOnGround() ? MoveSpeed : MoveSpeed * 0.5f;

    if (!FMath::IsNearlyZero(MoveInput.X))
    {
        // 폰의 정면을 기준으로 전진/후진
        FVector Direction = GetActorForwardVector();
        MovementComponent->AddInputVector(Direction * MoveInput.X * CurrentMoveSpeed);
    }

    if (!FMath::IsNearlyZero(MoveInput.Y))
    {
        // 폰의 오른쪽 방향을 기준으로 좌우 이동
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
        // 현재 Pawn의 회전값을 가져옴
        FRotator NewRotation = GetActorRotation();

        // Yaw 값을 입력값에 따라 변경
        NewRotation.Yaw += RotateInput * RotateSpeed;

        // Pawn 자체의 회전 적용
        SetActorRotation(NewRotation);
    }
}

void AFly_Player_Pawn::UpDown(const FInputActionValue& value)
{
    if (!MovementComponent) return;

    float UpDownInput = value.Get<float>();

    // 입력이랑 바닥에 닿았는지 체크
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

