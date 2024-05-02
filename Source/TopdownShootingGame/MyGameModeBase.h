// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class TOPDOWNSHOOTINGGAME_API AMyGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
public:
	// 생성자
	AMyGameModeBase();

	//게임 시작 시 호출되는 함수
	virtual void StartPlay() override;
	//게임 일시 정지 여부 설정
	UFUNCTION(BlueprintCallable, Category = "Game")
	void SetGamePaused(bool bPaused);


	//UFUNCTION(BlueprintCallable, Category="UMG_Game")
	//void ChangeMenuWidjet(TSubclassOf<UUserWidget> NewWidgetClass);

//protected:
	//virtual void BeginPlay() override;

	///UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UMG_Game")
	//TSubclassOf<UUserWidget> StartingWidgetClass;

	//UPROPERTY()
	//UUserWidget* CurrentWidget;

//protected:
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DefaultClasses")
	//TSubclassOf<APawn> DefaultPawnClass;
};
