// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyTriggerBox.h"
#include "GameCharacter.h"

AEnemyTriggerBox::AEnemyTriggerBox() {
	OnActorBeginOverlap.AddDynamic(this, &AEnemyTriggerBox::Event);
}

void AEnemyTriggerBox::BeginPlay() {
	Super::BeginPlay();
}

void AEnemyTriggerBox::Event(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {

		if (otherActor->IsA(ACreature::StaticClass())) {
			UE_LOG(LogTemp, Warning, TEXT("event meghivva"));

			for (int i = 0; i < enemiesToCome.Num(); i++) {
				enemiesToCome[i]->MoveToCreature();
			}
		}
		if (otherActor->IsA(AEnemy::StaticClass())) {
			AEnemy* arrivedEnemy = Cast<AEnemy>(otherActor);
			UE_LOG(LogTemp, Warning, TEXT("eves elotti"));
			if (arrivedEnemy != nullptr) {
				UE_LOG(LogTemp, Warning, TEXT("eves"));
				arrivedEnemy->StartEating();
			}
		}
		
	}
}