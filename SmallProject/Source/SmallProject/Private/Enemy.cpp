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



}

void AEnemy::OnConstruction(const FTransform& Transform) {



	SetSpline();
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

		prevSplineMeshComp = splineMeshComponent;
	}
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (creature != nullptr) {
		TArray<FVector> points;

		points.Add(GetActorLocation());
		points.Add(creature->GetActorLocation());

		splineComponent->SetSplinePoints(points, ESplineCoordinateSpace::World, true);

		UE_LOG(LogTemp, Warning, TEXT("spline feltoltve"));

		SetSpline();

		startTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	currentTime = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - startTime;

	if (creature != nullptr && currentTime > 0.08f) {
		TArray<FVector> points;

		points.Add(GetActorLocation());
		points.Add(creature->GetActorLocation());

		splineComponent->SetSplinePoints(points, ESplineCoordinateSpace::World, true);

		UE_LOG(LogTemp, Warning, TEXT("spline feltoltve"));

		SetSpline();

		startTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	}
}

