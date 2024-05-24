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
#include "Math/UnrealMathUtility.h"

#include "Misc/OutputDeviceNull.h" // ���� ����� �������� �ʰ� �α� �޽��� ���� -> ���� ������Ʈ �� ���
//#include "Engine/GameInstance.h" // UGameInstance ��� ���� -> ���� ���� ��峪 ���� �ν��Ͻ� Ŭ������ �̵��ؾ��ϴ� ��� -> SetGamePaused().
#include "Kismet/GameplayStatics.h" // ���� ���� ��� ����ϱ� ���� �Լ�
#include "Components/SceneComponent.h" //�� ������Ʈ�� �����ؼ� ��ų ��� �� �Ǻ� ��ġ�� �޾ƿ��� ���� �Լ�
#include "TimerManager.h" // Ÿ�̸� - ���� ���� �� ���Ǵ� Ÿ�̸� �Լ�
#include "EngineUtils.h" // ���忡 ������ ���� ã�� �� ���

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//���� �޾ƿ���
	//World = GetWorld();

	//ī�޶�
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));   //������ �� ������Ʈ �޾���
	CameraBoom->SetupAttachment(GetRootComponent());							    //��Ʈ ������Ʈ �Ʒ��� �޾���
	CameraBoom->TargetArmLength = 2500.f;										    //Ÿ�� �� ���� ����
	CameraBoom->SetWorldRotation(FRotator(-50.f, 0.f, 0.f));
	// ������ ���� ȸ�� ������Ƽ�� ����� ����
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	//������ �� ���� ���� 
	//CameraBoom->bUsePawnControlRotation = true;								    // ĳ������ ȸ���� ���� ������ ���� ȸ��

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));		//ī�޶� ������Ʈ �޾���
	ViewCamera->SetupAttachment(CameraBoom);									    //ī�޶� ������ �Ͽ� �޾���
	//ViewCamera->bUsePawnControlRotation = false;									// ������ ���� ȸ���� ����մϴ�.

	//�÷��̾� �ִ� ü�� ����
	PlayerMaxHealth = 50.0f;
	//�÷��̾� �ʱ�(�⺻) ü�� ����
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
	
	// �ִϸ��̼� �������Ʈ Ŭ���� ��� ����
	FSoftObjectPath AnimInstanceClassPath = TEXT("/Game/Convert/Player/PlayerAnimation/ABP_Forest_Creature");

	// ������ ����� �ִϸ��̼� �������Ʈ Ŭ���� �ε�
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(AnimInstanceClassPath.TryLoad());
	if (AnimBlueprint) //��ȿ�� �˻�
	{
		GetMesh()->SetAnimInstanceClass(AnimBlueprint->GetClass());// �ִϸ��̼� �ν��Ͻ� ����
	}
	else //AnimBlueprint�� ���������� �ε���� �ʾ��� ��
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed to load Anim Instance class"));
	}

	//Enhanced Input ĳ���� �̵�

	// Enhanced Input�� ����ϱ� ���� �ʿ��� �Է� ���� ���ؽ�Ʈ �ε�
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> 
		InputMappingContextRef(TEXT("/Game/Convert/Player/PlayerMovement/IMC_Player.IMC_Player"));	
	if (InputMappingContextRef.Object)	
	{		
		DefaultMappingContext = InputMappingContextRef.Object;	
	}  	
	
	// ���� �׼��� ���� Enhanced Input �Է� �׼� �ε�
	static ConstructorHelpers::FObjectFinder<UInputAction> 
		InputActionJumpRef(TEXT("/Game/Convert/Player/PlayerMovement/IA_Jump.IA_Jump"));	
	if (InputActionJumpRef.Object)	
	{		
		JumpAction = InputActionJumpRef.Object;	
	} 	

	// �̵� �׼��� ���� Enhanced Input �Է� �׼� �ε�
	static ConstructorHelpers::FObjectFinder<UInputAction> 
		InputActionMoveRef(TEXT("/Game/Convert/Player/PlayerMovement/IA_Movement.IA_Movement"));	
	if (InputActionMoveRef.Object)	
	{		
		MoveAction = InputActionMoveRef.Object;	
	} 	

	//�浹 ó�� (ĸ�� ������Ʈ)
	PlayerCapsuleComponent = GetCapsuleComponent();


	// CrosshairMesh ����
	CrosshairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrosshairMesh"));
	// ��Ʈ ������Ʈ�� ���̱�
	CrosshairMesh->SetupAttachment(RootComponent);
	CrosshairMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	//�⺻ ��ų �Ǻ�
	SkillPivot = CreateDefaultSubobject<USceneComponent>(TEXT("SkillPivot"));
	SkillPivot->SetupAttachment(PlayerCapsuleComponent);

	//��ų 3  �Ǻ� ���� �� ���̱�
	Skill3Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Skill3Pivot"));
	Skill3Pivot->SetupAttachment(PlayerCapsuleComponent);
	// X �� +50 �̵� (��ų ���Ͱ� ĳ���Ϳ� �ʹ� ������ �����Ǵ� ������ �ذ��ϱ� ����)
	Skill3Pivot->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f)); //����ȸ�� (���� ���� ȸ��) ���� ����
	//SkillPivot->SetRelativeLocation(SkillPivot->GetRelativeLocation() + FVector(90.0f, 0.0f, 0.0f));
	

	//��ų 3 ���� ��� �� �ʿ��� ��ġ ������Ʈ ���� �� ���̱�
	RotatingSkillSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("RotatingSkillSpawn"));
	RotatingSkillSpawn->SetupAttachment(Skill3Pivot); //RotatingSkillSpawn->SetupAttachment(PlayerCapsuleComponent); ���� ������.
	RotatingSkillSpawn->SetRelativeLocation(RotatingSkillSpawn->GetRelativeLocation() + FVector(300.0f, 0.0f, 0.0f)); //�Ÿ� ����
	RotatingSkillSpawn->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	
	// ����ƽ �޽��� �ε��Ͽ� CrosshairMesh�� �Ҵ�
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere")); //���Ǿ� ���� /Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder' �ƴϸ� �Ǹ����� �����ص� ��
	if (MeshAsset.Succeeded())
	{
		CrosshairMesh->SetStaticMesh(MeshAsset.Object);
	}

	// �޽� ������ ����
	FVector MeshScale(0.7f, 0.7f, 0.08f); //���ϴ� ������ ������ ����
	CrosshairMesh->SetWorldScale3D(MeshScale);

	// ���� ���
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprints/Player/Widgets/WBP_Player"));
	if (WidgetClassFinder.Succeeded())
	{
		PlayerWidgetClass = WidgetClassFinder.Class;
		// ���� �ν��Ͻ� ����
		PlayerWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerWidgetClass);
		if (PlayerWidget) // ������ ���������� �����Ǿ��� ���
		{
			PlayerWidget->AddToViewport();
			//WBP_Player ���� �г��� WBP_DeathScreen ����
			DeathScreenWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_DeathScreen")));
			//WBP_Player ���� �г��� WBP_Leveling ����
			LevelingWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_Leveling")));
			//WBP_Player ���� �г��� WBP_TopRightHUD ����
			TopRightHUDWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_TopRightHUD")));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to Create PlayerWidget"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to Find a WBP_Player"));
	}
	//Set timer by event
	//��ų 1 ��������Ʈ�� �Լ� ���ε�
	SkillDelegate.BindUFunction(this, FName("Skill_1"));
	//��ų 2 ��������Ʈ�� �Լ� ���ε�
	SkillDelegate2.BindUFunction(this, FName("Skill_2"));
	//��ų 3 ��������Ʈ�� �Լ� ���ε�
	SkillDelegate3.BindUFunction(this, FName("Skill_3"));
	//DistanceToSpawnSkillActor = 100.0f;

	FString Skill3ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Light_Straight.Spell_Light_Straight_C"; //Skill3 �� ���� Ŭ���� ��� ����
	Skill3ActorClass = LoadClass<AActor>(nullptr, *Skill3ActorClassPath);
	
	SkillRotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SkillRotationTimeline")); // ��ų 3 Ÿ�Ӷ��� ����
	// Ÿ�Ӷ����� ���α׷��� �Լ� ���ε�
	ProgressFunction.BindUFunction(this, "UpdateRotationProgress");
	// Ÿ�Ӷ����� �Ϸ� �Լ� ���ε�
	TimelineFinishedEvent.BindUFunction(this, "OnRotationComplete");
		
	
	// CurveFloat�� �ε��մϴ�.
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Convert/Skill/Skill3Curve.Skill3Curve"));
	if (Curve.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("CurveSucceed"));
		RotationCurve = Curve.Object;
	}
	
	//�� Enemy Dragon3 Ŭ���� �ε��ϱ�
	Enemy_Dragon3 = LoadClass<ACharacter>(nullptr, *Dragon3ActorClassPath);
	EnemySpawnDelegate.BindUFunction(this, FName("SpawnEnemy"));
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
	Skill3PivotComponent = PlayerCapsuleComponent->GetChildComponent(0);
	StartSkill1Timer();

	SkillRotationTimeline->SetLooping(true); //Ÿ�Ӷ��� ���� ���� ����
	SkillRotationTimeline->SetTimelineLength(4.0f); // Ÿ�Ӷ��� ���� ����
	//SkillRotationTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	SkillRotationTimeline->SetTimelineFinishedFunc(TimelineFinishedEvent); // Ÿ�Ӷ��� ���� �� ȣ��� �ݹ��Լ� �߰�
	SkillRotationTimeline->AddInterpFloat(RotationCurve, ProgressFunction); // Ÿ�Ӷ����� Curve, �ݹ��Լ� �߰�
	
	//Enemy Spawn
	FVector SpawnVector;
	FRotator SpawnRotator;
	SpawnVector = FVector(0.0f, 0.0f, 0.0f);
	SpawnRotator = FRotator(0.0f, 0.0f, 0.0f);
	SpawnLocation = SpawnVector;
	SpawnRotation = SpawnRotator;
	
	//SpawnVector = FVector(rand(), rand(), rand());
	//SpawnRotator = FRotator(rand(), rand(), rand());
	//SpawnEnemy();
	StartSpawnEnemy();
	
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
	if (SkillLevel_1 == 5 && SkillLevel_2 == 5 && SkillLevel_3 == 5)
		return;
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

