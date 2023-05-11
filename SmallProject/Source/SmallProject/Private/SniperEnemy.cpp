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


ASniperEnemy::ASniperEnemy(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer) {

	PrimaryActorTick.bCanEverTick = true;

	actualStatus = EnemyStatus::Initial;
	enemyTargeting = EnemyTargeting::CreatureTargeting;

	ProjectileOrigin = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileOrigin"));
	ProjectileOrigin->SetupAttachment(RootComponent);

	SkeletalBody = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalBody"));
	SkeletalBody->SetupAttachment(RootComponent);

	smokeNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("smokeNiagara"));
	smokeNiagara->SetupAttachment(RootComponent);

	laserTargetingNiagara1 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("laserTargetingNiagara1"));
	laserTargetingNiagara1->AttachToComponent(SkeletalBody, FAttachmentTransformRules::KeepRelativeTransform);

	dieTornadoNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("dieTornadoNiagara"));

	weaponOverloadingSound1 = CreateDefaultSubobject<UAudioComponent>(TEXT("weaponOverloadingSound"));
	weaponOverloadingSound1->SetupAttachment(RootComponent);

	loopedMosquitoSound = CreateDefaultSubobject<UAudioComponent>(TEXT("loopedMosquitoSound"));
	loopedMosquitoSound->SetupAttachment(RootComponent);

	dyingBeeSound = CreateDefaultSubobject<UAudioComponent>(TEXT("dyingBeeSound"));
	dyingBeeSound->SetupAttachment(RootComponent);
}

void ASniperEnemy::BeginPlay() {
	Super::BeginPlay();

	SniperMaterialInstance = SkeletalBody->CreateDynamicMaterialInstance(7);

	FHashedMaterialParameterInfo ParameterInfo("Color");
	SniperMaterialInstance->GetVectorParameterValue(ParameterInfo, defaultColor);

	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	APawn* pawn = OurPlayerController->GetPawn();

	gameCharacter = Cast<AGameCharacter, APawn>(pawn);

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation());

	windZone = GetWorld()->SpawnActor<AWindZone>(WindzoneClass, SpawnTransform);

	projectile = GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnTransform);
}

void ASniperEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (actualStatus == EnemyStatus::Moving) {
		FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), creature->GetActorLocation());
		SetActorRotation(targetRotation);
	}

	else if (actualStatus == EnemyStatus::Targeting) {

		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= targetingTime;

		FLinearColor currentColor = FMath::Lerp(defaultColor, targetColor, currentTime);
		SniperMaterialInstance->SetVectorParameterValue("Color", currentColor);

		if (currentTime >= targetingPercentageWhenSmokeNeeds) {
			smokeNiagara->Activate();

			laserTargetingNiagara1->Activate();

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
			currentTarget = gameCharacter->GetActorLocation();
		}
		else {
			enemyTargeting = EnemyTargeting::CreatureTargeting;
			gameCharacter->NotifyTargeting(false);
			currentTarget = creature->GetActorLocation();
		}

		if (enemyTargeting == EnemyTargeting::PlayerTargeting) {
			FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), gameCharacter->GetActorLocation());
			SetActorRotation(targetRotation);
		}
		else if (enemyTargeting == EnemyTargeting::CreatureTargeting) {
			FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), creature->GetActorLocation());
			SetActorRotation(targetRotation);
		}
	}
	else if (actualStatus == EnemyStatus::SniperDying) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= dieRotatingTime;

		FRotator rotatorDelta(0.0f, FMath::Lerp(0.f, dieRotatingSpeed, currentTime), 0.0f);

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

			startTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (actualStatus == EnemyStatus::SniperDead) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= levelRemoveTime;

		if (currentTime >= 1.f) {
			Destroy();
		}
	}

	if (laserTargetingNiagara1->IsActive()) {
		ECollisionChannel CollisionChannel = ECC_WorldDynamic; // Set the desired collision channel for the line trace

		FHitResult HitResult;
		FCollisionQueryParams TraceParams(FName(TEXT("LineTraceByChannel")), true);

		// Perform the line trace
		bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, laserTargetingNiagara1->GetComponentLocation(), FVector(1000.f, 0.f, 0.f), CollisionChannel, TraceParams);

		float dist = FVector::Distance(laserTargetingNiagara1->GetComponentLocation(), currentTarget);

		laserTargetingNiagara1->SetVectorParameter(FName(TEXT("User.Beam End")), FVector(dist, 0.f, 0.f));

	}

	TimeManagement();
}

void ASniperEnemy::TimeManagement() {
	if (gameCharacter == nullptr) { return; }

	loopedMosquitoSound->SetPitchMultiplier(gameCharacter->GetCurrentSoundPitchMultiplier());
	weaponOverloadingSound1->SetPitchMultiplier(gameCharacter->GetCurrentSoundPitchMultiplier());
}

void ASniperEnemy::CreateWindZone() {

	windZone->SetActorLocation(GetActorLocation());
	windZone->Activate(true);
}

void ASniperEnemy::CreateProjectile() {

	projectile->SetActorLocation(ProjectileOrigin->GetComponentLocation());

	laserTargetingNiagara1->Deactivate();

	if (enemyTargeting == EnemyTargeting::CreatureTargeting)
		projectile->SetTarget(creature, this);
	else if (enemyTargeting == EnemyTargeting::PlayerTargeting) {
		projectile->SetTarget(gameCharacter, this);
	}
}

FVector ASniperEnemy::GetEndPosition() const {
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

	loopedMosquitoSound->Play();
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

	loopedMosquitoSound->Stop();

	dyingBeeSound->Play();

	CreateWindZone();
}