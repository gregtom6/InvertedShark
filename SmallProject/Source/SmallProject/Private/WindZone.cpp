// Fill out your copyright notice in the Description page of Project Settings.


#include "WindZone.h"
#include "GameCharacter.h"
#include "ResourceDataAsset.h"

AWindZone::AWindZone(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	PrimaryActorTick.bCanEverTick = true;

	globalSettings = NewObject<UResourceDataAsset>(GetTransientPackage(), FName("globalSettings"));
}

// Called when the game starts or when spawned
void AWindZone::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddUniqueDynamic(this, &AWindZone::EnterEvent);
	OnActorEndOverlap.AddUniqueDynamic(this, &AWindZone::ExitEvent);
}

void AWindZone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnActorBeginOverlap.RemoveDynamic(this, &AWindZone::EnterEvent);
	OnActorEndOverlap.RemoveDynamic(this, &AWindZone::ExitEvent);
}

// Called every frame
void AWindZone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!isActivated) { return; }

	if (gameCharacter == nullptr) { return; }

	UStaticMeshComponent* skeletal = gameCharacter->GetStaticMeshComponent();

	FVector WindDirection = FVector(0.0f, 0.0f, 1.0f) * FMath::RandRange(minWindStrength,maxWindStrength);
 	skeletal->AddImpulse(WindDirection);

	float currentTime = GetWorld()->GetTimeSeconds() - startTime;
	currentTime /= dieTime;

	if (currentTime >= globalSettings->FullPercent) {
		Destroy();
	}
}

void AWindZone::EnterEvent(class AActor* overlappedActor, class AActor* otherActor) {

	if (otherActor && otherActor != this) {
		if (otherActor->IsA(AGameCharacter::StaticClass())) {

			gameCharacter = Cast<AGameCharacter>(otherActor);
		}
	}
}

void AWindZone::ExitEvent(class AActor* overlappedActor, class AActor* otherActor) {
	
	if (otherActor && otherActor != this) {
		if (otherActor->IsA(AGameCharacter::StaticClass())) {

			gameCharacter = nullptr;
		}
	}
}

void AWindZone::Activate(bool isActive) {

	startTime = GetWorld()->GetTimeSeconds();

	isActivated = isActive;
}