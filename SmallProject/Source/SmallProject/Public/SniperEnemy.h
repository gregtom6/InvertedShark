// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Enemy.h"
#include "SniperEnemy.generated.h"

class AProjectile;
class AWindZone;

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

	virtual FVector GetEndPosition() const override;

	virtual void MovingToCreatureEnded() override;

	virtual void RemoveEnemy() override;

	void TimeManagement();

	float currentTime;

	EnemyTargeting enemyTargeting;

	UPROPERTY(EditAnywhere)
		float distanceFromCreature;

	UPROPERTY(EditAnywhere)
		float targetingTime;

	UPROPERTY(EditAnywhere)
		float shootTime;

	UPROPERTY(EditAnywhere)
		float targetingPercentageWhenSmokeNeeds;

	UPROPERTY(EditAnywhere)
		float targetingPercentageWhenAudioNeeds;

	UPROPERTY(EditAnywhere)
		float startWeaponOverloadingSoundPercentage;

	UPROPERTY(EditAnywhere)
		float distancePercentageAfterTargetingPlayer;

	UPROPERTY(EditAnywhere)
		float dieRotatingTime;

	UPROPERTY(EditAnywhere)
		float dieRotatingSpeed;

	UPROPERTY(EditAnywhere)
		float levelRemoveTime;

	UPROPERTY(EditAnywhere)
		UAudioComponent* loopedMosquitoSound;

	UPROPERTY(EditAnywhere)
		UAudioComponent* weaponOverloadingSound1;

	UPROPERTY(EditAnywhere)
		UAudioComponent* dyingBeeSound;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* dieTornadoNiagara;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* smokeNiagara;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* laserTargetingNiagara1;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* ProjectileOrigin;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* SkeletalBody;

	UPROPERTY(EditAnywhere)
		UMaterialInstanceDynamic* SniperMaterialInstance;

	UPROPERTY(EditAnywhere)
		UAnimSequence* weaponLoadingSequence;

	UPROPERTY(EditAnywhere)
		UAnimSequence* weaponShootingSequence;

	UPROPERTY(EditAnywhere)
		UAnimSequence* dieStartedSequence;

	UPROPERTY(EditAnywhere)
		FLinearColor targetColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<AActor> WindzoneClass;

	UPROPERTY()
		AProjectile* projectile;
	UPROPERTY()
		AWindZone* windZone;

	FLinearColor defaultColor;

	bool soundAlreadyStartedPlaying;

	void CreateProjectile();

	void CreateWindZone();

	FVector currentTarget;

public:
	ASniperEnemy(const FObjectInitializer& ObjectInitializer);

	void TargetingCreature();
};

UENUM()
enum class EnemyTargeting : uint8
{
	CreatureTargeting,
	PlayerTargeting,
};