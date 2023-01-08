// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "UMG/Public/Components/Button.h"
#include "UMG/Public/Components/WidgetSwitcher.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>

void UMainMenu::NativeConstruct() {
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &UMainMenu::OnStartClick);

	ControlsButton->OnClicked.AddDynamic(this, &UMainMenu::OnControlsClick);

	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClick);

	BackButton->OnClicked.AddDynamic(this, &UMainMenu::OnBackClick);

	HowToPlayButton->OnClicked.AddDynamic(this, &UMainMenu::OnHowToPlayClick);

	BackToMainMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OnBackClick);
}

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