// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Blueprint/UserWidget.h"
#include "Components/WidgetComponent.h"
#include "Components/AudioComponent.h"
#include <Sound/SoundCue.h >
#include "Creature.generated.h"

UCLASS()
class SMALLPROJECT_API ACreature : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	//ACreature();
	ACreature(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	
	UFUNCTION()
		void EnterEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void ExitEvent(class AActor* overlappedActor, class AActor* otherActor);

	UFUNCTION()
		void TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USoundCue* whaleSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UAudioComponent* WhaleAudioComp;

	UPROPERTY(EditAnywhere)
		TSubclassOf<UUserWidget> widgetclass;

	UPROPERTY(VisibleInstanceOnly)
		class UCreatureUserWidget* creatureuserwidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "settings")
		float deltaDamage;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* CreatureMesh;

	UPROPERTY(EditAnywhere)
		TArray<AActor*> positionsToMove;

	UPROPERTY(EditAnywhere)
		float movementSpeed;

	UPROPERTY(EditAnywhere)
		float fastMovementSpeed;

	UPROPERTY(EditAnywhere)
		float waitTimeBeforeFirstMove;

	UPROPERTY(EditAnywhere)
		float waitingTimeToMoveForwardAfterDefeatingEnemies;

	UPROPERTY(EditAnywhere)
		float waitTimeAfterHuggedToMoveForward;

	void StepTargetIndex();

	void GetHugged();

	void SwitchingToMovingFast();

	Status actualStatus;

	FVector actualStartPosition;
	FVector actualEndPosition;

	double startTime;

	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
	FVector GetLocation() const { return GetActorLocation(); }

	float Health;
	float MaxHealth = 120;

	int actualTargetIndex = 0;

	TArray<AActor*> enemiesActuallyAttacking;
};

UENUM()
enum class Status : uint8
{
	Initial,
	Stopped,
	Moving,
	UnderAttack,
	MovingFast,
	WaitBeforeMoveFast,
	WaitAfterHuggedByPlayer,
};