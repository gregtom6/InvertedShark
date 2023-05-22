// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/Projectile.h"
#include "Allies/Creature.h"
#include "Components/AudioComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "Allies/GameCharacter.h"
#include "Materials/MaterialInterface.h"
#include <Kismet/GameplayStatics.h>
#include "DataAssets/ResourceDataAsset.h"

// Sets default values
AProjectile::AProjectile(const FObjectInitializer& ObjectInitializer) :Super(ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	projectileHittedTargetAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("projectileHittedTargetAudioComp"));
	SetRootComponent(projectileHittedTargetAudioComp);

	creatureHitBloodNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("creatureHitBloodNiagara"));

	staticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("staticMesh"));
	staticMesh->AttachToComponent(projectileHittedTargetAudioComp, FAttachmentTransformRules::KeepRelativeTransform);

	globalSettings = NewObject<UResourceDataAsset>(GetTransientPackage(), FName("globalSettings"));
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	status = EProjectileStatus::Initial;


	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	APawn* pawn = OurPlayerController->GetPawn();

	gameChar = Cast<AGameCharacter, APawn>(pawn);


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

	if (status == EProjectileStatus::FlyToTarget && target != FVector::Zero()) {

		FVector newLocation = GetActorLocation() + directionVector * speed;

		SetActorLocation(newLocation);
	}
	else if (status == EProjectileStatus::MoveInsideTarget && target != FVector::Zero()) {

		FVector newLocation = GetActorLocation() + directionVector * speed;

		float currentDistance = FVector::Distance(target, newLocation);

		if (currentDistance >= distanceToProceedInsideTarget + startDistance) {
			status = EProjectileStatus::StopMovement;
			startTime = GetWorld()->GetTimeSeconds();
			staticMesh->SetMaterial(0, invisibleMaterial);
		}
		else {
			SetActorLocation(newLocation);
		}
	}
	else if (status == EProjectileStatus::StopMovement) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		currentTime /= timeUntilDestroy;
		if (currentTime >= globalSettings->FullPercent) {
			bIsActivated = false;
			status = EProjectileStatus::Initial;
			SetActorLocation(shooterActor->GetActorLocation());
			staticMesh->SetMaterial(0, invisibleMaterial);
		}
	}

	if (creatureHitBloodNiagara->IsActive()) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTimeForBloodFlow;

		if (currentTime >= timeUntilBloodflowStop) {
			creatureHitBloodNiagara->Deactivate();
		}
	}

	TimeManagement();
}

void AProjectile::TimeManagement() {
	if (!gameChar) { return; }

	projectileHittedTargetAudioComp->SetPitchMultiplier(gameChar->GetCurrentSoundPitchMultiplier());
}

void AProjectile::Event(class AActor* overlappedActor, class AActor* otherActor) {

	if (status != EProjectileStatus::FlyToTarget || !bIsActivated) { return; }

	if (otherActor && otherActor != this) {
		if (otherActor->IsA(ACreature::StaticClass()) || otherActor->IsA(AGameCharacter::StaticClass())) {
			startDistance = FVector::Distance(otherActor->GetActorLocation(), GetActorLocation());

			targetedActor = otherActor;

			if (targetedActor && shooterActor) {
				FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(targetedActor->GetActorLocation(), shooterActor->GetActorLocation());
				FRotator actorRotation = targetedActor->GetActorRotation();
				FRotator sum = targetRotation - actorRotation;
				sum.Roll += sum.Pitch;
				sum.Pitch -= sum.Pitch;
				sum.Yaw += 90.f;

				FVector direction = targetedActor->GetActorLocation() - shooterActor->GetActorLocation();

				if (targetedActor->IsA(AGameCharacter::StaticClass())) {

					AGameCharacter* gameCharacter = Cast<AGameCharacter>(targetedActor);

					if (gameCharacter->GetStatus() == EGameCharacterStatus::Dead) { return; }

					CheckOverlappingComponents(gameCharacter, direction, sum);
				}
				else if (targetedActor->IsA(ACreature::StaticClass())) {
					ACreature* creature = Cast<ACreature>(targetedActor);

					CheckOverlappingComponents(creature, direction, sum);
				}
			}
		}
	}
}

