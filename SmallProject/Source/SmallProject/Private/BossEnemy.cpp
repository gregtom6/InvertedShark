// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "BossUserWidget.h"

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