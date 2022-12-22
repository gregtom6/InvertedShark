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
	
public:
	virtual void NativeConstruct() override;

	ACreature* creature;

	//with this, we can upload that with values
	UPROPERTY()
		UCreatureUserWidget* CreatureHUD;
	//this spawns the HUD
	UPROPERTY(EditAnywhere)
		TSubclassOf<class UUserWidget> ProgressBarWidgetRef;

	UPROPERTY(meta=(BindWidget))
		class UProgressBar* ProgressBar;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
