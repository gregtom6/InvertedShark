// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileShootable.generated.h"

UCLASS(Abstract)
class SMALLPROJECT_API AProjectileShootable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileShootable();

protected:

	virtual void BeginPlay() override;

	int projectileShootedThroughMeCacheIndex;

	UPROPERTY(EditAnywhere)
		int projectilesShootedThroughMeCacheSize;

	UPROPERTY()
		TArray<UStaticMeshComponent*> projectilesShootedThroughMe;

	void SetupProjectile(FRotator rotator, FVector scale, UStaticMesh* mesh, UMaterialInterface* material, FVector offset);
	virtual AActor* GetActor();
};