void APlayerCharacter::Skill_1() //��ų1 ��� ����
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

void APlayerCharacter::Skill_2()//��ų 2 ��� ���� 
{
	switch (SkillLevel_2)
	{
	case 1:
		SpawnSkill_2(2, 0.0, 180.0);
		break;
	case 2:
		SpawnSkill_2(3, 0.0, 120.0);
		break;
	case 3:
		SpawnSkill_2(4, 0.0, 90.0);
		break;
	case 4:
		SpawnSkill_2(6, 0.0, 60.0);
		break;
	case 5:
		SpawnSkill_2(8, 0.0, 45.0);
		break;

	}
}

void APlayerCharacter::Skill_3()//��ų3 ��� ���� LIghtBallSpell �Լ� ����
{
	UWorld* World = GetWorld();
	if (World)
	{
		DestroyAllActorsOfClass(World, Skill3ActorClass);
	}

	switch (SkillLevel_3)
	{
	case 1:
		InitializeSkill_3(1, 0.0);
		break;
	case 2:
		InitializeSkill_3(2, 180.0);
		break;
	case 3:
		InitializeSkill_3(3, 120.0);
		break;
	case 4:
		InitializeSkill_3(4, 90.0);
		break;
	case 5:
		InitializeSkill_3(6, 60.0);
		break;
	}
}

