// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Creature.h"
#include <Sound/SoundCue.h >
#include <PauseUserWidget.h>
#include "GameCharacter.generated.h"

UCLASS(BlueprintType)
class SMALLPROJECT_API AGameCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGameCharacter(const FObjectInitializer& ObjectInitializer);

	void StrafeLR(float movementDelta);

	void WingBeat();

	void RotateLR(float rotateDelta);

	void HugCreature();

	void Attack();

	void Pause();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//main pawn camera
	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* CameraMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Tongue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "character settings")
		class UWidgetComponent* EnergyWidgetComp;

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

	FVector startPos;

	float time;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPhysicsConstraintComponent* LeftLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPhysicsConstraintComponent* RightArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* AudioComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* wingBeat;

	UPROPERTY(EditAnywhere) TSubclassOf<UPauseUserWidget> widgetPauseMenu;
	UPauseUserWidget* widgetPauseMenuInstance;

	GameCharacterStatus GetStatus();

	bool isHugging;

	GameCharacterStatus actualStatus;

	PauseStatus pauseStatus;

	float startTime;

	float actualEnergy;

	float GetEnergy();

	float GetMaxEnergy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};

UENUM()
enum class GameCharacterStatus : uint8
{
	Calm,
	Attack,
};

UENUM()
enum class PauseStatus : uint8
{
	Played,
	Paused,
};