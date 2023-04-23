// Fill out your copyright notice in the Description page of Project Settings.


#include "BossUserWidget.h"
#include "BossEnemy.h"
#include "Components/ProgressBar.h"

void UBossUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bossLifeStatus = BossLifeStatus::Normal;

	bossdeltadecrease->SetPercent(0.f);
}

/*
setting boss life visuals based on boss life
*/

void UBossUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (boss == nullptr) { return; }

	float currentLifePercentage = boss->GetLife() / boss->GetMaxLife();
	float deltaLifePercentage = boss->GetOriginalLifeBeforeAttack() / boss->GetMaxLife();

	bosslifebar->SetPercent(currentLifePercentage);

	if (bossLifeStatus==BossLifeStatus::Normal && currentLifePercentage != deltaLifePercentage) {
		bossLifeStatus = BossLifeStatus::WaitUntilDecrease;
		bossdeltadecrease->SetPercent(deltaLifePercentage);
		startTime = GetWorld()->GetTimeSeconds();
	}

	if (bossLifeStatus == BossLifeStatus::WaitUntilDecrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= timeUntilDeltaDecreaseStartDisappear) {
			bossLifeStatus = BossLifeStatus::Decrease;
			startTime= GetWorld()->GetTimeSeconds();
		}
	}
	else if (bossLifeStatus == BossLifeStatus::Decrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= timeForDisappear;

 		bossdeltadecrease->SetPercent(FMath::Lerp(deltaLifePercentage, currentLifePercentage,currentTime));

		if (currentTime >= timeForDisappear) {
			bossLifeStatus = BossLifeStatus::Normal;
			bossdeltadecrease->SetPercent(currentLifePercentage);
			boss->OriginalLifeRepresentationEnded();
		}
	}
}