void APlayerCharacter::Skill_4()//��ų4 ��� ���� 
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

void APlayerCharacter::StartSkill1Timer() //��������Ʈ�� ����Ͽ� ��ų 1 Ÿ�̸� ����, ��ų ��� 
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, SkillDelegate, 1.0f, true, 1.5f);
}

void APlayerCharacter::StartSkill2Timer() //��������Ʈ�� ����Ͽ� ��ų 2 Ÿ�̸� ����, ��ų ���
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, SkillDelegate2, 1.5f, true, 2.0f);
}



void APlayerCharacter::SpawnSkill_1(int NumProjectile, float StartRotation, float RotIncrement)
{
	USceneComponent* Skill1PivotComponent = PlayerCapsuleComponent->GetChildComponent(0); // ĸ�� ������Ʈ�� ù ��° �ڽ� ������Ʈ�� �����ɴϴ�.
	USceneComponent* Skill1SpawnComponent = PlayerCapsuleComponent->GetChildComponent(1); // FireBallPivot ������Ʈ ������ FireBall Spawn ������Ʈ �޾ƿ���
	FQuat NewRotationQuat(FRotator(0.f, StartRotation, 0.f));//�Լ� ȣ�� �� ������ Rot ���� �����Ͽ� ����
	FQuat LevelUpRotationQuat(FRotator(0.f, RotIncrement, 0.f));//��ų ���� �� �� ��µǴ� ��ų�� ������ �����Ϸ��� Add Rotation �ϱ� ���� FQuat

	if (Skill1PivotComponent != nullptr)
	{
		Skill1PivotComponent->SetRelativeRotation(NewRotationQuat); //��ų ������ ��µǴ� ��� ���� ������ �ǹ̰� ����?

		if (Skill1SpawnComponent != nullptr)
		{
			FVector SpawnTransformLocation = SkillPivot->GetComponentLocation();
			FRotator SpawnTransformRotation = SkillPivot->GetComponentRotation();
			//SpawnTransformRotation.Yaw += 90.0f; //��ų ��� ������ ĳ������ ���� �ȿ��� ������ ���� ����...

			FString Skill1ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Fire.Spell_Fire_C"; //Skill1 �� ���� Ŭ���� ��� ����

			UClass* Skill1ActorClass = LoadClass<AActor>(nullptr, *Skill1ActorClassPath);

			for (int32 i = 0; i < NumProjectile; ++i)
			{
				UE_LOG(LogTemp, Warning, TEXT("Skill Level : %d"), SkillLevel_1);
				UE_LOG(LogTemp, Warning, TEXT("Num of Projectile : %d"), NumProjectile);
				UE_LOG(LogTemp, Warning, TEXT("Increase Rot : %f"), RotIncrement);
				//Skill1PivotComponent->AddRelativeRotation(LevelUpRotationQuat);
				FRotator SpawnedRotation = SpawnTransformRotation + FRotator(0.0f, RotIncrement * i + StartRotation , 0.0f);
				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Skill1ActorClass, SpawnTransformLocation, SpawnedRotation);
			}

			Skill1PivotComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Skill1SpawnComponent !="));

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill1PivotComponent !="));
	}
}

