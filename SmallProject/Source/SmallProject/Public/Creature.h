// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Creature.generated.h"

UCLASS()
class SMALLPROJECT_API ACreature : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACreature();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	

	//UPROPERTY(VisibleAnywhere)
		//UWidgetComponent* HealthWidgetComp;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }

	float Health;
	float MaxHealth = 120;
};
