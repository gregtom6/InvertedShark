// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CreatureUserWidget.generated.h"

class UProgressBar;
class ACreature;
class UHealthComponent;

//shows the life of the Creature, which player needs to defend constantly

UCLASS()
class SMALLPROJECT_API UCreatureUserWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UFUNCTION()
		void TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation,
			UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const UDamageType* DamageType, AActor* DamageCauser);

	UPROPERTY(meta = (BindWidget))
		UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
		UProgressBar* IncreaseDeltaBar;

	UPROPERTY(meta = (BindWidget))
		UProgressBar* DecreaseDeltaBar;

	UPROPERTY(EditAnywhere)
		float timeUntilDeltaDecreaseStartDisappear;

	UPROPERTY(EditAnywhere)
		float timeForDisappear;

	ECreatureLifeStatus creatureLifeStatus;
	float startTime;

	float deltaLifePercentage;

public:
	UPROPERTY()
		UHealthComponent* creatureHealthComp;

	UPROPERTY()
		ACreature* creature;
};


UENUM()
enum class ECreatureLifeStatus : uint8
{
	Normal,
	WaitUntilDecrease,
	Decrease,
};