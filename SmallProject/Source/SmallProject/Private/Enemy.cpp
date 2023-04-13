// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SplineComponent.h"
#include "Creature.h"
#include "GameCharacter.h"
#include <Sound/SoundCue.h >

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(splineComponent);

	for (int32 i = 0; i < countOfInnerPointsInSpline + 1; i++)
	{
		USplineMeshComponent* SComp = CreateDefaultSubobject<USplineMeshComponent>(*FString::Printf(TEXT("SMeshComp%d"), i));
		SComp->SetupAttachment(RootComponent);
		SMeshComps.Add(SComp);
	}

	PopAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("PopAudioComp"));
	PopAudioComp->SetupAttachment(RootComponent);

	SlurpAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SlurpAudioComp"));
	SlurpAudioComp->SetupAttachment(RootComponent);

	SwallowSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwallowSphere"));
	SwallowSphere->SetupAttachment(RootComponent);

	actualStatus = EnemyStatus::Initial;
}

/*
this method creates spline between the enemy and the creature. Spline is to show the blood transfusion between the enemy and the fur creature.
*/

void AEnemy::SetSpline() {

	for (int i = 0; i < SMeshComps.Num(); i++) {
		FVector startPoint = splineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector startTangent = splineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector endPoint = splineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		FVector endTangent = splineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		SetSplineMeshComponent(SMeshComps[i], startPoint, startTangent, endPoint, endTangent);
	}
}

/*
this method creates spline between the enemy and the creature. Spline is to show the blood transfusion between the enemy and the fur creature.
*/
void AEnemy::SetSplineMeshComponent(class USplineMeshComponent* splineMeshComp, FVector startPoint, FVector startTangent, FVector endPoint, FVector endTangent) {
	splineMeshComp->SetStartAndEnd(startPoint, startTangent, endPoint, endTangent, true);

	splineMeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	splineMeshComp->SetForwardAxis(forwardAxis);

	splineMeshComp->SetGenerateOverlapEvents(true);
}

/*
setting up the Moving state
*/
void AEnemy::MoveToCreature() {
	startTime = GetWorld()->GetTimeSeconds();
	actualStartPosition = GetActorLocation();
	actualStatus = EnemyStatus::Moving;
}

/*
setting up the Eating state, starting slurp sound. Creating spline between fur creature and enemy to represent blood transfusion.
*/
void AEnemy::StartEating() {
	startTime = GetWorld()->GetTimeSeconds();
	actualStartPosition = GetActorLocation();
	actualStatus = EnemyStatus::Eating;

	SlurpAudioComp->Play(FMath::FRandRange(0.f, 3.f));

	FTransform transform = splineComponent->GetTransformAtSplinePoint(1, ESplineCoordinateSpace::World);
	if (transform.GetLocation() != lastCurveEndPosition) {
		TArray<FVector> points;

		points.Add(GetActorLocation());

		FVector direction = creature->GetActorLocation() - GetActorLocation();
		direction.Normalize();
		double dist = FVector::Distance(creature->GetActorLocation(), GetActorLocation());
		double oneUnit = dist / (float)countOfInnerPointsInSpline;
		for (int i = 0; i < countOfInnerPointsInSpline; i++) {
			FVector point = GetActorLocation() + direction * oneUnit * (i + 1);
			point.Z = point.Z + FMath::FRandRange(-innerSplinePointRangeBorderZ, innerSplinePointRangeBorderZ);
			point.X = point.X + FMath::FRandRange(-innerSplinePointRangeBorderXY, innerSplinePointRangeBorderXY);
			point.Y = point.Y + FMath::FRandRange(-innerSplinePointRangeBorderXY, innerSplinePointRangeBorderXY);
			points.Add(point);
		}

		points.Add(creature->GetActorLocation());

		splineComponent->SetSplinePoints(points, ESplineCoordinateSpace::World, true);

		UE_LOG(LogTemp, Warning, TEXT("spline uploaded"));

		SetSpline();

		lastCurveEndPosition = creature->GetActorLocation();

		startTime = GetWorld()->GetTimeSeconds();
	}
}

/*
initializing components and subscribing to events
*/
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PopAudioComp && popSound) {
		UE_LOG(LogTemp, Warning, TEXT("pop sound is okay"));
		PopAudioComp->SetSound(popSound);
	}

	if (SlurpAudioComp && slurpSound) {
		UE_LOG(LogTemp, Warning, TEXT("slurp sound is okay"));
		SlurpAudioComp->SetSound(slurpSound);
	}

	for (int i = 0; i < SMeshComps.Num(); i++) {
		SplineMeshCompAttach(SMeshComps[i]);
	}

	actualLife = maxLife;

	OnActorBeginOverlap.AddUniqueDynamic(this, &AEnemy::EnterEvent);
	OnActorEndOverlap.AddUniqueDynamic(this, &AEnemy::ExitEvent);
}

void AEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnActorBeginOverlap.RemoveDynamic(this, &AEnemy::EnterEvent);
	OnActorEndOverlap.RemoveDynamic(this, &AEnemy::ExitEvent);
}

