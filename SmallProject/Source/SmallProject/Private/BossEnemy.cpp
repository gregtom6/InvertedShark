// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "BossUserWidget.h"
#include <Kismet/GameplayStatics.h>

ABossEnemy::ABossEnemy(const FObjectInitializer& ObjectInitializer)
{
	
	BossWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("bossbar"));

	OnActorBeginOverlap.AddDynamic(this, &AEnemy::EnterEvent);
}

void ABossEnemy::BeginPlay() {
	Super::BeginPlay();

	if (BossWidgetComp != nullptr) {
		UUserWidget* wid = BossWidgetComp->GetUserWidgetObject();

		if (wid != nullptr) {


			UBossUserWidget* widg = Cast<UBossUserWidget>(wid);

			UE_LOG(LogTemp, Warning, TEXT("UUserWidget nem null"));

			widg->boss = this;

			if (widg != nullptr) {
				UE_LOG(LogTemp, Warning, TEXT("creatureuserwidget nem null"));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("creatureuserwidget null"));
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UUserWidget null"));
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
			FLatentActionInfo latentInfo;
			UGameplayStatics::OpenLevel(this, levelToLoadAfterDefeat, true);
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

void ABossEnemy::RemoveEnemy() {
	UE_LOG(LogTemp, Warning, TEXT("victory"));

	startTime= GetWorld()->GetTimeSeconds();
	startScale = GetActorScale3D();
	endScale = FVector(0.f, 0.f, 0.f);
	actualStatus = EnemyStatus::SpecialDying;
}