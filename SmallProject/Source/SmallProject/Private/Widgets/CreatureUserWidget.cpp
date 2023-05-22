// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/CreatureUserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/HealthComponent.h"
#include "Allies/Creature.h"

void UCreatureUserWidget::NativeConstruct() {
	Super::NativeConstruct();

	creatureLifeStatus = ECreatureLifeStatus::Normal;

	IncreaseDeltaBar->SetPercent(0.f);

	DecreaseDeltaBar->SetPercent(0.f);

	if (creature)
		creature->OnTakePointDamage.AddUniqueDynamic(this, &UCreatureUserWidget::TakePointDamage);
}

void UCreatureUserWidget::NativeDestruct() {
	Super::NativeDestruct();

	if (creature)
		creature->OnTakePointDamage.RemoveDynamic(this, &UCreatureUserWidget::TakePointDamage);
}

/*
showing health of creature
*/
void UCreatureUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!creatureHealthComp) { return; }

	float currentLifePercentage = creatureHealthComp->GetHealth() / creatureHealthComp->GetMaxHealth();

	HealthBar->SetPercent(currentLifePercentage);

	if (creature->GetStatus() == EStatus::Healing) {
		IncreaseDeltaBar->SetPercent(creatureHealthComp->GetDeltaIncreaseHealth());
	}
	else if (FMath::IsNearlyEqual(IncreaseDeltaBar->GetPercent(), 1.f)) {
		IncreaseDeltaBar->SetPercent(0.f);
	}

	if (creatureLifeStatus == ECreatureLifeStatus::WaitUntilDecrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= timeUntilDeltaDecreaseStartDisappear) {
			creatureLifeStatus = ECreatureLifeStatus::Decrease;
			startTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (creatureLifeStatus == ECreatureLifeStatus::Decrease) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= timeForDisappear;

		DecreaseDeltaBar->SetPercent(FMath::Lerp(deltaLifePercentage, currentLifePercentage, currentTime));

		if (currentTime >= timeForDisappear) {
			creatureLifeStatus = ECreatureLifeStatus::Normal;
			DecreaseDeltaBar->SetPercent(0.f);
			creatureHealthComp->OriginalLifeRepresentationEnded();
		}
	}
}

void UCreatureUserWidget::TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType,
	AActor* DamageCauser) {


	if (creatureLifeStatus != ECreatureLifeStatus::Normal) { return; }

	deltaLifePercentage = creatureHealthComp->GetOriginalLifeBeforeAttack() / creatureHealthComp->GetMaxHealth();

	creatureLifeStatus = ECreatureLifeStatus::WaitUntilDecrease;
	DecreaseDeltaBar->SetPercent(deltaLifePercentage);
	startTime = GetWorld()->GetTimeSeconds();

}