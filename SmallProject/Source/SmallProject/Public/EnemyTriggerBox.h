// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "EnemyTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API AEnemyTriggerBox : public ATriggerBox
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
		TArray<class AEnemy*> enemiesToCome;

	UPROPERTY(EditAnywhere)
		float maxMoveTime = 3.f;
public:
	AEnemyTriggerBox();

	UFUNCTION()
		void Event(class AActor* overlappedActor, class AActor* otherActor);

};
