// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Components/Button.h"

void UDeadUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	ResurrectButton->OnClicked.AddUniqueDynamic(this, &UDeadUserWidget::OnResurrectClick);
	BackToMenuButton->OnClicked.AddUniqueDynamic(this, &UDeadUserWidget::OnBackToMenuClick);
}

void UDeadUserWidget::NativeDestruct()
{
	Super::NativeDestruct();

	ResurrectButton->OnClicked.RemoveDynamic(this, &UDeadUserWidget::OnResurrectClick);
	BackToMenuButton->OnClicked.RemoveDynamic(this, &UDeadUserWidget::OnBackToMenuClick);
}

/*
death screen (player felt down) button clickevents
*/
void UDeadUserWidget::OnResurrectClick() {
	UE_LOG(LogTemp, Warning, TEXT("pause gamechar null"));

	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, mapLevelToLoad, true);
}

void UDeadUserWidget::OnBackToMenuClick() {
	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, menuLevelToLoad, true);
}