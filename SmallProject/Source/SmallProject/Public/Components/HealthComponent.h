// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SMALLPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		void OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation,
			UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection,
			const UDamageType* DamageType, AActor* DamageCauser);

	UPROPERTY(EditAnywhere)
		float MaxHealth;

	float Health;

	float originalLifeBeforeAttack;

	float actualHealthWhenStartedHealing;

	float deltaHeal;

	float actualTime;

public:

	float GetOriginalLifeBeforeAttack() const;

	void OriginalLifeRepresentationEnded();

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	float GetActualHealthWhenStartedHealing() const { return actualHealthWhenStartedHealing; }
	float GetDeltaIncreaseHealth() const { return actualHealthWhenStartedHealing + (deltaHeal * actualTime); }

	void HealingStarted();
};
