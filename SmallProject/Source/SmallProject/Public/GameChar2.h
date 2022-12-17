// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "GameChar2.generated.h"

UCLASS()
class SMALLPROJECT_API AGameChar2 : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGameChar2();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UCameraComponent* Camera;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
