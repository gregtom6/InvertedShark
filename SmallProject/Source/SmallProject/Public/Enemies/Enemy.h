// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineMeshComponent.h"
#include "Enemy.generated.h"

#pragma region Forward Declarations

class USplineComponent;
class USplineMeshComponent;
class UStaticMeshComponent;
class AGameCharacter;
class UNiagaraComponent;
class ACreature;
class UAudioComponent;
class UResourceDataAsset;
class UHealthComponent;

#pragma endregion

//base class of all enemy types
//simplest enemies are also these

UCLASS()
class SMALLPROJECT_API AEnemy : public AActor
{
	GENERATED_BODY()

public:
	AEnemy();
	// Sets default values for this actor's properties
	AEnemy(const FObjectInitializer& ObjectInitializer);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void Tick(float DeltaTime) override;
	void SetSpline();

	void SetSplineMeshComponent(USplineMeshComponent* splineMeshComp, const FVector startPoint, const FVector startTangent, const FVector endPoint, const FVector endTangent);

	void StateManagement(const float DeltaTime);

	void TimeManagement();

	void DecreaseLife();

	void SplineManagement();

	void TriggerActorOverlapWithSplines();

	void StartActualEating();

	void DestroySpline();

	void DestroySplineMeshComp(USplineMeshComponent* splineMeshComp);

	void SplineMeshCompAttach(USplineMeshComponent* splineMeshComp);

	virtual UStaticMeshComponent* GetCurrentBodyMesh() const;

	virtual void DoAfterDead();

	virtual FVector GetEndPosition() const;

	virtual void MovingToCreatureEnded();

	USplineMeshComponent* prevSplineMeshComp;

	float currentTime;

	float startTime;

	float timeBeforeActualMoving;

	UPROPERTY()
		AGameCharacter* gameCharacter;

	EEnemyStatus actualStatus;

	FVector actualStartPosition;

	FVector actualEndPosition;

	FVector lastCurveEndPosition;

	FVector startScale;
	FVector endScale;


	UPROPERTY(EditAnywhere, Category = "Spline")
		USplineComponent* splineComponent;


	UPROPERTY(EditAnywhere, Category = "Spline")
		TArray< USplineMeshComponent*> SMeshComps;

	UPROPERTY(EditAnywhere, Category = "Spline")
		TArray< UStaticMeshComponent*> SMeshContainers;

	UPROPERTY(EditAnywhere, Category = "Spline")
		TArray< UNiagaraComponent*> SplineNiagaras;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		UStaticMeshComponent* SwallowSphere;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		UStaticMeshComponent* Body12;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spline")
		TEnumAsByte<ESplineMeshAxis::Type> forwardAxis;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* EyePivot2;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LeftEyeWhite2;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LeftEyeBlack2;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LeftEyeLid12;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LeftEyeLid22;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* RightEyeWhite2;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* RightEyeBlack2;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* RightEyeLid12;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* RightEyeLid22;

	UPROPERTY(EditAnywhere)
		ACreature* creature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* PopAudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* SlurpAudioComp;

	UPROPERTY(EditAnywhere)
		UResourceDataAsset* globalSettings;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UDamageType> continuousDamageType;

	UPROPERTY(EditAnywhere)
		UHealthComponent* healthComponent;

	UPROPERTY(EditAnywhere)
		float movementSpeed;

	UPROPERTY(EditAnywhere)
		float dyingTime;

	UPROPERTY(EditAnywhere)
		int countOfInnerPointsInSpline = 3;

	UPROPERTY(EditAnywhere)
		float splineGrowTime = 1.f;

	UPROPERTY(EditAnywhere)
		float innerSplinePointRangeBorderZ = 100.f;

	UPROPERTY(EditAnywhere)
		float innerSplinePointRangeBorderXY = 50.f;

	UPROPERTY(EditAnywhere)
		float damageToCreature;

public:
	UFUNCTION()
		virtual void EnterEvent(AActor* overlappedActor, AActor* otherActor);

	UFUNCTION()
		virtual void ExitEvent(AActor* overlappedActor, AActor* otherActor);
	// Called every frame

	virtual void MoveToCreature(const float timeToStart);

	void StartEating();

	float GetOriginalLifeBeforeAttack() const;

	void OriginalLifeRepresentationEnded();

	virtual void RemoveEnemy();

	EEnemyStatus GetActualStatus() const;

	float GetCurrentLife() const;
};

UENUM()
enum class EEnemyStatus : uint8
{
	Initial,
	WaitBeforeMoving,
	Moving,
	StartEating,
	Eating,
	SpecialDying,
	Healing,
	Targeting,
	Shooting,
	SniperDying,
	SniperDead,
};