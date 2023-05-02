// Fill out your copyright notice in the Description page of Project Settings.


#include "SniperEnemy.h"
#include <Creature.h>
#include <Projectile.h>
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include <Kismet/GameplayStatics.h>
#include "GameCharacter.h"
#include <Kismet/KismetMathLibrary.h>


ASniperEnemy::ASniperEnemy() {
	PrimaryActorTick.bCanEverTick = true;

	actualStatus = EnemyStatus::Initial;
	enemyTargeting = EnemyTargeting::CreatureTargeting;

	ProjectileOrigin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileOrigin"));
	ProjectileOrigin->SetupAttachment(RootComponent);

	SkeletalBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalBody"));
	SkeletalBody->SetupAttachment(RootComponent);

	smokeNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("smokeNiagara"));
	smokeNiagara->SetupAttachment(RootComponent);

	weaponOverloadingSound = CreateDefaultSubobject<UAudioComponent>(TEXT("weaponOverloadingSound"));
	weaponOverloadingSound->SetupAttachment(RootComponent);
}

void ASniperEnemy::BeginPlay() {
	Super::BeginPlay();

	SniperMaterialInstance = SkeletalBody->CreateDynamicMaterialInstance(7);

	FHashedMaterialParameterInfo ParameterInfo("Color");
	SniperMaterialInstance->GetVectorParameterValue(ParameterInfo, defaultColor);

	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	APawn* pawn = OurPlayerController->GetPawn();

	gameCharacter = Cast<AGameCharacter, APawn>(pawn);

}

void ASniperEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (actualStatus == EnemyStatus::Targeting) {

		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= targetingTime;

		FLinearColor currentColor = FMath::Lerp(defaultColor, targetColor, currentTime);
		SniperMaterialInstance->SetVectorParameterValue("Color", currentColor);

		if (currentTime >= targetingPercentageWhenSmokeNeeds) {
			smokeNiagara->Activate();
		}

		if (currentTime >= targetingPercentageWhenAudioNeeds && !soundAlreadyStartedPlaying) {
			weaponOverloadingSound->Play(startWeaponOverloadingSoundPercentage);
			soundAlreadyStartedPlaying = true;
		}

		if (currentTime >= 1.f) {
			currentTime = 1.f;
			startTime = GetWorld()->GetTimeSeconds();
			CreateProjectile();
			smokeNiagara->Deactivate();
			soundAlreadyStartedPlaying = false;
		}


		float distanceBetweenMeAndCreature = FVector::Distance(GetActorLocation(), creature->GetActorLocation());
		float distanceBetweenMeAndPlayer = FVector::Distance(GetActorLocation(), gameCharacter->GetActorLocation());

		float relatedDist = distanceBetweenMeAndPlayer / distanceBetweenMeAndCreature;
		if (relatedDist <= distancePercentageAfterTargetingPlayer) {
			enemyTargeting = EnemyTargeting::PlayerTargeting;
		}
		else {
			enemyTargeting = EnemyTargeting::CreatureTargeting;
		}

		if (enemyTargeting == EnemyTargeting::PlayerTargeting) {
			FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), gameCharacter->GetActorLocation());
			//targetRotation.Yaw += 90.f;
			SetActorRotation(targetRotation);
		}
		else if (enemyTargeting == EnemyTargeting::CreatureTargeting) {
			FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), creature->GetActorLocation());
			//targetRotation.Yaw += 90.f;
			SetActorRotation(targetRotation);
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

	if (enemyTargeting == EnemyTargeting::CreatureTargeting)
		NewActor->SetTarget(creature);
	else if (enemyTargeting == EnemyTargeting::PlayerTargeting)
		NewActor->SetTarget(gameCharacter);
}

FVector ASniperEnemy::GetEndPosition() {
	FVector directionVector = creature->GetActorLocation() - GetActorLocation();
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