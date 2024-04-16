// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/SphereComponent.h"
#include "PlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class TOPDOWNSHOOTINGGAME_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//데미지를 처리하고 체력을 업데이트하는 함수
	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void GetHurtAndUpdateHealth(float DamageAmount);
	//
	UFUNCTION(BlueprintCallable)

	void LoadGameOver(float HealthPercentage); //LoadGameOver 위젯 출력 함수
	// UpdateHealthPercentage 함수 호출을 위한 메서드
	//void UpdateHealthPercentage(float HealthPercentage);

//protected:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	class UWBP_Player* PlayerWidget; // WBP_Player의 인스턴스에 대한 포인터

	// Health Percentage 업데이트를 위한 함수
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void UpdateHealthPercentage(float InputHealthPercentage);

	//// XP Percentage 업데이트를 위한 함수
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void UpdateXPPercentage(float InPercent);

	//// Kill Count 증가를 위한 함수
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void IncreaseKillCount();

	//// Level 증가를 위한 함수
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void IncreaseLevel(int PlayerLevelInput);

	//// Timer 업데이트를 위한 함수
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void UpdateTimer(int Min, int Second);

private:
	//카메라, 스프링 암 
	USpringArmComponent* CameraBoom;
	UCameraComponent* ViewCamera;

	//위젯 클래스를 저장할 멤버 변수 선언
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerWidgetClass;
	//위젯 인스턴스를 저장할 변수 선언
	UUserWidget* PlayerWidget;


protected:	
	virtual void BeginPlay() override;
	
	// 향상된입력 - 캐릭터 이동 및 점프
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))	
		TObjectPtr<class UInputMappingContext> DefaultMappingContext;//늘리고싶으면 아래에 또하나 만들면됌. 	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))	
		TObjectPtr<class UInputAction> JumpAction; 	 //점프 액션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))	
		TObjectPtr<class UInputAction> MoveAction; 	//이동 액션
	void Move(const FInputActionValue& Value);	
	void Jump() override;

	//마우스 커서 방향으로 회전
	class APlayerController* CachedPlayerController = nullptr;
	FVector2D CachedMouseScreenPosition;
	void RotateMeshTowardsCursor();
	//void SetSphereMeshLocationUnderCursor();

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* CrosshairMesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	float PlayerHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	float PlayerMaxHealth;



};
