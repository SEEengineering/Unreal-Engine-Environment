// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Strokes.generated.h"

UCLASS()
class LIGHTPAINTING_API AStrokes : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStrokes();

	void Update(FVector CursorLocation);

private:
	FTransform GetNextSegmentTransform(FVector CurrentLocation) const;

	FVector GetNextSegmentScale(FVector CurrentLocation) const;
	FQuat GetNextSegmentRotation(FVector CurrentLocation) const;
	FVector GetNextSegmentLocation(FVector CurrentLocation) const;

	UPROPERTY(VisibleAnywhere)
		USceneComponent* Root;

	// Config
	UPROPERTY(VisibleAnywhere)
		UInstancedStaticMeshComponent* StrokeMeshes;

	// State
	FVector PreviousCursorLocation;
};
