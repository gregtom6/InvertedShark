// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

#pragma region Forward Declarations

class UStaticMeshComponent;
class UNiagaraComponent;
class UAudioComponent;
class UMaterialInterface;
class AGameCharacter;
class UResourceDataAsset;

#pragma endregion

// projectile fired by the SniperEnemy

UCLASS()
class SMALLPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectile(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	void TimeManagement();

	void CheckOverlappingComponents(AActor* currentlyTargetedActor, const FVector direction, const FRotator sum);

	FVector SettingUpScale(AActor* currentTargetedActor);

	UPROPERTY(EditAnywhere)
		float speed;

	UPROPERTY(EditAnywhere)
		float distanceToProceedInsideTarget;

	UPROPERTY(EditAnywhere)
		float offsetForHittingTarget;

	UPROPERTY(EditAnywhere)
		float timeUntilBloodflowStop;

	UPROPERTY(EditAnywhere)
		float timeUntilDestroy;

	UPROPERTY(EditAnywhere)
		float damageCaused;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* staticMesh;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* creatureHitBloodNiagara;

	UPROPERTY(EditAnywhere)
		UAudioComponent* projectileHittedTargetAudioComp;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* originalMaterial;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* invisibleMaterial;

	UPROPERTY(EditAnywhere)
		UResourceDataAsset* globalSettings;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UDamageType> projectileDamageType;

	EProjectileStatus status;

	FVector target;

	float startDistance;

	FVector directionVector;

	bool bIsActivated;

	UPROPERTY()
		AActor* targetedActor;
	UPROPERTY()
		AActor* shooterActor;
	UPROPERTY()
		AGameCharacter* gameChar;

	float startTimeForBloodFlow;

	float startTime;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTarget(AActor* t, AActor* origin);

	UFUNCTION()
		void Event(AActor* overlappedActor, AActor* otherActor);
};

UENUM()
enum class EProjectileStatus : uint8
{
	Initial,
	FlyToTarget,
	MoveInsideTarget,
	StopMovement,
	CanGetBackInPool,
};