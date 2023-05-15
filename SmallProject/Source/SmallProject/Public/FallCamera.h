// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallCamera.generated.h"

class AGameCharacter;
class UCameraComponent;
class UStaticMeshComponent;
class UUserWidget;
class UDeadUserWidget;
class UResourceDataAsset;

//this is the death camera, used to follow player while it dies

UCLASS()
class SMALLPROJECT_API AFallCamera : public AActor
{
	GENERATED_BODY()

private:
	virtual void Tick(float DeltaTime) override;


public:
	// Sets default values for this actor's properties
	AFallCamera(const FObjectInitializer& ObjectInitializer);

protected:

	UPROPERTY()
		AGameCharacter* gameCharacter;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> widgetclass;

	UPROPERTY(VisibleInstanceOnly)
		UDeadUserWidget* deaduserwidget;

	UPROPERTY(EditAnywhere)
		UResourceDataAsset* globalSettings;

	UPROPERTY(EditAnywhere)
		float showTime = 3.f;

	float startTime;

public:

	UFUNCTION()
		void DieHappened();
};
