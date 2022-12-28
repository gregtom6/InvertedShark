// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Creature.h"
#include "GameCharacter.generated.h"

UCLASS(BlueprintType)
class SMALLPROJECT_API AGameCharacter : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGameCharacter();

	void StrafeLR(float movementDelta);

	void WingBeat();

	void RotateLR(float rotateDelta);

	void HugCreature();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//main pawn camera
	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* CameraMesh;

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
		
	FVector startPos;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPhysicsConstraintComponent* LeftArm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UPhysicsConstraintComponent* RightArm;

	bool isHugging;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
