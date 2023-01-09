// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "BossUserWidget.h"
#include <Kismet/GameplayStatics.h>

ABossEnemy::ABossEnemy(const FObjectInitializer& ObjectInitializer)
{
	OnActorBeginOverlap.AddDynamic(this, &AEnemy::EnterEvent);
}

void ABossEnemy::BeginPlay() {
	Super::BeginPlay();	

	UE_LOG(LogTemp, Warning, TEXT("bossenemy1"));

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("bossenemy2"));
		bossuserwidget = Cast<UBossUserWidget>(CreateWidget(GetWorld(), widgetclass));

		if (bossuserwidget != nullptr) {

			bossuserwidget->boss = this;

			UE_LOG(LogTemp, Warning, TEXT("bossenemy3"));
			bossuserwidget->AddToViewport(0);
		}
	}
}

void ABossEnemy::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	if (actualStatus == EnemyStatus::SpecialDying) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= dyingTime;

		if (currentTime >= 1.f)
			currentTime = 1.f;

		SetActorScale3D(FMath::Lerp(startScale, endScale, currentTime));

		if (currentTime >= 1.f) {
			DoAfterDead();
		}
	}
}

FVector ABossEnemy::GetEndPosition() {
	FVector vec = creature->GetActorLocation();
	double z = GetActorLocation().Z;
	vec.Z = z;
	return vec;
}

float ABossEnemy::GetLife() {
	return actualLife;
}

float ABossEnemy::GetMaxLife() {
	return maxLife;
}

void ABossEnemy::DoAfterDead() {
	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, levelToLoadAfterDefeat, true);
}