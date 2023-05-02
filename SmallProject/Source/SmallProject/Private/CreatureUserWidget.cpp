// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureUserWidget.h"
#include "Components/ProgressBar.h"
#include "Creature.h"

void UCreatureUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	creatureLifeStatus = CreatureLifeStatus::Normal;

	IncreaseDeltaBar->SetPercent(0.f);

	DecreaseDeltaBar->SetPercent(0.f);
}

/*
showing health of creature
*/
void UCreatureUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (creature == nullptr) { return; }

	float currentLifePercentage = creature->GetHealth() / creature->GetMaxHealth();
	float deltaLifePercentage = creature->GetOriginalLifeBeforeAttack() / creature->GetMaxHealth();

	HealthBar->SetPercent(creature->GetHealth() / creature->GetMaxHealth());

	if (creature->GetStatus() == Status::Healing) {
		IncreaseDeltaBar->SetPercent(creature->GetDeltaIncreaseHealth());
	}
	else if (IncreaseDeltaBar->GetPercent() != 0.f) {
		IncreaseDeltaBar->SetPercent(0.f);
	}

	if (creatureLifeStatus == CreatureLifeStatus::Normal && FMath::Abs(currentLifePercentage-deltaLifePercentage)>0.2f) {
		creatureLifeStatus = CreatureLifeStatus::WaitUntilDecrease;
		DecreaseDeltaBar->SetPercent(deltaLifePercentage);
		startTime = GetWorld()->GetTimeSeconds();
	}

	if (creatureLifeStatus == CreatureLifeStatus::WaitUntilDecrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= timeUntilDeltaDecreaseStartDisappear) {
			creatureLifeStatus = CreatureLifeStatus::Decrease;
			startTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (creatureLifeStatus == CreatureLifeStatus::Decrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= timeForDisappear;

		DecreaseDeltaBar->SetPercent(FMath::Lerp(deltaLifePercentage, currentLifePercentage, currentTime));

		if (currentTime >= timeForDisappear) {
			creatureLifeStatus = CreatureLifeStatus::Normal;
			DecreaseDeltaBar->SetPercent(0.f);
			creature->OriginalLifeRepresentationEnded();
		}
	}
}