// Fill out your copyright notice in the Description page of Project Settings.


#include "FallFollower.h"

// Sets default values
AFallFollower::AFallFollower()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
}

// Called when the game starts or when spawned
void AFallFollower::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFallFollower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);




}

// Called to bind functionality to input
void AFallFollower::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

