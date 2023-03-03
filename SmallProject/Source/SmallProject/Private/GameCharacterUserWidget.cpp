// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterUserWidget.h"

void UGameCharacterUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	AddToPlayerScreen();
}

/*
reading player energy and setting visuals
*/
void UGameCharacterUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (player != nullptr) {
		Energybar->SetPercent(player->GetEnergy() / player->GetMaxEnergy());
	}
}