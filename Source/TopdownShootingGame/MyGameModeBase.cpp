// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "GameFramework/PlayerController.h" // 플레이어 컨트롤러 헤더를 포함합니다.
#include "Kismet/GameplayStatics.h" // GameplayStatics 헤더를 포함합니다.


//void AMyGameModeBase::BeginPlay()
//{
//	Super::BeginPlay();
//	//ChangeMenuWidjet(StartingWidgetClass);
//}

AMyGameModeBase::AMyGameModeBase()
{
  /*  static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClass(TEXT("/Script/TopdownShootingGame.PlayerCharacter"));
    if (PlayerPawnClass.Class != NULL)
    {
        DefaultPawnClass = PlayerPawnClass.Class;
    }*/
  
}

void AMyGameModeBase::StartPlay()
{
    Super::StartPlay();

    // 게임이 시작될 때 수행할 초기화 작업을 여기에 작성 (필요한 경우)
}

void AMyGameModeBase::SetGamePaused(bool bPaused)
{
    if (bPaused)
    {
        UE_LOG(LogTemp, Warning, TEXT("Game Paused"));
        APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        if (PlayerController)
        {
            PlayerController->SetPause(true);
        }
        // 게임을 일시 정지시키는 로직을 여기에 작성
        // 예를 들어, 특정 변수를 설정하거나 일시 정지 메시지를 출력하는 등의 작업을 수행할 수 있습니다.
    }
    //else
    //{
    //    // 게임을 다시 재개하는 로직을 여기에 작성
    //    // 예를 들어, 일시 정지된 상태를 해제하거나 일시 정지 메시지를 감추는 등의 작업을 수행할 수 있습니다.
    //}
}


//void AMyGameModeBase::ChangeMenuWidjet(TSubclassOf<UUserWidget> NewWidgetClass)
//{
//	if (CurrentWidget != nullptr)
//	{
//		CurrentWidget->RemoveFromViewport();
//		CurrentWidget = nullptr;
//	}
//
//	if (NewWidgetClass != nullptr)
//	{
//		CurrentWidget = CreateWidget(GetWorld(), NewWidgetClass);
//		if (CurrentWidget != nullptr)
//		{
//			CurrentWidget->AddToViewport();
//		}
//	}
//}

