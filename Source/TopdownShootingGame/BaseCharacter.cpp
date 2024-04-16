// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
//#include "EnhancedInputComponent.h"
//#include "EnhancedInputSubsystems.h"
//#include "InputMappingContext.h"


// Sets default values
ABaseCharacter::ABaseCharacter()
{
	//static ConstructorHelpers::FObjectFinder<UInputMappingContext>DEFAULT_CONTEXT //생성자
	//(TEXT("/Game/Blueprints/Player/IMC_Player"));
	//if (DEFAULT_CONTEXT.Succeeded())
	//{
	//	DefaultContext = DEFAULT_CONTEXT.Object;
	//}

	//static ConstructorHelpers::FObjectFinder<UInputAction>IA_MOVE
	//(TEXT("/Game/Blueprints/Player/IA_Movement"));
	//if (IA_MOVE.Succeeded())
	//{
	//	MovementAction = IA_MOVE.Object;
	//}
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	//if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	//{
	//	if (UEnhancedInputLocalPlayerSubsystem* SubSystem =
	//		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	//		SubSystem->AddMappingContext(DefaultContext, 0);
	//}

	//APlayerController* PlayerController = Cast<APlayerController>(Controller);
	//if (PlayerController != nullptr)
	//{
	//	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem< UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	//	if (Subsystem != nullptr)
	//	{
	//		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	//	}
	//}
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	/*Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);
	}*/

}

//void ABaseCharacter::Move(const FInputActionInstance& Value)
//{
	//if (Value.Get<bool>())
	//	UE_LOG(LogTemp, Warning, TEXT("Input Action Triggered"));

	//FVector2D MovementVector = Instance.GetValue().Get<FVector2D>();

	//if (Controller != nullptr)
	//{
	//	const FRotator Rotation = Controller->GetControlRotation();
	//	const FRotator YawRotation(0, Rotation.Yaw, 0);
	//	const FVector FowardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	//	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	//	AddMovementInput(FowardDirection, MovementVector.Y);
	//	AddMovementInput(RightDirection, MovementVector.X);
	//}
//}

//void ABaseCharacter::Look(const FInputActionInstance& Instance)
//{
//	FVector2D LookVector = Instance.GetValue().Get<FVector2D>();
//
//	if (Controller != nullptr)
//	{
//		AddControllerYawInput(LookVector.X);
//		AddControllerPitchInput(LookVector.Y);
//	}
//}

// Called to bind functionality to input
//EnhancedInputComponent로 캐스트해주는 작업
//void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) 
//{
//	Super::SetupPlayerInputComponent(PlayerInputComponent);
//	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
//	{
//		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABaseCharacter::Move);
//	}
//}
//
//void ATank::Move(const FInputActionValue& Value)
//{
//	if (Value.Get<bool>())
//		UE_LOG(LogTemp, Warning, TEXT("Input Action Triggered"));
//}