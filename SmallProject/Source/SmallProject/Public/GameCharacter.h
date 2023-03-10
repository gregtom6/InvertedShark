// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PauseUserWidget.h"
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

	class ACreature* creature;

	class AActor* bossEnemy;

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
		class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* CameraMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* Tongue;

	UPROPERTY(EditAnywhere)
		class USpringArmComponent* SpringArm;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> widgetclass;

	UPROPERTY(VisibleInstanceOnly)
		class UGameCharacterUserWidget* energyuserwidget;

	UPROPERTY(EditAnywhere)
		class UNiagaraComponent* Spark;

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
		class UStaticMeshComponent* leftHandVisual;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* rightHandVisual;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPhysicsConstraintComponent* LeftLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UPhysicsConstraintComponent* RightArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* AudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* TongueAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* MetalScratchAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UAudioComponent* DashAudio;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* wingBeat;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* tongueSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* metalScratchSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USoundCue* dashSound;

	UPROPERTY(EditAnywhere) TSubclassOf<UPauseUserWidget> widgetPauseMenu;
	class UPauseUserWidget* widgetPauseMenuInstance;

	UFUNCTION()
	bool GetOverlapInfluenceSphere(class UStaticMeshComponent* StaticMeshComponent, FVector& Actor1ClosestPoint, FVector& Actor2ClosestPoint);


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