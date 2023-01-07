// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryScreenUserWidget.h"
#include "UMG/Public/Components/Button.h"
#include <Kismet/GameplayStatics.h>

void UVictoryScreenUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	ReturnToMenuButton->OnClicked.AddDynamic(this, &UVictoryScreenUserWidget::OnReturnMenuClick);
}

void UVictoryScreenUserWidget::OnReturnMenuClick() {
	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, levelToLoad, true);
}