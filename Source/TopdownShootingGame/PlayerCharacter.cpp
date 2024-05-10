// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h" ///������ ��
#include "Camera/CameraComponent.h"  //ī�޶� ������Ʈ
#include "Components/CapsuleComponent.h" //ĸ�� ������Ʈ
#include "EnhancedInputComponent.h" //�Է�
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "MyGameModeBase.h"

#include "Misc/OutputDeviceNull.h" // ���� ����� �������� �ʰ� �α� �޽��� ���� -> ���� ������Ʈ �� ���
//#include "Engine/GameInstance.h" // UGameInstance ��� ���� -> ���� ���� ��峪 ���� �ν��Ͻ� Ŭ������ �̵��ؾ��ϴ� ��� -> SetGamePaused().
#include "Kismet/GameplayStatics.h" // ���� ���� ��� ����ϱ� ���� �Լ�
#include "Components/SceneComponent.h" //�� ������Ʈ�� �����ؼ� ��ų ��� �� �Ǻ� ��ġ�� �޾ƿ��� ���� �Լ�


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
	PlayerMaxHealth = 50.0f;
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
			//WBP_Player ���� �г��� WBP_DeathScreen ����
			DeathScreenWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_DeathScreen")));
			//WBP_Player ���� �г��� WBP_Leveling ����
			LevelingWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_Leveling")));
			//WBP_Player ���� �г��� WBP_TopRightHUD ����
			TopRightHUDWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_TopRightHUD")));
		}
	}
	//Set timer by event
	// FTimerManager& AActor::GetWorldTimerManager() const �Լ� ��뿹) GetWorldTimerManager().SetTimer(...);
	

	//if (!DeathScreenWidget)// ���� �гο� ���� �׼����� �������� ���� ó��
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Failed to access DeathScreenWidget!"));
	//}
	//static ConstructorHelpers::FClassFinder<UUserWidget> DeathScreenWidgetClassFinder(TEXT("/Game/Blueprints/Player/Widgets/WBP_Death_Screen")); 
	//if (DeathScreenWidgetClassFinder.Succeeded())
	//{
	//	DeathScreenWidgetClass = DeathScreenWidgetClassFinder.Class;
	//	UE_LOG(LogTemp, Warning, TEXT("Death Screen Widget Find!"));
	//	// ���� �ν��Ͻ� ����
	//	DeathScreenWidget = CreateWidget<UUserWidget>(GetWorld(), DeathScreenWidgetClass);
	//	if (DeathScreenWidget)
	//	{
	//		// ���
	//		DeathScreenWidget->AddToViewport();
	//		//UE_LOG(LogTemp, Warning, TEXT("Death Screen Widget Create!"));
	//	}
	//}
	
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

void APlayerCharacter::StartSelectingSkills()
{
	if (GetWorld() != nullptr)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true); // ���� ��� �۵�

		if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) //���콺 Ŀ�� Ȱ��ȭ
		{
			if (PlayerController != nullptr)
			{
				// ���콺 Ŀ���� ���ü� True�� ����
				PlayerController->bShowMouseCursor = true;

				//Set Movement Mode ����

				//WBP_Leveling ���� ���̰Բ� Visivle ����
				LevelingWidget->SetVisibility(ESlateVisibility::Visible);

				//SetInputModeUIOnly ��� ����
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(LevelingWidget->TakeWidget());
				//���콺�� ����Ʈ�� �����Ұ��� �����Ӱ� ���Ѱ��� ���� ���
				//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PlayerController->SetInputMode(InputMode);

			}
		}
		
	}



}

void APlayerCharacter::ReturnToGame()
{
	if (GetWorld() != nullptr)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false); // ���� ��� ����

		if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) //���콺 Ŀ�� ��Ȱ��ȭ
		{
			if (PlayerController != nullptr)
			{
				// ���콺 Ŀ���� ���ü� false�� ����
				PlayerController->bShowMouseCursor = false;

				//Set Movement Mode ���� ??

				//WBP_Leveling ���� �� ���̰Բ� Hidden ����
				LevelingWidget->SetVisibility(ESlateVisibility::Hidden);

				//SetInputModeGameOnly ��� ����
				FInputModeGameOnly InputMode;
				PlayerController->SetInputMode(InputMode);
				//InputMode.SetWidgetToFocus(LevelingWidget->TakeWidget());
				//���콺�� ����Ʈ�� �����Ұ��� �����Ӱ� ���Ѱ��� ���� ���
				//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			}
		}

	}
}

void APlayerCharacter::TopRightHUD_1()
{
	//TopRightHUDWidget �� Update_1 Ŀ���� �̺�Ʈ ȣ��
	FOutputDeviceNull pAR;
	TopRightHUDWidget->CallFunctionByNameWithArguments(*FString::Printf(TEXT("Update_1 %f"), SkillLevel_1), pAR, nullptr, true);
	//������Ʈ �ȵ�
}
void APlayerCharacter::TopRightHUD_2()
{
	FOutputDeviceNull pAR;
	TopRightHUDWidget->CallFunctionByNameWithArguments(*FString::Printf(TEXT("Update_2 %f"), SkillLevel_2), pAR, nullptr, true);
}

void APlayerCharacter::TopRightHUD_3()
{
	FOutputDeviceNull pAR;
	TopRightHUDWidget->CallFunctionByNameWithArguments(*FString::Printf(TEXT("Update_3 %f"), SkillLevel_3), pAR, nullptr, true);
}

void APlayerCharacter::TopRightHUD_4()
{
	FOutputDeviceNull pAR;
	TopRightHUDWidget->CallFunctionByNameWithArguments(*FString::Printf(TEXT("Update_4 %f"), SkillLevel_4), pAR, nullptr, true);
}

