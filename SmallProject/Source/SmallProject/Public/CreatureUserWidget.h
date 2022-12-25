// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Creature.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "CreatureUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API UCreatureUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	/*
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY(EditAnywhere)
		UProgressBar* HealthBar;
		*/
public:
	ACreature* creature;
	
};
