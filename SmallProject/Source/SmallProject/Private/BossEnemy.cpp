// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"

ABossEnemy::ABossEnemy() {
	
	OnActorBeginOverlap.AddDynamic(this, &AEnemy::EnterEvent);
}

void ABossEnemy::BeginPlay() {
	Super::BeginPlay();
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