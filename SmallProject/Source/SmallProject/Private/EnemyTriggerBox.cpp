// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyTriggerBox.h"
#include "GameCharacter.h"
#include <Enemy.h>
#include <Creature.h>
#include "GameFramework/Actor.h"
#include <SniperEnemy.h>

void AEnemyTriggerBox::BeginPlay() {
	Super::BeginPlay();

	OnActorBeginOverlap.AddUniqueDynamic(this, &AEnemyTriggerBox::Event);
}

void AEnemyTriggerBox::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	OnActorBeginOverlap.RemoveDynamic(this, &AEnemyTriggerBox::Event);
}

/*
telling the enemies to attack creature, when this trigger box has been reached by creature
*/
void AEnemyTriggerBox::Event(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {

		if (otherActor->IsA(ACreature::StaticClass())) {
			UE_LOG(LogTemp, Warning, TEXT("event meghivva"));

			if (enemiesToCome.Num() > 0)
				enemiesToCome[0]->MoveToCreature(3.f);

			for (int i = 1; i < enemiesToCome.Num(); i++) {
				enemiesToCome[i]->MoveToCreature(FMath::RandRange(3.f,maxMoveTime));
			}
		}
		if (otherActor->IsA(AEnemy::StaticClass())) {
			AEnemy* arrivedEnemy = Cast<AEnemy>(otherActor);
			UE_LOG(LogTemp, Warning, TEXT("eves elotti"));
			if (arrivedEnemy != nullptr) {
				UE_LOG(LogTemp, Warning, TEXT("eves"));

				if (!arrivedEnemy->IsA(ASniperEnemy::StaticClass())) {
					arrivedEnemy->StartEating();
				}
			}
		}
		
	}
}