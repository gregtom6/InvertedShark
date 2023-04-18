// Fill out your copyright notice in the Description page of Project Settings.


#include "BossEnemy.h"
#include "BossUserWidget.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Creature.h"

ABossEnemy::ABossEnemy(const FObjectInitializer& ObjectInitializer)
{
	bodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bodyMesh"));
	bodyMesh->SetupAttachment(splineComponent);

	OnActorBeginOverlap.AddUniqueDynamic(this, &AEnemy::EnterEvent);
	OnActorEndOverlap.AddUniqueDynamic(this, &AEnemy::ExitEvent);
}

/*
overriding MoveToCreature(), because we need to setup values to step into Moving state (that happens in the parent), but we also need to add boss lifebar widget to viewport (the boss is coming and fight gets started)
*/

void ABossEnemy::MoveToCreature(float timeToStart) {

	Super::MoveToCreature(timeToStart);

	UE_LOG(LogTemp, Warning, TEXT("bossenemy1"));

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("bossenemy2"));
		bossuserwidget = Cast<UBossUserWidget>(CreateWidget(GetWorld(), widgetclass));

		if (bossuserwidget != nullptr) {

			bossuserwidget->boss = this;

			UE_LOG(LogTemp, Warning, TEXT("bossenemy3"));
			bossuserwidget->AddToViewport(0);
		}
	}
}

/*
after ending movement state, bossenemy will be above the player vertically, not around the player. 
*/

FVector ABossEnemy::GetEndPosition() {
	FVector vec = creature->GetActorLocation();
	double z = GetActorLocation().Z;
	vec.Z = z;
	return vec;
}

FVector ABossEnemy::GetPositionOfBodyMesh() {
	return bodyMesh->GetComponentLocation();
}

float ABossEnemy::GetBodyMeshRadius() {
	FCollisionShape shape = bodyMesh->GetCollisionShape();
	return shape.GetSphereRadius();
}

/*
extra methods for boss life bar
*/

float ABossEnemy::GetLife() {
	return actualLife;
}

float ABossEnemy::GetMaxLife() {
	return maxLife;
}

UStaticMeshComponent* ABossEnemy::GetCurrentBodyMesh() {
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