// Fill out your copyright notice in the Description page of Project Settings.


#include "HealerEnemy.h"
#include "GameCharacter.h"
#include "Components/SplineComponent.h"
#include "Components/AudioComponent.h"
#include <Sound/SoundCue.h>
#include "Creature.h"

AHealerEnemy::AHealerEnemy() {
	DeflateAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("DeflateAudioComp"));
	DeflateAudioComp->SetupAttachment(RootComponent);
}

void AHealerEnemy::BeginPlay() {
	Super::BeginPlay();

	MaterialInstance = GetCurrentBodyMesh()->CreateDynamicMaterialInstance(0);

	FHashedMaterialParameterInfo ParameterInfo("Color");
	MaterialInstance->GetVectorParameterValue(ParameterInfo, defaultColor);


	GetCurrentBodyMesh()->SetRelativeScale3D(defaultBodyScale);

	if (DeflateAudioComp && deflateSound) {
		UE_LOG(LogTemp, Warning, TEXT("pop sound is okay"));
		DeflateAudioComp->SetSound(deflateSound);
	}

	GetCurrentBodyMesh()->OnComponentBeginOverlap.AddUniqueDynamic(this, &AHealerEnemy::TriggerEnter);
	GetCurrentBodyMesh()->OnComponentEndOverlap.AddUniqueDynamic(this, &AHealerEnemy::TriggerExit);
}

void AHealerEnemy::TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {

	overlappingGameCharacter = Cast<AGameCharacter>(OtherActor);

	if (canHealingStarted && actualStatus != EnemyStatus::Healing && actualStatus != EnemyStatus::SpecialDying && overlappingGameCharacter != nullptr && overlappingGameCharacter->GetStatus() == GameCharacterStatus::DownDash) {
		actualStatus = EnemyStatus::Healing;
		startTime = GetWorld()->GetTimeSeconds();
		startScale = GetCurrentBodyMesh()->GetRelativeScale3D();
		endScale = FVector::OneVector;
		creature->HealingStarted();

		DeflateAudioComp->Play(0.f);

		canHealingStarted = false;
	}
}

void AHealerEnemy::TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	if (OtherActor && OtherActor != this) {
		if (OtherActor->IsA(AGameCharacter::StaticClass())) {

			overlappingGameCharacter = nullptr;

			canPlayerDamageMe = false;
		}
	}
}

void AHealerEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (actualStatus == EnemyStatus::Healing) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= timeForHeal;

		if (currentTime >= 1.f) {
			currentTime = 1.f;
			actualStatus = EnemyStatus::Eating;
		}


		FVector newScale = FMath::Lerp(startScale, endScale, currentTime);
		GetCurrentBodyMesh()->SetRelativeScale3D(newScale);
		FLinearColor currentColor = FMath::Lerp(defaultColor, targetColor, currentTime);
		MaterialInstance->SetVectorParameterValue("Color", currentColor);


		currentTime = GetWorld()->GetTimeSeconds() - startTimeForHealingSphere;
		currentTime *= 2.f;
		if (currentTime >= 1.f)
			startTimeForHealingSphere = GetWorld()->GetTimeSeconds();

		float SplineLength = splineComponent->GetSplineLength();
		float SplineDistance = SplineLength * currentTime;
		FVector Position = splineComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);

		SwallowSphere->SetWorldLocation(Position);
	}
}

UStaticMeshComponent* AHealerEnemy::GetCurrentBodyMesh() {
	return Body12;
}

float AHealerEnemy::GetPercentageOfMaxLifeToHealBack() {
	return percentageOfMaxLifeToHealBack;
}

float AHealerEnemy::GetTimeForHeal() {
	return timeForHeal;
}