void APlayerCharacter::SpawnSkill_2(int NumProjectile, float StartRotation, float RotIncrement)
{
	USceneComponent* Skill2PivotComponent = PlayerCapsuleComponent->GetChildComponent(0); // ĸ�� ������Ʈ�� ù ��° �ڽ� ������Ʈ�� �����ɴϴ�.
	USceneComponent* Skill2SpawnComponent = PlayerCapsuleComponent->GetChildComponent(1); // FireBallPivot ������Ʈ ������ FireBall Spawn ������Ʈ �޾ƿ���
	FQuat NewRotationQuat(FRotator(0.f, StartRotation, 0.f));//�Լ� ȣ�� �� ������ Rot ���� �����Ͽ� ����
	FQuat LevelUpRotationQuat(FRotator(0.f, RotIncrement, 0.f));//��ų ���� �� �� ��µǴ� ��ų�� ������ �����Ϸ��� Add Rotation �ϱ� ���� FQuat

	if (Skill2PivotComponent != nullptr)
	{
		Skill2PivotComponent->SetRelativeRotation(NewRotationQuat); //��ų ������ ��µǴ� ��� ���� ������ �ǹ̰� ����?

		if (Skill2SpawnComponent != nullptr)
		{
			FVector SpawnTransformLocation = SkillPivot->GetComponentLocation();
			FRotator SpawnTransformRotation = SkillPivot->GetComponentRotation();
			//SpawnTransformRotation.Yaw += 90.0f; //��ų ��� ������ ĳ������ ���� �ȿ��� ������ ���� ����...

			FString Skill2ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Ice.Spell_Ice_C"; //Skill2 �� ���� Ŭ���� ��� ����

			UClass* Skill2ActorClass = LoadClass<AActor>(nullptr, *Skill2ActorClassPath);

			for (int32 i = 0; i < NumProjectile; ++i)
			{
				UE_LOG(LogTemp, Warning, TEXT("Skill 2 Level : %d"), SkillLevel_1);
				UE_LOG(LogTemp, Warning, TEXT("2 , Num of Projectile : %d"), NumProjectile);
				UE_LOG(LogTemp, Warning, TEXT("2, Increase Rot : %f"), RotIncrement);
				//Skill2PivotComponent->AddRelativeRotation(LevelUpRotationQuat);
				FRotator SpawnedRotation = SpawnTransformRotation + FRotator(0.0f, RotIncrement * i + StartRotation, 0.0f);
				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Skill2ActorClass, SpawnTransformLocation, SpawnedRotation);
			}

			Skill2PivotComponent->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Skill2SpawnComponent !="));

		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill2PivotComponent !="));
	}

}

