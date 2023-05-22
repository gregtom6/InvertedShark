// Fill out your copyright notice in the Description page of Project Settings.


#include "Allies/Creature.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/CreatureUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Enemies/Enemy.h"
#include "Enemies/HealerEnemy.h"
#include "Allies/GameCharacter.h"
#include <Sound/SoundCue.h >
#include <Kismet/KismetMathLibrary.h>
#include "Enemies/SniperEnemy.h"
#include "Components/ProjectileCompPositioner.h"
#include "DataAssets/ResourceDataAsset.h"
#include "Items/EnemyTriggerBox.h"
#include "Components/HealthComponent.h"
#include "ProjectileDamageType.h"

ACreature::ACreature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	WhaleAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("WhaleAudioComp"));
	SetRootComponent(WhaleAudioComp);

	WhaleCryAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("WhaleCryAudio"));
	WhaleCryAudio->SetupAttachment(WhaleAudioComp);

	huggableComp = CreateDefaultSubobject<UBoxComponent>(TEXT("huggableComp"));
	huggableComp->SetupAttachment(WhaleAudioComp);

	headMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("headMesh"));
	headMesh->SetupAttachment(WhaleAudioComp);
	LeftEye = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftEye"));
	LeftEye->SetupAttachment(WhaleAudioComp);
	RightEye = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightEye"));
	RightEye->SetupAttachment(WhaleAudioComp);

	projectilePositioner = CreateDefaultSubobject<UProjectileCompPositioner>(TEXT("projectilePositioner"));

	healthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("healthComponent"));

	globalSettings = NewObject<UResourceDataAsset>(GetTransientPackage(), FName("globalSettings"));
}

/*
setting up whale sound and playing it
setting up initial status. While on initial status, creature goes slowly to the destination.
We adds creature life to viewport, setting creature reference inside it.
we rotates the creature to face towards his destination, and subscribing to events.
*/

void ACreature::BeginPlay()
{
	Super::BeginPlay();

	WhaleAudioComp->Play(0.f);

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("creature1"));
		creatureuserwidget = Cast<UCreatureUserWidget>(CreateWidget(GetWorld(), widgetclass));

		if (creatureuserwidget != nullptr) {

			creatureuserwidget->creature = this;
			creatureuserwidget->creatureHealthComp = healthComponent;

			UE_LOG(LogTemp, Warning, TEXT("creature2"));
			creatureuserwidget->AddToViewport(0);
		}
	}

	StepTargetIndex();

	startTime = GetWorld()->GetTimeSeconds();

	actualStatus = EStatus::Initial;

	prevHeadState = EHeadState::ForwardLooking;
	headState = EHeadState::ForwardLooking;

	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), positionsToMove[0]->GetActorLocation());

	SetActorRotation(PlayerRot);

	OnActorBeginOverlap.AddUniqueDynamic(this, &ACreature::EnterEvent);
	OnActorEndOverlap.AddUniqueDynamic(this, &ACreature::ExitEvent);

	OnTakePointDamage.AddUniqueDynamic(this, &ACreature::TakePointDamage);

	huggableComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ACreature::TriggerEnter);
	huggableComp->OnComponentEndOverlap.AddUniqueDynamic(this, &ACreature::TriggerExit);
}

void ACreature::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnActorBeginOverlap.RemoveDynamic(this, &ACreature::EnterEvent);
	OnActorEndOverlap.RemoveDynamic(this, &ACreature::ExitEvent);

	OnTakePointDamage.RemoveDynamic(this, &ACreature::TakePointDamage);

	huggableComp->OnComponentBeginOverlap.RemoveDynamic(this, &ACreature::TriggerEnter);
	huggableComp->OnComponentEndOverlap.RemoveDynamic(this, &ACreature::TriggerExit);
}

/*
if there is still a new location, where the creature can move, then we set that up.
*/

