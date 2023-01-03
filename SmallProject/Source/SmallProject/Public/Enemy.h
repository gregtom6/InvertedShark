// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Creature.h"
#include "GameCharacter.h"
#include "Enemy.generated.h"

UCLASS()
class SMALLPROJECT_API AEnemy : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemy();

	void OnConstruction(const FTransform& Transform) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetSpline();

	void MoveToCreature();

	void StartEating();

	void DecreaseLife();

	virtual FVector GetEndPosition();

	UPROPERTY(EditAnywhere, Category = "Spline")
		USplineComponent* splineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		TEnumAsByte<ESplineMeshAxis::Type> forwardAxis;

	UPROPERTY(EditAnywhere)
		ACreature* creature;

	UPROPERTY(EditAnywhere)
		float movementSpeed;

	UPROPERTY(EditAnywhere)
		float maxLife;

	UPROPERTY(EditAnywhere)
		float lifeDecreaseAfterAttack;

	UFUNCTION()
		void EnterEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void ExitEvent(class AActor* overlappedActor, class AActor* otherActor);

	float actualLife;

	USplineMeshComponent* prevSplineMeshComp;

	float currentTime;

	float startTime;

	AGameCharacter* overlappingGameCharacter;

	EnemyStatus actualStatus;

	FVector actualStartPosition;

	FVector actualEndPosition;

	FVector lastCurveEndPosition;
};

UENUM()
enum class EnemyStatus : uint8
{
	Initial,
	Moving,
	Eating,
};