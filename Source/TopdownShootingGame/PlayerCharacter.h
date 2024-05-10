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

	//XP ���� UI ������Ʈ ó��
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void AddCoinExpericence(float PickupValue);
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void StartSelectingSkills(); //��ų ���� �� UI ���� �� 
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void ReturnToGame(); // ��ų ���� �� UI ���� 
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TopRightHUD_1(); // UI Update �Լ�
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TopRightHUD_2(); // UI Update �Լ�
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TopRightHUD_3(); // UI Update �Լ�
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TopRightHUD_4(); // UI Update �Լ�

	//��ų ���
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void skill_1();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void skill_2();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void skill_3();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void skill_4();

	void SpawnSkill_1(int NumProjectile, float Rotation, float RotIncrement); //SpellSpawnFireballs
	void SpawnSkill_2(int NumProjectile, float Rotation, float RotIncrement); //SpellSpawnIceballs
	void SpawnSkill_3(int NumProjectile, float Rotation); //SpellSpawnlightballs
	void SpawnSkill_4(); //SpellSpawnDarkballs
private:
	//ī�޶�, ������ �� 
	USpringArmComponent* CameraBoom;
	UCameraComponent* ViewCamera;

	//���� Ŭ������ ������ ��� ���� ����
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerWidgetClass; 
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> DeathScreenWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> LevelingWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> TopRightHUDClass;

	//���� �ν��Ͻ��� ������ ���� ����
	UUserWidget* PlayerWidget; //WBP_Player
	UUserWidget* DeathScreenWidget; //WBP_DeathScreen
	UUserWidget* LevelingWidget; //WBP_Leveling
	UUserWidget* TopRightHUDWidget; //WBP_TopRightHUD

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	float PlayerHealthPercentage;


	//ĳ���� XP ���� 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float CurrentExperience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float InPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float ExpericenCap = 20.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int CharacterLevel = 0;

public:
	//ĳ���� ��ų ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int SkillLevel_1 = 1; //SpellLevelFire

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int SkillLevel_2 = 0; //SpellLevelIce

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int SkillLevel_3 = 0; //SpellLevelLight

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int SkillLevel_4 = 0; //SpellLevelDark

	//��ų ���� Ŭ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillActor")
	TSubclassOf<AActor> SpawnToSkill1;

private:
	UPROPERTY(EditAnywhere, Category = "Rotaion")
	FRotator PivotRotation;
};