void APlayerCharacter::InitializeSkill_3(int NumProjectile, float Rotation) // initial Skill3
{
	
	Skill3PivotComponent = Skill3Pivot;
	Skill3SpawnComponent = RotatingSkillSpawn;
	//Skill3PivotComponent = PlayerCapsuleComponent->GetChildComponent(0);
	//Skill3SpawnComponent = Skill3PivotComponent->GetChildComponent(0);
	FQuat RotationQuat(FRotator(0.f, Rotation, 0.f));
	FAttachmentTransformRules Skill3AttachmentRules(EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, EAttachmentRule::KeepWorld, true);
	if (Skill3PivotComponent != nullptr)
	{
		if (Skill3SpawnComponent != nullptr)
		{
			FVector SpawnTransformLocation = RotatingSkillSpawn->GetComponentLocation();
			FRotator SpawnTransformRotation = RotatingSkillSpawn->GetComponentRotation();

			//FString Skill3ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Light_Straight.Spell_Light_Straight_C"; //Skill3 �� ���� Ŭ���� ��� ����
			//UClass* Skill3ActorClass = LoadClass<AActor>(nullptr, *Skill3ActorClassPath);
			for (int32 i = 0; i < NumProjectile; ++i)
			{
				FRotator SpawnedRotation = SpawnTransformRotation + FRotator(0.0f, Rotation * i , 0.0f);
				AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>
					(Skill3ActorClass, SpawnTransformLocation, SpawnedRotation);
				SpawnedActor->AttachToComponent(Skill3PivotComponent, Skill3AttachmentRules);

				Skill3PivotComponent->AddLocalRotation(RotationQuat);
			}
		}
	}
}

void APlayerCharacter::StartSkill_3()
{

}

void APlayerCharacter::SpawnSkill_4()
{
}

void APlayerCharacter::DestroyAllActorsOfClass(UWorld* World, TSubclassOf<AActor> ActorClass) //��ų 3 �ʱ�ȭ �Լ�
{
	if (!World || !ActorClass)
	{
		return;
	}

	for (TActorIterator<AActor> It(World, ActorClass); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor)
		{
			Actor->Destroy();
		}
	}
}

void APlayerCharacter::StartRotatingLight()
{
	//Skill3PivotComponent = PlayerCapsuleComponent->GetChildComponent(1);
	if (SkillRotationTimeline && RotationCurve)
	{   
		SkillRotationTimeline->PlayFromStart();// Ÿ�Ӷ��� ����
		UE_LOG(LogTemp, Warning, TEXT("TimeLineStart"));

	}
}

void APlayerCharacter::UpdateRotationProgress(float Value)
{
	//Skill3PivotComponent = PlayerCapsuleComponent->GetChildComponent(1);
	UE_LOG(LogTemp, Warning, TEXT("UpdateRotationProgress"));

	if (Skill3PivotComponent)
	{
		FRotator NewRotation = FRotator(0.0f, Value, 0.0f); // z�� ȸ��
		Skill3PivotComponent->SetWorldRotation(NewRotation);
		UE_LOG(LogTemp, Warning, TEXT("Updating Rotation: %f"), Value);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Skill3PivotComponent is not valid"));
	}
}

