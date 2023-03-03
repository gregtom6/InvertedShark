// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeadUserWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class SMALLPROJECT_API UDeadUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
		virtual void NativeConstruct() override;

protected:

	UPROPERTY(meta = (BindWidget))
		class UButton* ResurrectButton;

	UPROPERTY(meta = (BindWidget))
		class UButton* BackToMenuButton;

	UPROPERTY(EditAnywhere)
		FName menuLevelToLoad;

	UPROPERTY(EditAnywhere)
		FName mapLevelToLoad;

	UFUNCTION()
		void OnResurrectClick();

	UFUNCTION()
		void OnBackToMenuClick();
};
