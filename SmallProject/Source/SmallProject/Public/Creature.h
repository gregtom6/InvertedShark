// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
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
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UPROPERTY(EditAnywhere, Category="widgets")
		//UUserWidget* LifeBarWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes, meta = (DisplayName = "HUD Class"))
		TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY()
	UUserWidget* LifeBarWidget;
};
