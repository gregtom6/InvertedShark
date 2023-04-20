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

	TArray<UStaticMeshComponent*> StaticMeshComponents;
	GetComponents<UStaticMeshComponent>(StaticMeshComponents);

	for (int i = 0; i < StaticMeshComponents.Num(); i++)
	{
		if (StaticMeshComponents[i]->GetFName() == FName("Body"))
		{
			BodyMesh = StaticMeshComponents[i];
			break;
		}
	}

	MaterialInstance = GetCurrentBodyMesh()->CreateDynamicMaterialInstance(0);

	FHashedMaterialParameterInfo ParameterInfo("Color");
	MaterialInstance->GetVectorParameterValue(ParameterInfo, defaultColor);


	GetCurrentBodyMesh()->SetRelativeScale3D(defaultBodyScale);

	if (DeflateAudioComp && deflateSound) {
		UE_LOG(LogTemp, Warning, TEXT("pop sound is okay"));
		DeflateAudioComp->SetSound(deflateSound);
	}
}

void AHealerEnemy::EnterEvent(class AActor* overlappedActor, class AActor* otherActor) {
	Super::EnterEvent(overlappedActor, otherActor);

	overlappingGameCharacter = Cast<AGameCharacter>(otherActor);

	if (canHealingStarted && actualStatus != EnemyStatus::Healing && overlappingGameCharacter != nullptr && overlappingGameCharacter->GetStatus() == GameCharacterStatus::Calm) {
		actualStatus = EnemyStatus::Healing;
		startTime = GetWorld()->GetTimeSeconds();
		startScale = GetCurrentBodyMesh()->GetRelativeScale3D();
		endScale = FVector::OneVector;
		creature->HealingStarted();

		DeflateAudioComp->Play(0.f);

		canHealingStarted = false;
	}
}

void AHealerEnemy::ExitEvent(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {
		if (otherActor->IsA(AGameCharacter::StaticClass())) {

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
	return BodyMesh;
}

float AHealerEnemy::GetPercentageOfMaxLifeToHealBack() {
	return percentageOfMaxLifeToHealBack;
}

float AHealerEnemy::GetTimeForHeal() {
	return timeForHeal;
}