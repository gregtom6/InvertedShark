// Fill out your copyright notice in the Description page of Project Settings.


#include "BossUserWidget.h"
#include "BossEnemy.h"
#include "Components/ProgressBar.h"

void UBossUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	bossLifeStatus = EBossLifeStatus::Normal;

	bossdeltadecrease->SetPercent(0.f);
}

/*
setting boss life visuals based on boss life
*/

void UBossUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!boss) { return; }

	float currentLifePercentage = boss->GetLife() / boss->GetMaxLife();
	float deltaLifePercentage = boss->GetOriginalLifeBeforeAttack() / boss->GetMaxLife();

	bosslifebar->SetPercent(currentLifePercentage);

	if (bossLifeStatus==EBossLifeStatus::Normal && currentLifePercentage != deltaLifePercentage) {
		bossLifeStatus = EBossLifeStatus::WaitUntilDecrease;
		bossdeltadecrease->SetPercent(deltaLifePercentage);
		startTime = GetWorld()->GetTimeSeconds();
	}

	if (bossLifeStatus == EBossLifeStatus::WaitUntilDecrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= timeUntilDeltaDecreaseStartDisappear) {
			bossLifeStatus = EBossLifeStatus::Decrease;
			startTime= GetWorld()->GetTimeSeconds();
		}
	}
	else if (bossLifeStatus == EBossLifeStatus::Decrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= timeForDisappear;

 		bossdeltadecrease->SetPercent(FMath::Lerp(deltaLifePercentage, currentLifePercentage,currentTime));

		if (currentTime >= timeForDisappear) {
			bossLifeStatus = EBossLifeStatus::Normal;
			bossdeltadecrease->SetPercent(currentLifePercentage);
			boss->OriginalLifeRepresentationEnded();
		}
	}
}
