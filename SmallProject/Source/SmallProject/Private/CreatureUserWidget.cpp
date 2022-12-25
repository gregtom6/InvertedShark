// Fill out your copyright notice in the Description page of Project Settings.


#include "CreatureUserWidget.h"

void UCreatureUserWidget::NativeConstruct() {
	Super::NativeConstruct();
		

	UE_LOG(LogTemp, Warning, TEXT("lefutok"));
	
	//if (HUDClass)
	//{
		/*
		UE_LOG(LogTemp, Warning, TEXT("lefutok2"));
		LifeBarWidget = CreateWidget(GetWorld(), HUDClass);
		if (LifeBarWidget)
		{
			LifeBarWidget->AddToViewport();
		}
		*/
	//}
	
}

void UCreatureUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (creature != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("creature nem null"));

		HealthBar->SetPercent(creature->GetHealth() / creature->GetMaxHealth());
	}
}