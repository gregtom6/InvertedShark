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
#include "ActorSequenceComponent.h"
#include "ActorSequencePlayer.h"
#include <WindZone.h>


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

	dieTornadoNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("dieTornadoNiagara"));
	//dieTornadoNiagara2->SetupAttachment(RootComponent);

	weaponOverloadingSound1 = CreateDefaultSubobject<UAudioComponent>(TEXT("weaponOverloadingSound"));
	weaponOverloadingSound1->SetupAttachment(RootComponent);
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

			if (weaponLoadingSequence && !SkeletalBody->IsPlaying())
			{
				SkeletalBody->PlayAnimation(weaponLoadingSequence, false);
			}
		}

		if (currentTime >= targetingPercentageWhenAudioNeeds && !soundAlreadyStartedPlaying) {
			weaponOverloadingSound1->Play(startWeaponOverloadingSoundPercentage);
			soundAlreadyStartedPlaying = true;
		}

		if (currentTime >= 1.f) {
			currentTime = 1.f;
			startTime = GetWorld()->GetTimeSeconds();
			CreateProjectile();
			smokeNiagara->Deactivate();
			soundAlreadyStartedPlaying = false;
			
			if (weaponShootingSequence)
			{
				SkeletalBody->PlayAnimation(weaponShootingSequence, false);
			}

		}


		float distanceBetweenMeAndCreature = FVector::Distance(GetActorLocation(), creature->GetActorLocation());
		float distanceBetweenMeAndPlayer = FVector::Distance(GetActorLocation(), gameCharacter->GetActorLocation());

		float relatedDist = distanceBetweenMeAndPlayer / distanceBetweenMeAndCreature;
		if (relatedDist <= distancePercentageAfterTargetingPlayer) {
			enemyTargeting = EnemyTargeting::PlayerTargeting;
			gameCharacter->NotifyTargeting(true);
		}
		else {
			enemyTargeting = EnemyTargeting::CreatureTargeting;
			gameCharacter->NotifyTargeting(false);
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
	else if (actualStatus == EnemyStatus::SniperDying) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= dieRotatingTime;

		FRotator rotatorDelta(0.0f, FMath::Lerp(0.f, dieRotatingSpeed,currentTime), 0.0f);

		SkeletalBody->AddRelativeRotation(rotatorDelta);

		Body12->AddRelativeRotation(rotatorDelta);

		if (currentTime >= 1.f) {

			if (SwallowSphere != nullptr)
				SwallowSphere->DestroyComponent();

			SkeletalBody->Stop();

			SkeletalBody->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			Body12->SetSimulatePhysics(true);
			SkeletalBody->SetSimulatePhysics(true);

			for (int i = 0; i < SMeshContainers.Num(); i++) {
				SMeshContainers[i]->SetSimulatePhysics(true);
			}


			if (EyePivot2 != nullptr)
				EyePivot2->SetSimulatePhysics(true);

			dieTornadoNiagara->Deactivate();

			actualStatus = EnemyStatus::SniperDead;
		}
	}
}

void ASniperEnemy::CreateWindZone() {
	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation());
	SpawnTransform.SetRotation(FQuat::Identity);

	// Get a reference to the Blueprint class
	UClass* BPClass = LoadClass<ATriggerBox>(nullptr, TEXT("/Game/Blueprints/BP_WindZone.BP_WindZone_C"));

	// Spawn the actor and store a reference to the new instance
	AWindZone* NewActor = GetWorld()->SpawnActor<AWindZone>(BPClass, SpawnTransform);

	if (NewActor == nullptr) {
		FRotator rot = FRotator::ZeroRotator;
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
		NewActor->SetTarget(creature, this);
	else if (enemyTargeting == EnemyTargeting::PlayerTargeting) {
		NewActor->SetTarget(gameCharacter, this);
	}
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

void ASniperEnemy::RemoveEnemy() {

	actualStatus = EnemyStatus::SniperDying;

	startTime = GetWorld()->GetTimeSeconds();

	FRotator rot = FRotator::ZeroRotator;
	SetActorRotation(rot);

	if (dieStartedSequence)
	{
		SkeletalBody->PlayAnimation(dieStartedSequence, false);
	}

	smokeNiagara->Deactivate();

	dieTornadoNiagara->SetWorldLocation(GetActorLocation());
	dieTornadoNiagara->Activate();

	CreateWindZone();
}