void ACreature::StepTargetIndex() {
	if (actualTargetIndex < positionsToMove.Num()) {
		actualStartPosition = GetActorLocation();
		actualEndPosition = positionsToMove[actualTargetIndex]->GetActorLocation();

		actualTargetIndex += 1;
	}
}

void ACreature::SetupProjectile(const FRotator rotator, const FVector scale, UStaticMesh* const& mesh, UMaterialInterface* const& material, const FVector offset) {

	projectilePositioner->SetupProjectile(rotator, scale, mesh, material, offset);
}

void ACreature::TakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy,
	FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName,
	FVector ShotFromDirection, const UDamageType* DamageType,
	AActor* DamageCauser) {

	if (DamagedActor != this) { return; }

	if (!DamageType->IsA(UProjectileDamageType::StaticClass())) { return; }

	WhaleCryAudio->Play();
}

void ACreature::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StateManagement();

	HeadStateManagement();

	HealthManagement(DeltaTime);
}

/*
managing different states.
initial status: basically waiting a bit to make player realize where he is at the moment.
moving status: this state is only for the first initial movement. It's a slow movement, only for entering the triggerbox, where enemies will arrive.
stopped status: creature reached its destination, movement can be stopped
WaitBeforeMoveFast status: when a group of enemies have been defeated, the creature gives a whale sound, and waits a bit before fast traveling to a new location.
			Player can survive only if hugs the creature during WaitBeforeMoveFast status
WaitAfterHuggedByPlayer status: this is only when creature is ready to travel forward. If player hugs the creature, it will still wait a bit before moving.
RotatingTowardsTarget status: before creature would move to its new location, we rotate the whole actor towards destination
MovingFast status: the fast travel status of the creature.
*/
void ACreature::StateManagement() {
	if (actualStatus == EStatus::Initial) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		if (currentTime >= waitTimeBeforeFirstMove) {
			startTime = GetWorld()->GetTimeSeconds();
			actualStatus = EStatus::Moving;
		}
	}
	else if (actualStatus == EStatus::Moving) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		currentTime *= movementSpeed;
		if (currentTime > globalSettings->FullPercent) {

			actualStatus = EStatus::Stopped;
			currentTime = globalSettings->FullPercent;
		}

		FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), actualEndPosition);

		SetActorRotation(PlayerRot);

		SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, currentTime));
	}
	else if (actualStatus == EStatus::UnderAttack) {

		SwitchingToWaitBeforeMoveFastWhenEnemiesCleared();
	}
	else if (actualStatus == EStatus::WaitBeforeMoveFast) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime > waitingTimeToMoveForwardAfterDefeatingEnemies) {
			SwitchingToMovingFast();
		}
	}
	else if (actualStatus == EStatus::WaitAfterHuggedByPlayer) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime > waitTimeAfterHuggedToMoveForward) {
			SwitchingToMovingFast();
		}
	}
	else if (actualStatus == EStatus::RotatingTowardsTarget) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		if (currentTime > globalSettings->FullPercent) {

			actualStatus = EStatus::MovingFast;
			currentTime = globalSettings->FullPercent;
		}

		FQuat newRot = FQuat::Slerp(startActorRotation.Quaternion(), targetActorRotation.Quaternion(), currentTime);
		FRotator rotator = newRot.Rotator();

		SetActorRotation(rotator);

	}
	else if (actualStatus == EStatus::MovingFast) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		currentTime *= fastMovementSpeed;
		if (currentTime > globalSettings->FullPercent) {

			actualStatus = EStatus::Stopped;
			currentTime = globalSettings->FullPercent;
		}

		SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, CurveFloat->GetFloatValue(currentTime)));
	}
}

