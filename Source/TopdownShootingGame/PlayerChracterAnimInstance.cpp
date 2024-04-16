// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerChracterAnimInstance.h"
#include "PlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UPlayerChracterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	//ĳ���� ��ü �ʱ�ȭ 
	PlayerCharacter = Cast<APlayerCharacter>(TryGetPawnOwner());
	if (PlayerCharacter != nullptr)//�����Ͱ� nullptr ���� Ȯ��
	{
		PlayerCharacterMovement = PlayerCharacter->GetCharacterMovement(); //ĳ���� �����Ʈ ������Ʈ ��ü �ʱ�ȭ
		//UE_LOG(LogTemp, Warning, TEXT("Player Character - initialize")); //�����
	}

}

void UPlayerChracterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);


	if (PlayerCharacterMovement != nullptr)
	{
		Velocity = PlayerCharacterMovement->Velocity;
		GroundSpeed = UKismetMathLibrary::VSizeXY(Velocity);
		//UE_LOG(LogTemp, Warning, TEXT("Ground Speed Succeed!")); //����� 
		ShouldMove = IsMove();
		IsFalling = PlayerCharacterMovement->IsFalling();
	}
}

bool UPlayerChracterAnimInstance::IsMove()
{
	if (PlayerCharacterMovement == nullptr) { return false; }
	return GroundSpeed > 3.f && !PlayerCharacterMovement->GetCurrentAcceleration().Equals(FVector::ZeroVector, 0);
}
