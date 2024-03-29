// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "WindZone.generated.h"

class AGameCharacter;
class UResourceDataAsset;

//created by SniperEnemy, blows wind upwards 
// 
//used for blow player upwards after defeating SniperEnemy
//player can return to the creature, because he will ran out of energy
//after defeating SniperEnemy

UCLASS()
class SMALLPROJECT_API AWindZone : public ATriggerBox
{
	GENERATED_BODY()

		AWindZone(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere)
		FVector direction;

	UPROPERTY(EditAnywhere)
		FFloatInterval WindStrength;

	UPROPERTY(EditAnywhere)
		float dieTime;

	UPROPERTY(EditAnywhere)
		UResourceDataAsset* globalSettings;

	float startTime;

	bool bIsActivated;

	UPROPERTY()
		AGameCharacter* gameCharacter;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void EnterEvent(AActor* overlappedActor, AActor* otherActor);

	UFUNCTION()
		virtual void ExitEvent(AActor* overlappedActor, AActor* otherActor);

	void Activate(const bool isActive);

};
