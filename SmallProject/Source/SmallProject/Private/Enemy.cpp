// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Math/UnrealMathUtility.h"
#include "Components/SplineComponent.h"
#include "Creature.h"
#include "GameCharacter.h"
#include <Sound/SoundCue.h >
#include "NiagaraComponent.h"
#include "ResourceDataAsset.h"

AEnemy::AEnemy() {}

AEnemy::AEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(splineComponent);

	for (int32 i = 0; i < countOfInnerPointsInSpline + 1; i++)
	{
		USplineMeshComponent* SComp = CreateDefaultSubobject<USplineMeshComponent>(*FString::Printf(TEXT("SMeshComp%d"), i));
		UStaticMeshComponent* SCompContainer = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("SMeshCompContainer%d"), i));
		UNiagaraComponent* NiagaraLeft = CreateDefaultSubobject<UNiagaraComponent>(*FString::Printf(TEXT("NiagaraLeft%d"), i));
		UNiagaraComponent* NiagaraRight = CreateDefaultSubobject<UNiagaraComponent>(*FString::Printf(TEXT("NiagaraRight%d"), i));

		SplineNiagaras.Add(NiagaraLeft);
		SplineNiagaras.Add(NiagaraRight);
		SMeshContainers.Add(SCompContainer);
		SMeshComps.Add(SComp);
		SMeshContainers[i]->SetupAttachment(RootComponent);
		SMeshContainers[i]->SetSimulatePhysics(false);
	}

	PopAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("PopAudioComp"));
	PopAudioComp->SetupAttachment(RootComponent);

	SlurpAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SlurpAudioComp"));
	SlurpAudioComp->SetupAttachment(RootComponent);

	SwallowSphere = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SwallowSphere"));
	SwallowSphere->SetupAttachment(RootComponent);

	actualStatus = EEnemyStatus::Initial;

	Body12 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Body12"));
	Body12->SetupAttachment(RootComponent);

	EyePivot2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EyePivot2"));

	LeftEyeWhite2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftEyeWhite2"));

	LeftEyeBlack2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftEyeBlack2"));

	LeftEyeLid12 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftEyeLid12"));

	LeftEyeLid22 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LeftEyeLid22"));

	RightEyeWhite2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightEyeWhite2"));

	RightEyeBlack2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightEyeBlack2"));

	RightEyeLid12 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightEyeLid12"));

	RightEyeLid22 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RightEyeLid22"));

	EyePivot2->AttachToComponent(Body12, FAttachmentTransformRules::KeepRelativeTransform);
	LeftEyeWhite2->AttachToComponent(EyePivot2, FAttachmentTransformRules::KeepRelativeTransform);
	LeftEyeBlack2->AttachToComponent(LeftEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	LeftEyeLid12->AttachToComponent(LeftEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	LeftEyeLid22->AttachToComponent(LeftEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	RightEyeWhite2->AttachToComponent(EyePivot2, FAttachmentTransformRules::KeepRelativeTransform);
	RightEyeBlack2->AttachToComponent(RightEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	RightEyeLid22->AttachToComponent(RightEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	RightEyeLid12->AttachToComponent(RightEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);

	globalSettings = NewObject<UResourceDataAsset>(GetTransientPackage(), FName("globalSettings"));
}

/*
this method creates spline between the enemy and the creature. Spline is to show the blood transfusion between the enemy and the fur creature.
*/

void AEnemy::SetSpline() {

	FVector startPoint;
	FVector endPoint;

	for (int i = 0; i < SMeshComps.Num(); i++) {
		startPoint = splineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector startTangent = splineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		endPoint = splineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		FVector endTangent = splineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
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
void AEnemy::MoveToCreature(float timeToStart) {
	timeBeforeActualMoving = timeToStart;
	startTime = GetWorld()->GetTimeSeconds();
	actualStartPosition = GetActorLocation();
	actualStatus = EEnemyStatus::WaitBeforeMoving;
}

/*
setting up the Eating state, starting slurp sound. Creating spline between fur creature and enemy to represent blood transfusion.
*/
void AEnemy::StartEating() {

	if (actualStatus == EEnemyStatus::StartEating || actualStatus == EEnemyStatus::Eating) { return; }

	startTime = GetWorld()->GetTimeSeconds();
	actualStartPosition = GetActorLocation();
	actualStatus = EEnemyStatus::StartEating;

	FTransform transform = splineComponent->GetTransformAtSplinePoint(1, ESplineCoordinateSpace::World);
	if (transform.GetLocation() != lastCurveEndPosition) {
		TArray<FVector> points;

		points.Add(GetActorLocation());

		FVector direction = creature->GetActorLocation() - GetActorLocation();
		direction.Normalize();
		float dist = FVector::Distance(creature->GetActorLocation(), GetActorLocation());
		float oneUnit = dist / (float)countOfInnerPointsInSpline;
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

		lastCurveEndPosition = creature->GetActorLocation();

		startTime = GetWorld()->GetTimeSeconds();
	}
}

void AEnemy::StartActualEating() {
	actualStatus = EEnemyStatus::Eating;

	SlurpAudioComp->Play(FMath::FRandRange(0.f, 3.f));
}

/*
initializing components and subscribing to events
*/
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < SMeshComps.Num(); i++) {
		SplineMeshCompAttach(SMeshComps[i]);
	}

	actualLife = maxLife;

	originalLifeBeforeAttack = actualLife;

	for (int i = 0; i < SMeshComps.Num(); i++) {
		SMeshComps[i]->AttachToComponent(SMeshContainers[i], FAttachmentTransformRules::KeepRelativeTransform);
	}

	EyePivot2->AttachToComponent(Body12, FAttachmentTransformRules::KeepRelativeTransform);
	LeftEyeWhite2->AttachToComponent(EyePivot2, FAttachmentTransformRules::KeepRelativeTransform);
	LeftEyeBlack2->AttachToComponent(LeftEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	LeftEyeLid12->AttachToComponent(LeftEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	LeftEyeLid22->AttachToComponent(LeftEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	RightEyeWhite2->AttachToComponent(EyePivot2, FAttachmentTransformRules::KeepRelativeTransform);
	RightEyeBlack2->AttachToComponent(RightEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	RightEyeLid22->AttachToComponent(RightEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);
	RightEyeLid12->AttachToComponent(RightEyeWhite2, FAttachmentTransformRules::KeepRelativeTransform);


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

	SplineManagement();
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

	if (actualStatus == EEnemyStatus::WaitBeforeMoving && creature != nullptr) {

		currentTime = GetWorld()->GetTimeSeconds() - startTime;
		if (currentTime >= timeBeforeActualMoving) {
			startTime = GetWorld()->GetTimeSeconds();
			actualStatus = EEnemyStatus::Moving;

			if (EyePivot2 != nullptr) {
				FVector Direction = creature->GetActorLocation() - EyePivot2->GetComponentLocation();
				Direction.Normalize();
				FRotator targetRotation = Direction.Rotation();
				targetRotation.Yaw -= 90.f;
				EyePivot2->SetRelativeRotation(targetRotation);
			}

		}

	}

	else if (actualStatus == EEnemyStatus::Moving && creature != nullptr) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime *= movementSpeed;

		actualEndPosition = GetEndPosition();

		if (currentTime > globalSettings->FullPercent) {
			currentTime = globalSettings->FullPercent;
			MovingToCreatureEnded();
		}

		SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, currentTime));
	}

	else if (actualStatus == EEnemyStatus::Eating) {

		currentTime = GetWorld()->GetTimeSeconds() - startTime;
		if (currentTime >= globalSettings->FullPercent)
			startTime = GetWorld()->GetTimeSeconds();

		float SplineLength = splineComponent->GetSplineLength();
		float SplineDistance = SplineLength * (globalSettings->FullPercent - currentTime);
		FVector Position = splineComponent->GetLocationAtDistanceAlongSpline(SplineDistance, ESplineCoordinateSpace::World);

		if (SwallowSphere != nullptr)
			SwallowSphere->SetWorldLocation(Position);
	}

	else if (actualStatus == EEnemyStatus::SpecialDying) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime /= dyingTime;

		if (currentTime >= globalSettings->FullPercent)
			currentTime = globalSettings->FullPercent;


		FVector newScale = FMath::Lerp(startScale, endScale, currentTime);
		GetCurrentBodyMesh()->SetWorldScale3D(newScale);


		for (int i = 0; i < SMeshContainers.Num(); i++) {
			SMeshContainers[i]->SetWorldScale3D(newScale);
		}


		if (currentTime >= globalSettings->FullPercent) {
			DoAfterDead();
		}
	}
}

void AEnemy::MovingToCreatureEnded() {
	//child classes overriding this
}

/*
manages life decrease when getting attacked by player, for both the enemy and the bossenemy.
*/
void AEnemy::LifeManagement() {
	if (overlappingGameCharacter != nullptr) {

		if (overlappingGameCharacter->GetStatus() == EGameCharacterStatus::Attack && overlappingGameCharacter->GetPrevStatus() == EGameCharacterStatus::Calm) {
			canPlayerDamageMe = true;
			overlappingGameCharacter->SetPrevStatusToActualStatus();
		}

		if (overlappingGameCharacter->GetStatus() == EGameCharacterStatus::Attack && canPlayerDamageMe) {
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

void AEnemy::SplineManagement() {

	if (actualStatus != EEnemyStatus::StartEating) { return; }

	currentTime = GetWorld()->GetTimeSeconds() - startTime;

	currentTime /= splineGrowTime;

	if (currentTime > globalSettings->FullPercent) {
		currentTime = globalSettings->FullPercent;

		for (int i = 0; i < SMeshComps.Num(); i++) {
			SMeshComps[i]->SetRelativeLocation(SMeshComps[i]->GetRelativeLocation() + FVector(0.01f, 0.f, 0.f));
		}

		StartActualEating();
	}

	FVector firstPointOfSpline = splineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);

	int j = 0;

	for (int i = 0; i < SMeshComps.Num(); i++) {
		FVector startPoint = splineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector startTangent = splineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::Local);
		FVector endPoint = splineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);
		FVector endTangent = splineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::Local);

		SplineNiagaras[j]->SetRelativeLocation(FMath::Lerp(firstPointOfSpline, startPoint, currentTime));
		SplineNiagaras[j + 1]->SetRelativeLocation(FMath::Lerp(firstPointOfSpline, endPoint, currentTime));


		j += 2;

		SetSplineMeshComponent(SMeshComps[i], FMath::Lerp(firstPointOfSpline, startPoint, currentTime), startTangent, FMath::Lerp(firstPointOfSpline, endPoint, currentTime), endTangent);
		SMeshComps[i]->UpdateMesh();
	}

	j = 0;
	for (int i = 0; i < SplineNiagaras.Num(); i++) {
		if (i != 0 && i % 2 == 0) {
			j += 1;
		}

		SplineNiagaras[i]->AttachToComponent(SMeshComps[j], FAttachmentTransformRules::KeepRelativeTransform);
	}
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

FVector AEnemy::GetEndPosition() const {
	return creature->GetActorLocation();
}

/*
managing life decrease for both enemy and bossenemy.
*/
void AEnemy::DecreaseLife() {

	if (actualStatus == EEnemyStatus::SpecialDying) { return; }

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

	//TODO: body1 es mas komponensek

	startTime = GetWorld()->GetTimeSeconds();
	startScale = GetCurrentBodyMesh()->GetComponentScale();
	endScale = FVector(0.001f, 0.001f, 0.001f);
	actualStatus = EEnemyStatus::SpecialDying;

	GetCurrentBodyMesh()->SetSimulatePhysics(true);


	for (int i = 0; i < SMeshContainers.Num(); i++) {
		SMeshContainers[i]->SetSimulatePhysics(true);
	}


	if (EyePivot2 != nullptr)
		EyePivot2->SetSimulatePhysics(true);


	for (int i = 0; i < SplineNiagaras.Num(); i++) {
		SplineNiagaras[i]->Activate();
	}


	SlurpAudioComp->Stop();
	PopAudioComp->Play(0.f);

	if (gameCharacter != nullptr)
		gameCharacter->SlowdownTime();
}

UStaticMeshComponent* AEnemy::GetCurrentBodyMesh() const {
	return Body12;
}

/*
normal enemies just get destroyed, this is overriden in the bossenemy
*/
void AEnemy::DoAfterDead() {
	Destroy();
}

float AEnemy::GetOriginalLifeBeforeAttack() const {
	return originalLifeBeforeAttack;
}

void AEnemy::OriginalLifeRepresentationEnded() {
	originalLifeBeforeAttack = actualLife;
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