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

	//�������� ó���ϰ� ü���� ������Ʈ�ϴ� �Լ�
	UFUNCTION(BlueprintCallable, Category = "CustomFunctions")
	void GetHurtAndUpdateHealth(float DamageAmount);
	//
	UFUNCTION(BlueprintCallable)

	void LoadGameOver(float HealthPercentage); //LoadGameOver ���� ��� �Լ�
	// UpdateHealthPercentage �Լ� ȣ���� ���� �޼���
	//void UpdateHealthPercentage(float HealthPercentage);

//protected:
//	UPROPERTY(EditAnywhere, BlueprintReadWrite)
//	class UWBP_Player* PlayerWidget; // WBP_Player�� �ν��Ͻ��� ���� ������

	// Health Percentage ������Ʈ�� ���� �Լ�
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void UpdateHealthPercentage(float InputHealthPercentage);

	//// XP Percentage ������Ʈ�� ���� �Լ�
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void UpdateXPPercentage(float InPercent);

	//// Kill Count ������ ���� �Լ�
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void IncreaseKillCount();

	//// Level ������ ���� �Լ�
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void IncreaseLevel(int PlayerLevelInput);

	//// Timer ������Ʈ�� ���� �Լ�
	//UFUNCTION(BlueprintCallable, Category = "CustomEvents")
	//void UpdateTimer(int Min, int Second);

private:
	//ī�޶�, ������ �� 
	USpringArmComponent* CameraBoom;
	UCameraComponent* ViewCamera;

	//���� Ŭ������ ������ ��� ���� ����
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerWidgetClass;
	//���� �ν��Ͻ��� ������ ���� ����
	UUserWidget* PlayerWidget;


protected:	
	virtual void BeginPlay() override;
	
	// �����Է� - ĳ���� �̵� �� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))	
		TObjectPtr<class UInputMappingContext> DefaultMappingContext;//�ø�������� �Ʒ��� ���ϳ� ������. 	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))	
		TObjectPtr<class UInputAction> JumpAction; 	 //���� �׼�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))	
		TObjectPtr<class UInputAction> MoveAction; 	//�̵� �׼�
	void Move(const FInputActionValue& Value);	
	void Jump() override;

	//���콺 Ŀ�� �������� ȸ��
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
