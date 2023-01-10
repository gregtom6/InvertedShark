// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseUserWidget.h"
#include "UMG/Public/Components/Button.h"
#include "GameCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>

void UPauseUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	
}

void UPauseUserWidget::OnResumeClick() {
	if (gameChar != nullptr) {
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

void UPauseUserWidget::SetGameCharacter(AGameCharacter* gameCharacter) {

	UE_LOG(LogTemp, Warning, TEXT("pause setgamechar"));

	gameChar = gameCharacter;

	ResumeButton->OnClicked.AddDynamic(this, &UPauseUserWidget::OnResumeClick);

	QuitToMainMenuButton->OnClicked.AddDynamic(this, &UPauseUserWidget::OnQuitToMainMenuClick);
}