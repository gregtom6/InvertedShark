// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WindZone.h"
#include "Allies/GameCharacter.h"
#include "DataAssets/ResourceDataAsset.h"

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

	if (!bIsActivated || !gameCharacter) { return; }

	UStaticMeshComponent* skeletal = gameCharacter->GetStaticMeshComponent();

	FVector WindDirection = direction * FMath::RandRange(WindStrength.Min,WindStrength.Max);
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

void AWindZone::Activate(const bool isActive) {

	startTime = GetWorld()->GetTimeSeconds();

	bIsActivated = isActive;
}