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

	//XP 관련 UI 업데이트 처리
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void AddCoinExpericence(float PickupValue);
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void StartSelectingSkills(); //스킬 레벨 업 UI 시작 시 
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void ReturnToGame(); // 스킬 레벨 업 UI 종료 
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TopRightHUD_1(); // UI Update 함수
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TopRightHUD_2(); // UI Update 함수
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TopRightHUD_3(); // UI Update 함수
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void TopRightHUD_4(); // UI Update 함수

	//스킬 출력
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
	//카메라, 스프링 암 
	USpringArmComponent* CameraBoom;
	UCameraComponent* ViewCamera;

	//위젯 클래스를 저장할 멤버 변수 선언
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> PlayerWidgetClass; 
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> DeathScreenWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> LevelingWidgetClass;
	UPROPERTY(EditAnywhere, Category = "Widget")
	TSubclassOf<class UUserWidget> TopRightHUDClass;

	//위젯 인스턴스를 저장할 변수 선언
	UUserWidget* PlayerWidget; //WBP_Player
	UUserWidget* DeathScreenWidget; //WBP_DeathScreen
	UUserWidget* LevelingWidget; //WBP_Leveling
	UUserWidget* TopRightHUDWidget; //WBP_TopRightHUD

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Player")
	float PlayerHealthPercentage;


	//캐릭터 XP 관련 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float CurrentExperience;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float InPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	float ExpericenCap = 20.0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int CharacterLevel = 0;

public:
	//캐릭터 스킬 레벨
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int SkillLevel_1 = 1; //SpellLevelFire

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int SkillLevel_2 = 0; //SpellLevelIce

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int SkillLevel_3 = 0; //SpellLevelLight

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Experience")
	int SkillLevel_4 = 0; //SpellLevelDark

	//스킬 액터 클래스
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SkillActor")
	TSubclassOf<AActor> SpawnToSkill1;

private:
	UPROPERTY(EditAnywhere, Category = "Rotaion")
	FRotator PivotRotation;
};
