// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "UMG/Public/Components/Button.h"
#include <Kismet/KismetSystemLibrary.h>
#include <Kismet/GameplayStatics.h>

void UMainMenu::NativeConstruct() {
	Super::NativeConstruct();

	StartButton->OnClicked.AddDynamic(this, &UMainMenu::OnStartClick);

	ControlsButton->OnClicked.AddDynamic(this, &UMainMenu::OnControlsClick);

	QuitButton->OnClicked.AddDynamic(this, &UMainMenu::OnQuitClick);
}

void UMainMenu::OnStartClick() {
	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, levelToLoad, true);
}

void UMainMenu::OnControlsClick() {

}

void UMainMenu::OnQuitClick() {
	UKismetSystemLibrary::QuitGame(GetWorld(), UGameplayStatics::GetPlayerController(GetWorld(), 0), EQuitPreference::Quit, false);
}