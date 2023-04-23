// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossUserWidget.generated.h"

UCLASS()
class SMALLPROJECT_API UBossUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* bosslifebar;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* bossdeltadecrease;

	UPROPERTY(EditAnywhere)
		float timeUntilDeltaDecreaseStartDisappear;

	UPROPERTY(EditAnywhere)
		float timeForDisappear;

	BossLifeStatus bossLifeStatus;
	float startTime;

public:
	class ABossEnemy* boss;
};

UENUM()
enum class BossLifeStatus : uint8
{
	Normal,
	WaitUntilDecrease,
	Decrease,
};