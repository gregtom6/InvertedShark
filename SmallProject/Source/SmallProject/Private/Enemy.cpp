// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AEnemy::AEnemy()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	splineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(splineComponent);

	actualStatus = EnemyStatus::Initial;
}

void AEnemy::OnConstruction(const FTransform& Transform) {

}

void AEnemy::SetSpline() {

	if (prevSplineMeshComp != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("spline torles"));
		prevSplineMeshComp->DestroyComponent();
	}

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
	startTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	actualStartPosition = GetActorLocation();
	actualStatus = EnemyStatus::Moving;
}

void AEnemy::StartEating() {
	startTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	actualStartPosition = GetActorLocation();
	actualStatus = EnemyStatus::Eating;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	actualLife = maxLife;
	
	OnActorBeginOverlap.AddDynamic(this, &AEnemy::EnterEvent);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (actualStatus == EnemyStatus::Moving && creature != nullptr) {
		currentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - startTime;

		currentTime *= movementSpeed;

		actualEndPosition = GetEndPosition();

		if (currentTime > 1.f)
			currentTime = 1.f;

			SetActorLocation(FMath::Lerp(actualStartPosition, actualEndPosition, currentTime));
	}

	else if (actualStatus == EnemyStatus::Eating && creature != nullptr) {
		
		currentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - startTime;

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

				startTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
			}

			
		}
		
	}

	if (overlappingGameCharacter != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("jatekos van"));
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("jatekos nincs"));
	}
	/*
	if (overlappingGameCharacter != nullptr && overlappingGameCharacter->GetStatus() == GameCharacterStatus::Attack) {
		DecreaseLife();
	}
	*/
}

void AEnemy::EnterEvent(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {
		if (otherActor->IsA(AGameCharacter::StaticClass())) {

			overlappingGameCharacter = Cast<AGameCharacter>(otherActor);
			
			if (overlappingGameCharacter->GetStatus() == GameCharacterStatus::Attack) {
				DecreaseLife();
			}
		}
	}
}

void AEnemy::ExitEvent(class AActor* overlappedActor, class AActor* otherActor) {
	if (otherActor && otherActor != this) {
		if (otherActor->IsA(AGameCharacter::StaticClass())) {

			overlappingGameCharacter = Cast<AGameCharacter>(otherActor);

			if (overlappingGameCharacter->GetStatus() == GameCharacterStatus::Attack) {
				DecreaseLife();
			}


			overlappingGameCharacter = nullptr;
		}
	}
}

FVector AEnemy::GetEndPosition() {
	return creature->GetActorLocation();
}

void AEnemy::DecreaseLife() {
	actualLife -= lifeDecreaseAfterAttack;

	if (actualLife <= 0) {
		Destroy();
	}
}