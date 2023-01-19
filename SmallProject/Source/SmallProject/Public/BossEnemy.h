// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "BossEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API ABossEnemy : public AEnemy
{
	GENERATED_BODY()

public:
	ABossEnemy(const FObjectInitializer& ObjectInitializer);
	
protected:

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> widgetclass;

	UPROPERTY(VisibleInstanceOnly)
		class UBossUserWidget* bossuserwidget;

	UPROPERTY(EditAnywhere)
		FName levelToLoadAfterDefeat;

public:

	virtual void MoveToCreature() override;

	virtual FVector GetEndPosition() override;

	float GetLife();

	float GetMaxLife();

	virtual void DoAfterDead() override;
};
