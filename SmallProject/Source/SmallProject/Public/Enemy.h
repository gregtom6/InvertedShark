// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
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

	void SetSplineMeshComponent(class USplineMeshComponent* splineMeshComp, FVector startPoint, FVector startTangent, FVector endPoint, FVector endTangent);


	void StateManagement();

	void LifeManagement();

	void TimeManagement();

	void DecreaseLife();

	void RemoveEnemy();

	void DestroySpline();

	void DestroySplineMeshComp(class USplineMeshComponent* splineMeshComp);

	void SplineMeshCompAttach(class USplineMeshComponent* splineMeshComp);

	virtual UStaticMeshComponent* GetCurrentBodyMesh();
	
	virtual void DoAfterDead();

	virtual FVector GetEndPosition();

	float actualLife;

	float originalLifeBeforeAttack;

	class USplineMeshComponent* prevSplineMeshComp;

	float currentTime;

	float startTime;

	float timeBeforeActualMoving;

	bool canPlayerDamageMe = true;

	//TODO: merge them, remove nullptr setting
	class AGameCharacter* overlappingGameCharacter;

	class AGameCharacter* gameCharacter;

	EnemyStatus actualStatus;

	FVector actualStartPosition;

	FVector actualEndPosition;

	FVector lastCurveEndPosition;

	FVector startScale;
	FVector endScale;


	UPROPERTY(EditAnywhere, Category = "Spline")
		class USplineComponent* splineComponent;


	UPROPERTY(EditAnywhere, Category = "Spline")
		TArray<class USplineMeshComponent*> SMeshComps;

	UPROPERTY(EditAnywhere, Category = "Spline")
		TArray<class UStaticMeshComponent*> SMeshContainers;

	UPROPERTY(EditAnywhere, Category = "Spline")
		TArray<class UNiagaraComponent*> SplineNiagaras;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		class UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		class UStaticMeshComponent* SwallowSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		class UStaticMeshComponent* Body12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		TEnumAsByte<ESplineMeshAxis::Type> forwardAxis;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* EyePivot2;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* LeftEyeWhite2;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* LeftEyeBlack2;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* LeftEyeLid12;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* LeftEyeLid22;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* RightEyeWhite2;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* RightEyeBlack2;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* RightEyeLid12;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* RightEyeLid22;


	UPROPERTY(EditAnywhere)
		class ACreature* creature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* popSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* PopAudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* slurpSound;


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* SlurpAudioComp;

	UPROPERTY(EditAnywhere)
		float movementSpeed;

	UPROPERTY(EditAnywhere)
		float maxLife;

	UPROPERTY(EditAnywhere)
		float lifeDecreaseAfterAttack;

	UPROPERTY(EditAnywhere)
		float dyingTime;

	UPROPERTY(EditAnywhere)
		int countOfInnerPointsInSpline = 3;

	UPROPERTY(EditAnywhere)
		float innerSplinePointRangeBorderZ = 100.f;

	UPROPERTY(EditAnywhere)
		float innerSplinePointRangeBorderXY = 50.f;

public:
	UFUNCTION()
		virtual void EnterEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		virtual void ExitEvent(class AActor* overlappedActor, class AActor* otherActor);
	// Called every frame

	virtual void MoveToCreature(float timeToStart);

	void StartEating();

	float GetOriginalLifeBeforeAttack();

	void OriginalLifeRepresentationEnded();
};

UENUM()
enum class EnemyStatus : uint8
{
	Initial,
	WaitBeforeMoving,
	Moving,
	Eating,
	SpecialDying,
	Healing,
};