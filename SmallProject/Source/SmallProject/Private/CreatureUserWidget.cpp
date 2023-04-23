// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureUserWidget.h"
#include "Components/ProgressBar.h"
#include "Creature.h"

void UCreatureUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	IncreaseDeltaBar->SetPercent(0.f);

}

/*
showing health of creature
*/
void UCreatureUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (creature != nullptr) {
		HealthBar->SetPercent(creature->GetHealth() / creature->GetMaxHealth());

		if (creature->GetStatus() == Status::Healing) {
			IncreaseDeltaBar->SetPercent(creature->GetDeltaIncreaseHealth());
		}
		else if (IncreaseDeltaBar->GetPercent() != 0.f) {
			IncreaseDeltaBar->SetPercent(0.f);
		}
	}
}