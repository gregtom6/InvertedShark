// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperEnemy.h"
#include <Creature.h>
#include <Projectile.h>
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"


ASniperEnemy::ASniperEnemy() {
	PrimaryActorTick.bCanEverTick = true;

	actualStatus = EnemyStatus::Initial;

	ProjectileOrigin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileOrigin"));
	ProjectileOrigin->SetupAttachment(RootComponent);

	SkeletalBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalBody"));
	SkeletalBody->SetupAttachment(RootComponent);

	smokeNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("smokeNiagara"));
	smokeNiagara->SetupAttachment(RootComponent);
}

void ASniperEnemy::BeginPlay() {
	Super::BeginPlay();	

	SniperMaterialInstance = SkeletalBody->CreateDynamicMaterialInstance(7);

	FHashedMaterialParameterInfo ParameterInfo("Color");
	SniperMaterialInstance->GetVectorParameterValue(ParameterInfo, defaultColor);

}

void ASniperEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (actualStatus == EnemyStatus::Targeting) {

		currentTime= GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= targetingTime;

		FLinearColor currentColor = FMath::Lerp(defaultColor, targetColor, currentTime);
		SniperMaterialInstance->SetVectorParameterValue("Color", currentColor);

		if (currentTime >= targetingPercentageWhenSmokeNeeds) {
			smokeNiagara->Activate();
		}

		if (currentTime >= 1.f) {
			currentTime = 1.f;
			startTime = GetWorld()->GetTimeSeconds();
			CreateProjectile();
			smokeNiagara->Deactivate();
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