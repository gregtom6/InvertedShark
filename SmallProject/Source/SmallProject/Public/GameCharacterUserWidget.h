// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameCharacterUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API UGameCharacterUserWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	FadeStatus fadeStatus;
	float fadeStartTime;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* Energybar;

	UPROPERTY(EditAnywhere)
		float fadeTime;

public:
	UPROPERTY()
	class AGameCharacter* player;
};

UENUM()
enum class FadeStatus : uint8
{
	Visible,
	Invisible,
	InvisibleToVisible,
	VisibleToInvisible,
};