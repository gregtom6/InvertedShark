// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/TriggerBox.h"
#include <Enemy.h>
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
		TArray<AEnemy*> enemiesToCome;
public:
	AEnemyTriggerBox();

	UFUNCTION()
		void Event(class AActor* overlappedActor, class AActor* otherActor);

};
