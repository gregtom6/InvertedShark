// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BossUserWidget.generated.h"

class UProgressBar;
class ABossEnemy;
class UHealthComponent;

//shows the life of the boss enemy
//only appears, when player (and creature) reaches the last area

UCLASS()
class SMALLPROJECT_API UBossUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(meta = (BindWidget))
		UProgressBar* bosslifebar;

	UPROPERTY(meta = (BindWidget))
		UProgressBar* bossdeltadecrease;

	UPROPERTY(EditAnywhere)
		float timeUntilDeltaDecreaseStartDisappear;

	UPROPERTY(EditAnywhere)
		float timeForDisappear;

	EBossLifeStatus bossLifeStatus;
	float startTime;

public:

	UPROPERTY()
		UHealthComponent* bossHealthComp;

	UPROPERTY()
		ABossEnemy* boss;
};

UENUM()
enum class EBossLifeStatus : uint8
{
	Normal,
	WaitUntilDecrease,
	Decrease,
};