// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h" ///스프링 암
#include "Camera/CameraComponent.h"  //카메라 컴포넌트
#include "Components/CapsuleComponent.h" //캡슐 컴포넌트
#include "EnhancedInputComponent.h" //입력
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "MyGameModeBase.h"
#include "Math/UnrealMathUtility.h"

#include "Misc/OutputDeviceNull.h" // 실제 출력을 수행하지 않고 로그 메시지 무시 -> 위젯 업데이트 시 사용
//#include "Engine/GameInstance.h" // UGameInstance 헤더 포함 -> 추후 게임 모드나 게임 인스턴스 클래스로 이동해야하는 기능 -> SetGamePaused().
#include "Kismet/GameplayStatics.h" // 게임 퍼즈 기능 사용하기 위한 함수
#include "Components/SceneComponent.h" //씬 컴포넌트에 접근해서 스킬 출력 시 피봇 위치를 받아오기 위한 함수
#include "TimerManager.h" // 타이머 - 액터 생성 시 사용되는 타이머 함수
#include "EngineUtils.h" // 월드에 생성된 액터 찾을 때 사용

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//월드 받아오기
	//World = GetWorld();

	//카메라
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));   //스프링 암 컴포넌트 달아줌
	CameraBoom->SetupAttachment(GetRootComponent());							    //루트 컴포넌트 아래로 달아줌
	CameraBoom->TargetArmLength = 2500.f;										    //타겟 암 길이 조정
	CameraBoom->SetWorldRotation(FRotator(-50.f, 0.f, 0.f));
	// 스프링 암의 회전 프로퍼티를 월드로 변경
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	//스프링 암 각도 조정 
	//CameraBoom->bUsePawnControlRotation = true;								    // 캐릭터의 회전에 따라 스프링 암이 회전

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));		//카메라 컴포넌트 달아줌
	ViewCamera->SetupAttachment(CameraBoom);									    //카메라를 스프링 암에 달아줌
	//ViewCamera->bUsePawnControlRotation = false;									// 스프링 암의 회전만 사용합니다.

	//플레이어 최대 체력 설정
	PlayerMaxHealth = 50.0f;
	//플레이어 초기(기본) 체력 설정
	PlayerHealth = PlayerMaxHealth;

	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -88.0f), FRotator(0.0f, -90.0f, 0.0f)); // 스켈레탈 메쉬 위치 조정
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);                           // 캡슐 컴포넌트 위치 조정

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> SM(TEXT("/Game/Blueprints/Enemies/SK_Forest_Creature.SK_Forest_Creature"));
	if (SM.Succeeded())
	{
		USkeletalMeshComponent* MeshComponent = GetMesh();
		if (MeshComponent) // GetMesh() 함수가 유효한 메시 컴포넌트를 반환했는지 확인
		{
			MeshComponent->SetSkeletalMesh(SM.Object);

			static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material0(TEXT("/Game/Forest_Creature/Materials/Summer/M_Forest_Creature.M_Forest_Creature"));
			static ConstructorHelpers::FObjectFinder<UMaterialInterface> Material1(TEXT("/Game/Forest_Creature/Materials/Summer/M_Plants.M_Plants"));
			if (Material0.Succeeded()&& Material1.Succeeded())
			{
				// 머티리얼을 스켈레탈 메시의 슬롯에 할당
				MeshComponent->SetMaterial(0, Material0.Object);
				MeshComponent->SetMaterial(1, Material1.Object);
			}
			// 스케일 조정
			FVector Scale(0.7f, 0.7f, 0.7f); // 원하는 스케일 값으로 수정
			MeshComponent->SetRelativeScale3D(Scale);
		}
	}
	
	// 애니메이션 블루프린트 클래스 경로 지정
	FSoftObjectPath AnimInstanceClassPath = TEXT("/Game/Convert/Player/PlayerAnimation/ABP_Forest_Creature");

	// 지정한 경로의 애니메이션 블루프린트 클래스 로드
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(AnimInstanceClassPath.TryLoad());
	if (AnimBlueprint) //유효성 검사
	{
		GetMesh()->SetAnimInstanceClass(AnimBlueprint->GetClass());// 애니메이션 인스턴스 설정
	}
	else //AnimBlueprint가 정상적으로 로드되지 않았을 때
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed to load Anim Instance class"));
	}

	//Enhanced Input 캐릭터 이동

	// Enhanced Input을 사용하기 위해 필요한 입력 매핑 컨텍스트 로드
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> 
		InputMappingContextRef(TEXT("/Game/Convert/Player/PlayerMovement/IMC_Player.IMC_Player"));	
	if (InputMappingContextRef.Object)	
	{		
		DefaultMappingContext = InputMappingContextRef.Object;	
	}  	
	
	// 점프 액션을 위한 Enhanced Input 입력 액션 로드
	static ConstructorHelpers::FObjectFinder<UInputAction> 
		InputActionJumpRef(TEXT("/Game/Convert/Player/PlayerMovement/IA_Jump.IA_Jump"));	
	if (InputActionJumpRef.Object)	
	{		
		JumpAction = InputActionJumpRef.Object;	
	} 	

	// 이동 액션을 위한 Enhanced Input 입력 액션 로드
	static ConstructorHelpers::FObjectFinder<UInputAction> 
		InputActionMoveRef(TEXT("/Game/Convert/Player/PlayerMovement/IA_Movement.IA_Movement"));	
	if (InputActionMoveRef.Object)	
	{		
		MoveAction = InputActionMoveRef.Object;	
	} 	

	//충돌 처리 (캡슐 컴포넌트)
	PlayerCapsuleComponent = GetCapsuleComponent();


	// CrosshairMesh 생성
	CrosshairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrosshairMesh"));
	// 루트 컴포넌트에 붙이기
	CrosshairMesh->SetupAttachment(RootComponent);
	CrosshairMesh->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	//기본 스킬 피봇
	SkillPivot = CreateDefaultSubobject<USceneComponent>(TEXT("SkillPivot"));
	SkillPivot->SetupAttachment(PlayerCapsuleComponent);

	//스킬 3  피봇 생성 및 붙이기
	Skill3Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Skill3Pivot"));
	Skill3Pivot->SetupAttachment(PlayerCapsuleComponent);
	// X 값 +50 이동 (스킬 액터가 캐릭터와 너무 가까이 스폰되는 문제를 해결하기 위해)
	Skill3Pivot->SetWorldRotation(FRotator(0.0f, 0.0f, 0.0f)); //절대회전 (월드 기준 회전) 으로 변경
	//SkillPivot->SetRelativeLocation(SkillPivot->GetRelativeLocation() + FVector(90.0f, 0.0f, 0.0f));
	

	//스킬 3 액터 출력 시 필요한 위치 컴포넌트 생성 및 붙이기
	RotatingSkillSpawn = CreateDefaultSubobject<USceneComponent>(TEXT("RotatingSkillSpawn"));
	RotatingSkillSpawn->SetupAttachment(Skill3Pivot); //RotatingSkillSpawn->SetupAttachment(PlayerCapsuleComponent); 에서 수정함.
	RotatingSkillSpawn->SetRelativeLocation(RotatingSkillSpawn->GetRelativeLocation() + FVector(300.0f, 0.0f, 0.0f)); //거리 설정
	RotatingSkillSpawn->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	
	// 스태틱 메쉬를 로드하여 CrosshairMesh에 할당
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere")); //스피어 생성 /Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder' 아니면 실린더로 생성해도 됨
	if (MeshAsset.Succeeded())
	{
		CrosshairMesh->SetStaticMesh(MeshAsset.Object);
	}

	// 메쉬 스케일 수정
	FVector MeshScale(0.7f, 0.7f, 0.08f); //원하는 스케일 값으로 수정
	CrosshairMesh->SetWorldScale3D(MeshScale);

	// 위젯 출력
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprints/Player/Widgets/WBP_Player"));
	if (WidgetClassFinder.Succeeded())
	{
		PlayerWidgetClass = WidgetClassFinder.Class;
		// 위젯 인스턴스 생성
		PlayerWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerWidgetClass);
		if (PlayerWidget) // 위젯이 정상적으로 생성되었을 경우
		{
			PlayerWidget->AddToViewport();
			//WBP_Player 하위 패널인 WBP_DeathScreen 연결
			DeathScreenWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_DeathScreen")));
			//WBP_Player 하위 패널인 WBP_Leveling 연결
			LevelingWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_Leveling")));
			//WBP_Player 하위 패널인 WBP_TopRightHUD 연결
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
	//스킬 1 델리게이트에 함수 바인딩
	SkillDelegate.BindUFunction(this, FName("Skill_1"));
	//스킬 2 델리게이트에 함수 바인딩
	SkillDelegate2.BindUFunction(this, FName("Skill_2"));
	//스킬 3 델리게이트에 함수 바인딩
	SkillDelegate3.BindUFunction(this, FName("Skill_3"));
	//DistanceToSpawnSkillActor = 100.0f;

	FString Skill3ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Light_Straight.Spell_Light_Straight_C"; //Skill3 의 액터 클래스 경로 설정
	Skill3ActorClass = LoadClass<AActor>(nullptr, *Skill3ActorClassPath);
	
	SkillRotationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("SkillRotationTimeline")); // 스킬 3 타임라인 생성
	// 타임라인의 프로그래스 함수 바인딩
	ProgressFunction.BindUFunction(this, "UpdateRotationProgress");
	// 타임라인의 완료 함수 바인딩
	TimelineFinishedEvent.BindUFunction(this, "OnRotationComplete");
		
	
	// CurveFloat를 로드합니다.
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve(TEXT("/Game/Convert/Skill/Skill3Curve.Skill3Curve"));
	if (Curve.Succeeded())
	{
		UE_LOG(LogTemp, Error, TEXT("CurveSucceed"));
		RotationCurve = Curve.Object;
	}
	
	//적 Enemy Dragon3 클래스 로드하기
	Enemy_Dragon3 = LoadClass<ACharacter>(nullptr, *Dragon3ActorClassPath);
	EnemySpawnDelegate.BindUFunction(this, FName("SpawnEnemy"));
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	//이동 (향상된 입력)
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) {
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	Skill3PivotComponent = PlayerCapsuleComponent->GetChildComponent(0);
	StartSkill1Timer();

	SkillRotationTimeline->SetLooping(true); //타임라인 루프 여부 설정
	SkillRotationTimeline->SetTimelineLength(4.0f); // 타임라인 길이 설정
	//SkillRotationTimeline->SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	SkillRotationTimeline->SetTimelineFinishedFunc(TimelineFinishedEvent); // 타임라인 끝날 때 호출될 콜백함수 추가
	SkillRotationTimeline->AddInterpFloat(RotationCurve, ProgressFunction); // 타임라인의 Curve, 콜백함수 추가
	
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
	
	////충돌 처리 (캡슐 컴포넌트)
	//UCapsuleComponent* PlayerCapsuleComponent = GetCapsuleComponent();
	//if (PlayerCapsuleComponent)// 캡슐 컴포넌트에 대한 접근이 유효한 경우
	//{
	//	//// 예시: 캡슐 컴포넌트의 위치 가져오기
	//	//FVector CapsuleLocation = PlayerCapsuleComponent->GetComponentLocation();

	//	//// 예시: 캡슐 컴포넌트의 충돌 반경 가져오기
	//	//float CapsuleRadius = PlayerCapsuleComponent->GetScaledCapsuleRadius();

	//	//// 예시: 캡슐 컴포넌트의 높이 가져오기
	//	//float CapsuleHeight = PlayerCapsuleComponent->GetScaledCapsuleHalfHeight();

	//	//// 여기에 다른 캡슐 컴포넌트의 속성이나 기능을 사용할 수 있습니다.
	//}
	//else
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Player Capsule Component Failed"));// 캡슐 컴포넌트에 대한 접근이 유효하지 않은 경우
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
	Super::Jump(); //Chracter.h 에 있는 기본 Jump 기능 사용
	//UE_LOG(LogTemp, Warning, TEXT("Input Jump Action Triggered"));
}

