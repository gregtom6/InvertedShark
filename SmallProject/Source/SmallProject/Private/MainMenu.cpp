// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/WidgetSwitcher.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <Sound/SoundCue.h>

void UMainMenu::NativeConstruct() {
	Super::NativeConstruct();

	StartButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::OnStartClick);

	ControlsButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::OnControlsClick);

	QuitButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::OnQuitClick);

	BackButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::OnBackClick);

	HowToPlayButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::OnHowToPlayClick);

	BackToMainMenuButton->OnClicked.AddUniqueDynamic(this, &UMainMenu::OnBackClick);
}

void UMainMenu::NativeDestruct() {
	Super::NativeDestruct();

	StartButton->OnClicked.RemoveDynamic(this, &UMainMenu::OnStartClick);

	ControlsButton->OnClicked.RemoveDynamic(this, &UMainMenu::OnControlsClick);

	QuitButton->OnClicked.RemoveDynamic(this, &UMainMenu::OnQuitClick);

	BackButton->OnClicked.RemoveDynamic(this, &UMainMenu::OnBackClick);

	HowToPlayButton->OnClicked.RemoveDynamic(this, &UMainMenu::OnHowToPlayClick);

	BackToMainMenuButton->OnClicked.RemoveDynamic(this, &UMainMenu::OnBackClick);
}

/*
button click event management
*/
void UMainMenu::OnStartClick() {
	UGameplayStatics::PlaySound2D(this, ButtonClickSound);

	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, levelToLoad, true);
}

void UMainMenu::OnControlsClick() {

	UGameplayStatics::PlaySound2D(this, ButtonClickSound);

	WidgetSwitcher->SetActiveWidgetIndex(1);
}

void UMainMenu::OnQuitClick() {
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
}

void UMainMenu::OnHowToPlayClick() {
	UGameplayStatics::PlaySound2D(this, ButtonClickSound);

	WidgetSwitcher->SetActiveWidgetIndex(2);
}

void UMainMenu::OnBackClick() {
	UGameplayStatics::PlaySound2D(this, ButtonClickSound);

	WidgetSwitcher->SetActiveWidgetIndex(0);
}