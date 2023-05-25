// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BossEnemy.h"
#include "Widgets/BossUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Allies/Creature.h"
#include "Components/SplineComponent.h"

ABossEnemy::ABossEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bodyMesh"));
	bodyMesh->SetupAttachment(splineComponent);
}

void ABossEnemy::BeginPlay() {
	Super::BeginPlay();

	ensure(widgetclass);

	OnActorBeginOverlap.AddUniqueDynamic(this, &AEnemy::EnterEvent);
	OnActorEndOverlap.AddUniqueDynamic(this, &AEnemy::ExitEvent);
}

void ABossEnemy::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	OnActorBeginOverlap.RemoveDynamic(this, &AEnemy::EnterEvent);
	OnActorEndOverlap.RemoveDynamic(this, &AEnemy::ExitEvent);
}

/*
overriding MoveToCreature(), because we need to setup values to step into Moving state (that happens in the parent), but we also need to add boss lifebar widget to viewport (the boss is coming and fight gets started)
*/

void ABossEnemy::MoveToCreature(const float timeToStart) {

	Super::MoveToCreature(timeToStart);

	UE_LOG(LogTemp, Warning, TEXT("bossenemy1"));

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("bossenemy2"));
		bossuserwidget = Cast<UBossUserWidget>(CreateWidget(GetWorld(), widgetclass));

		if (bossuserwidget) {

			bossuserwidget->boss = this;
			bossuserwidget->bossHealthComp = healthComponent;

			UE_LOG(LogTemp, Warning, TEXT("bossenemy3"));
			bossuserwidget->AddToViewport(0);
		}
	}
}

/*
after ending movement state, bossenemy will be above the player vertically, not around the player. 
*/

FVector ABossEnemy::GetEndPosition() const {
	FVector vec = creature->GetActorLocation();
	double z = GetActorLocation().Z;
	vec.Z = z;
	return vec;
}

FVector ABossEnemy::GetPositionOfBodyMesh() const {
	return bodyMesh ? bodyMesh->GetComponentLocation() : FVector::ZeroVector;
}

float ABossEnemy::GetBodyMeshRadius() const {
	FCollisionShape shape = bodyMesh->GetCollisionShape();
	return shape.GetSphereRadius();
}

UStaticMeshComponent* ABossEnemy::GetCurrentBodyMesh() const {
	return bodyMesh;
}

/*
when boss dies, boss itself doesn't need to be destroyed, because level replacement will destroy that. We remove all widgets from viewport and open the new level
*/

void ABossEnemy::DoAfterDead() {

	UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

	FLatentActionInfo latentInfo;
	UGameplayStatics::OpenLevel(this, levelToLoadAfterDefeat, true);
}