void APlayerCharacter::StartSelectingSkills()
{
	if (SkillLevel_1 == 5 && SkillLevel_2 == 5 && SkillLevel_3 == 5)
		return;
	if (GetWorld() != nullptr)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true); // 퍼즈 기능 작동

		if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) //마우스 커서 활성화
		{
			if (PlayerController != nullptr)
			{
				// 마우스 커서의 가시성 True로 설정
				PlayerController->bShowMouseCursor = true;

				//Set Movement Mode 설정

				//WBP_Leveling 위젯 보이게끔 Visivle 설정
				LevelingWidget->SetVisibility(ESlateVisibility::Visible);

				//SetInputModeUIOnly 노드 구현
				FInputModeUIOnly InputMode;
				InputMode.SetWidgetToFocus(LevelingWidget->TakeWidget());
				//마우스를 뷰포트에 고정할건지 자유롭게 놔둘건지 지정 기능
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
		UGameplayStatics::SetGamePaused(GetWorld(), false); // 퍼즈 기능 해제

		if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) //마우스 커서 비활성화
		{
			if (PlayerController != nullptr)
			{
				// 마우스 커서의 가시성 false로 설정
				PlayerController->bShowMouseCursor = false;

				//Set Movement Mode 설정 ??

				//WBP_Leveling 위젯 안 보이게끔 Hidden 설정
				LevelingWidget->SetVisibility(ESlateVisibility::Hidden);

				//SetInputModeGameOnly 노드 구현
				FInputModeGameOnly InputMode;
				PlayerController->SetInputMode(InputMode);
				//InputMode.SetWidgetToFocus(LevelingWidget->TakeWidget());
				//마우스를 뷰포트에 고정할건지 자유롭게 놔둘건지 지정 기능
				//InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

			}
		}

	}
}