void APlayerCharacter::OnRotationComplete()
{
	UE_LOG(LogTemp, Warning, TEXT("Rotation Complete"));
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

void APlayerCharacter::SpawnEnemy() 
{
	//AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(Skill1ActorClass, SpawnTransformLocation, SpawnedRotation);
	//SpawnLocation = FVector(rand() % 50 - 100, rand() % 50 - 100, 0.0);
	//SpawnRotation = FRotator(rand(), rand(), rand());
	SelectRandomSpawnLocation();
	UWorld* World = GetWorld();
	if (Enemy_Dragon3)
	{
		
		if (World)
		{
			FActorSpawnParameters SpawnParams;
			SpawnedEnemy = World->SpawnActor<ACharacter>(Enemy_Dragon3, SpawnLocation, SpawnRotation, SpawnParams);//���� Ȯ��
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy_Dragon3 class is not valid"));
	}
}

void APlayerCharacter::StartSpawnEnemy()
{
	
	GetWorld()->GetTimerManager().SetTimer(EnemySpawnTimerHandle, EnemySpawnDelegate, 3.0f, true, 2.0f);
}

void APlayerCharacter::SelectRandomSpawnLocation()
{
	int randNum = FMath::RandRange(1, 4);

	//���� ��ǥ ���� ����
	// X : 3230 	      Y : -3800 ~ 3800
	// X : -3790          Y : -3800 ~ 3800
	// X : - 3600 ~ 3600  Y : 3880
	// X : -3600 ~ 3600   Y : -3920

	switch (randNum)
	{
	case 1:
		SpawnLocation = FVector(3230.0f, FMath::FRandRange(0.0f,7600.0f)-3800.0f, 0.0f);
		//SpawnLocation = FVector(300.0f, 300.0f, 0.0f);
		break;
	case 2:
		SpawnLocation = FVector(-3790.0f, FMath::FRandRange(0.0f, 7600.0f) - 3800.0f, 0.0f);
		//SpawnLocation = FVector(-300.0f, -300.0f, 0.0f);
		break;
	case 3:
		SpawnLocation = FVector(FMath::FRandRange(0.0f, 7200.0f) - 3600.0f, 3880.0f, 0.0f);
		//SpawnLocation = FVector(-300.0f, 300.0f, 0.0f);
		break;
	case 4:
		SpawnLocation = FVector(FMath::FRandRange(0.0f, 7200.0f) - 3600.0f, -3920.0f, 0.0f);
		//SpawnLocation = FVector(300.0f, -300.0f, 0.0f);
		break;
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RotateMeshTowardsCursor();
	//SetSphereMeshLocationUnderCursor();

	if (SkillRotationTimeline)
	{
		SkillRotationTimeline->TickComponent(DeltaTime, ELevelTick::LEVELTICK_TimeOnly, nullptr);
	}

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
	FOutputDeviceNull pAR;
	if (SkillLevel_1 == 5 && SkillLevel_2 == 5 && SkillLevel_3 == 5)
	{
		PlayerWidget->CallFunctionByNameWithArguments
		(*FString::Printf(TEXT("Level_Max")), pAR, nullptr, true);
		return;
	}
	CurrentExperience += PickupValue;

	InPercent = CurrentExperience / ExpericenCap;

	
	PlayerHealthPercentage = PlayerHealth / PlayerMaxHealth;
	PlayerWidget->CallFunctionByNameWithArguments
	(*FString::Printf(TEXT("UpdateXP_Percentage %f"), InPercent), pAR, nullptr, true);

	if (CurrentExperience >= ExpericenCap)
	{
		PlayerHealthPercentage = PlayerHealth / PlayerMaxHealth;
		PlayerWidget->CallFunctionByNameWithArguments
		(*FString::Printf(TEXT("UpdateXP_Percentage %f"), 0.0), pAR, nullptr, true);
		CharacterLevel++;
		CurrentExperience = 0;
		StartSelectingSkills();
	}
	//PlayerWidget->CallFunctionByNameWithArguments
	//(*FString::Printf(TEXT("IncreaseLevel %f"), CharacterLevel), pAR, nullptr, true);
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
