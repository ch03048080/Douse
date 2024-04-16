// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h" ///������ ��
#include "Camera/CameraComponent.h"  //ī�޶� ������Ʈ
#include "Components/CapsuleComponent.h" //ĸ�� ������Ʈ
#include "EnhancedInputComponent.h" //�Է�
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//ī�޶�
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));   //������ �� ������Ʈ �޾���
	CameraBoom->SetupAttachment(GetRootComponent());							    //��Ʈ ������Ʈ �Ʒ��� �޾���
	CameraBoom->TargetArmLength = 800.f;										    //Ÿ�� �� ���� ����
	CameraBoom->SetWorldRotation(FRotator(-50.f, 0.f, 0.f));
	// ������ ���� ȸ�� ������Ƽ�� ����� ����
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	//������ �� ���� ���� 
	//CameraBoom->bUsePawnControlRotation = true;										// ĳ������ ȸ���� ���� ������ ���� ȸ��

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));		//ī�޶� ������Ʈ �޾���
	ViewCamera->SetupAttachment(CameraBoom);									    //ī�޶� ������ �Ͽ� �޾���
	//ViewCamera->bUsePawnControlRotation = false;									// ������ ���� ȸ���� ����մϴ�.

	// ��Ʈ�ѷ��� ȸ���� ���� ������ �ϰ� ī�޶��� ȸ���� �����մϴ�.
	//bUseControllerRotationPitch = false;
	//bUseControllerRotationYaw = false;
	//bUseControllerRotationRoll = false;

	//////////////�÷��̾� �ִ� ü�� ���� ///////////////////
	PlayerMaxHealth = 30.0f;
	//�÷��̾� �ʱ� ü�� ����
	PlayerHealth = PlayerMaxHealth;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f)); // ���̷�Ż �޽� ��ġ ����
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);                           // ĸ�� ������Ʈ ��ġ ����

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("/Game/Blueprints/Enemies/SK_Forest_Creature.SK_Forest_Creature"));
	if (SM.Succeeded())
	{
		USkeletalMeshComponent* MeshComponent = GetMesh();
		if (MeshComponent) // GetMesh() �Լ��� ��ȿ�� �޽� ������Ʈ�� ��ȯ�ߴ��� Ȯ��
		{
			MeshComponent->SetSkeletalMesh(SM.Object);

			static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material0(TEXT("/Game/Forest_Creature/Materials/Summer/M_Forest_Creature.M_Forest_Creature"));
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material1(TEXT("/Game/Forest_Creature/Materials/Summer/M_Plants.M_Plants"));
			if (Material0.Succeeded()&& Material1.Succeeded())
			{
				// ��Ƽ������ ���̷�Ż �޽��� ���Կ� �Ҵ�
				MeshComponent->SetMaterial(0, Material0.Object);
				MeshComponent->SetMaterial(1, Material1.Object);
			}
			// ������ ����
			FVector Scale(0.7f, 0.7f, 0.7f); // ���ϴ� ������ ������ ����
			MeshComponent->SetRelativeScale3D(Scale);
		}
	}
	
	// �ִϸ��̼� �������Ʈ Ŭ������ ���
	FSoftObjectPath AnimInstanceClassPath = TEXT("/Game/Convert/Player/PlayerAnimation/ABP_Forest_Creature");

	// �ִϸ��̼� �������Ʈ Ŭ���� �ε�
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(AnimInstanceClassPath.TryLoad());

	if (AnimBlueprint)
	{
		// �ִϸ��̼� �ν��Ͻ� ����
		GetMesh()->SetAnimInstanceClass(AnimBlueprint->GetClass());
		//UE_LOG(LogTemp, Warning, TEXT("Anim Instance Succeed"));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed to load Anim Instance class"));
	}

	//���� �Է� - �̵�
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> InputMappingContextRef(TEXT("/Game/Convert/Player/PlayerMovement/IMC_Player.IMC_Player"));	
	if (InputMappingContextRef.Object)	
	{		
		DefaultMappingContext = InputMappingContextRef.Object;	
	}  	
	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionJumpRef(TEXT("/Game/Convert/Player/PlayerMovement/IA_Jump.IA_Jump"));	
	if (InputActionJumpRef.Object)	
	{		
		JumpAction = InputActionJumpRef.Object;	
	} 	
	static ConstructorHelpers::FObjectFinder<UInputAction> InputActionMoveRef(TEXT("/Game/Convert/Player/PlayerMovement/IA_Movement.IA_Movement"));	
	if (InputActionMoveRef.Object)	
	{		
		MoveAction = InputActionMoveRef.Object;	
	} 	

	//�浹 ó�� (ĸ�� ������Ʈ)
	UCapsuleComponent* PlayerCapsuleComponent = GetCapsuleComponent();
	// CrosshairMesh ����
	CrosshairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrosshairMesh"));
	// �θ� ������Ʈ�� ���̱� (��: RootComponent)
	CrosshairMesh->SetupAttachment(PlayerCapsuleComponent);

	// ����ƽ �޽��� �ε��Ͽ� CrosshairMesh�� �Ҵ�
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere")); //���Ǿ� ���� /Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder' �ƴϸ� �Ǹ����� �����ص� ��
	if (MeshAsset.Succeeded())
	{
		CrosshairMesh->SetStaticMesh(MeshAsset.Object);
	}

	// �޽� ������ ����
	FVector MeshScale(0.7f, 0.7f, 0.08f); // ���ϴ� ������ ������ ����
	CrosshairMesh->SetWorldScale3D(MeshScale);

	//���� ���
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprints/Player/Widgets/WBP_Player"));
	if (WidgetClassFinder.Succeeded())
	{
		PlayerWidgetClass = WidgetClassFinder.Class;
		UE_LOG(LogTemp, Warning, TEXT("Player Widget Find!"));
		// ���� �ν��Ͻ� ����
		PlayerWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerWidgetClass);
		if (PlayerWidget)
		{
			// ���
			PlayerWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("Player Widget Create!"));
		}
		
	}

	
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//�̵� (���� �Է�)
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	
	////�浹 ó�� (ĸ�� ������Ʈ)
	//UCapsuleComponent* PlayerCapsuleComponent = GetCapsuleComponent();
	//if (PlayerCapsuleComponent)// ĸ�� ������Ʈ�� ���� ������ ��ȿ�� ���
	//{
	//	//// ����: ĸ�� ������Ʈ�� ��ġ ��������
	//	//FVector CapsuleLocation = PlayerCapsuleComponent->GetComponentLocation();

	//	//// ����: ĸ�� ������Ʈ�� �浹 �ݰ� ��������
	//	//float CapsuleRadius = PlayerCapsuleComponent->GetScaledCapsuleRadius();

	//	//// ����: ĸ�� ������Ʈ�� ���� ��������
	//	//float CapsuleHeight = PlayerCapsuleComponent->GetScaledCapsuleHalfHeight();

	//	//// ���⿡ �ٸ� ĸ�� ������Ʈ�� �Ӽ��̳� ����� ����� �� �ֽ��ϴ�.
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Player Capsule Component Failed"));// ĸ�� ������Ʈ�� ���� ������ ��ȿ���� ���� ���
	//}
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>(); 	

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); 	

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);	
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); 	
	AddMovementInput(RightDirection, MovementVector.X);

}

