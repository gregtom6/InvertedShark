// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "CreatureUserWidget.h"
#include "Blueprint/UserWidget.h"

// Sets default values
ACreature::ACreature()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("lefutok"));
	
	if (HUDClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("lefutok2"));
		LifeBarWidget = CreateWidget(GetWorld(), HUDClass);
		if (LifeBarWidget)
		{
			LifeBarWidget->AddToViewport();
		}
	}
	
}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//UE_LOG(LogTemp, Warning, TEXT("Hello"));
}

