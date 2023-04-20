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

	UFUNCTION()
		void TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* BodyMesh;

	UPROPERTY(EditAnywhere)
		class UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* DeflateAudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* deflateSound;

	UPROPERTY(EditAnywhere)
		FVector defaultBodyScale;

	UPROPERTY(EditAnywhere)
		float percentageOfMaxLifeToHealBack;

	UPROPERTY(EditAnywhere)
		float timeForHeal;

	UPROPERTY(EditAnywhere)
		FLinearColor targetColor;

	FLinearColor defaultColor;

	float startTimeForHealingSphere;

	bool canHealingStarted = true;

public:
	AHealerEnemy();

	float GetPercentageOfMaxLifeToHealBack();

	float GetTimeForHeal();
};
