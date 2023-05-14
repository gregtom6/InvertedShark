// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUserWidget.generated.h"

class AGameCharacter;
class UButton;

/**
 * pause menu of the game
 */
UCLASS()
class SMALLPROJECT_API UPauseUserWidget : public UUserWidget
{
	GENERATED_BODY()

		UPROPERTY()
		AGameCharacter* gameChar;
protected:
	virtual void NativeDestruct() override;

	UPROPERTY(meta = (BindWidget))
		UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
		UButton* QuitToMainMenuButton;

	UPROPERTY(EditAnywhere)
		FName levelToLoad;

	UFUNCTION()
		void OnResumeClick();

	UFUNCTION()
		void OnQuitToMainMenuClick();
public:
	UFUNCTION()
		void SetGameCharacter(AGameCharacter* gameCharacter);
};