/*
setting up received spline mesh component
*/
void AEnemy::SplineMeshCompAttach(class USplineMeshComponent* splineMeshComp) {
	splineMeshComp->Mobility = EComponentMobility::Movable;
	splineMeshComp->AttachToComponent(splineComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	StateManagement();

	LifeManagement();

	TimeManagement();
}
/*
moving state : when enemy is moving, it goes from actualStartPosition to actualEndPosition.actualStartPosition setted by the same way in Enemy.cpp and
BossEnemy.cpp, it will always be the enemy's actual position during state switching.
actualEndPosition will be different for enemyand bossenemy. For enemy, it will be the creature itself.For the bossenemy, it will be above the creature.

eating state: managing swallow sphere to create an illusion of eating

dying state : for enemies, it will mean only to scale down the actor. For the bossenemy, the functionality gets extended.
Removes widgets from viewport.
*/
void AEnemy::StateManagement() {
	if (actualStatus == EnemyStatus::Moving && creature != nullptr) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime *= movementSpeed;

		actualEndPosition = GetEndPosition();

		if (currentTime > 1.f)
			currentTime = 1.f;

		SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, currentTime));
	}

	else if (actualStatus == EnemyStatus::Eating) {

		currentTime = GetWorld()->GetTimeSeconds() - startTime;
		if (currentTime >= 1.f)
			startTime = GetWorld()->GetTimeSeconds();

		float SplineLength = splineComponent->GetSplineLength();
		float SplineDistance = SplineLength * (1.f - currentTime);
		FVector Position = splineComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);

		SwallowSphere->SetWorldLocation(Position);
	}

	else if (actualStatus == EnemyStatus::SpecialDying) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= dyingTime;

		if (currentTime >= 1.f)
			currentTime = 1.f;

		SetActorScale3D(FMath::Lerp(startScale, endScale, currentTime));

		if (currentTime >= 1.f) {
			DoAfterDead();
		}
	}
}

/*
manages life decrease when getting attacked by player, for both the enemy and the bossenemy.
*/
void AEnemy::LifeManagement() {
	if (overlappingGameCharacter != nullptr) {

		if (overlappingGameCharacter->GetStatus() == GameCharacterStatus::Attack && overlappingGameCharacter->GetPrevStatus() == GameCharacterStatus::Calm) {
			canPlayerDamageMe = true;
			overlappingGameCharacter->SetPrevStatusToActualStatus();
		}

		if (overlappingGameCharacter->GetStatus() == GameCharacterStatus::Attack && canPlayerDamageMe) {
			DecreaseLife();
			canPlayerDamageMe = false;
		}
	}
	else {
		canPlayerDamageMe = false;
	}
}

void AEnemy::TimeManagement() {
	if (gameCharacter == nullptr) { return; }

	PopAudioComp->SetPitchMultiplier(gameCharacter->GetCurrentSoundPitchMultiplier());
	SlurpAudioComp->SetPitchMultiplier(gameCharacter->GetCurrentSoundPitchMultiplier());
}

/*
events to detect player actor to receive the correct amount of damaging
*/

void AEnemy::EnterEvent(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {
		if (otherActor->IsA(AGameCharacter::StaticClass())) {

			overlappingGameCharacter = Cast<AGameCharacter>(otherActor);

			gameCharacter = Cast<AGameCharacter>(otherActor);

			canPlayerDamageMe = true;
		}
	}
}

void AEnemy::ExitEvent(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {
		if (otherActor->IsA(AGameCharacter::StaticClass())) {

			overlappingGameCharacter = nullptr;

			canPlayerDamageMe = false;
		}
	}
}

/*
GetEndPosition gets back different end position for enemy movement, when it's for normal enemy, and for bossenemy.
enemy will stop at the creature position, BossEnemy will stop above the creature.
*/

FVector AEnemy::GetEndPosition() {
	return creature->GetActorLocation();
}

/*
managing life decrease for both enemy and bossenemy.
*/
void AEnemy::DecreaseLife() {
	actualLife -= lifeDecreaseAfterAttack;

	if (actualLife <= 0) {
		RemoveEnemy();
	}

	canPlayerDamageMe = false;
}

/*
spline and swallow sphere gets destroyed, because enemy is not eating anymore (no more blood transfusion between creature and enemy)
stopping audios, saving values for lerps
*/
void AEnemy::RemoveEnemy() {

	if (SwallowSphere != nullptr)
		SwallowSphere->DestroyComponent();

	DestroySpline();
	startTime = GetWorld()->GetTimeSeconds();
	startScale = GetActorScale3D();
	endScale = FVector(0.f, 0.f, 0.f);
	actualStatus = EnemyStatus::SpecialDying;

	SlurpAudioComp->Stop();
	PopAudioComp->Play(0.f);

 	if (gameCharacter != nullptr)
		gameCharacter->EnemyDefeated();
}

/*
normal enemies just get destroyed, this is overriden in the bossenemy
*/
void AEnemy::DoAfterDead() {
	Destroy();
}

/*
destroying certain components upon death
*/
void AEnemy::DestroySpline() {

	for (int i = 0; i < SMeshComps.Num(); i++) {
		DestroySplineMeshComp(SMeshComps[i]);
	}
}

/*
destroying certain components upon death
*/
void AEnemy::DestroySplineMeshComp(class USplineMeshComponent* splineMeshComp) {
	if (splineMeshComp != nullptr)
		splineMeshComp->DestroyComponent();
}