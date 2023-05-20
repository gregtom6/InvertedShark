// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUserWidget.h"
#include "UMG/Public/Components/Button.h"
#include "GameCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>

/*
pause management methods
*/
void UPauseUserWidget::OnResumeClick() {
	if (gameChar) {
		UE_LOG(LogTemp, Warning, TEXT("pause hivas"));
		gameChar->Pause();
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("pause gamechar null"));
	}
}

void UPauseUserWidget::OnQuitToMainMenuClick() {
	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, levelToLoad, true);
}

/*
setting player reference
*/
void UPauseUserWidget::SetGameCharacter(AGameCharacter* gameCharacter) {

	UE_LOG(LogTemp, Warning, TEXT("pause setgamechar"));

	gameChar = gameCharacter;

	ResumeButton->OnClicked.AddUniqueDynamic(this, &UPauseUserWidget::OnResumeClick);

	QuitToMainMenuButton->OnClicked.AddUniqueDynamic(this, &UPauseUserWidget::OnQuitToMainMenuClick);
}

void UPauseUserWidget::NativeDestruct() {
	Super::NativeDestruct();

	ResumeButton->OnClicked.RemoveDynamic(this, &UPauseUserWidget::OnResumeClick);

	QuitToMainMenuButton->OnClicked.RemoveDynamic(this, &UPauseUserWidget::OnQuitToMainMenuClick);
}