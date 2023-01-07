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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "boss settings")
		class UWidgetComponent* BossWidgetComp;

	UPROPERTY(EditAnywhere)
		float dyingTime;

	UPROPERTY(EditAnywhere)
		FName levelToLoadAfterDefeat;

	FVector startScale;
	FVector endScale;

public:
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetEndPosition() override;

	float GetLife();

	float GetMaxLife();

	virtual void RemoveEnemy();
};
