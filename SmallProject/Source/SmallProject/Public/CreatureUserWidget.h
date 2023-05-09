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

	void BigDeltaDamageHappened(float originalLifeBeforeAttack);

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
		class UProgressBar* IncreaseDeltaBar;

	UPROPERTY(meta = (BindWidget))
		class UProgressBar* DecreaseDeltaBar;

	UPROPERTY(EditAnywhere)
		float timeUntilDeltaDecreaseStartDisappear;

	UPROPERTY(EditAnywhere)
		float timeForDisappear;

	CreatureLifeStatus creatureLifeStatus;
	float startTime;

	float deltaLifePercentage;
		
public:
	class ACreature* creature;
	
};


UENUM()
enum class CreatureLifeStatus : uint8
{
	Normal,
	WaitUntilDecrease,
	Decrease,
};