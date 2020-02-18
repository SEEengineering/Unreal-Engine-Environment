// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "NavigationSystem.h"
#include "MotionControllerComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VRRoot);
	LeftController->SetTrackingSource(EControllerHand::Left);

	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VRRoot);
	RightController->SetTrackingSource(EControllerHand::Right);

	TeleportPath = CreateDefaultSubobject<USplineComponent>(TEXT("TeleportPath"));
	TeleportPath->SetupAttachment(RightController);


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


bool AVRCharacter::FindTeleportDestination(TArray<FVector> &OutPath, FVector &OutLocation) {

	FVector Start = RightController->GetComponentLocation();
	FVector Look = RightController->GetForwardVector();
	

	FPredictProjectilePathParams Params(
		TeleportProjectileRadius,
		Start,
		Look * TeleportProjectileSpeed,
		TeleportSimulationTime,
		ECollisionChannel::ECC_Visibility,
		this
	);

	Params.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	Params.bTraceComplex = true;
	FPredictProjectilePathResult Result;
	bool bHit = UGameplayStatics::PredictProjectilePath(this, Params, Result);


	if (!bHit) return false;

	for (FPredictProjectilePathPointData PointData : Result.PathData)
	{
		OutPath.Add(PointData.Location);
	}


	FNavLocation NavLocation;
	bool bOnNavMesh = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld())->ProjectPointToNavigation(Result.HitResult.Location, NavLocation, TeleportProjectionExtent);

	if (!bOnNavMesh) return false;

	OutLocation = NavLocation.Location;

	return true;
}



void AVRCharacter::UpdateDestinationMarker()
{
	TArray<FVector> Path;
	FVector Location;
	bool bHasDestinationMarker = FindTeleportDestination(Path, Location);

	if (bHasDestinationMarker)
	{
		DestinationMarker->SetVisibility(true);

		DestinationMarker->SetWorldLocation(Location);

		DrawTeleportPath(Path);
	}
	else
	{
		DestinationMarker->SetVisibility(false);
	}
}


void AVRCharacter::DrawTeleportPath(const TArray<FVector> &Path) 
{
	UpdateSpline(Path);

	for (int32 i = 0; i < Path.Num(); ++i)
	{
		if (TeleportPathMeshPool.Num() <= i)
		{
			UStaticMeshComponent* DynamicMesh = NewObject<UStaticMeshComponent>(this);
			DynamicMesh->AttachToComponent(VRRoot, FAttachmentTransformRules::KeepRelativeTransform);
			DynamicMesh->SetStaticMesh(TeleportArchMesh);
			DynamicMesh->SetMaterial(0, TeleportArchMaterial);
			DynamicMesh->RegisterComponent();

			TeleportPathMeshPool.Add(DynamicMesh);
		}

		UStaticMeshComponent*DynamicMesh = TeleportPathMeshPool[i];

		DynamicMesh->SetWorldLocation(Path[i]);

	}




}



void AVRCharacter::UpdateSpline(const TArray<FVector> &Path)
{
	TeleportPath->ClearSplinePoints(false);

	for (int32 i = 0; i < Path.Num(); ++i)
	{
		FVector LocalPosition = TeleportPath->GetComponentTransform().InverseTransformPosition(Path[i]);
		FSplinePoint Point(i, LocalPosition, ESplinePointType::Curve);
		TeleportPath->AddPoint(Point, false);
	}

	TeleportPath->UpdateSpline();
}



// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Teleport"), IE_Released, this, &AVRCharacter::BeginTeleport);
}

void AVRCharacter::MoveForward(float throttle)
{
	AddMovementInput(throttle * Camera->GetForwardVector());
}

void AVRCharacter::MoveRight(float throttle)
{
	AddMovementInput(throttle * Camera->GetRightVector());
}


void AVRCharacter::BeginTeleport()
{
	StartFade(0, 1);

	
	FTimerHandle Handle;
	GetWorldTimerManager().SetTimer(Handle, this, &AVRCharacter::FinishTeleport, TeleportFadeTime);
}

void AVRCharacter::FinishTeleport()
{
	SetActorLocation(DestinationMarker->GetComponentLocation() + GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

	StartFade(1, 0);
}


void AVRCharacter::StartFade(float FromAlpha, float ToAlpha)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC != nullptr)
	{
		PC->PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, TeleportFadeTime, FLinearColor::Black);
	}
}
