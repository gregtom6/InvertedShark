// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUserWidget.h"
#include "UMG/Public/Components/Button.h"
#include <Kismet/GameplayStatics.h>

void UPauseUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	ResumeButton->OnClicked.AddDynamic(this, &UPauseUserWidget::OnResumeClick);

	QuitToMainMenuButton->OnClicked.AddDynamic(this, &UPauseUserWidget::OnQuitToMainMenuClick);
}

void UPauseUserWidget::OnResumeClick() {

}

void UPauseUserWidget::OnQuitToMainMenuClick() {
	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, levelToLoad, true);
}