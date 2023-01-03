// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BossEnemy.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "BossUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class SMALLPROJECT_API UBossUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* bosslifebar;

public:
	ABossEnemy* boss;
};
