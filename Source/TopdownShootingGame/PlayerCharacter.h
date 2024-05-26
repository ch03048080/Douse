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

	//UWorld* World;

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
	//�� ����
	FTimerHandle EnemySpawnTimerHandle;
	FTimerDelegate EnemySpawnDelegate;
	//���� �÷��� Ÿ�̸�
	FTimerHandle GamePlayTimerHandle;
	FTimerDelegate GamePlayDelegate;

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	TSubclassOf<class UUserWidget> StageClearWidgetClass;

	//���� �ν��Ͻ��� ������ ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite,  Category = "Widget")
	UUserWidget* PlayerWidget; //WBP_Player
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* DeathScreenWidget; //WBP_DeathScreen
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* LevelingWidget; //WBP_Leveling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* TopRightHUDWidget; //WBP_TopRightHUD
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	UUserWidget* StageClearWidget; //WBP_StageClearWidget

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
	int SkillLevel_1 = -1; //SpellLevelFire

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

public:

	//�� ���� ��� ����
	//�� Ŭ���� �޾ƿ���
	UClass* Enemy1_Dragon3;
	FString Dragon3ActorClassPath = "/Game/Blueprints/Enemies/BPMaster_Enemies.BPMaster_Enemies_C";

	UClass* Enemy2_BabyDragon;
	FString BabyDragonActorClassPath = "/Game/Blueprints/Enemies/BPMaster_Enemies_Child_Enemy2_BabyDragon.BPMaster_Enemies_Child_Enemy2_BabyDragon_C";

	UClass* Enemy3_GoldDragon;
	FString GoldDragonActorClassPath = "/Game/Blueprints/Enemies/BPMaster_Enemies_Child_Enemy3_GoldDragon.BPMaster_Enemies_Child_Enemy3_GoldDragon_C";

	UClass* Enemy4_MaxDragon3;
	FString MaxDragon3ActorClassPath = "/Game/Blueprints/Enemies/BPMaster_Enemies_Child_Enemy4_MaxDragon.BPMaster_Enemies_Child_Enemy4_MaxDragon_C";
	ACharacter* SpawnedEnemy; // ������ Enemy ����

	UFUNCTION(BlueprintCallable, Category = "SpawnEnemyTimeline")
	void SpawnEnemy(); 
	void StartSpawnEnemy();
	float SpawnInterval = 0.0f;
	float SpawnIntervalDecreaseRate = 0.0f;
	float MinSpawnInterval = 0.0f;
	///FVector SpawnLocation, FRotator SpawnRotation
	void SelectRandomSpawnLocation();
	FVector SpawnLocation;
	FRotator SpawnRotation;


//���� �÷��� Ÿ�̸� ���
	float TimerDuration = 0.0f;
	float CurrentTime = 0.0f;

	UFUNCTION(BlueprintCallable, Category = "GamePlayTimer")
	void UpdateTimer();
	void OnTimerEnd();

	//Ÿ�̸� ���� �Լ�
	UFUNCTION(BlueprintCallable, Category = "ResetTimer")
	void ResetTimer();

	//�÷��̾� ���ھ� ��� �Լ�
	int PlayerScore = 0;
	void CalculatePlayerScore();
};