void APlayerCharacter::TopRightHUD_1()
{
	//TopRightHUDWidget 의 Update_1 커스텀 이벤트 호출
	FOutputDeviceNull pAR;
	TopRightHUDWidget->CallFunctionByNameWithArguments(*FString::Printf(TEXT("Update_1 %f"), SkillLevel_1), pAR, nullptr, true);
	//업데이트 안됨
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

void APlayerCharacter::Skill_1() //스킬1 출력 관리
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

void APlayerCharacter::Skill_2()//스킬 2 출력 관리 
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

void APlayerCharacter::Skill_3()//스킬3 출력 관리 LIghtBallSpell 함수 역할
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

void APlayerCharacter::Skill_4()//스킬4 출력 관리 
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

void APlayerCharacter::StartSkill1Timer() //델리게이트를 사용하여 스킬 1 타이머 설정, 스킬 출력 
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, SkillDelegate, 1.0f, true, 1.5f);
}

void APlayerCharacter::StartSkill2Timer() //델리게이트를 사용하여 스킬 2 타이머 설정, 스킬 출력
{
	GetWorld()->GetTimerManager().SetTimer(TimerHandle2, SkillDelegate2, 1.5f, true, 2.0f);
}



void APlayerCharacter::SpawnSkill_1(int NumProjectile, float StartRotation, float RotIncrement)
{
	USceneComponent* Skill1PivotComponent = PlayerCapsuleComponent->GetChildComponent(0); // 캡슐 컴포넌트의 첫 번째 자식 컴포넌트를 가져옵니다.
	USceneComponent* Skill1SpawnComponent = PlayerCapsuleComponent->GetChildComponent(1); // FireBallPivot 컴포넌트 하위의 FireBall Spawn 컴포넌트 받아오기
	FQuat NewRotationQuat(FRotator(0.f, StartRotation, 0.f));//함수 호출 시 설정한 Rot 값을 적용하여 설정
	FQuat LevelUpRotationQuat(FRotator(0.f, RotIncrement, 0.f));//스킬 레벨 업 시 출력되는 스킬의 각도를 조절하려고 Add Rotation 하기 위한 FQuat

	if (Skill1PivotComponent != nullptr)
	{
		Skill1PivotComponent->SetRelativeRotation(NewRotationQuat); //스킬 여러개 출력되는 기능 구현 전에는 의미가 없음?

		if (Skill1SpawnComponent != nullptr)
		{
			FVector SpawnTransformLocation = SkillPivot->GetComponentLocation();
			FRotator SpawnTransformRotation = SkillPivot->GetComponentRotation();
			//SpawnTransformRotation.Yaw += 90.0f; //스킬 출력 방향이 캐릭터의 왼쪽 팔에서 나오는 문제 수정...

			FString Skill1ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Fire.Spell_Fire_C"; //Skill1 의 액터 클래스 경로 설정

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
	USceneComponent* Skill2PivotComponent = PlayerCapsuleComponent->GetChildComponent(0); // 캡슐 컴포넌트의 첫 번째 자식 컴포넌트를 가져옵니다.
	USceneComponent* Skill2SpawnComponent = PlayerCapsuleComponent->GetChildComponent(1); // FireBallPivot 컴포넌트 하위의 FireBall Spawn 컴포넌트 받아오기
	FQuat NewRotationQuat(FRotator(0.f, StartRotation, 0.f));//함수 호출 시 설정한 Rot 값을 적용하여 설정
	FQuat LevelUpRotationQuat(FRotator(0.f, RotIncrement, 0.f));//스킬 레벨 업 시 출력되는 스킬의 각도를 조절하려고 Add Rotation 하기 위한 FQuat

	if (Skill2PivotComponent != nullptr)
	{
		Skill2PivotComponent->SetRelativeRotation(NewRotationQuat); //스킬 여러개 출력되는 기능 구현 전에는 의미가 없음?

		if (Skill2SpawnComponent != nullptr)
		{
			FVector SpawnTransformLocation = SkillPivot->GetComponentLocation();
			FRotator SpawnTransformRotation = SkillPivot->GetComponentRotation();
			//SpawnTransformRotation.Yaw += 90.0f; //스킬 출력 방향이 캐릭터의 왼쪽 팔에서 나오는 문제 수정...

			FString Skill2ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Ice.Spell_Ice_C"; //Skill2 의 액터 클래스 경로 설정

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

			//FString Skill3ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Light_Straight.Spell_Light_Straight_C"; //Skill3 의 액터 클래스 경로 설정
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

void APlayerCharacter::DestroyAllActorsOfClass(UWorld* World, TSubclassOf<AActor> ActorClass) //스킬 3 초기화 함수
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
		SkillRotationTimeline->PlayFromStart();// 타임라인 시작
		UE_LOG(LogTemp, Warning, TEXT("TimeLineStart"));

	}
}

void APlayerCharacter::UpdateRotationProgress(float Value)
{
	//Skill3PivotComponent = PlayerCapsuleComponent->GetChildComponent(1);
	UE_LOG(LogTemp, Warning, TEXT("UpdateRotationProgress"));

	if (Skill3PivotComponent)
	{
		FRotator NewRotation = FRotator(0.0f, Value, 0.0f); // z축 회전
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
	// 플레이어 컨트롤러 가져오기
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (!PlayerController)
	{
		return;
	}

	// 충돌 검출을 위한 변수
	FHitResult HitResult;

	// 마우스 커서 아래에서 충돌 검출
	PlayerController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);

	// 충돌 지점이 유효한 경우
	if (HitResult.IsValidBlockingHit())
	{
		// 캐릭터의 위치와 마우스 위치의 차이를 이용하여 캐릭터를 회전시킴
		FVector DirectionToCursor = HitResult.ImpactPoint - GetActorLocation();
		DirectionToCursor.Z = 0.0f; // Z 축 회전 방지
		DirectionToCursor.Normalize();

		// 캐릭터의 회전값 계산
		FRotator NewRotation = DirectionToCursor.Rotation();
		SetActorRotation(NewRotation);								    
	}
	// 부모 컴포넌트의 로컬 공간에서의 충돌 지점을 가져옴
	//FVector LocalHitLocation = GetTransform().InverseTransformPosition(HitResult.Location);

	//// CrosshairMesh의 위치를 변경
	//CrosshairMesh->SetWorldLocation(LocalHitLocation);

	//크로스 헤어 이동 부분
	//CrosshairMesh = FindComponentByClass<UStaticMeshComponent>();
	//static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	//if(CrosshairMesh) UE_LOG(LogTemp, Warning, TEXT("Crosshair Settings !")); // 여기까진 출력되는거 확인
	//if (CrosshairMesh && CrosshairMesh->ComponentHasTag("Crosshair"))
	

	//크로스 헤어 출력
	if (CrosshairMesh)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Crosshair Find !"));
		if (HitResult.IsValidBlockingHit())// 충돌 지점이 유효한 경우
		{
			// CrosshairMesh의 위치를 변경
			CrosshairMesh->SetRelativeLocation(GetTransform().InverseTransformPosition(HitResult.Location));
			//CrosshairMesh->SetWorldLocation(HitResult.Location);// Sphere 메쉬의 위치를 마우스 커서 아래의 충돌 지점으로 설정
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
			SpawnedEnemy = World->SpawnActor<ACharacter>(Enemy_Dragon3, SpawnLocation, SpawnRotation, SpawnParams);//스폰 확인
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

	//랜덤 좌표 범위 설정
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

	//WBP_Player 의 UpdateHealthPercentage 커스텀 이벤트 호출
	FOutputDeviceNull pAR;
	PlayerHealthPercentage = PlayerHealth / PlayerMaxHealth;
	PlayerWidget->CallFunctionByNameWithArguments(*FString::Printf(TEXT("UpdateHealthPercentage %f"), PlayerHealthPercentage), pAR, nullptr, true);
	//HP 바 업데이트 되는 것 확인

	if (PlayerHealth <= 0) //플레이어 사망 시
	{
		UE_LOG(LogTemp, Warning, TEXT("Player Dead!"));

		if (GetWorld() != nullptr && GetWorld()->GetGameViewport() != nullptr)
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true); // 퍼즈 기능 작동
			DeathScreenWidget->SetVisibility(ESlateVisibility::Visible); //해당 위젯 블루프린트의 비저빌리티를 보이도록 변경
			
			if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) //마우스 커서 활성화
			{
				if (PlayerController != nullptr)
				{
					// 마우스 커서의 가시성 True로 설정
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
