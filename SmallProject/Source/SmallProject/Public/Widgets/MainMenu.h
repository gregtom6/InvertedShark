// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

class USoundBase;
class UButton;
class UWidgetSwitcher;

/**
 * the main menu of the game
 */
UCLASS()
class SMALLPROJECT_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

		virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ButtonClickSound;

	UPROPERTY(meta = (BindWidget))
		UButton* StartButton;
	UPROPERTY(meta = (BindWidget))
		UButton* ControlsButton;
	UPROPERTY(meta = (BindWidget))
		UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
		UButton* HowToPlayButton;

	UPROPERTY(meta = (BindWidget))
		UButton* BackToMainMenuButton;

	UPROPERTY(meta = (BindWidget))
		UWidgetSwitcher* WidgetSwitcher;

	UPROPERTY(EditAnywhere)
		FName levelToLoad;

	UFUNCTION()
		void OnStartClick();

	UFUNCTION()
		void OnControlsClick();

	UFUNCTION()
		void OnHowToPlayClick();

	UFUNCTION()
		void OnQuitClick();

	UFUNCTION()
		void OnBackClick();
};
