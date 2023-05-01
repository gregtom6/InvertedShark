// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "SniperEnemy.generated.h"

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API ASniperEnemy : public AEnemy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual FVector GetEndPosition() override;

	virtual void MovingToCreatureEnded() override;

	float currentTime;

	UPROPERTY(EditAnywhere)
		float distanceFromCreature;

	UPROPERTY(EditAnywhere)
		float targetingTime;

	UPROPERTY(EditAnywhere)
		float shootTime;

	UPROPERTY(EditAnywhere)
		float targetingPercentageWhenSmokeNeeds;

	UPROPERTY(EditAnywhere)
		class USoundCue* weaponOverloadingSound;

	UPROPERTY(EditAnywhere)
		class UNiagaraComponent* smokeNiagara;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* ProjectileOrigin;

	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* SkeletalBody;

	UPROPERTY(EditAnywhere)
		class UMaterialInstanceDynamic* SniperMaterialInstance;

	UPROPERTY(EditAnywhere)
		FLinearColor targetColor;

	FLinearColor defaultColor;

	void CreateProjectile();
	
public:
	ASniperEnemy();

	void TargetingCreature();
};