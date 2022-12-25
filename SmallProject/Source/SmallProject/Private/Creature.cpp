// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Components/WidgetComponent.h"
#include "CreatureUserWidget.h"

// Sets default values
ACreature::ACreature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	if (RootComponent == nullptr)
	{
		RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	}

	HealthWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("Healthbar"));
	HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	Health = MaxHealth;
	/*
	UUserWidget* wid = HealthWidgetComp->GetUserWidgetObject();

	if (wid != nullptr) {


		UCreatureUserWidget* widg = Cast<UCreatureUserWidget>(wid);

		UE_LOG(LogTemp, Warning, TEXT("UUserWidget nem null"));

		if (widg != nullptr) {
			widg->creature = this;
		}

	}
	*/
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UUserWidget* wid = HealthWidgetComp->GetUserWidgetObject();

	if (wid != nullptr) {


		UCreatureUserWidget* widg = Cast<UCreatureUserWidget>(wid);

		UE_LOG(LogTemp, Warning, TEXT("UUserWidget nem null"));

		widg->creature = this;

		//UCreatureUserWidget* dynamic_cast<UCreatureUserWidget*> (widg);

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

