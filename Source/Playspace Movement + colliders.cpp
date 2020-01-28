#include "VRCharacter.h"
#include "Camera/CameraComponent.h"


// Sets default values
AVRCharacter::AVRCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

  //takes a template parameter and attaches to the parent
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);

}

// Called when the game starts or when spawned
void AVRCharacter::BeginPlay()
{
	Super::BeginPlay();
  
  //A vector in 3-D space composed of components (X, Y, Z) with floating point precision.
  Fvector NewCameraOffset = Camera->GetComponenetLocation() - GetActorLocation();
  
  NewCameraOffset.Z = 0;
  
  AddActorWorldOffset(NewCameraOffset);
  VRroot->AddActorWorldOffset(-NewCameraOffset);

	
}

// Called every frame
void AVRCharacter::Tick(float DeltaTime)
{

}


// Called to bind functionality to input
void AVRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

  //binding text macro to text move forward
	PlayerInputComponent->BindAxis(TEXT("Forward"), this, &AVRCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AVRCharacter::MoveRight);


}

void AVRCharacter::MoveForward(float throttle) {

	AddMovementInput(throttle * Camera->GetForwardVector());

}
void AVRCharacter::MoveRight(float throttle) {

	AddMovementInput(throttle * Camera->GetRightVector());

}

