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
	
private:
	virtual void NativeConstruct() override;
public:

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
		USoundBase* ButtonClickSound;

	UPROPERTY(meta = (BindWidget))
		class UButton* ReturnToMenuButton;

	UPROPERTY(EditAnywhere)
		FName levelToLoad;

	UFUNCTION()
		void OnReturnMenuClick();
};
