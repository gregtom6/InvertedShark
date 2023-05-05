// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FallCamera.generated.h"

UCLASS()
class SMALLPROJECT_API AFallCamera : public AActor
{
	GENERATED_BODY()

private:
	virtual void Tick(float DeltaTime) override;

	
public:	
	// Sets default values for this actor's properties
	AFallCamera();

protected:


	class AGameCharacter* gameCharacter;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
		class UCameraComponent* Camera;

	UPROPERTY(EditAnywhere)
		class UStaticMeshComponent* Mesh;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> widgetclass;

	UPROPERTY(VisibleInstanceOnly)
		class UDeadUserWidget* deaduserwidget;

	UPROPERTY(EditAnywhere)
		float showTime = 3.f;

	float startTime;

public:	

	UFUNCTION()
		void DieHappened();
};
