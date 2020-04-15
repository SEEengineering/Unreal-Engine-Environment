// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Strokes.h"
#include "MotionControllerComponent.h"


#include "HandController.generated.h"

UCLASS()
class LIGHTPAINTING_API AHandController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHandController();

	void TriggerPressed();
	void TriggerReleased();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	// Config
	UPROPERTY(EditAnywhere)
		TSubclassOf<AStrokes> StrokeClass;

	// Components
	UPROPERTY(VisibleAnywhere)
	UMotionControllerComponent* MotionController;

	AStrokes* CurrentStroke;

};
