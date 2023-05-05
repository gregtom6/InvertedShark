// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class SMALLPROJECT_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();

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
		class UStaticMeshComponent* staticMesh;

	UPROPERTY(EditAnywhere)
		class UNiagaraComponent* creatureHitBloodNiagara;

	UPROPERTY(EditAnywhere)
		class UAudioComponent* projectileHittedTargetAudioComp;

	UPROPERTY(EditAnywhere)
		class UMaterialInterface* invisibleMaterial;

	ProjectileStatus status;

	FVector target;

	float startDistance;

	FVector directionVector;

	class AActor* targetedActor;

	class AActor* shooterActor;

	class AGameCharacter* gameChar;

	float startTimeForBloodFlow;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetTarget(AActor* t, AActor* origin);

	UFUNCTION()
		void Event(class AActor* overlappedActor, class AActor* otherActor);
};

UENUM()
enum class ProjectileStatus : uint8
{
	Initial,
	FlyToTarget,
	MoveInsideTarget,
	StopMovement,
};