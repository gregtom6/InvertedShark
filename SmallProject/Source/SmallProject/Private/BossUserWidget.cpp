// Fill out your copyright notice in the Description page of Project Settings.


#include "BossUserWidget.h"
#include "BossEnemy.h"
#include "Components/ProgressBar.h"

void UBossUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

/*
setting boss life visuals based on boss life
*/

void UBossUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (boss != nullptr && bosslifebar!=nullptr) {
		bosslifebar->SetPercent(boss->GetLife() / boss->GetMaxLife());
	}
}