void APlayerCharacter::skill_1() //��ų1 ��� ���� 
{
	switch (SkillLevel_1)
	{
	case 1:
		SpawnSkill_1(1, 0.0, 0.0);
		break;
	case 2:
		SpawnSkill_1(2, -10.0, 20.0);
		break;
	case 3:
		SpawnSkill_1(3, -15.0, 15.0);
		break;
	case 4:
		SpawnSkill_1(4, -15.0, 10.0);
		break;
	case 5:
		SpawnSkill_1(5, -20.0, 10.0);
		break;
	}
}

void APlayerCharacter::skill_2()//��ų 2 ��� ���� 
{
	switch (SkillLevel_2)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;

	}
}

void APlayerCharacter::skill_3()//��ų3 ��� ���� 
{
	switch (SkillLevel_3)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;

	}
}

void APlayerCharacter::skill_4()//��ų4 ��� ���� 
{
	switch (SkillLevel_4)
	{
	case 1:
		break;
	case 2:
		break;
	case 3:
		break;
	case 4:
		break;

	}
}

void APlayerCharacter::SpawnSkill_1(int NumProjectile, float Rotation, float RotIncrement)
{
	//USceneComponent* Skill1PivotComponent = GetOwner()->GetComponentByClass<USceneComponent>(TEXT("Skill1Pivot"));
	UCapsuleComponent* PlayerCapsuleComponent = GetCapsuleComponent();
	USceneComponent* Skill1PivotComponent = PlayerCapsuleComponent->GetChildComponent(0); // ĸ�� ������Ʈ�� ù ��° �ڽ� ������Ʈ�� �����ɴϴ�.
	USceneComponent* Skill1SpawnComponent = Skill1PivotComponent->GetChildComponent(0); // FireBallPivot ������Ʈ ������ FireBall Spawn ������Ʈ �޾ƿ���
	FQuat NewRotationQuat(FRotator(0.f, Rotation, 0.f));//�Լ� ȣ�� �� ������ Rot ���� �����Ͽ� ����

	if(Skill1PivotComponent != nullptr)
	{
		Skill1PivotComponent->SetRelativeRotation(NewRotationQuat);

		FVector SpawnTransformLocation = Skill1SpawnComponent->GetComponentLocation();
		FRotator SpawnTransformRotation = Skill1SpawnComponent->GetComponentRotation();

		FString Skill1ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Fire.Spell_Fire_C"; //Skill1 �� ���� Ŭ���� ��� ����

		UClass* Skill1ActorClass = LoadClass<AActor>(nullptr, *Skill1ActorClassPath);

		for (int32 i = 0; i < NumProjectile; ++i)
		{
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Skill1ActorClass, SpawnTransformLocation, SpawnTransformRotation);
		}

		Skill1PivotComponent->AddRelativeRotation(NewRotationQuat);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill1PivotComponent !="));
	}
}

void APlayerCharacter::SpawnSkill_2(int NumProjectile, float Rotation, float RotIncrement)
{
}

void APlayerCharacter::SpawnSkill_3(int NumProjectile, float Rotation)
{
}

void APlayerCharacter::SpawnSkill_4()
{
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
	PlayerHealthPercentage = PlayerMaxHealth / PlayerHealth;

	//PlayerWidgetClass->(HealthPercentage);
	UE_LOG(LogTemp, Warning, TEXT("Player Hurt!"));

	//WBP_Player �� UpdateHealthPercentage Ŀ���� �̺�Ʈ ȣ��
	FOutputDeviceNull pAR;
	PlayerHealthPercentage = PlayerHealth / PlayerMaxHealth;
	PlayerWidget->CallFunctionByNameWithArguments(*FString::Printf(TEXT("UpdateHealthPercentage %f"), PlayerHealthPercentage), pAR, nullptr, true);
	//HP �� ������Ʈ �Ǵ� �� Ȯ��

	if (PlayerHealth <= 0) //�÷��̾� ��� ��
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Dead!"));

		if (GetWorld() != nullptr && GetWorld()->GetGameViewport() != nullptr)
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true); // ���� ��� �۵�
			DeathScreenWidget->SetVisibility(ESlateVisibility::Visible); //�ش� ���� �������Ʈ�� ��������Ƽ�� ���̵��� ����
			
			if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) //���콺 Ŀ�� Ȱ��ȭ
			{
				if (PlayerController != nullptr)
				{
					// ���콺 Ŀ���� ���ü� True�� ����
					PlayerController->bShowMouseCursor = true;
				}
			}

		}

	}
	

}

void APlayerCharacter::AddCoinExpericence(float PickupValue)
{
	CurrentExperience += PickupValue;

	InPercent = CurrentExperience / ExpericenCap;

	FOutputDeviceNull pAR;
	PlayerHealthPercentage = PlayerHealth / PlayerMaxHealth;
	PlayerWidget->CallFunctionByNameWithArguments
	(*FString::Printf(TEXT("UpdateXPPercentage %f"), InPercent), pAR, nullptr, true);

	if (CurrentExperience >= ExpericenCap)
	{
		PlayerHealthPercentage = PlayerHealth / PlayerMaxHealth;
		PlayerWidget->CallFunctionByNameWithArguments
		(*FString::Printf(TEXT("UpdateXPPercentage %f"), 0.0), pAR, nullptr, true);
		CharacterLevel++;
		CurrentExperience = 0;
	}
	StartSelectingSkills();
	PlayerWidget->CallFunctionByNameWithArguments
	(*FString::Printf(TEXT("IncreaseLevel %f"), CharacterLevel), pAR, nullptr, true);
}

//void APlayerCharacter::LoadGameOver(float HealthPercentage)
//{
//
//	UE_LOG(LogTemp, Warning, TEXT("Load GameOver!"));
//}

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
