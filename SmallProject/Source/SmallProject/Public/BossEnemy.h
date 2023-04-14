// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "Components/SplineComponent.h"
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
		class UStaticMeshComponent* bodyMesh;

	UPROPERTY(EditAnywhere)
		FName levelToLoadAfterDefeat;

	virtual FVector GetEndPosition() override;
	virtual void DoAfterDead() override;
public:

	virtual void MoveToCreature(float timeToStart) override;


	float GetLife();

	float GetMaxLife();

	FVector GetPositionOfBodyMesh();

	float GetBodyMeshRadius();
};
