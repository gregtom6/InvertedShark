// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Kismet/GameplayStatics.h>
#include "Components/AudioComponent.h"
#include "Math/UnrealMathUtility.h"


// Sets default values
AEnemy::AEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(splineComponent);

	PopAudioComp=CreateDefaultSubobject<UAudioComponent>(TEXT("PopAudioComp"));
	PopAudioComp->SetupAttachment(RootComponent);

	SlurpAudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("SlurpAudioComp"));
	SlurpAudioComp->SetupAttachment(RootComponent);

	actualStatus = EnemyStatus::Initial;
}

void AEnemy::OnConstruction(const FTransform& Transform) {

}

void AEnemy::SetSpline() {

	DestroySpline();

	for (int SplineCount = 0; SplineCount < (splineComponent->GetNumberOfSplinePoints() - 1); SplineCount++) {
		USplineMeshComponent* splineMeshComponent = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
		splineMeshComponent->SetStaticMesh(Mesh);
		splineMeshComponent->Mobility = EComponentMobility::Movable;
		splineMeshComponent->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		splineMeshComponent->RegisterComponentWithWorld(GetWorld());
		splineMeshComponent->AttachToComponent(splineComponent, FAttachmentTransformRules::KeepRelativeTransform);

		const FVector startPoint = splineComponent->GetLocationAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
		const FVector startTangent = splineComponent->GetTangentAtSplinePoint(SplineCount, ESplineCoordinateSpace::Local);
		const FVector endPoint = splineComponent->GetLocationAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);
		const FVector endTangent = splineComponent->GetTangentAtSplinePoint(SplineCount + 1, ESplineCoordinateSpace::Local);

		splineMeshComponent->SetStartAndEnd(startPoint, startTangent, endPoint, endTangent, true);

		splineMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

		splineMeshComponent->SetForwardAxis(forwardAxis);

		splineMeshComponent->SetGenerateOverlapEvents(true);

		prevSplineMeshComp = splineMeshComponent;
	}
}

void AEnemy::MoveToCreature() {
	startTime = GetWorld()->GetTimeSeconds();
	actualStartPosition = GetActorLocation();
	actualStatus = EnemyStatus::Moving;
}

void AEnemy::StartEating() {
	startTime = GetWorld()->GetTimeSeconds();
	actualStartPosition = GetActorLocation();
	actualStatus = EnemyStatus::Eating;

	SlurpAudioComp->Play(FMath::FRandRange(0.f,3.f));
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PopAudioComp && popSound) {
		UE_LOG(LogTemp, Warning, TEXT("hang jo"));
		PopAudioComp->SetSound(popSound);
	}

	if (SlurpAudioComp && slurpSound) {
		UE_LOG(LogTemp, Warning, TEXT("slurp beallitas"));
		SlurpAudioComp->SetSound(slurpSound);
	}

	actualLife = maxLife;
	
	OnActorBeginOverlap.AddDynamic(this, &AEnemy::EnterEvent);
}

// Called every frame
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

	else if (actualStatus == EnemyStatus::Eating && creature != nullptr) {
		
		currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime > 0.08f) {

			FTransform transform=splineComponent->GetTransformAtSplinePoint(1, ESplineCoordinateSpace::World);
			if (transform.GetLocation() != lastCurveEndPosition) {
				TArray<FVector> points;

				points.Add(GetActorLocation());
				points.Add(creature->GetActorLocation());

				splineComponent->SetSplinePoints(points, ESplineCoordinateSpace::World, true);

				UE_LOG(LogTemp, Warning, TEXT("spline feltoltve"));

				SetSpline();

				lastCurveEndPosition = creature->GetActorLocation();

				startTime = GetWorld()->GetTimeSeconds();
			}

			
		}
		
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
		UE_LOG(LogTemp, Warning, TEXT("jatekos van"));

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
		UE_LOG(LogTemp, Warning, TEXT("jatekos nincs"));
	}
}

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
	if (prevSplineMeshComp != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("spline torles"));
		prevSplineMeshComp->DestroyComponent();
	}
}