void AProjectile::CheckOverlappingComponents(AActor* currentlyTargetedActor, const FVector direction, const FRotator sum) {
	
	TArray<UPrimitiveComponent*> OverlappingComponents;
	staticMesh->GetOverlappingComponents(OverlappingComponents);

	UE_LOG(LogTemp, Log, TEXT("overlapping comp --------------"));

	for (UPrimitiveComponent* Component : OverlappingComponents)
	{
		// Check if the static mesh is overlapping with the current component
		if (staticMesh->IsOverlappingComponent(Component))
		{
			// The static mesh is overlapping with this component
			FName ComponentName = Component->GetFName();
			FString MyString = ComponentName.ToString();
			MyString.RemoveAt(MyString.Len() - 1);

			UE_LOG(LogTemp, Log, TEXT("overlapping comp: %s"), *MyString);
			if (MyString != FString("InnerProjectile_") && MyString != FString("InnerProjectile_1")/* && MyString != FString("Bo")*/) {
				UE_LOG(LogTemp, Log, TEXT("overlapping comp letrehozas %s"), *MyString);

				projectileHittedTargetAudioComp->Play(0.f);

				status = EProjectileStatus::MoveInsideTarget;
				FVector scale=SettingUpScale(currentlyTargetedActor);
				if (currentlyTargetedActor->IsA(AGameCharacter::StaticClass())) {

					AGameCharacter* gameCharacter = Cast<AGameCharacter>(currentlyTargetedActor);
					gameCharacter->SetupProjectile(sum, scale, staticMesh->GetStaticMesh(), staticMesh->GetMaterial(0), target);
					gameCharacter->DoAfterGettingHitFromProjectile(direction);
				}
				else if (currentlyTargetedActor->IsA(ACreature::StaticClass())) {

					ACreature* creature = Cast<ACreature>(currentlyTargetedActor);
					creature->SetupProjectile(sum, scale, staticMesh->GetStaticMesh(), staticMesh->GetMaterial(0), target);

					FHitResult HitOut;

					UGameplayStatics::ApplyPointDamage(creature, damageCaused, direction, HitOut, GetInstigatorController(), this, projectileDamageType);
				}
				
				staticMesh->SetMaterial(0, invisibleMaterial);
			}
			else {
				UE_LOG(LogTemp, Log, TEXT("overlapping comp internal volt"));
			}
		}
	}
}

FVector AProjectile::SettingUpScale(AActor* currentTargetedActor) {

	if (currentTargetedActor->IsA(AGameCharacter::StaticClass())) {
		AGameCharacter* gameCharacter = Cast<AGameCharacter>(currentTargetedActor);
		UStaticMeshComponent* gameCharacterStaticMesh = gameCharacter->GetStaticMeshComponent();
		FVector compScale = staticMesh->GetComponentScale() * 1.f + (1.f - gameCharacterStaticMesh->GetComponentScale().X);
		return compScale * compScale;
	}
	else if (currentTargetedActor->IsA(ACreature::StaticClass())) {
		return staticMesh->GetComponentScale();
	}

	return FVector::OneVector;
}

void AProjectile::SetTarget(AActor* tar, AActor* origin) {

	if (!tar) { return; }

	float offset = offsetForHittingTarget;

	FVector actorLocation = tar->GetActorLocation();

	if (tar->IsA(AGameCharacter::StaticClass())) {

		AGameCharacter* gameCharacter = Cast<AGameCharacter>(tar);

		if (gameCharacter->GetStatus() != EGameCharacterStatus::Calm) {
			actorLocation = gameCharacter->GetBackBeforeDashLocation();
		}
	}

	target = FVector(FMath::FRandRange(actorLocation.X - offset, actorLocation.X + offset),
		FMath::FRandRange(actorLocation.Y - offset, actorLocation.Y + offset),
		FMath::FRandRange(actorLocation.Z - offset, actorLocation.Z + offset));

	shooterActor = origin;

	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(target, shooterActor->GetActorLocation());
	SetActorRotation(targetRotation);

	status = EProjectileStatus::FlyToTarget;

	directionVector = target - GetActorLocation();
	directionVector.Normalize();

	bIsActivated = true;

	staticMesh->SetMaterial(0, originalMaterial);
}