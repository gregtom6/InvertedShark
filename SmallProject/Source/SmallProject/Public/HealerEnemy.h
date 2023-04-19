// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "HealerEnemy.generated.h"

/**
 *
 */
UCLASS()
class SMALLPROJECT_API AHealerEnemy : public AEnemy
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual UStaticMeshComponent* GetCurrentBodyMesh();

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere)
		class UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere)
		FVector defaultBodyScale;

	UPROPERTY(EditAnywhere)
		float percentageOfMaxLifeToHealBack;

	UPROPERTY(EditAnywhere)
		float timeForHeal;

	UPROPERTY(EditAnywhere)
		FLinearColor targetColor;

	FLinearColor defaultColor;


public:

	virtual void EnterEvent(class AActor* overlappedActor, class AActor* otherActor) override;


	virtual void ExitEvent(class AActor* overlappedActor, class AActor* otherActor) override;
};
