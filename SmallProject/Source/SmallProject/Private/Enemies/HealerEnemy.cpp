// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/HealerEnemy.h"
#include "Allies/GameCharacter.h"
#include "Components/SplineComponent.h"
#include "Components/AudioComponent.h"
#include <Sound/SoundCue.h>
#include "Allies/Creature.h"
#include "Materials/MaterialInterface.h"
#include "DataAssets/ResourceDataAsset.h"
#include <Kismet/GameplayStatics.h>

AHealerEnemy::AHealerEnemy(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer) {
	DeflateAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("DeflateAudioComp"));
	DeflateAudioComp->SetupAttachment(RootComponent);
}

void AHealerEnemy::BeginPlay() {
	Super::BeginPlay();

	UStaticMeshComponent* bodyMesh = GetCurrentBodyMesh();

	MaterialInstance = bodyMesh->CreateDynamicMaterialInstance(0);

	FHashedMaterialParameterInfo ParameterInfo("Color");
	MaterialInstance->GetVectorParameterValue(ParameterInfo, defaultColor);

	bodyMesh->SetRelativeScale3D(defaultBodyScale);

	originalHealingSphereScale = SwallowSphere->GetRelativeScale3D();

	bodyMesh->OnComponentBeginOverlap.AddUniqueDynamic(this, &AHealerEnemy::TriggerEnter);
	bodyMesh->OnComponentEndOverlap.AddUniqueDynamic(this, &AHealerEnemy::TriggerExit);
}

void AHealerEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	GetCurrentBodyMesh()->OnComponentBeginOverlap.RemoveDynamic(this, &AHealerEnemy::TriggerEnter);
	GetCurrentBodyMesh()->OnComponentEndOverlap.RemoveDynamic(this, &AHealerEnemy::TriggerExit);
}

void AHealerEnemy::TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	overlappingGameCharacter = Cast<AGameCharacter>(OtherActor);

	if (bCanHealingStarted && actualStatus != EEnemyStatus::Healing && actualStatus != EEnemyStatus::SpecialDying && overlappingGameCharacter && overlappingGameCharacter->GetStatus() == EGameCharacterStatus::DownDash) {
		actualStatus = EEnemyStatus::Healing;
		startTime = GetWorld()->GetTimeSeconds();
		startScale = GetCurrentBodyMesh()->GetRelativeScale3D();
		endScale = FVector::OneVector;
		creature->HealingStarted();

		DeflateAudioComp->Play(0.f);

		overlappingGameCharacter->SlowdownTime();
		overlappingGameCharacter->PlayCameraShake();

		bCanHealingStarted = false;
	}
}

void AHealerEnemy::TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor && OtherActor != this) {
		if (OtherActor->IsA(AGameCharacter::StaticClass())) {

			overlappingGameCharacter = nullptr;
		}
	}
}

void AHealerEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (actualStatus == EEnemyStatus::Healing) {

		HealingSphereManagement();

		BodyManagement();

		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= timeForHeal;

		if (currentTime >= globalSettings->FullPercent) {
			currentTime = globalSettings->FullPercent;
			actualStatus = EEnemyStatus::Eating;
			SwallowSphere->SetRelativeScale3D(originalHealingSphereScale);
			SwallowSphere->SetMaterial(0, originalSwallowSphereMaterial);

			GetCurrentBodyMesh()->SetRelativeScale3D(endScale);
			MaterialInstance->SetVectorParameterValue("Color", targetColor);
		}


		float deltaHeal = creature->GetMaxHealth() * percentageOfMaxLifeToHealBack;

		float t = GetWorld()->GetTimeSeconds() - startTime;

		float actualTime = t / timeForHeal;

		if (creature->GetActualHealthWhenStartedHealing() + deltaHeal > creature->GetMaxHealth()) {
			deltaHeal = creature->GetMaxHealth() - creature->GetActualHealthWhenStartedHealing();
		}

		float DamageAmount = deltaHeal * actualTime;

		FHitResult HitOut;

		UGameplayStatics::ApplyPointDamage(creature, DamageAmount, FVector::OneVector, HitOut, GetInstigatorController(), this, healDamageType);
	
	}

	TimeManagement();
}

void AHealerEnemy::HealingSphereManagement() {
	currentTime = GetWorld()->GetTimeSeconds() - startTimeForHealingSphere;
	float fasterCurrentTime = currentTime * 2.f;
	if (fasterCurrentTime >= globalSettings->FullPercent) {
		startTimeForHealingSphere = GetWorld()->GetTimeSeconds();
		SwallowSphere->SetRelativeScale3D(originalHealingSphereScale * 2.f);
		SwallowSphere->SetMaterial(0, healingSwallowSphereMaterial);
	}

	float SplineLength = splineComponent->GetSplineLength();
	float SplineDistance = SplineLength * fasterCurrentTime;
	FVector Position = splineComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);

	SwallowSphere->SetWorldLocation(Position);
}

void AHealerEnemy::BodyManagement() {

	currentTime = GetWorld()->GetTimeSeconds() - startTime;

	currentTime /= timeForHeal;

	FVector newScale = FMath::Lerp(startScale, endScale, currentTime);
	GetCurrentBodyMesh()->SetRelativeScale3D(newScale);
	FLinearColor currentColor = FMath::Lerp(defaultColor, targetColor, currentTime);
	MaterialInstance->SetVectorParameterValue("Color", currentColor);
}

void AHealerEnemy::TimeManagement() {

	if (!gameCharacter) { return; }

	DeflateAudioComp->SetPitchMultiplier(gameCharacter->GetCurrentSoundPitchMultiplier());
}

UStaticMeshComponent* AHealerEnemy::GetCurrentBodyMesh() const {
	return Body12;
}

float AHealerEnemy::GetPercentageOfMaxLifeToHealBack() const {
	return percentageOfMaxLifeToHealBack;
}

float AHealerEnemy::GetTimeForHeal() const {
	return timeForHeal;
}