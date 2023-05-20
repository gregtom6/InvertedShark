// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacterUserWidget.h"
#include <GameCharacter.h>
#include "Components/ProgressBar.h"
#include "Components/CanvasPanel.h"
#include "ResourceDataAsset.h"


void UGameCharacterUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	fadeStatus = EFadeStatus::Invisible;
	Energybar->SetRenderOpacity(0.f);

	globalSettings = NewObject<UResourceDataAsset>(GetTransientPackage(), FName("globalSettings"));
}

/*
reading player energy and setting visuals
*/
void UGameCharacterUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime) {
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!player) { return; }

	float percentage = player->GetEnergy() / player->GetMaxEnergy();

	Energybar->SetPercent(percentage);

	if (fadeStatus == EFadeStatus::Visible) {

		if (percentage >= globalSettings->FullPercent) {
			fadeStatus = EFadeStatus::VisibleToInvisible;
			fadeStartTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (fadeStatus == EFadeStatus::Invisible) {
		if (percentage < globalSettings->FullPercent) {
			fadeStatus = EFadeStatus::InvisibleToVisible;
			fadeStartTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (fadeStatus == EFadeStatus::InvisibleToVisible) {
		float current = GetWorld()->GetTimeSeconds() - fadeStartTime;
		float fadePercent = current / fadeTime;
		Energybar->SetRenderOpacity(FMath::Lerp(0.f, 1.f, fadePercent));

		if (fadePercent >= globalSettings->FullPercent) {
			fadeStatus = EFadeStatus::Visible;
		}
	}
	else if (fadeStatus == EFadeStatus::VisibleToInvisible) {
		float current = GetWorld()->GetTimeSeconds() - fadeStartTime;
		float fadePercent = current / fadeTime;
		Energybar->SetRenderOpacity(FMath::Lerp(1.f, 0.f, fadePercent));

		if (fadePercent >= globalSettings->FullPercent) {
			fadeStatus = EFadeStatus::Invisible;
		}
	}
}