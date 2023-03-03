// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/AudioComponent.h"
#include "Creature.h"
#include "GameCharacter.h"
#include <Sound/SoundCue.h >
#include "Enemy.generated.h"

UCLASS()
class SMALLPROJECT_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;
	void SetSpline();

	void SetSplineMeshComponent(USplineMeshComponent* splineMeshComp, FVector startPoint, FVector startTangent, FVector endPoint, FVector endTangent);
	

	void StateManagement();

	void LifeManagement();

	void DecreaseLife();

	void RemoveEnemy();

	void DestroySpline();

	void DestroySplineMeshComp(USplineMeshComponent* splineMeshComp);

	void SplineMeshCompAttach(USplineMeshComponent* splineMeshComp);

	virtual void DoAfterDead();

	virtual FVector GetEndPosition();

	float actualLife;

	USplineMeshComponent* prevSplineMeshComp;

	float currentTime;

	float startTime;

	bool canPlayerDamageMe = true;

	AGameCharacter* overlappingGameCharacter;

	EnemyStatus actualStatus;

	FVector actualStartPosition;

	FVector actualEndPosition;

	FVector lastCurveEndPosition;

	FVector startScale;
	FVector endScale;

	UPROPERTY(EditAnywhere, Category = "Spline")
		USplineComponent* splineComponent;

	UPROPERTY(EditAnywhere, Category = "Spline")
		USplineMeshComponent* SMeshComp0;

	UPROPERTY(EditAnywhere, Category = "Spline")
		USplineMeshComponent* SMeshComp1;

	UPROPERTY(EditAnywhere, Category = "Spline")
		USplineMeshComponent* SMeshComp2;

	UPROPERTY(EditAnywhere, Category = "Spline")
		USplineMeshComponent* SMeshComp3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		UStaticMeshComponent* SwallowSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		TEnumAsByte<ESplineMeshAxis::Type> forwardAxis;

	UPROPERTY(EditAnywhere)
		ACreature* creature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* popSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* PopAudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* slurpSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* SlurpAudioComp;

	UPROPERTY(EditAnywhere)
		float movementSpeed;

	UPROPERTY(EditAnywhere)
		float maxLife;

	UPROPERTY(EditAnywhere)
		float lifeDecreaseAfterAttack;

	UPROPERTY(EditAnywhere)
		float dyingTime;

public:	
	UFUNCTION()
		void EnterEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void ExitEvent(class AActor* overlappedActor, class AActor* otherActor);
	// Called every frame

	virtual void MoveToCreature();

	void StartEating();
};

UENUM()
enum class EnemyStatus : uint8
{
	Initial,
	Moving,
	Eating,
	SpecialDying,
};