void APlayerCharacter::Jump()
{
	Super::Jump(); //Chracter.h �� �ִ� �⺻ Jump ��� ���
	//UE_LOG(LogTemp, Warning, TEXT("Input Jump Action Triggered"));
}

void APlayerCharacter::RotateMeshTowardsCursor()
{
	// �÷��̾� ��Ʈ�ѷ� ��������
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	// �浹 ������ ���� ����
	FHitResult HitResult;

	// ���콺 Ŀ�� �Ʒ����� �浹 ����
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

	// �浹 ������ ��ȿ�� ���
	if (HitResult.IsValidBlockingHit())
	{
		// ĳ������ ��ġ�� ���콺 ��ġ�� ���̸� �̿��Ͽ� ĳ���͸� ȸ����Ŵ
		FVector DirectionToCursor = HitResult.ImpactPoint - GetActorLocation();
		DirectionToCursor.Z = 0.0f; // Z �� ȸ�� ����
		DirectionToCursor.Normalize();

		// ĳ������ ȸ���� ���
		FRotator NewRotation = DirectionToCursor.Rotation();
		SetActorRotation(NewRotation);								    
	}
	// �θ� ������Ʈ�� ���� ���������� �浹 ������ ������
	//FVector LocalHitLocation = GetTransform().InverseTransformPosition(HitResult.Location);

	//// CrosshairMesh�� ��ġ�� ����
	//CrosshairMesh->SetWorldLocation(LocalHitLocation);

	//ũ�ν� ��� �̵� �κ�
	//CrosshairMesh = FindComponentByClass<UStaticMeshComponent>();
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	//if(CrosshairMesh) UE_LOG(LogTemp, Warning, TEXT("Crosshair Settings !")); // ������� ��µǴ°� Ȯ��
	//if (CrosshairMesh && CrosshairMesh->ComponentHasTag("Crosshair"))
	

	//ũ�ν� ��� ���
	if (CrosshairMesh)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Crosshair Find !"));
		if (HitResult.IsValidBlockingHit())// �浹 ������ ��ȿ�� ���
		{
			// CrosshairMesh�� ��ġ�� ����
			CrosshairMesh->SetRelativeLocation(GetTransform().InverseTransformPosition(HitResult.Location));
			//CrosshairMesh->SetWorldLocation(HitResult.Location);// Sphere �޽��� ��ġ�� ���콺 Ŀ�� �Ʒ��� �浹 �������� ����
			//UE_LOG(LogTemp, Warning, TEXT("Crosshair Location !"));
		}
	
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateMeshTowardsCursor();
	//SetSphereMeshLocationUnderCursor();
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Jump);
	}

}

void APlayerCharacter::GetHurtAndUpdateHealth(float DamageAmount)
{
	PlayerHealth -= DamageAmount;
	float HealthPercentage = PlayerMaxHealth / PlayerHealth;

	//PlayerWidgetClass->(HealthPercentage);
	UE_LOG(LogTemp, Warning, TEXT("Player Hurt!"));

	// Health Percentage ���
	
	
	//}
	if (PlayerHealth <= 0) //�÷��̾� ��� ��
	{
	
		UE_LOG(LogTemp, Warning, TEXT("Player Widjet!!!"));
		
	}
	

}

void APlayerCharacter::LoadGameOver(float HealthPercentage)
{

	UE_LOG(LogTemp, Warning, TEXT("Load GameOver!"));
}

//void APlayerCharacter::UpdateHealthPercentage(float NewPercentage)
//{
//}
//
//void APlayerCharacter::UpdateXPPercentage(float NewPercentage)
//{
//}
//
//void APlayerCharacter::IncreaseKillCount()
//{
//}
//
//void APlayerCharacter::IncreaseLevel()
//{
//}
