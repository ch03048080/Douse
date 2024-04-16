// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChracterAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UPlayerChracterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//캐릭터 객체 초기화 
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter != nullptr)//포인터가 nullptr 인지 확인
	{
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement(); //캐릭터 무브먼트 컴포넌트 객체 초기화
		//UE_LOG(LogTemp, Warning, TEXT("Player Character - initialize")); //디버깅
	}

}

void UPlayerChracterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);


	if (PlayerCharacterMovement != nullptr)
	{
		Velocity = PlayerCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		//UE_LOG(LogTemp, Warning, TEXT("Ground Speed Succeed!")); //디버깅 
		ShouldMove = IsMove();
		IsFalling = PlayerCharacterMovement->IsFalling();
	}
}

bool UPlayerChracterAnimInstance::IsMove()
{
	if (PlayerCharacterMovement == nullptr) { return false; }
	return GroundSpeed > 3.f && !PlayerCharacterMovement->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0);
}
