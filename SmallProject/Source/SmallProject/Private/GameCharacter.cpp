// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AGameCharacter::AGameCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create components
	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	LeftArmPhysicsConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LeftArm"));
	RightArmPhysicsConstraint = CreateDefaultSubobject< UPhysicsConstraintComponent>(TEXT("RightArm"));


	Camera->FieldOfView = 120.f;
	Camera->SetupAttachment(CameraMesh);
	Camera->SetRelativeLocation(FVector(-100.0f, 0.0f, 50.0f));

	LeftArmPhysicsConstraint->SetupAttachment(CameraMesh);
	RightArmPhysicsConstraint->SetupAttachment(CameraMesh);

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACreature::StaticClass(), FoundActors);

	for (int i = 0; i < FoundActors.Num(); i++) {
		ACreature* creature = Cast<ACreature>(FoundActors[i]);

		if (creature != nullptr) {
			UE_LOG(LogTemp, Warning, TEXT("creature fizikahoz megkapva"));
		}

		RightArmPhysicsConstraint->ConstraintActor1 = creature;
		FConstrainComponentPropName name;
		name.ComponentName = "StaticMesh";
		RightArmPhysicsConstraint->ComponentName1 = name;

		RightArmPhysicsConstraint->ConstraintActor2 = this;
		FConstrainComponentPropName name2;
		name2.ComponentName = CameraMesh->GetFName();
		RightArmPhysicsConstraint->ComponentName2 = name2;
	}
}

void AGameCharacter::RotateLR(float rotateDelta) {
	FRotator actualRotation = GetActorRotation();
	actualRotation.Yaw += rotateDelta * RotateSpeed;
	SetActorRotation(actualRotation);
}

void AGameCharacter::StrafeLR(float movementDelta) {
	FVector rightVector = GetActorRightVector();
	FVector upVector = GetActorUpVector();
	FVector forwardVector = GetActorForwardVector();

	FVector newLocation = GetActorLocation();
	//rightVector.Y += movementDelta * MovementSpeed;

	SetActorLocation(newLocation+(rightVector*movementDelta*MovementSpeed));
}

void AGameCharacter::WingBeat() {

	FVector actorForwardVector = GetActorForwardVector() * ForwardFlyStrength;
	FVector actorUpVector = GetActorUpVector() * WingStrength;
	FVector impulseDirection = actorUpVector + actorForwardVector;

	CameraMesh->AddImpulse(impulseDirection);
}

// Called when the game starts or when spawned
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	startPos = GetActorLocation();

	//PhysicsConstraint.atta
}

// Called every frame
void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector currentVelocity = CameraMesh->GetPhysicsLinearVelocity();
	FVector clampedVelocity = currentVelocity.GetClampedToMaxSize(velocityLimit);
	CameraMesh->SetPhysicsLinearVelocity(clampedVelocity);

	FVector actorLocation = GetActorLocation();
	//UE_LOG(LogTemp, Log, TEXT("height %lf"), actorLocation.Z);

	if (actorLocation.Z <= heightToDie) {
		//SetActorLocation(startPos);
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

// Called to bind functionality to input
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//register for LR movement
	PlayerInputComponent->BindAxis(TEXT("StrafeLR"), this, &AGameCharacter::StrafeLR);

	PlayerInputComponent->BindAxis(TEXT("RotateLR"), this, &AGameCharacter::RotateLR);

	PlayerInputComponent->BindAction(TEXT("WingBeat"), IE_Pressed, this, &AGameCharacter::WingBeat);
}

