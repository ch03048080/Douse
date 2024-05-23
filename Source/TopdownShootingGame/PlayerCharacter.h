// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/SphereComponent.h"
#include "Components/TimelineComponent.h"
#include "PlayerCharacter.generated.h"

DECLARE_DYNAMIC_DELEGATE(FSkillDelegate);

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

	// Ÿ�̸� ���� �Լ�
	//UFUNCTION()
	void StartSkill1Timer();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void StartSkill2Timer();
private:
	// Ÿ�̸ӿ��� ȣ��Ǵ� �Լ�
	//UFUNCTION(BlueprintCallable, Category = "SkillDelegate") //��������Ʈ �Լ��� UFUNCTION �ȳ����� ũ���� �� 
	//void Skill1Function(); //��ų 1 ��������Ʈ�� ���ε� �� �Լ�
	//UFUNCTION(BlueprintCallable, Category = "SkillDelegate")
	//void Skill2Function();
	
	//Ÿ�̸� �ڵ鷯, ��������Ʈ ���� 
	//Skill 1 
	FTimerHandle TimerHandle;
	FTimerDelegate SkillDelegate;
	//Skill 2
	FTimerHandle TimerHandle2;
	FTimerDelegate SkillDelegate2;
	//Skill 3
	FTimerHandle TimerHandle3;
	FTimerDelegate SkillDelegate3;
public:
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

	//��ų ��� �Լ�
	UFUNCTION(BlueprintCallable, Category = "Skill")//��ų 1 ��������Ʈ�� ���ε��� �Լ�
	void Skill_1();
	UFUNCTION(BlueprintCallable, Category = "Skill")//��ų 2 ��������Ʈ�� ���ε��� �Լ�
	void Skill_2();
	UFUNCTION(BlueprintCallable, Category = "Skill")//��ų 3 ��������Ʈ�� ���ε��� �Լ�
	void Skill_3();
	UFUNCTION(BlueprintCallable, Category = "Skill")//��ų 4 ��������Ʈ�� ���ε��� �Լ�
	void Skill_4();

	void SpawnSkill_1(int NumProjectile, float StartRotation, float RotIncrement); //SpellSpawnFireballs
	void SpawnSkill_2(int NumProjectile, float StartRotation, float RotIncrement); //SpellSpawnIceballs
	void InitializeSkill_3(int NumProjectile, float Rotation); //SpellSpawnlightballs
	
	//��ų 3 ���� �����
	UClass* Skill3ActorClass;
	void DestroyAllActorsOfClass(UWorld* World, TSubclassOf<AActor> ActorClass);

	UFUNCTION(BlueprintCallable, Category = "Skill")
	void StartRotatingLight();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timeline") //��ų ���͸� ȸ�� ��Ű�� ���� Ÿ�Ӷ��� ������Ʈ
	UTimelineComponent* SkillRotationTimeline;

	
	UPROPERTY(EditAnywhere, Category = "Skill")
	USceneComponent* Skill3PivotComponent;

	UPROPERTY(EditAnywhere, Category = "Skill")
	USceneComponent* Skill3SpawnComponent;

	UPROPERTY(EditAnywhere, Category = "Skill")
	FOnTimelineFloat ProgressFunction; //Ÿ�Ӷ��� 
	
	UPROPERTY(EditAnywhere, Category = "Skill")
	FOnTimelineEvent TimelineFinishedEvent; //Ÿ�Ӷ���

	UPROPERTY(EditAnywhere, Category = "Timeline") //��ų ���͸� ȸ�� ��Ű�� ���� Ÿ�Ӷ��� Ŀ��
	UCurveFloat* RotationCurve;

	UFUNCTION(BlueprintCallable, Category = "Skill")//��ų 3 Start �Լ�
	void StartSkill_3();
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void UpdateRotationProgress(float Value);
	UFUNCTION(BlueprintCallable, Category = "Skill")
	void OnRotationComplete();

	void SpawnSkill_4(); //SpellSpawnDarkballs

	
private:
	//ī�޶�, ������ �� 
	USpringArmComponent* CameraBoom;
	UCameraComponent* ViewCamera;

public:
	//���� Ŭ������ ������ ��� ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerWidgetClass; 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> DeathScreenWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> LevelingWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> TopRightHUDClass;

	//���� �ν��Ͻ��� ������ ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "Widget")
	UUserWidget* PlayerWidget; //WBP_Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* DeathScreenWidget; //WBP_DeathScreen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* LevelingWidget; //WBP_Leveling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* TopRightHUDWidget; //WBP_TopRightHUD

protected:	
	virtual void BeginPlay() override;
	
	// �����Է� - ĳ���� �̵� �� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, Meta = (AllowPrivateAccess = "true"))	
		TObjectPtr<class UInputMappingContext> DefaultMappingContext; //�ø�������� �Ʒ��� ���ϳ� ������. 	
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
	UCapsuleComponent* PlayerCapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
	UStaticMeshComponent* CrosshairMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") //�⺻ ��ų �Ǻ� ����
	USceneComponent* SkillPivot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") //��ų �Ǻ� ����
	USceneComponent* Skill3Pivot;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components") //��ų 3 ���� �Ǻ� ����
	USceneComponent* RotatingSkillSpawn;

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
	float ExpericenCap = 50.0;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillActor")
	TSubclassOf<AActor> SpawnToSkill3;

	

private:
	//UPROPERTY(EditAnywhere, Category = "Rotaion")
	//FRotator PivotRotation;

	float DistanceToSpawnSkillActor;
};
