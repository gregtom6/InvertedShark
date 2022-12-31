// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameCharacter.h>
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "GameCharacterUserWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SMALLPROJECT_API UGameCharacterUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* Energybar;

public:
	AGameCharacter* player;
};
