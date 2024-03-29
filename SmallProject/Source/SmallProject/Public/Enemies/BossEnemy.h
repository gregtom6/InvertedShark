// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "BossEnemy.generated.h"

class UUserWidget;
class UBossUserWidget;

/**
 * special type of Enemy
 * has unique visuals, has a different path towards creature
 * his life is also shown
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
		UBossUserWidget* bossuserwidget;

	UPROPERTY(EditAnywhere)
		FName levelToLoadAfterDefeat;

	virtual FVector GetEndPosition() const override;
	virtual void DoAfterDead() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:

	virtual void MoveToCreature(const float timeToStart) override;

	FVector GetPositionOfBodyMesh() const;

	float GetBodyMeshRadius() const;
};