/*
managing head rotation towards player, when its in the front area. Using dot product to decide if player can be looked at.
*/
void ACreature::HeadStateManagement() {
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	APawn* pawn = OurPlayerController->GetPawn();

	AGameCharacter* gameCharacter = Cast<AGameCharacter, APawn>(pawn);
	if (gameCharacter && prevHeadState == headState) {

		FVector forwardVector = GetActorForwardVector();
		forwardVector.Normalize();
		FVector gameCharDirection = gameCharacter->GetActorLocation() - GetActorLocation();
		gameCharDirection.Normalize();
		double degree = FVector::DotProduct(forwardVector, gameCharDirection);

		FRotator HeadRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), gameCharacter->GetActorLocation());

		if (degree > lookAtPlayerBorder) {

			if (headState == EHeadState::ForwardLooking) {
				startHeadRotation = UKismetMathLibrary::FindLookAtRotation(WhaleAudioComp->GetComponentLocation(), headMesh->GetComponentLocation());
				targetHeadRotation = HeadRot;

				prevHeadState = headState;
				headState = EHeadState::FollowingPlayer;
				headRotationStartTime = GetWorld()->GetTimeSeconds();
			}
		}
		else {
			if (headState == EHeadState::FollowingPlayer) {
				startHeadRotation = HeadRot;
				targetHeadRotation = UKismetMathLibrary::FindLookAtRotation(WhaleAudioComp->GetComponentLocation(), headMesh->GetComponentLocation());

				prevHeadState = headState;
				headState = EHeadState::ForwardLooking;
				headRotationStartTime = GetWorld()->GetTimeSeconds();
			}
		}
	}

	if (prevHeadState == headState) {
		if (headState == EHeadState::FollowingPlayer) {
			FRotator HeadRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), gameCharacter->GetActorLocation());

			headMesh->SetWorldRotation(HeadRot);
			LeftEye->SetWorldRotation(HeadRot);
			RightEye->SetWorldRotation(HeadRot);
		}
	}
	else {

		if (headState == EHeadState::FollowingPlayer) {
			targetHeadRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), gameCharacter->GetActorLocation());
		}

		float currentTime = GetWorld()->GetTimeSeconds() - headRotationStartTime;

		FQuat newRot = FQuat::Slerp(startHeadRotation.Quaternion(), targetHeadRotation.Quaternion(), currentTime);
		FRotator rotator = newRot.Rotator();

		headMesh->SetWorldRotation(rotator);
		LeftEye->SetWorldRotation(rotator);
		RightEye->SetWorldRotation(rotator);

		if (currentTime >= globalSettings->FullPercent) {
			prevHeadState = headState;
		}
	}
}
/*
creature health management. Decrease depends on the currently attacking enemy count. Restarting level, when creature died.
*/
void ACreature::HealthManagement(const float DeltaTime) {

	if (actualStatus == EStatus::Healing && healthComponent->GetHealth() == healthComponent->GetMaxHealth() && enemiesActuallyAttacking.Num() > 0) {
		actualStatus = EStatus::UnderAttack;
	}

	if (healthComponent->GetHealth() <= 0) {
		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

/*
detecting enemy attacking (detecting the spline created between enemy and creature)
we detect the attack in the whole creature actor and setting enemy count and states based on this.
*/

void ACreature::EnterEvent(class AActor* overlappedActor, class AActor* otherActor) {
	UE_LOG(LogTemp, Warning, TEXT("enemy attacking1"));
	if (otherActor && otherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("enemy attacking2"));
		if (otherActor->IsA(AEnemy::StaticClass())) {

			AEnemy* attackingEnemy = Cast<AEnemy>(otherActor);
			if (!enemiesActuallyAttacking.Contains(attackingEnemy))
				enemiesActuallyAttacking.Add(attackingEnemy);

			if (attackingEnemy->IsA(AHealerEnemy::StaticClass())) {
				attackingHealer = Cast<AHealerEnemy>(attackingEnemy);
			}

			actualStatus = EStatus::UnderAttack;
		}
		else if (otherActor->IsA(AEnemyTriggerBox::StaticClass())) {
			currentEnemyTriggerBox = Cast<AEnemyTriggerBox>(otherActor);
		}
	}
}

void ACreature::ExitEvent(class AActor* overlappedActor, class AActor* otherActor) {
	UE_LOG(LogTemp, Warning, TEXT("enemy leaving1"));
	if (otherActor && otherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("enemy leaving2"));
		if (otherActor->IsA(AEnemy::StaticClass()) && !otherActor->IsA(ASniperEnemy::StaticClass())) {

			AEnemy* attackingEnemy = Cast<AEnemy>(otherActor);
			if (enemiesActuallyAttacking.Contains(attackingEnemy))
				enemiesActuallyAttacking.Remove(attackingEnemy);

			SwitchingToWaitBeforeMoveFastWhenEnemiesCleared();

			if (attackingEnemy->IsA(AHealerEnemy::StaticClass())) {
				attackingHealer = nullptr;
			}
		}
	}
}

