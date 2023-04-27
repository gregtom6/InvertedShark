// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperEnemy.h"
#include <Creature.h>
#include <Projectile.h>

ASniperEnemy::ASniperEnemy() {
	PrimaryActorTick.bCanEverTick = true;

	actualStatus = EnemyStatus::Initial;

	ProjectileOrigin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileOrigin"));
	ProjectileOrigin->SetupAttachment(RootComponent);
}

void ASniperEnemy::BeginPlay() {
	Super::BeginPlay();	
}

void ASniperEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (actualStatus == EnemyStatus::Targeting) {

		currentTime= GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= targetingTime;

		if (currentTime >= 1.f) {
			currentTime = 1.f;
			shootStartTime = GetWorld()->GetTimeSeconds();
			actualStatus = EnemyStatus::Shooting;
			CreateProjectile();
		}

	}
	else if (actualStatus == EnemyStatus::Shooting) {
		currentTime = GetWorld()->GetTimeSeconds() - shootStartTime;

		currentTime /= shootTime;

		if (currentTime >= 1.f) {
			currentTime = 1.f;
			shootStartTime = GetWorld()->GetTimeSeconds();
			actualStatus = EnemyStatus::Shooting;
			CreateProjectile();
		}
	}
}

void ASniperEnemy::CreateProjectile() {
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(ProjectileOrigin->GetComponentLocation());
	SpawnTransform.SetRotation(FQuat::Identity);

	// Get a reference to the Blueprint class
	UClass* BPClass = LoadClass<AActor>(nullptr, TEXT("/Game/Blueprints/BP_SniperProjectile.BP_SniperProjectile_C"));

	// Spawn the actor and store a reference to the new instance
	AProjectile* NewActor = GetWorld()->SpawnActor<AProjectile>(BPClass, SpawnTransform);

	NewActor->SetTarget(creature);
}

FVector ASniperEnemy::GetEndPosition() {
	FVector directionVector = creature->GetActorLocation()-GetActorLocation();
	directionVector.Normalize();

	FVector endPosition = creature->GetActorLocation() - directionVector * distanceFromCreature;
	return endPosition;
}

void ASniperEnemy::MovingToCreatureEnded() {
	TargetingCreature();
}

void ASniperEnemy::TargetingCreature() {
	startTime = GetWorld()->GetTimeSeconds();
	actualStatus = EnemyStatus::Targeting;
}