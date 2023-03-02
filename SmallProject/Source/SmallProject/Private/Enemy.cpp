// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Math/UnrealMathUtility.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(splineComponent);

	SMeshComp0 = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SMeshComp0"));
	SMeshComp0->SetupAttachment(RootComponent);

	SMeshComp1 = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SMeshComp1"));
	SMeshComp1->SetupAttachment(RootComponent);

	SMeshComp2 = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SMeshComp2"));
	SMeshComp2->SetupAttachment(RootComponent);

	SMeshComp3 = CreateDefaultSubobject<USplineMeshComponent>(TEXT("SMeshComp3"));
	SMeshComp3->SetupAttachment(RootComponent);

	PopAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("PopAudioComp"));
	PopAudioComp->SetupAttachment(RootComponent);

	SlurpAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SlurpAudioComp"));
	SlurpAudioComp->SetupAttachment(RootComponent);

	actualStatus = EnemyStatus::Initial;
}

/*
this method creates spline between the enemy and the creature. Spline is to show the blood transfusion between the enemy and the fur creature.
*/

void AEnemy::SetSpline() {

	FVector startPoint = splineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::Local);
	FVector startTangent = splineComponent->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::Local);
	FVector endPoint = splineComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);
	FVector endTangent = splineComponent->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::Local);

	SetSplineMeshComponent(SMeshComp0, startPoint, startTangent, endPoint, endTangent);

	startPoint = splineComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::Local);
	startTangent = splineComponent->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::Local);
	endPoint = splineComponent->GetLocationAtSplinePoint(2, ESplineCoordinateSpace::Local);
	endTangent = splineComponent->GetTangentAtSplinePoint(2, ESplineCoordinateSpace::Local);

	SetSplineMeshComponent(SMeshComp1, startPoint, startTangent, endPoint, endTangent);

	startPoint = splineComponent->GetLocationAtSplinePoint(2, ESplineCoordinateSpace::Local);
	startTangent = splineComponent->GetTangentAtSplinePoint(2, ESplineCoordinateSpace::Local);
	endPoint = splineComponent->GetLocationAtSplinePoint(3, ESplineCoordinateSpace::Local);
	endTangent = splineComponent->GetTangentAtSplinePoint(3, ESplineCoordinateSpace::Local);

	SetSplineMeshComponent(SMeshComp2, startPoint, startTangent, endPoint, endTangent);

	startPoint = splineComponent->GetLocationAtSplinePoint(3, ESplineCoordinateSpace::Local);
	startTangent = splineComponent->GetTangentAtSplinePoint(3, ESplineCoordinateSpace::Local);
	endPoint = splineComponent->GetLocationAtSplinePoint(4, ESplineCoordinateSpace::Local);
	endTangent = splineComponent->GetTangentAtSplinePoint(4, ESplineCoordinateSpace::Local);

	SetSplineMeshComponent(SMeshComp3, startPoint, startTangent, endPoint, endTangent);
}

void AEnemy::SetSplineMeshComponent(USplineMeshComponent* splineMeshComp, FVector startPoint, FVector startTangent, FVector endPoint, FVector endTangent) {
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
		double oneUnit = dist / 3.f;
		for (int i = 0; i < 3; i++) {
			FVector point = GetActorLocation() + direction * oneUnit * (i + 1);
			point.Z = point.Z + FMath::FRandRange(-100.f, 100.f);
			point.X = point.X + FMath::FRandRange(-50.f, 50.f);
			point.Y = point.Y + FMath::FRandRange(-50.f, 50.f);
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

// Called when the game starts or when spawned
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

	SplineMeshCompAttach(SMeshComp0);
	SplineMeshCompAttach(SMeshComp1);
	SplineMeshCompAttach(SMeshComp2);
	SplineMeshCompAttach(SMeshComp3);

	actualLife = maxLife;

	OnActorBeginOverlap.AddDynamic(this, &AEnemy::EnterEvent);
	OnActorEndOverlap.AddDynamic(this, &AEnemy::ExitEvent);
}

void AEnemy::SplineMeshCompAttach(USplineMeshComponent* splineMeshComp) {
	splineMeshComp->Mobility = EComponentMobility::Movable;
	splineMeshComp->AttachToComponent(splineComponent, FAttachmentTransformRules::KeepRelativeTransform);
}

/*
moving state: when enemy is moving, it goes from actualStartPosition to actualEndPosition. actualStartPosition setted by the same way in Enemy.cpp and
BossEnemy.cpp, it will always be the enemy's actual position during state switching.
actualEndPosition will be different for enemy and bossenemy. For enemy, it will be the creature itself. For the bossenemy, it will be above the creature.

dying state: for enemies, it will mean only to scale down the actor. For the bossenemy, the functionality gets extended, the DoAfterDead will load a new level
and removes widgets from viewport.

Tick method also manages life decrease when getting attacked by player, for both the enemy and the bossenemy.
*/
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (actualStatus == EnemyStatus::Moving && creature != nullptr) {
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		currentTime *= movementSpeed;

		actualEndPosition = GetEndPosition();

		if (currentTime > 1.f)
			currentTime = 1.f;

		SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, currentTime));
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

/*
events to detect player actor to receive the correct amount of damaging
*/

void AEnemy::EnterEvent(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {
		if (otherActor->IsA(AGameCharacter::StaticClass())) {

			overlappingGameCharacter = Cast<AGameCharacter>(otherActor);

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

void AEnemy::DecreaseLife() {
	actualLife -= lifeDecreaseAfterAttack;

	if (actualLife <= 0) {
		RemoveEnemy();
	}

	canPlayerDamageMe = false;
}

/*
spline gets destroyed, because enemy is not eating anymore (no more blood transfusion between creature and enemy)
stopping audios, saving values for lerps
*/

void AEnemy::RemoveEnemy() {

	DestroySpline();
	startTime = GetWorld()->GetTimeSeconds();
	startScale = GetActorScale3D();
	endScale = FVector(0.f, 0.f, 0.f);
	actualStatus = EnemyStatus::SpecialDying;

	SlurpAudioComp->Stop();
	PopAudioComp->Play(0.f);
}

void AEnemy::DoAfterDead() {
	Destroy();
}

void AEnemy::DestroySpline() {
	DestroySplineMeshComp(SMeshComp0);
	DestroySplineMeshComp(SMeshComp1);
	DestroySplineMeshComp(SMeshComp2);
	DestroySplineMeshComp(SMeshComp3);
}

void AEnemy::DestroySplineMeshComp(USplineMeshComponent* splineMeshComp) {
	if (splineMeshComp != nullptr)
		splineMeshComp->DestroyComponent();
}