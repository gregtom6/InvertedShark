// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Creature.generated.h"

#pragma region Forward Declarations

class AHealerEnemy;
class UStaticMeshComponent;
class UAudioComponent;
class UUserWidget;
class USkeletalMeshComponent;
class UStaticMeshComponent;
class UBoxComponent;
class UCurveFloat;
class UMaterialInterface;
class UCreatureUserWidget;
class UProjectileCompPositioner;
class UResourceDataAsset;
class AEnemyTriggerBox;

#pragma endregion

DECLARE_DELEGATE_OneParam(FBigDeltaDamageHappened, float);

//player needs to defent this Creature constantly

UCLASS()
class SMALLPROJECT_API ACreature : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACreature(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	bool bIsCharInFur;

	void StepTargetIndex();

	void SwitchingToWaitBeforeMoveFastWhenEnemiesCleared();

	void SwitchingToMovingFast();

	void HeadStateManagement();

	void HealthManagement(const float DeltaTime);

	void StateManagement();

	UPROPERTY()
		AHealerEnemy* attackingHealer;

	UPROPERTY()
		AEnemyTriggerBox* currentEnemyTriggerBox;

	float actualHealthWhenStartedHealing;

	FRotator startHeadRotation;
	FRotator targetHeadRotation;

	FRotator startActorRotation;
	FRotator targetActorRotation;

	EHeadState prevHeadState;
	EHeadState headState;
	float headRotationStartTime;

	EStatus actualStatus;

	FVector actualStartPosition;
	FVector actualEndPosition;

	float startTime;
	float actualTime;

	float Health;

	float deltaHeal;

	float originalLifeBeforeAttack;


	int32 actualTargetIndex = 0;


	UFUNCTION()
		void EnterEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void ExitEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* WhaleAudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* WhaleCryAudio;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> widgetclass;

	UPROPERTY(VisibleInstanceOnly)
		UCreatureUserWidget* creatureuserwidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "settings")
		float deltaDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "settings")
		float damageAfterSting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "settings")
		float MaxHealth = 120.f;

	UPROPERTY(EditAnywhere)
		TArray<AActor*> positionsToMove;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* headMesh;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* LeftEye;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* RightEye;

	UPROPERTY(EditAnywhere)
		UBoxComponent* huggableComp;

	UPROPERTY(EditAnywhere)
		UCurveFloat* CurveFloat;

	UPROPERTY(EditAnywhere)
		UProjectileCompPositioner* projectilePositioner;

	UPROPERTY(EditAnywhere)
		UResourceDataAsset* globalSettings;

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
	UPROPERTY(EditAnywhere)
		TArray<AActor*> enemiesActuallyAttacking;


	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetDeltaIncreaseHealth() const { return actualHealthWhenStartedHealing + (deltaHeal * actualTime); }
	FVector GetLocation() const { return GetActorLocation(); }
	bool IsCharacterInFur() const;
	void GetHugged();
	void HealingStarted();
	EStatus GetStatus() const;

	void SetupProjectile(const FRotator rotator, const FVector scale, UStaticMesh* const& mesh, UMaterialInterface* const& material, const FVector offset);

	void DoAfterGettingHitFromProjectile();

	float GetOriginalLifeBeforeAttack() const;

	void OriginalLifeRepresentationEnded();

	FBigDeltaDamageHappened bigDeltaDamageHappenedDelegate;
};

UENUM()
enum class EStatus : uint8
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
enum class EHeadState : uint8
{
	ForwardLooking,
	FollowingPlayer,
};