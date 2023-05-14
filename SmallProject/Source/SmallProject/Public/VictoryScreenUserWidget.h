// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VictoryScreenUserWidget.generated.h"

class USoundBase;
class UButton;

/**
 * this screen appears, when player defeats the boss in the end of the game
 */
UCLASS()
class SMALLPROJECT_API UVictoryScreenUserWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ButtonClickSound;

	UPROPERTY(meta = (BindWidget))
		UButton* ReturnToMenuButton;

	UPROPERTY(EditAnywhere)
		FName levelToLoad;

	UFUNCTION()
		void OnReturnMenuClick();
};
