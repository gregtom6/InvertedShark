// Fill out your copyright notice in the Description page of Project Settings.


#include "GameChar2.h"

// Sets default values
AGameChar2::AGameChar2()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}

// Called when the game starts or when spawned
void AGameChar2::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGameChar2::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AGameChar2::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

