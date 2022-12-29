// Fill out your copyright notice in the Description page of Project Settings.

#include "GameCharacter.h"
#include "EnemyTriggerBox.h"

AEnemyTriggerBox::AEnemyTriggerBox() {
	OnActorBeginOverlap.AddDynamic(this, &AEnemyTriggerBox::Event);
}

void AEnemyTriggerBox::BeginPlay() {
	Super::BeginPlay();
}

void AEnemyTriggerBox::Event(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this && otherActor->IsA(AGameCharacter::StaticClass())) {
		UE_LOG(LogTemp, Warning, TEXT("event meghivva"));
	}
}