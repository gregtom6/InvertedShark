// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureUserWidget.h"
#include "Components/ProgressBar.h"
#include "Creature.h"

void UCreatureUserWidget::NativeConstruct() {
	Super::NativeConstruct();
	
}

/*
showing health of creature
*/
void UCreatureUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (creature != nullptr) {
		HealthBar->SetPercent(creature->GetHealth() / creature->GetMaxHealth());
	}
}