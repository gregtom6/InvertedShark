// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ProjectileCompPositioner.generated.h"

class UStaticMeshComponent;
class UStaticMesh;
class UMaterialInterface;

//this component is used for positioning the impacted projectile
//on its target actor. 

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

	int32 projectileShotThroughMeCacheIndex;

	UPROPERTY()
		TArray<UStaticMeshComponent*> projectilesShotThroughMe;

	UPROPERTY(EditAnywhere)
		int32 projectilesShotThroughMeCacheSize;

public:	

	void SetupProjectile(const FRotator rotator, const FVector scale, UStaticMesh* mesh, UMaterialInterface* material, const FVector offset);
};