void ACreature::SwitchingToWaitBeforeMoveFastWhenEnemiesCleared() {
	if (actualStatus == EStatus::UnderAttack && enemiesActuallyAttacking.Num() == 0 &&
		currentEnemyTriggerBox && currentEnemyTriggerBox->AreAllEnemiesDefeated()) {

		WhaleAudioComp->Play(0.f);
		startTime = GetWorld()->GetTimeSeconds();
		actualStatus = EStatus::WaitBeforeMoveFast;
	}
}

/*
when player is inside the UBoxComponent or leaves that, then we set the boolean status. It is used for making player to hug the creature.
hugging is good for staying alive, when creature does fast traveling, and that regenerates enery of player.
*/

void ACreature::TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	UE_LOG(LogTemp, Warning, TEXT("getting inside fur1"));
	if (OtherActor && OtherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("getting inside fur2"));
		if (OtherActor->IsA(AEnemy::StaticClass())) {
			UE_LOG(LogTemp, Warning, TEXT("getting inside fur3"));
		}
		else if (OtherActor->IsA(AGameCharacter::StaticClass())) {
			UE_LOG(LogTemp, Warning, TEXT("getting inside fur4"));
			bIsCharInFur = true;
		}
	}
}

void ACreature::TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {
	UE_LOG(LogTemp, Warning, TEXT("away from fur1"));
	if (OtherActor && OtherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("away from fur2"));
		if (OtherActor->IsA(AEnemy::StaticClass())) {
			UE_LOG(LogTemp, Warning, TEXT("away from fur3"));
		}
		else if (OtherActor->IsA(AGameCharacter::StaticClass())) {
			UE_LOG(LogTemp, Warning, TEXT("away from fur4"));
			bIsCharInFur = false;
		}
	}
}

void ACreature::HealingStarted() {

	healthComponent->HealingStarted();

	actualStatus = EStatus::Healing;
}

/*
these methods are for setting values for state changes
*/

void ACreature::GetHugged() {

	if (actualStatus == EStatus::WaitBeforeMoveFast) {
		startTime = GetWorld()->GetTimeSeconds();
		actualStatus = EStatus::WaitAfterHuggedByPlayer;
	}
}

void ACreature::SwitchingToMovingFast() {
	startTime = GetWorld()->GetTimeSeconds();
	StepTargetIndex();
	actualStatus = EStatus::RotatingTowardsTarget;

	startActorRotation = UKismetMathLibrary::FindLookAtRotation(WhaleAudioComp->GetComponentLocation(), headMesh->GetComponentLocation());
	targetActorRotation = UKismetMathLibrary::FindLookAtRotation(WhaleAudioComp->GetComponentLocation(), actualEndPosition);
}

/*
isCharInFur state can be asked from creature
*/

bool ACreature::IsCharacterInFur() const {
	return bIsCharInFur;
}

EStatus ACreature::GetStatus() const {
	return actualStatus;
}

float ACreature::GetMaxHealth() {
	return healthComponent->GetMaxHealth();
}

float ACreature::GetActualHealthWhenStartedHealing() {
	return healthComponent->GetActualHealthWhenStartedHealing();
}