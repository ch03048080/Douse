// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "PlayerChracterAnimInstance.generated.h"

UCLASS()
class TOPDOWNSHOOTINGGAME_API UPlayerChracterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;
	bool IsMove();
	UPROPERTY(BlueprintReadOnly)
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* PlayerCharacterMovement;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	FVector Velocity;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool ShouldMove;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	bool IsFalling;

};
