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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Classes, meta = (DisplayName = "HUD Class"))
		TSubclassOf<class UUserWidget> HUDClass;

	UPROPERTY()
		UUserWidget* LifeBarWidget;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthBar;

	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
};
