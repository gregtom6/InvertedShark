// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;
class UAudioComponent;
class UMaterialInterface;
class AGameCharacter;

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
		UStaticMeshComponent* staticMesh;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* creatureHitBloodNiagara;

	UPROPERTY(EditAnywhere)
		UAudioComponent* projectileHittedTargetAudioComp;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* originalMaterial;

	UPROPERTY(EditAnywhere)
		UMaterialInterface* invisibleMaterial;

	ProjectileStatus status;

	FVector target;

	float startDistance;

	FVector directionVector;

	bool isActivated;

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
enum class ProjectileStatus : uint8
{
	Initial,
	FlyToTarget,
	MoveInsideTarget,
	StopMovement,
	CanGetBackInPool,
};