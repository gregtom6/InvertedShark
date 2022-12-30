// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Components/WidgetComponent.h"
#include "CreatureUserWidget.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
ACreature::ACreature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	/*
	if (RootComponent == nullptr)
	{
		RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	}
	*/
	HealthWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("Healthbar"));
	//HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Health = MaxHealth;
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();

	if (HealthWidgetComp != nullptr) {
		UUserWidget* wid = HealthWidgetComp->GetUserWidgetObject();

		if (wid != nullptr) {


			UCreatureUserWidget* widg = Cast<UCreatureUserWidget>(wid);

			UE_LOG(LogTemp, Warning, TEXT("UUserWidget nem null"));

			widg->creature = this;

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

	actualStartPosition = GetActorLocation();
	actualEndPosition = positionsToMove[0]->GetActorLocation();

	startTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	actualStatus = Status::Initial;
}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (actualStatus == Status::Initial) {
		float currentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - startTime;
		if (currentTime >= waitTimeBeforeFirstMove) {
			startTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
			actualStatus = Status::Moving;
		}
	}
	else if (actualStatus == Status::Moving) {
		float currentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - startTime;
		currentTime *= movementSpeed;
		if (currentTime > 1.f) {

			actualStatus = Status::Stopped;
			currentTime = 1.f;
		}

		UE_LOG(LogTemp, Warning, TEXT("%lf"), currentTime);

		SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, currentTime));
	}


	
	Health = Health > 0 ? Health - (deltaDamage * DeltaTime) : 0;

	if (Health <= 0) {
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

