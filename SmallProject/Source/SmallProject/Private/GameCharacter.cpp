// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <GameCharacterUserWidget.h>
#include "Components/AudioComponent.h"


// Sets default values
AGameCharacter::AGameCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//create components
	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	LeftLeft = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LeftLeft"));
	RightArm = CreateDefaultSubobject< UPhysicsConstraintComponent>(TEXT("RightArm"));

	Tongue = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tongue"));

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));

	Tongue->SetupAttachment(CameraMesh);

	Camera->FieldOfView = 120.f;
	Camera->SetupAttachment(CameraMesh);
	Camera->SetRelativeLocation(FVector(-100.0f, 0.0f, 50.0f));

	LeftLeft->SetupAttachment(CameraMesh);
	RightArm->SetupAttachment(CameraMesh);

	AudioComp->SetupAttachment(CameraMesh);

	EnergyWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("Energybar"));

	isHugging = false;
}

void AGameCharacter::RotateLR(float rotateDelta) {

	if (isHugging) { return; }

	FRotator actualRotation = GetActorRotation();
	actualRotation.Yaw += rotateDelta * RotateSpeed;
	SetActorRotation(actualRotation);
}

void AGameCharacter::StrafeLR(float movementDelta) {

	if (isHugging) { return; }

	FVector rightVector = GetActorRightVector();
	FVector upVector = GetActorUpVector();
	FVector forwardVector = GetActorForwardVector();

	FVector newLocation = GetActorLocation();
	//rightVector.Y += movementDelta * MovementSpeed;

	SetActorLocation(newLocation + (rightVector * movementDelta * MovementSpeed));
}

void AGameCharacter::WingBeat() {

	if (isHugging) { return; }

	if (actualEnergy < energyDecreaseAfterWingBeat) { return; }

	FVector actorForwardVector = GetActorForwardVector() * ForwardFlyStrength;
	FVector actorUpVector = GetActorUpVector() * WingStrength;
	FVector impulseDirection = actorUpVector + actorForwardVector;

	CameraMesh->AddImpulse(impulseDirection);

	actualEnergy -= energyDecreaseAfterWingBeat;

	if (AudioComp && wingBeat) {
		UE_LOG(LogTemp, Warning, TEXT("hang jo"));
	}

	AudioComp->Play(0.f);
}

void AGameCharacter::HugCreature() {

	if (!isHugging) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACreature::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0) {
			ACreature* creature = Cast<ACreature>(FoundActors[0]);

			if (creature != nullptr) {
				UE_LOG(LogTemp, Warning, TEXT("creature fizikahoz megkapva"));

				creature->GetHugged();
			}

			RightArm->ConstraintActor1 = this;
			FConstrainComponentPropName name1;
			name1.ComponentName = CameraMesh->GetFName();
			RightArm->ComponentName1 = name1;

			RightArm->ConstraintActor2 = creature;
			FConstrainComponentPropName name2;
			name2.ComponentName = "StaticMesh";
			RightArm->ComponentName2 = name2;

			CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
			CameraMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

			RightArm->SetWorldLocation(creature->GetLocation());

			RightArm->InitComponentConstraint();

			isHugging = true;
		}
	}
	else {

		RightArm->BreakConstraint();

		isHugging = false;
	}

}

void AGameCharacter::Attack() {
	UE_LOG(LogTemp, Warning, TEXT("attack tortent"));

	Tongue->SetVisibility(true);
	startTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	actualStatus = GameCharacterStatus::Attack;

}

// Called when the game starts or when spawned
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AudioComp && wingBeat) {
		UE_LOG(LogTemp, Warning, TEXT("hang jo"));
	AudioComp->SetSound(wingBeat);
	}


	startPos = GetActorLocation();

	Tongue->SetVisibility(false);

	actualEnergy = maxEnergy;

	actualStatus = GameCharacterStatus::Calm;

	if (EnergyWidgetComp != nullptr) {
		UUserWidget* wid = EnergyWidgetComp->GetUserWidgetObject();

		if (wid != nullptr) {


			UGameCharacterUserWidget* widg = Cast<UGameCharacterUserWidget>(wid);

			UE_LOG(LogTemp, Warning, TEXT("UUserWidget nem null"));

			widg->player = this;

			if (widg != nullptr) {
				UE_LOG(LogTemp, Warning, TEXT("creatureuserwidget nem null"));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("creatureuserwidget null"));
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UUserWidget null"));
		}
	}
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

	if (actualStatus == GameCharacterStatus::Attack) {

		float currentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - startTime;

		if (currentTime >= attackTime) {
			Tongue->SetVisibility(false);
			actualStatus = GameCharacterStatus::Calm;
		}
	}

	float newEnergy = actualEnergy + energyRegeneration * DeltaTime;
	if (newEnergy <= maxEnergy)
		actualEnergy += energyRegeneration * DeltaTime;

	if (actorLocation.Z <= heightToDie) {
		//SetActorLocation(startPos);
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

float AGameCharacter::GetEnergy() {
	return actualEnergy;
}

float AGameCharacter::GetMaxEnergy() {
	return maxEnergy;
}

GameCharacterStatus AGameCharacter::GetStatus() {
	return actualStatus;
}

// Called to bind functionality to input
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//register for LR movement
	PlayerInputComponent->BindAxis(TEXT("StrafeLR"), this, &AGameCharacter::StrafeLR);

	PlayerInputComponent->BindAxis(TEXT("RotateLR"), this, &AGameCharacter::RotateLR);

	PlayerInputComponent->BindAction(TEXT("WingBeat"), IE_Pressed, this, &AGameCharacter::WingBeat);

	PlayerInputComponent->BindAction(TEXT("HugCreature"), IE_Pressed, this, &AGameCharacter::HugCreature);

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AGameCharacter::Attack);
}

