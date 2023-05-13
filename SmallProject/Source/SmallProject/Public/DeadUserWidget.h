// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeadUserWidget.generated.h"

class UButton;

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API UDeadUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
		virtual void NativeConstruct() override;
		virtual void NativeDestruct() override;

protected:

	UPROPERTY(meta = (BindWidget))
		UButton* ResurrectButton;

	UPROPERTY(meta = (BindWidget))
		UButton* BackToMenuButton;

	UPROPERTY(EditAnywhere)
		FName menuLevelToLoad;

	UPROPERTY(EditAnywhere)
		FName mapLevelToLoad;

	UFUNCTION()
		void OnResurrectClick();

	UFUNCTION()
		void OnBackToMenuClick();
};
