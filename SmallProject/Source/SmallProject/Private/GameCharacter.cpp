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

	TongueAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("TongueAudio"));

	Tongue->SetupAttachment(CameraMesh);

	Camera->FieldOfView = 120.f;
	Camera->SetupAttachment(CameraMesh);
	Camera->SetRelativeLocation(FVector(-100.0f, 0.0f, 50.0f));

	LeftLeft->SetupAttachment(CameraMesh);
	RightArm->SetupAttachment(CameraMesh);

	AudioComp->SetupAttachment(CameraMesh);

	TongueAudio->SetupAttachment(CameraMesh);

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
		AudioComp->Play(0.f);
	}
}

void AGameCharacter::HugCreature() {

	if (!isHugging) {
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACreature::StaticClass(), FoundActors);

		if (FoundActors.Num() > 0) {
			ACreature* creature = Cast<ACreature>(FoundActors[0]);

			if (creature != nullptr && creature->IsCharacterInFur()) {

				creature->GetHugged();

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
	}
	else {

		RightArm->BreakConstraint();

		isHugging = false;
	}

}

void AGameCharacter::Attack() {

	if (actualStatus == GameCharacterStatus::Attack) { return; }

	UE_LOG(LogTemp, Warning, TEXT("attack happened"));

	Tongue->SetVisibility(true);
	startTime = GetWorld()->GetTimeSeconds();
	prevStatus = actualStatus;
	actualStatus = GameCharacterStatus::Attack;

	TongueAudio->Play(0.f);
}

void AGameCharacter::Pause() {

	if (pauseStatus == PauseStatus::Played) {
		UE_LOG(LogTemp, Warning, TEXT("pause activate"));
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		pauseStatus = PauseStatus::Paused;

		APlayerController* PC = Cast<APlayerController>(GetController());

		if (PC)
		{
			PC->bShowMouseCursor = true;
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;
		}

		widgetPauseMenuInstance->AddToViewport();
	}
	else if (pauseStatus == PauseStatus::Paused) {
		UE_LOG(LogTemp, Warning, TEXT("pause deactivate"));
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		pauseStatus = PauseStatus::Played;

		APlayerController* PC = Cast<APlayerController>(GetController());

		if (PC)
		{
			PC->bShowMouseCursor = false;
			PC->bEnableClickEvents = false;
			PC->bEnableMouseOverEvents = false;
		}

		widgetPauseMenuInstance->RemoveFromViewport();
	}
}

// Called when the game starts or when spawned
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AudioComp && wingBeat) {
		AudioComp->SetSound(wingBeat);
	}

	if (TongueAudio && tongueSound) {
		TongueAudio->SetSound(tongueSound);
	}

	widgetPauseMenuInstance = CreateWidget<UPauseUserWidget>(GetWorld(), widgetPauseMenu);

	if (widgetPauseMenuInstance != nullptr)
		widgetPauseMenuInstance->SetGameCharacter(this);

	startPos = GetActorLocation();

	Tongue->SetVisibility(false);

	actualEnergy = maxEnergy;

	actualStatus = GameCharacterStatus::Calm;
	prevStatus = GameCharacterStatus::Calm;

	pauseStatus = PauseStatus::Played;

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("energy1"));
		energyuserwidget = Cast<UGameCharacterUserWidget>(CreateWidget(GetWorld(), widgetclass));

		if (energyuserwidget != nullptr) {

			energyuserwidget->player = this;

			UE_LOG(LogTemp, Warning, TEXT("energy2"));
			energyuserwidget->AddToViewport(0);
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

	if (actualStatus == GameCharacterStatus::Attack) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= attackTime) {
			Tongue->SetVisibility(false);
			prevStatus = actualStatus;
			actualStatus = GameCharacterStatus::Calm;
		}
	}

	float newEnergy = actualEnergy + energyRegeneration * DeltaTime;
	float restMult = 1.f;

	if (isHugging || (currentVelocity.X < restVelocity && currentVelocity.Y < restVelocity && currentVelocity.Z < restVelocity)) {
		restMult = restingMultiplier;
	}

	if (newEnergy <= maxEnergy)
		actualEnergy += energyRegeneration * restMult * DeltaTime;

	if (actorLocation.Z <= heightToDie) {
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

GameCharacterStatus AGameCharacter::GetPrevStatus() {
	return prevStatus;
}

void AGameCharacter::SetPrevStatusToActualStatus() {
	prevStatus = actualStatus;
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

	FInputActionBinding& toggle = PlayerInputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AGameCharacter::Pause);
	toggle.bExecuteWhenPaused = true;
}

