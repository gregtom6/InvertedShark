// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Creature.generated.h"

UCLASS()
class SMALLPROJECT_API ACreature : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	//ACreature();
	ACreature(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool isCharInFur;

	void StepTargetIndex();

	void SwitchingToMovingFast();

	void HeadStateManagement();

	void HealthManagement(float DeltaTime);

	void StateManagement();

	class AHealerEnemy* attackingHealer;
	float actualHealthWhenStartedHealing;

	FRotator startHeadRotation;
	FRotator targetHeadRotation;

	FRotator startActorRotation;
	FRotator targetActorRotation;

	HeadState prevHeadState;
	HeadState headState;
	float headRotationStartTime;

	Status actualStatus;

	FVector actualStartPosition;
	FVector actualEndPosition;

	double startTime;


	float Health;

	float deltaHeal;

	int actualTargetIndex = 0;

	TArray<AActor*> enemiesActuallyAttacking;

	UFUNCTION()
		void EnterEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void ExitEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* whaleSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* WhaleAudioComp;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> widgetclass;

	UPROPERTY(VisibleInstanceOnly)
		class UCreatureUserWidget* creatureuserwidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "settings")
		float deltaDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "settings")
		float MaxHealth = 120.f;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* CreatureMesh;

	UPROPERTY(EditAnywhere)
		TArray<AActor*> positionsToMove;

	UPROPERTY(EditAnywhere)
		class USkeletalMeshComponent* headMesh;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* LeftEye;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* RightEye;

	UPROPERTY(EditAnywhere)
		class UBoxComponent* huggableComp;

	UPROPERTY(EditAnywhere)
		class UCurveFloat* CurveFloat;

	UPROPERTY(EditAnywhere)
		float movementSpeed;

	UPROPERTY(EditAnywhere)
		float fastMovementSpeed;

	UPROPERTY(EditAnywhere)
		float waitTimeBeforeFirstMove;

	UPROPERTY(EditAnywhere)
		float waitingTimeToMoveForwardAfterDefeatingEnemies;

	UPROPERTY(EditAnywhere)
		float waitTimeAfterHuggedToMoveForward;

	UPROPERTY(EditAnywhere)
		float lookAtPlayerBorder;
public:	
	

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetDeltaIncreaseHealth() const { return actualHealthWhenStartedHealing + deltaHeal; }
	FVector GetLocation() const { return GetActorLocation(); }
	bool IsCharacterInFur();
	void GetHugged();
	void HealingStarted();
	Status GetStatus();
};

UENUM()
enum class Status : uint8
{
	Initial,
	Stopped,
	Moving,
	UnderAttack,
	MovingFast,
	WaitBeforeMoveFast,
	WaitAfterHuggedByPlayer,
	RotatingTowardsTarget,
	Healing,
};

UENUM()
enum class HeadState : uint8
{
	ForwardLooking,
	FollowingPlayer,
};