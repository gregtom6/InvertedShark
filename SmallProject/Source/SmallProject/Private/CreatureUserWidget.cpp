// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureUserWidget.h"
#include "Components/ProgressBar.h"
#include "Creature.h"

void UCreatureUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	creatureLifeStatus = ECreatureLifeStatus::Normal;

	IncreaseDeltaBar->SetPercent(0.f);

	DecreaseDeltaBar->SetPercent(0.f);

	if (creature != nullptr)
		creature->bigDeltaDamageHappenedDelegate.BindUObject(this, &UCreatureUserWidget::BigDeltaDamageHappened);
}

/*
showing health of creature
*/
void UCreatureUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (creature == nullptr) { return; }

	float currentLifePercentage = creature->GetHealth() / creature->GetMaxHealth();

	HealthBar->SetPercent(creature->GetHealth() / creature->GetMaxHealth());

	if (creature->GetStatus() == EStatus::Healing) {
		IncreaseDeltaBar->SetPercent(creature->GetDeltaIncreaseHealth());
	}
	else if (IncreaseDeltaBar->GetPercent() != 0.f) {
		IncreaseDeltaBar->SetPercent(0.f);
	}

	if (creatureLifeStatus == ECreatureLifeStatus::WaitUntilDecrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= timeUntilDeltaDecreaseStartDisappear) {
			creatureLifeStatus = ECreatureLifeStatus::Decrease;
			startTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (creatureLifeStatus == ECreatureLifeStatus::Decrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= timeForDisappear;

		DecreaseDeltaBar->SetPercent(FMath::Lerp(deltaLifePercentage, currentLifePercentage, currentTime));

		if (currentTime >= timeForDisappear) {
			creatureLifeStatus = ECreatureLifeStatus::Normal;
			DecreaseDeltaBar->SetPercent(0.f);
			creature->OriginalLifeRepresentationEnded();
		}
	}
}

void UCreatureUserWidget::BigDeltaDamageHappened(float originalLifeBeforeAttack) {
	if (creatureLifeStatus != ECreatureLifeStatus::Normal) { return; }
	
	deltaLifePercentage = originalLifeBeforeAttack / creature->GetMaxHealth();

	creatureLifeStatus = ECreatureLifeStatus::WaitUntilDecrease;
	DecreaseDeltaBar->SetPercent(deltaLifePercentage);
	startTime = GetWorld()->GetTimeSeconds();
}