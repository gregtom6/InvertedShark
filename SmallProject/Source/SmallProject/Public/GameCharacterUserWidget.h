// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameCharacterUserWidget.generated.h"

class UProgressBar;
class AGameCharacter;

/**
 * shows only the current energy level of the player character
 */
UCLASS()
class SMALLPROJECT_API UGameCharacterUserWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	EFadeStatus fadeStatus;
	float fadeStartTime;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
		UProgressBar* Energybar;

	UPROPERTY(EditAnywhere)
		float fadeTime;

public:
	UPROPERTY()
		AGameCharacter* player;
};

UENUM()
enum class EFadeStatus : uint8
{
	Visible,
	Invisible,
	InvisibleToVisible,
	VisibleToInvisible,
};