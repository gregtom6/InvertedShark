// Fill out your copyright notice in the Description page of Project Settings.


#include "VictoryScreenUserWidget.h"
#include "UMG/Public/Components/Button.h"
#include <Kismet/GameplayStatics.h>

void UVictoryScreenUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	ReturnToMenuButton->OnClicked.AddUniqueDynamic(this, &UVictoryScreenUserWidget::OnReturnMenuClick);
}

/*
returning to main menu, when player won the game (defeated the boss)
*/
void UVictoryScreenUserWidget::OnReturnMenuClick() {
	UGameplayStatics::PlaySound2D(this, ButtonClickSound);

	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, levelToLoad, true);
}