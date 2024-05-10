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

#include "Misc/OutputDeviceNull.h" // 실제 출력을 수행하지 않고 로그 메시지 무시 -> 위젯 업데이트 시 사용
//#include "Engine/GameInstance.h" // UGameInstance 헤더 포함 -> 추후 게임 모드나 게임 인스턴스 클래스로 이동해야하는 기능 -> SetGamePaused().
#include "Kismet/GameplayStatics.h" // 게임 퍼즈 기능 사용하기 위한 함수
#include "Components/SceneComponent.h" //씬 컴포넌트에 접근해서 스킬 출력 시 피봇 위치를 받아오기 위한 함수


// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//카메라
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));   //스프링 암 컴포넌트 달아줌
	CameraBoom->SetupAttachment(GetRootComponent());							    //루트 컴포넌트 아래로 달아줌
	CameraBoom->TargetArmLength = 800.f;										    //타겟 암 길이 조정
	CameraBoom->SetWorldRotation(FRotator(-50.f, 0.f, 0.f));
	// 스프링 암의 회전 프로퍼티를 월드로 변경
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	//스프링 암 각도 조정 
	//CameraBoom->bUsePawnControlRotation = true;										// 캐릭터의 회전에 따라 스프링 암이 회전

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));		//카메라 컴포넌트 달아줌
	ViewCamera->SetupAttachment(CameraBoom);									    //카메라를 스프링 암에 달아줌
	//ViewCamera->bUsePawnControlRotation = false;									// 스프링 암의 회전만 사용합니다.

	// 컨트롤러의 회전에 따라 스프링 암과 카메라의 회전을 조정합니다.
	//bUseControllerRotationPitch = false;
	//bUseControllerRotationYaw = false;
	//bUseControllerRotationRoll = false;

	//////////////플레이어 최대 체력 설정 ///////////////////
	PlayerMaxHealth = 50.0f;
	//플레이어 초기 체력 설정
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
	
	// 애니메이션 블루프린트 클래스의 경로
	FSoftObjectPath AnimInstanceClassPath = TEXT("/Game/Convert/Player/PlayerAnimation/ABP_Forest_Creature");

	// 애니메이션 블루프린트 클래스 로드
	UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(AnimInstanceClassPath.TryLoad());

	if (AnimBlueprint)
	{
		// 애니메이션 인스턴스 설정
		GetMesh()->SetAnimInstanceClass(AnimBlueprint->GetClass());
		//UE_LOG(LogTemp, Warning, TEXT("Anim Instance Succeed"));
	}
	else
	{
		//UE_LOG(LogTemp, Error, TEXT("Failed to load Anim Instance class"));
	}

	//향상된 입력 - 이동
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

	//충돌 처리 (캡슐 컴포넌트)
	UCapsuleComponent* PlayerCapsuleComponent = GetCapsuleComponent();
	// CrosshairMesh 생성
	CrosshairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CrosshairMesh"));
	// 부모 컴포넌트에 붙이기 (예: RootComponent)
	CrosshairMesh->SetupAttachment(PlayerCapsuleComponent);

	// 스태틱 메쉬를 로드하여 CrosshairMesh에 할당
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Sphere.Sphere")); //스피어 생성 /Script/Engine.StaticMesh'/Engine/BasicShapes/Cylinder.Cylinder' 아니면 실린더로 생성해도 됨
	if (MeshAsset.Succeeded())
	{
		CrosshairMesh->SetStaticMesh(MeshAsset.Object);
	}

	// 메쉬 스케일 수정
	FVector MeshScale(0.7f, 0.7f, 0.08f); // 원하는 스케일 값으로 수정
	CrosshairMesh->SetWorldScale3D(MeshScale);

	//위젯 출력
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetClassFinder(TEXT("/Game/Blueprints/Player/Widgets/WBP_Player"));
	if (WidgetClassFinder.Succeeded())
	{
		PlayerWidgetClass = WidgetClassFinder.Class;
		UE_LOG(LogTemp, Warning, TEXT("Player Widget Find!"));
		// 위젯 인스턴스 생성
		PlayerWidget = CreateWidget<UUserWidget>(GetWorld(), PlayerWidgetClass);
		if (PlayerWidget)
		{
			// 출력
			PlayerWidget->AddToViewport();
			UE_LOG(LogTemp, Warning, TEXT("Player Widget Create!"));
			//WBP_Player 하위 패널인 WBP_DeathScreen 연결
			DeathScreenWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_DeathScreen")));
			//WBP_Player 하위 패널인 WBP_Leveling 연결
			LevelingWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_Leveling")));
			//WBP_Player 하위 패널인 WBP_TopRightHUD 연결
			TopRightHUDWidget = Cast<UUserWidget>(PlayerWidget->GetWidgetFromName(TEXT("WBP_TopRightHUD")));
		}
	}
	//Set timer by event
	// FTimerManager& AActor::GetWorldTimerManager() const 함수 사용예) GetWorldTimerManager().SetTimer(...);
	

	//if (!DeathScreenWidget)// 하위 패널에 대한 액세스가 실패했을 때의 처리
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Failed to access DeathScreenWidget!"));
	//}
	//static ConstructorHelpers::FClassFinder<UUserWidget> DeathScreenWidgetClassFinder(TEXT("/Game/Blueprints/Player/Widgets/WBP_Death_Screen")); 
	//if (DeathScreenWidgetClassFinder.Succeeded())
	//{
	//	DeathScreenWidgetClass = DeathScreenWidgetClassFinder.Class;
	//	UE_LOG(LogTemp, Warning, TEXT("Death Screen Widget Find!"));
	//	// 위젯 인스턴스 생성
	//	DeathScreenWidget = CreateWidget<UUserWidget>(GetWorld(), DeathScreenWidgetClass);
	//	if (DeathScreenWidget)
	//	{
	//		// 출력
	//		DeathScreenWidget->AddToViewport();
	//		//UE_LOG(LogTemp, Warning, TEXT("Death Screen Widget Create!"));
	//	}
	//}
	
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

void APlayerCharacter::skill_1() //스킬1 출력 관리 
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

void APlayerCharacter::skill_2()//스킬 2 출력 관리 
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

void APlayerCharacter::skill_3()//스킬3 출력 관리 
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

void APlayerCharacter::skill_4()//스킬4 출력 관리 
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
	USceneComponent* Skill1PivotComponent = PlayerCapsuleComponent->GetChildComponent(0); // 캡슐 컴포넌트의 첫 번째 자식 컴포넌트를 가져옵니다.
	USceneComponent* Skill1SpawnComponent = Skill1PivotComponent->GetChildComponent(0); // FireBallPivot 컴포넌트 하위의 FireBall Spawn 컴포넌트 받아오기
	FQuat NewRotationQuat(FRotator(0.f, Rotation, 0.f));//함수 호출 시 설정한 Rot 값을 적용하여 설정

	if(Skill1PivotComponent != nullptr)
	{
		Skill1PivotComponent->SetRelativeRotation(NewRotationQuat);

		FVector SpawnTransformLocation = Skill1SpawnComponent->GetComponentLocation();
		FRotator SpawnTransformRotation = Skill1SpawnComponent->GetComponentRotation();

		FString Skill1ActorClassPath = "/Game/Blueprints/Player/Spells/Spell_Fire.Spell_Fire_C"; //Skill1 의 액터 클래스 경로 설정

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
