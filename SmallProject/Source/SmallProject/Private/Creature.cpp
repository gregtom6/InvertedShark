// Fill out your copyright notice in the Description page of Project Settings.


#include "Creature.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "CreatureUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Enemy.h"
#include "HealerEnemy.h"
#include "GameCharacter.h"
#include <Sound/SoundCue.h >
#include <Kismet/KismetMathLibrary.h>

ACreature::ACreature(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	WhaleAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("WhaleAudioComp"));
	WhaleAudioComp->SetupAttachment(CreatureMesh);

	huggableComp = CreateDefaultSubobject<UBoxComponent>(TEXT("huggableComp"));
	huggableComp->SetupAttachment(WhaleAudioComp);

	headMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("headMesh"));
	headMesh->SetupAttachment(WhaleAudioComp);
	LeftEye = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftEye"));
	LeftEye->SetupAttachment(WhaleAudioComp);
	RightEye = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightEye"));
	RightEye->SetupAttachment(WhaleAudioComp);


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

	Health = MaxHealth;

	if (WhaleAudioComp && whaleSound) {
		UE_LOG(LogTemp, Warning, TEXT("whale sound setted up"));
		WhaleAudioComp->SetSound(whaleSound);
		WhaleAudioComp->Play(0.f);
	}


	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("creature1"));
		creatureuserwidget = Cast<UCreatureUserWidget>(CreateWidget(GetWorld(), widgetclass));

		if (creatureuserwidget != nullptr) {

			creatureuserwidget->creature = this;

			UE_LOG(LogTemp, Warning, TEXT("creature2"));
			creatureuserwidget->AddToViewport(0);
		}
	}

	StepTargetIndex();

	startTime = GetWorld()->GetTimeSeconds();

	actualStatus = Status::Initial;

	prevHeadState = HeadState::ForwardLooking;
	headState = HeadState::ForwardLooking;

	FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), positionsToMove[0]->GetActorLocation());

	SetActorRotation(PlayerRot);

	OnActorBeginOverlap.AddDynamic(this, &ACreature::EnterEvent);
	OnActorEndOverlap.AddDynamic(this, &ACreature::ExitEvent);

	huggableComp->OnComponentBeginOverlap.AddUniqueDynamic(this, &ACreature::TriggerEnter);
	huggableComp->OnComponentEndOverlap.AddUniqueDynamic(this, &ACreature::TriggerExit);
}

void ACreature::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

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

