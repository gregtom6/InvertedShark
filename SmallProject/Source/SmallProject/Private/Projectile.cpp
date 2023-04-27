// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Creature.h"
#include "Components/AudioComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

// Sets default values
AProjectile::AProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	projectileHittedTargetAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("projectileHittedTargetAudioComp"));
	projectileHittedTargetAudioComp->SetupAttachment(RootComponent);

	creatureHitBloodNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("creatureHitBloodNiagara"));
	//creatureHitBloodNiagara->AttachToComponent(projectileHittedTargetAudioComp, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	status = ProjectileStatus::Initial;

	OnActorBeginOverlap.AddUniqueDynamic(this, &AProjectile::Event);
}

void AProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason) {
	Super::EndPlay(EndPlayReason);

	OnActorBeginOverlap.RemoveDynamic(this, &AProjectile::Event);
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (status == ProjectileStatus::FlyToTarget && target != FVector::Zero()) {

		if (directionVector == FVector::Zero()) {
			directionVector = target - GetActorLocation();
			directionVector.Normalize();
		}

		FVector newLocation = GetActorLocation() + directionVector * speed;

		SetActorLocation(newLocation);
	}
	else if (status == ProjectileStatus::MoveInsideTarget && target != FVector::Zero()) {

		FVector newLocation = GetActorLocation() + directionVector * speed;

		float currentDistance = FVector::Distance(target, newLocation);

		if (currentDistance >= distanceToProceedInsideTarget + startDistance) {
			status = ProjectileStatus::StopMovement;
		}
		else {
			SetActorLocation(newLocation);
		}
	}

	if (creatureHitBloodNiagara->IsActive()) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTimeForBloodFlow;

		if (currentTime >= timeUntilBloodflowStop) {
			creatureHitBloodNiagara->Deactivate();
		}
	}
}

void AProjectile::Event(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {
		if (otherActor->IsA(ACreature::StaticClass())) {
			startDistance = FVector::Distance(otherActor->GetActorLocation(), GetActorLocation());
			status = ProjectileStatus::MoveInsideTarget;

			projectileHittedTargetAudioComp->Play(0.f);
			creatureHitBloodNiagara->SetWorldLocation(otherActor->GetActorLocation());
			creatureHitBloodNiagara->Activate();
			startTimeForBloodFlow = GetWorld()->GetTimeSeconds();
		}
	}
}

void AProjectile::SetTarget(AActor* t) {

	float offset = offsetForHittingTarget;

	target = FVector(FMath::FRandRange(t->GetActorLocation().X-offset, t->GetActorLocation().X + offset),
		FMath::FRandRange(t->GetActorLocation().Y - offset, t->GetActorLocation().Y + offset),
		FMath::FRandRange(t->GetActorLocation().Z - offset, t->GetActorLocation().Z + offset));

	status = ProjectileStatus::FlyToTarget;
}