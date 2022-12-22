// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureUserWidget.h"

void UCreatureUserWidget::NativeConstruct() {
	Super::NativeConstruct();
		

	if (ProgressBarWidgetRef)
	{

		UUserWidget* widget = CreateWidget(this, ProgressBarWidgetRef);
		CreatureHUD->AddToPlayerScreen();
		CreatureHUD->AddToViewport(0);

		ProgressBar->SetPercent(0.2f);
	}
}

void UCreatureUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ProgressBar->SetPercent(creature->GetHealth() / creature->GetMaxHealth());
}