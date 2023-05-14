// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "HealerEnemy.generated.h"

/**
 * healer enemy, which constantly decreases life of Creature
 * but heals also, when player down dashes on its top
 */
UCLASS()
class SMALLPROJECT_API AHealerEnemy : public AEnemy
{
	GENERATED_BODY()

protected:

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Tick(float DeltaTime) override;
	virtual UStaticMeshComponent* GetCurrentBodyMesh() const;

	void TimeManagement();

	void HealingSphereManagement();

	void BodyManagement();

	UFUNCTION()
		void TriggerEnter(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerExit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere)
		UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* originalSwallowSphereMaterial;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* healingSwallowSphereMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* DeflateAudioComp;

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
	FVector originalHealingSphereScale;

	bool canHealingStarted = true;

public:
	AHealerEnemy(const FObjectInitializer& ObjectInitializer);

	float GetPercentageOfMaxLifeToHealBack() const;

	float GetTimeForHeal() const;
};
