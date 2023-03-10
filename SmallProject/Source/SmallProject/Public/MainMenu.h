// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

/**
 *
 */
UCLASS()
class SMALLPROJECT_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override; 
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Sounds") 
		class USoundBase* ButtonClickSound;

	UPROPERTY(meta = (BindWidget))
		class UButton* StartButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* ControlsButton;
	UPROPERTY(meta = (BindWidget))
		class UButton* QuitButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* BackButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* HowToPlayButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* BackToMainMenuButton;

	UPROPERTY(meta = (BindWidget))
		class UWidgetSwitcher* WidgetSwitcher;

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
