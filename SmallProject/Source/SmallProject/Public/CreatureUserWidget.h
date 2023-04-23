// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreatureUserWidget.generated.h"

UCLASS()
class SMALLPROJECT_API UCreatureUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* IncreaseDeltaBar;
		
public:
	class ACreature* creature;
	
};
