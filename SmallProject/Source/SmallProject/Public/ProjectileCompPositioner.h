// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileCompPositioner.generated.h"

class UStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SMALLPROJECT_API UProjectileCompPositioner : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UProjectileCompPositioner();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	int32 projectileShootedThroughMeCacheIndex;

	UPROPERTY()
		TArray<UStaticMeshComponent*> projectilesShootedThroughMe;

	UPROPERTY(EditAnywhere)
		int32 projectilesShootedThroughMeCacheSize;

public:	

	void SetupProjectile(FRotator rotator, FVector scale, UStaticMesh* mesh, UMaterialInterface* material, FVector offset);
};
