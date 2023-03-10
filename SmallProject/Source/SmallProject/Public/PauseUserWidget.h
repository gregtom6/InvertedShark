// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API UPauseUserWidget : public UUserWidget
{
	GENERATED_BODY()

		class AGameCharacter* gameChar;
protected:


	UPROPERTY(meta = (BindWidget))
		class UButton* ResumeButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* QuitToMainMenuButton;

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
