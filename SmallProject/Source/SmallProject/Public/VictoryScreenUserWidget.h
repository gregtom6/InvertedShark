// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VictoryScreenUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API UVictoryScreenUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
		class UButton* ReturnToMenuButton;

	UPROPERTY(EditAnywhere)
		FName levelToLoad;

	UFUNCTION()
		void OnReturnMenuClick();
};
