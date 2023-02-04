// Fill out your copyright notice in the Description page of Project Settings.


#include "DeadUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Components/Button.h"

void UDeadUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	ResurrectButton->OnClicked.AddDynamic(this, &UDeadUserWidget::OnResurrectClick);

	BackToMenuButton->OnClicked.AddDynamic(this, &UDeadUserWidget::OnBackToMenuClick);
}

void UDeadUserWidget::OnResurrectClick() {
	UE_LOG(LogTemp, Warning, TEXT("pause gamechar null"));

	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, mapLevelToLoad, true);
}

void UDeadUserWidget::OnBackToMenuClick() {
	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, menuLevelToLoad, true);
}