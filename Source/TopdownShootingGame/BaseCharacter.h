// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "BaseCharacter.generated.h"

UCLASS()
class TOPDOWNSHOOTINGGAME_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	/*Enhanced input*/
	//#include "InputActionValue.h"
	UPROPERTY(VisibleAnywhere, Category = Input)
		class UInputMappingContext* DefaultContext;
	UPROPERTY(VisibleAnywhere, Category = Input)
		class UInputAction* MoveAction;
	//UPROPERTY(EditAnywhere, Category = Input)
	//	UInputAction* JumpAction;
	void Move(const FInputActionValue& Value);
	//void Jump() override;

};
