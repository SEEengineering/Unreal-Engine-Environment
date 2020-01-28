// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"

// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	DestinationMarker = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DestinationMarker"));
	DestinationMarker->SetupAttachment(GetRootComponent());

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();

	DestinationMarker->SetVisibility(false);
	
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector NewCameraOffset = Camera->GetComponentLocation() - GetActorLocation();
	NewCameraOffset.Z = 0;
	AddActorWorldOffset(NewCameraOffset);
	VRRoot->AddWorldOffset(-NewCameraOffset);

	UpdateDestinationMarker();

}
void AVRCharacter::UpdateDestinationMarker()
{

	FVector Start = Camera->GetComponentLocation();
  
  //current location of the camera and multiplied how far we wanna ray trace
	FVector End = Start + Camera->GetForwardVector()* MaxTeleportDistance;
  
  //Structure containing information about  point of impact and surface normal at that point.
	FHitResult HitResult;
	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility);

	if (bHit)
	{
		DestinationMarker->SetVisibility(true);

		DestinationMarker->SetWorldLocation(HitResult.Location);
	}
	else
	{
		DestinationMarker->SetVisibility(false);
	}
}

// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::MoveRight);

}

void AVRCharacter::MoveForward(float throttle) {

	AddMovementInput(throttle * Camera->GetForwardVector());

}
void AVRCharacter::MoveRight(float throttle) {

	AddMovementInput(throttle * Camera->GetRightVector());

}


}


