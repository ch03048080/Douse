// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameModeBase.h"
#include "GameFramework/PlayerController.h" // �÷��̾� ��Ʈ�ѷ� ����� �����մϴ�.
#include "Kismet/GameplayStatics.h" // GameplayStatics ����� �����մϴ�.


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

    // ������ ���۵� �� ������ �ʱ�ȭ �۾��� ���⿡ �ۼ� (�ʿ��� ���)
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
        // ������ �Ͻ� ������Ű�� ������ ���⿡ �ۼ�
        // ���� ���, Ư�� ������ �����ϰų� �Ͻ� ���� �޽����� ����ϴ� ���� �۾��� ������ �� �ֽ��ϴ�.
    }
    //else
    //{
    //    // ������ �ٽ� �簳�ϴ� ������ ���⿡ �ۼ�
    //    // ���� ���, �Ͻ� ������ ���¸� �����ϰų� �Ͻ� ���� �޽����� ���ߴ� ���� �۾��� ������ �� �ֽ��ϴ�.
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

