// Fill out your copyright notice in the Description page of Project Settings.


#include "BossUserWidget.h"

void UBossUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	AddToPlayerScreen();
}

void UBossUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (boss != nullptr) {
		bosslifebar->SetPercent(boss->GetLife() / boss->GetMaxLife());
	}
}