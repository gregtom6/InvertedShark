// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Components/WidgetComponent.h"
#include "CreatureUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Enemy.h"
#include <Kismet/KismetMathLibrary.h>

// Sets default values
ACreature::ACreature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	/*
	if (RootComponent == nullptr)
	{
		RootComponent = ObjectInitializer.CreateDefaultSubobject<USceneComponent>(this, TEXT("Root"));
	}
	*/
	HealthWidgetComp = ObjectInitializer.CreateDefaultSubobject<UWidgetComponent>(this, TEXT("Healthbar"));
	//HealthWidgetComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	WhaleAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("WhaleAudioComp"));
	WhaleAudioComp->SetupAttachment(CreatureMesh);

	Health = MaxHealth;
}

// Called when the game starts or when spawned
void ACreature::BeginPlay()
{
	Super::BeginPlay();

	if (WhaleAudioComp && whaleSound) {
		UE_LOG(LogTemp, Warning, TEXT("hang jo"));
		WhaleAudioComp->SetSound(whaleSound);
	}

	WhaleAudioComp->Play(0.f);

	if (HealthWidgetComp != nullptr) {
		UUserWidget* wid = HealthWidgetComp->GetUserWidgetObject();

		if (wid != nullptr) {


			UCreatureUserWidget* widg = Cast<UCreatureUserWidget>(wid);

			UE_LOG(LogTemp, Warning, TEXT("UUserWidget nem null"));

			widg->creature = this;

			if (widg != nullptr) {
				UE_LOG(LogTemp, Warning, TEXT("creatureuserwidget nem null"));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("creatureuserwidget null"));
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("UUserWidget null"));
		}
	}

	StepTargetIndex();

	startTime = GetWorld()->GetTimeSeconds();

	actualStatus = Status::Initial;

	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), positionsToMove[actualTargetIndex]->GetActorLocation());

	SetActorRotation(PlayerRot);

	OnActorBeginOverlap.AddDynamic(this, &ACreature::EnterEvent);
	OnActorEndOverlap.AddDynamic(this, &ACreature::ExitEvent);
}

void ACreature::StepTargetIndex() {
	if (actualTargetIndex < positionsToMove.Num()) {
		actualStartPosition = GetActorLocation();
		actualEndPosition = positionsToMove[actualTargetIndex]->GetActorLocation();

		actualTargetIndex += 1;
	}
}

// Called every frame
void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
	if (actualStatus == Status::Initial) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		if (currentTime >= waitTimeBeforeFirstMove) {
			startTime = GetWorld()->GetTimeSeconds();
			actualStatus = Status::Moving;
		}
	}
	else if (actualStatus == Status::Moving) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		currentTime *= movementSpeed;
		if (currentTime > 1.f) {

			actualStatus = Status::Stopped;
			currentTime = 1.f;
		}

		UE_LOG(LogTemp, Warning, TEXT("%lf"), currentTime);


		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), actualEndPosition);

		SetActorRotation(PlayerRot);

		SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, currentTime));
	}
	else if (actualStatus == Status::WaitBeforeMoveFast) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime > waitingTimeToMoveForwardAfterDefeatingEnemies) {
			SwitchingToMovingFast();
		}
	}
	else if (actualStatus == Status::WaitAfterHuggedByPlayer) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime > waitTimeAfterHuggedToMoveForward) {
			SwitchingToMovingFast();
		}
	}
	else if (actualStatus == Status::MovingFast) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		currentTime *= fastMovementSpeed;
		if (currentTime > 1.f) {

			actualStatus = Status::Stopped;
			currentTime = 1.f;
		}

		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), actualEndPosition);

		SetActorRotation(PlayerRot);

		UE_LOG(LogTemp, Warning, TEXT("%lf"), currentTime);

		SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, currentTime));
	}

	UE_LOG(LogTemp, Warning, TEXT("attacking enemies: %d"), enemiesActuallyAttacking.Num());

	Health = Health > 0 ? Health - (deltaDamage * DeltaTime * enemiesActuallyAttacking.Num()) : 0;

	if (Health <= 0) {
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

void ACreature::EnterEvent(class AActor* overlappedActor, class AActor* otherActor) {
	UE_LOG(LogTemp, Warning, TEXT("enemy lefut"));
	if (otherActor && otherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("enemy lef2"));
		if (otherActor->IsA(AEnemy::StaticClass())) {

			AEnemy* attackingEnemy = Cast<AEnemy>(otherActor);
			if (!enemiesActuallyAttacking.Contains(attackingEnemy))
				enemiesActuallyAttacking.Add(attackingEnemy);
			actualStatus = Status::UnderAttack;
		}
	}
}

void ACreature::ExitEvent(class AActor* overlappedActor, class AActor* otherActor) {
	UE_LOG(LogTemp, Warning, TEXT("enemy lefut"));
	if (otherActor && otherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("enemy lef2"));
		if (otherActor->IsA(AEnemy::StaticClass())) {

			AEnemy* attackingEnemy = Cast<AEnemy>(otherActor);
			if (enemiesActuallyAttacking.Contains(attackingEnemy))
				enemiesActuallyAttacking.Remove(attackingEnemy);

			if (actualStatus == Status::UnderAttack && enemiesActuallyAttacking.Num() == 0) {
				WhaleAudioComp->Play(0.f);
				startTime = GetWorld()->GetTimeSeconds();
				actualStatus = Status::WaitBeforeMoveFast;
			}
		}
	}
}

void ACreature::TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	UE_LOG(LogTemp, Warning, TEXT("enemy lefut"));
	if (OtherActor && OtherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("enemy lef2"));
		if (OtherActor->IsA(AEnemy::StaticClass())) {
			UE_LOG(LogTemp, Warning, TEXT("enemy megtamadott"));


		}
	}
}

void ACreature::GetHugged() {

	if (actualStatus == Status::WaitBeforeMoveFast) {
		startTime = GetWorld()->GetTimeSeconds();
		actualStatus = Status::WaitAfterHuggedByPlayer;
	}
}

void ACreature::SwitchingToMovingFast() {
	startTime = GetWorld()->GetTimeSeconds();
	StepTargetIndex();
	actualStatus = Status::MovingFast;
}