// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "WindZone.generated.h"

class AGameCharacter;

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
		float minWindStrength;

	UPROPERTY(EditAnywhere)
		float maxWindStrength;

	UPROPERTY(EditAnywhere)
		float dieTime;

	float startTime;

	bool isActivated;

	UPROPERTY()
		AGameCharacter* gameCharacter;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		virtual void EnterEvent(AActor* overlappedActor, AActor* otherActor);

	UFUNCTION()
		virtual void ExitEvent(AActor* overlappedActor, AActor* otherActor);

	void Activate(bool isActive);

};
