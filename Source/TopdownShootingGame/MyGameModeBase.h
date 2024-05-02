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
	// ������
	AMyGameModeBase();

	//���� ���� �� ȣ��Ǵ� �Լ�
	virtual void StartPlay() override;
	//���� �Ͻ� ���� ���� ����
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
