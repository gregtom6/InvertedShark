// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterUserWidget.h"
#include <GameCharacter.h>
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"


void UGameCharacterUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	AddToPlayerScreen();

	fadeStatus = FadeStatus::Invisible;
	Energybar->SetRenderOpacity(0.f);
	
}

/*
reading player energy and setting visuals
*/
void UGameCharacterUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (player != nullptr) {

		float percentage = player->GetEnergy() / player->GetMaxEnergy();

		Energybar->SetPercent(percentage);

		if (fadeStatus == FadeStatus::Visible) {

			if (percentage >= 1.f) {
				fadeStatus = FadeStatus::VisibleToInvisible;
				fadeStartTime = GetWorld()->GetTimeSeconds();
			}
		}
		else if (fadeStatus == FadeStatus::Invisible) {
			if (percentage < 1.f) {
				fadeStatus = FadeStatus::InvisibleToVisible;
				fadeStartTime = GetWorld()->GetTimeSeconds();
			}
		}
		else if (fadeStatus == FadeStatus::InvisibleToVisible) {
			float current = GetWorld()->GetTimeSeconds() - fadeStartTime;
			float fadePercent = current / fadeTime;
			Energybar->SetRenderOpacity(FMath::Lerp(0.f, 1.f, fadePercent));

			if (fadePercent >= 1.f) {
				fadeStatus = FadeStatus::Visible;
			}
		}
		else if (fadeStatus == FadeStatus::VisibleToInvisible) {
			float current = GetWorld()->GetTimeSeconds() - fadeStartTime;
			float fadePercent = current / fadeTime;
			Energybar->SetRenderOpacity(FMath::Lerp(1.f, 0.f, fadePercent));

			if (fadePercent >= 1.f) {
				fadeStatus = FadeStatus::Invisible;
			}
		}
	}
}