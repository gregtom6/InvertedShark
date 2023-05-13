// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GameCharacter.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class UStaticMesh;
class UMaterialInterface;
class UActorSequencePlayer;
class UPauseUserWidget;
class UNiagaraComponent;
class UAudioComponent;
class UPhysicsConstraintComponent;
class UGameCharacterUserWidget;
class UUserWidget;
class USpringArmComponent;
class UCameraComponent;
class UInputComponent;
class ACreature;

DECLARE_DELEGATE(FOnDieHappenedSignature);

UCLASS()
class SMALLPROJECT_API AGameCharacter : public APawn
{
	GENERATED_BODY()
private:
	// Sets default values for this pawn's properties
	AGameCharacter(const FObjectInitializer& ObjectInitializer);

	void WingBeat();

	void RotateLR(float rotateDelta);

	void HugCreature();

	void Attack();

	void Dash();

	void UpDash();

	void DownDash();

	void LeftDash();

	void RightDash();

	void StateManagement();

	void EnergyManagement(float DeltaTime, FVector& currentVelocity);

	void VelocityManagement(FVector& currentVelocity);

	void CameraManagement();

	void MetalScratchManagement();

	void DeadManagement();

	void SetDieState();

	void TimeManagement();

	void SetRotationLocks(bool X, bool Y, bool Z);

	void InitializePause();

	virtual void BeginPlay() override;

	FVector startPos;

	float time;

	bool isHugging;

	UPROPERTY()
		ACreature* creature;
	UPROPERTY()
		AActor* bossEnemy;

	GameCharacterStatus actualStatus;
	GameCharacterStatus prevStatus;

	PauseStatus pauseStatus;

	SlowDownStatus slowdownStatus;

	float startTime;

	float actualEnergy;

	float startArmLength;
	float targetArmLength;
	float startTimeForSpringArm;

	float actualSoundPitchMultiplier = 1.f;

	float slowdownStartTime;
	bool canSlowdownTimeStarted;

	bool amITargeted;

	FVector positionBeforeDash;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

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
		UGameCharacterUserWidget* energyuserwidget;

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

	UPROPERTY(EditAnywhere, Category = "character settings")
		float minSlowdownTimeMultiplier;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float timeOfSlowDown;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float timeOfStaySlowedDown;

	UPROPERTY(EditAnywhere, Category = "character settings")
		float timeOfTimeRestore;

	UPROPERTY(EditAnywhere)
		USkeletalMeshComponent* skeletal;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* leftHandVisual;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* rightHandVisual;

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
		UAudioComponent* SneezeAudio;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UPauseUserWidget> widgetPauseMenu;

	UPROPERTY()
		UNiagaraComponent* leftNoseSneezeNiagara;
	UPROPERTY()
		UNiagaraComponent* rightNoseSneezeNiagara;
	UPROPERTY()
		UPauseUserWidget* widgetPauseMenuInstance;
	UPROPERTY()
		UActorSequencePlayer* loopedEyePlayer;
	UPROPERTY()
		UActorSequencePlayer* sneezeBlinkPlayer;
	UPROPERTY()
		UActorSequencePlayer* leftDashPlayer;
	UPROPERTY()
		UActorSequencePlayer* rightDashPlayer;
	UPROPERTY()
		UActorSequencePlayer* wingPlayer;

	UFUNCTION()
		bool GetOverlapInfluenceSphere(UStaticMeshComponent* StaticMeshComponent, FVector& Actor1ClosestPoint, FVector& Actor2ClosestPoint);

	UFUNCTION()
		void SneezeBlinkEnded();

public:
	UFUNCTION()
		void SlowdownTime();

	GameCharacterStatus GetStatus() const;

	GameCharacterStatus GetPrevStatus() const;

	float GetEnergy() const;

	float GetMaxEnergy() const;

	float GetCurrentSoundPitchMultiplier() const;

	void SetPrevStatusToActualStatus();

	void Pause();

	void SetupProjectile(FRotator rotator, FVector scale, UStaticMesh* mesh, UMaterialInterface* material, FVector offset, FVector direction);


	FOnDieHappenedSignature OnDieHappenedDelegate;

	USkeletalMeshComponent* GetSkeletalMeshComponent() const;
	UStaticMeshComponent* GetStaticMeshComponent() const;

	void NotifyTargeting(bool iAmTargeted);

	FVector GetBackBeforeDashLocation() const;

	FVector GetCameraLocation() const;

	bool IsHugging() const;
};

UENUM()
enum class GameCharacterStatus : uint8
{
	Calm,
	Attack,
	UpDash,
	DownDash,
	Dead,
	LeftDash,
	RightDash,
};

UENUM()
enum class PauseStatus : uint8
{
	Played,
	Paused,
};

UENUM()
enum class SlowDownStatus : uint8 {
	NormalTime,
	SlowDownTime,
	StaySlowedDownTime,
	RestoreTime,
};