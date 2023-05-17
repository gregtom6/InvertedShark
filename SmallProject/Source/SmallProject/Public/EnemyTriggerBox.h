// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "EnemyTriggerBox.generated.h"

class AEnemy;

/**
 * when Creature reaches one of these, the enemies assigned to
 * enemiesToCome will come and attack the Creature
 */
UCLASS()
class SMALLPROJECT_API AEnemyTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
		TArray<AEnemy*> enemiesToCome;

	UPROPERTY(EditAnywhere)
		float minMoveTime = 3.f;

	UPROPERTY(EditAnywhere)
		float maxMoveTime = 5.f;
public:

	UFUNCTION()
		void Event(AActor* overlappedActor, AActor* otherActor);

	UFUNCTION()
		bool AreAllEnemiesDefeated();
};
