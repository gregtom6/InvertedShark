// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"
#include "Creature.h"
#include "Components/AudioComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include <Kismet/KismetMathLibrary.h>
#include "GameCharacter.h"
#include "Materials/MaterialInterface.h"
#include <Kismet/GameplayStatics.h>

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
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	status = ProjectileStatus::Initial;


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

	if (status == ProjectileStatus::FlyToTarget && target != FVector::Zero()) {

		FVector newLocation = GetActorLocation() + directionVector * speed;

		SetActorLocation(newLocation);
	}
	else if (status == ProjectileStatus::MoveInsideTarget && target != FVector::Zero()) {

		FVector newLocation = GetActorLocation() + directionVector * speed;

		float currentDistance = FVector::Distance(target, newLocation);

		if (currentDistance >= distanceToProceedInsideTarget + startDistance) {
			status = ProjectileStatus::StopMovement;
			startTime = GetWorld()->GetTimeSeconds();
			staticMesh->SetMaterial(0, invisibleMaterial);
		}
		else {
			SetActorLocation(newLocation);
		}
	}
	else if (status == ProjectileStatus::StopMovement) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		currentTime /= timeUntilDestroy;
		if (currentTime >= 1.f) {
  			status = ProjectileStatus::Initial;
			SetActorLocation(shooterActor->GetActorLocation());
			staticMesh->SetMaterial(0, invisibleMaterial);
			isActivated = false;
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
	if (gameChar == nullptr) { return; }

	projectileHittedTargetAudioComp->SetPitchMultiplier(gameChar->GetCurrentSoundPitchMultiplier());
}

void AProjectile::Event(class AActor* overlappedActor, class AActor* otherActor) {

	if (status !=ProjectileStatus::FlyToTarget || !isActivated) { return; }

	if (otherActor && otherActor != this) {
		if (otherActor->IsA(ACreature::StaticClass()) || otherActor->IsA(AGameCharacter::StaticClass())) {
			startDistance = FVector::Distance(otherActor->GetActorLocation(), GetActorLocation());

			targetedActor = otherActor;

			if (targetedActor != nullptr && shooterActor != nullptr) {
				FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(targetedActor->GetActorLocation(), shooterActor->GetActorLocation());
				FRotator actorRotation = targetedActor->GetActorRotation();
				FRotator sum = targetRotation - actorRotation;
				sum.Roll += sum.Pitch;
				sum.Pitch -= sum.Pitch;
				sum.Yaw += 90.f;

				FVector direction = targetedActor->GetActorLocation() - shooterActor->GetActorLocation();

				if (targetedActor->IsA(AGameCharacter::StaticClass())) {

					AGameCharacter* gameCharacter = Cast<AGameCharacter>(targetedActor);

					if (gameCharacter->GetStatus() == GameCharacterStatus::Dead) { return; }

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

								const UEnum* EnumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("ProjectileStatus"), true);
								if (EnumPtr)
								{
									FString EnumValueString = EnumPtr->GetNameStringByIndex(static_cast<uint8>(status));
									UE_LOG(LogTemp, Log, TEXT("overlapping enum value: %s"), *EnumValueString);
								}

 								status = ProjectileStatus::MoveInsideTarget;
 								gameCharacter->SetupProjectile(sum, staticMesh->GetComponentScale(), staticMesh->GetStaticMesh(), staticMesh->GetMaterial(0), target);
								gameCharacter->DoAfterGettingHitFromProjectile(direction);
								staticMesh->SetMaterial(0, invisibleMaterial);
							}
							else {
								UE_LOG(LogTemp, Log, TEXT("overlapping comp internal volt"));
							}
						}
					}

				}
				else if (targetedActor->IsA(ACreature::StaticClass())) {
					ACreature* creature = Cast<ACreature>(targetedActor);

					TArray<UPrimitiveComponent*> OverlappingComponents;
					staticMesh->GetOverlappingComponents(OverlappingComponents);

					UE_LOG(LogTemp, Log, TEXT("overlapping comp --------------"));

					for (UPrimitiveComponent* Component : OverlappingComponents)
					{
						// Check if the static mesh is overlapping with the current component
						if (staticMesh->IsOverlappingComponent(Component))
						{
							FName ComponentName = Component->GetFName();
							FString MyString = ComponentName.ToString();
							MyString.RemoveAt(MyString.Len() - 1);

							UE_LOG(LogTemp, Log, TEXT("overlapping comp: %s"), *MyString);


							projectileHittedTargetAudioComp->Play(0.f);
  							status = ProjectileStatus::MoveInsideTarget;
 							creature->SetupProjectile(sum, staticMesh->GetComponentScale(), staticMesh->GetStaticMesh(), staticMesh->GetMaterial(0), target);
							creature->DoAfterGettingHitFromProjectile();
							staticMesh->SetMaterial(0, invisibleMaterial);

						}
					}
				}
			}
		}
	}
}

void AProjectile::SetTarget(AActor* tar, AActor* origin) {

	if (tar == nullptr) { return; }

	float offset = offsetForHittingTarget;

	FVector actorLocation = tar->GetActorLocation();

	if (tar->IsA(AGameCharacter::StaticClass())) {

		AGameCharacter* gameCharacter = Cast<AGameCharacter>(tar);

		if (gameCharacter->GetStatus() != GameCharacterStatus::Calm) {
			actorLocation = gameCharacter->GetBackBeforeDashLocation();
		}
	}

	target = FVector(FMath::FRandRange(actorLocation.X - offset, actorLocation.X + offset),
		FMath::FRandRange(actorLocation.Y - offset, actorLocation.Y + offset),
		FMath::FRandRange(actorLocation.Z - offset, actorLocation.Z + offset));

	shooterActor = origin;

	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), target);
	SetActorRotation(targetRotation);

	status = ProjectileStatus::FlyToTarget;

	directionVector = target - GetActorLocation();
	directionVector.Normalize();

	isActivated = true;

	staticMesh->SetMaterial(0, originalMaterial);
}