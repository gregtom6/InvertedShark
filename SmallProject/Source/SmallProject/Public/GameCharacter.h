// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Creature.h"
#include <Sound/SoundCue.h >
#include <PauseUserWidget.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "GameCharacter.generated.h"

DECLARE_DELEGATE(FOnDieHappenedSignature);

UCLASS(BlueprintType)
class SMALLPROJECT_API AGameCharacter : public APawn
{
	GENERATED_BODY()
private:
	// Sets default values for this pawn's properties
	AGameCharacter(const FObjectInitializer& ObjectInitializer);

	void StrafeLR(float movementDelta);

	void WingBeat();

	void RotateLR(float rotateDelta);

	void HugCreature();

	void Attack();

	void Dash();

	void StateManagement();

	void EnergyManagement(float DeltaTime, FVector& currentVelocity);

	void VelocityManagement(FVector& currentVelocity);

	void CameraManagement();

	void MetalScratchManagement();

	void DeadManagement();

	void SetRotationLocks(bool X, bool Y, bool Z);

	void InitializePause();

	virtual void BeginPlay() override;

	FVector startPos;

	float time;

	bool isHugging;

	ACreature* creature;

	AActor* bossEnemy;

	GameCharacterStatus actualStatus;
	GameCharacterStatus prevStatus;

	PauseStatus pauseStatus;

	float startTime;

	float actualEnergy;

	float startArmLength;
	float targetArmLength;
	float startTimeForSpringArm;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	//main pawn camera
	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* CameraMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Tongue;

	UPROPERTY(EditAnywhere)
		USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> widgetclass;

	UPROPERTY(VisibleInstanceOnly)
		class UGameCharacterUserWidget* energyuserwidget;

	UPROPERTY(EditAnywhere)
		UNiagaraComponent* Spark;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float MovementSpeed = 1.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float WingStrength = 1.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float ForwardFlyStrength = 1.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float RotateSpeed = 1.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float velocityLimit = 300.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float heightToDie = -1000.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float attackTime = 0.3f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float maxEnergy = 120.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float energyDecreaseAfterWingBeat = 10.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float energyRegeneration = 5.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float restingMultiplier = 5.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float restVelocity = 0.3f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float angularDamplingWhenHuggingFur = 10.0f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float normalAngularDampling = 50.0f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float defaultArmLength = 20.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float zoomedOutArmLength = 70.f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float springArmLengthSpeed = 0.5f;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float energyDecreaseAfterDash;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float dashStrength;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float dashCooldownTime;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* leftHandVisual;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* rightHandVisual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPhysicsConstraintComponent* LeftLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPhysicsConstraintComponent* RightArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* AudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* TongueAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* MetalScratchAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* DashAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* wingBeat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* tongueSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* metalScratchSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* dashSound;

	UPROPERTY(EditAnywhere) TSubclassOf<UPauseUserWidget> widgetPauseMenu;
	UPauseUserWidget* widgetPauseMenuInstance;

	UFUNCTION()
	bool GetOverlapInfluenceSphere(UStaticMeshComponent* StaticMeshComponent, FVector& Actor1ClosestPoint, FVector& Actor2ClosestPoint);


public:

	GameCharacterStatus GetStatus();

	GameCharacterStatus GetPrevStatus();

	float GetEnergy();

	float GetMaxEnergy();

	void SetPrevStatusToActualStatus();

	void Pause();

	FOnDieHappenedSignature OnDieHappenedDelegate;
};

UENUM()
enum class GameCharacterStatus : uint8
{
	Calm,
	Attack,
	Dash,
	Dead,
};

UENUM()
enum class PauseStatus : uint8
{
	Played,
	Paused,
};