void ACreature::SetupProjectile(FRotator rotator, FVector scale, UStaticMesh* mesh, UMaterialInterface* material, FVector offset) {

	UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(this);
	NewComponent->RegisterComponent();
	NewComponent->SetStaticMesh(mesh);
	NewComponent->AttachToComponent(WhaleAudioComp, FAttachmentTransformRules::KeepRelativeTransform);
	NewComponent->SetRelativeScale3D(scale);

	NewComponent->SetWorldLocation(offset);

	NewComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NewComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	NewComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	NewComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	NewComponent->SetRelativeRotation(rotator);
	NewComponent->SetMaterial(0, material);
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
	else if (actualStatus == Status::RotatingTowardsTarget) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		if (currentTime > 1.f) {

			actualStatus = Status::MovingFast;
			currentTime = 1.f;
		}

		FQuat newRot = FQuat::Slerp(startActorRotation.Quaternion(), targetActorRotation.Quaternion(), currentTime);
		FRotator rotator = newRot.Rotator();

		SetActorRotation(rotator);

	}
	else if (actualStatus == Status::MovingFast) {
		float currentTime = GetWorld()->GetTimeSeconds() - startTime;
		currentTime *= fastMovementSpeed;
		if (currentTime > 1.f) {

			actualStatus = Status::Stopped;
			currentTime = 1.f;
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
	if (gameCharacter != nullptr && prevHeadState == headState) {

		FVector forwardVector = GetActorForwardVector();
		forwardVector.Normalize();
		FVector gameCharDirection = gameCharacter->GetActorLocation() - GetActorLocation();
		gameCharDirection.Normalize();
		double degree = FVector::DotProduct(forwardVector, gameCharDirection);

		FRotator HeadRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), gameCharacter->GetActorLocation());

		if (degree > lookAtPlayerBorder) {

			if (headState == HeadState::ForwardLooking) {
				startHeadRotation = UKismetMathLibrary::FindLookAtRotation(WhaleAudioComp->GetComponentLocation(), headMesh->GetComponentLocation());
				targetHeadRotation = HeadRot;

				prevHeadState = headState;
				headState = HeadState::FollowingPlayer;
				headRotationStartTime = GetWorld()->GetTimeSeconds();
			}
		}
		else {
			if (headState == HeadState::FollowingPlayer) {
				startHeadRotation = HeadRot;
				targetHeadRotation = UKismetMathLibrary::FindLookAtRotation(WhaleAudioComp->GetComponentLocation(), headMesh->GetComponentLocation());

				prevHeadState = headState;
				headState = HeadState::ForwardLooking;
				headRotationStartTime = GetWorld()->GetTimeSeconds();
			}
		}
	}

	if (prevHeadState == headState) {
		if (headState == HeadState::FollowingPlayer) {
			FRotator HeadRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), gameCharacter->GetActorLocation());

			headMesh->SetWorldRotation(HeadRot);
			LeftEye->SetWorldRotation(HeadRot);
			RightEye->SetWorldRotation(HeadRot);
		}
	}
	else {

		if (headState == HeadState::FollowingPlayer) {
			targetHeadRotation = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), gameCharacter->GetActorLocation());
		}

		float currentTime = GetWorld()->GetTimeSeconds() - headRotationStartTime;

		FQuat newRot = FQuat::Slerp(startHeadRotation.Quaternion(), targetHeadRotation.Quaternion(), currentTime);
		FRotator rotator = newRot.Rotator();

		headMesh->SetWorldRotation(rotator);
		LeftEye->SetWorldRotation(rotator);
		RightEye->SetWorldRotation(rotator);

		if (currentTime >= 1.f) {
			prevHeadState = headState;
		}
	}
}
/*
creature health management. Decrease depends on the currently attacking enemy count. Restarting level, when creature died.
*/
void ACreature::HealthManagement(float DeltaTime) {

	if (actualStatus == Status::UnderAttack)
		Health = Health > 0 ? Health - (deltaDamage * DeltaTime * enemiesActuallyAttacking.Num()) : 0;
	else if (actualStatus == Status::Healing && attackingHealer != nullptr) {

		deltaHeal = MaxHealth * attackingHealer->GetPercentageOfMaxLifeToHealBack();

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= attackingHealer->GetTimeForHeal();

		Health = Health < MaxHealth ? actualHealthWhenStartedHealing + (deltaHeal * currentTime) : MaxHealth;
	}

	if (actualStatus == Status::Healing && Health == MaxHealth && enemiesActuallyAttacking.Num() > 0) {
		actualStatus = Status::UnderAttack;
	}

	if (Health <= 0) {
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

			actualStatus = Status::UnderAttack;
		}
	}
}

void ACreature::ExitEvent(class AActor* overlappedActor, class AActor* otherActor) {
	UE_LOG(LogTemp, Warning, TEXT("enemy leaving1"));
	if (otherActor && otherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("enemy leaving2"));
		if (otherActor->IsA(AEnemy::StaticClass())) {

			AEnemy* attackingEnemy = Cast<AEnemy>(otherActor);
			if (enemiesActuallyAttacking.Contains(attackingEnemy))
				enemiesActuallyAttacking.Remove(attackingEnemy);

			if (actualStatus == Status::UnderAttack && enemiesActuallyAttacking.Num() == 0) {
				WhaleAudioComp->Play(0.f);
				startTime = GetWorld()->GetTimeSeconds();
				actualStatus = Status::WaitBeforeMoveFast;
			}

			if (attackingEnemy->IsA(AHealerEnemy::StaticClass())) {
				attackingHealer = nullptr;
			}
		}
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
			isCharInFur = true;
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
			isCharInFur = false;
		}
	}
}

void ACreature::HealingStarted() {
	actualStatus = Status::Healing;
	startTime = GetWorld()->GetTimeSeconds();
	actualHealthWhenStartedHealing = Health;
}

/*
these methods are for setting values for state changes
*/

void ACreature::GetHugged() {

	if (actualStatus == Status::WaitBeforeMoveFast) {
		startTime = GetWorld()->GetTimeSeconds();
		actualStatus = Status::WaitAfterHuggedByPlayer;
	}
}

void ACreature::SwitchingToMovingFast() {
	startTime = GetWorld()->GetTimeSeconds();
	StepTargetIndex();
	actualStatus = Status::RotatingTowardsTarget;

	startActorRotation = UKismetMathLibrary::FindLookAtRotation(WhaleAudioComp->GetComponentLocation(), headMesh->GetComponentLocation());
	targetActorRotation = UKismetMathLibrary::FindLookAtRotation(WhaleAudioComp->GetComponentLocation(), actualEndPosition);
}

/*
isCharInFur state can be asked from creature
*/

bool ACreature::IsCharacterInFur() {
	return isCharInFur;
}

Status ACreature::GetStatus() {
	return actualStatus;
}