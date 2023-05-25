// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/HealthComponent.h"
#include "HealDamageType.h"
#include "Allies/GameCharacter.h"
#include "Enemies/Enemy.h"
#include "ProjectileDamageType.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	Health = MaxHealth;

	originalLifeBeforeAttack = Health;

	GetOwner()->OnTakePointDamage.AddUniqueDynamic(this, &UHealthComponent::OnTakePointDamage);
}

void UHealthComponent::EndPlay(const EEndPlayReason::Type EndPlayReason) {

	Super::EndPlay(EndPlayReason);

	GetOwner()->OnTakePointDamage.RemoveDynamic(this, &UHealthComponent::OnTakePointDamage);
}

void UHealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType,
	AActor* DamageCauser) {

	if (DamagedActor != GetOwner()) { return; }

	if (!GetOwner()->CanBeDamaged()) { return; }

	if (DamageType->IsA(UHealDamageType::StaticClass())) {
		//healing
		Health = Health < MaxHealth ? actualHealthWhenStartedHealing + Damage : MaxHealth;
	}
	else {
		originalLifeBeforeAttack = Health;
		//making damage
		Health = Health - Damage > 0 ? Health - Damage : 0;

		if (Health <= 0.f) {
			if (GetOwner()->IsA(AGameCharacter::StaticClass()) && DamageType->IsA(UProjectileDamageType::StaticClass())) {
				AGameCharacter* gameCharacter = Cast<AGameCharacter>(GetOwner());

				gameCharacter->DoAfterGettingHitFromProjectile(ShotFromDirection);
			}
			else if (GetOwner()->IsA(AEnemy::StaticClass())) {
				AEnemy* enemy = Cast<AEnemy>(GetOwner());

				if (enemy->GetActualStatus() == EEnemyStatus::SpecialDying) { return; }

				enemy->RemoveEnemy();
			}
		}
	}

}

void UHealthComponent::HealingStarted() {
	actualHealthWhenStartedHealing = Health;
}

float UHealthComponent::GetOriginalLifeBeforeAttack() const {
	return originalLifeBeforeAttack;
}

void UHealthComponent::OriginalLifeRepresentationEnded() {
	originalLifeBeforeAttack = Health;
}