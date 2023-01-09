// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureUserWidget.h"

void UCreatureUserWidget::NativeConstruct() {
	Super::NativeConstruct();
		

	UE_LOG(LogTemp, Warning, TEXT("lefutok"));
	
	//AddToPlayerScreen();
}

void UCreatureUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (creature != nullptr) {
		HealthBar->SetPercent(creature->GetHealth() / creature->GetMaxHealth());
	}
}