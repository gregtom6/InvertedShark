// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileShootable.h"

// Sets default values
AProjectileShootable::AProjectileShootable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

void AProjectileShootable::BeginPlay() {
	Super::BeginPlay();

	for (int i = 0; i < projectilesShootedThroughMeCacheSize; i++) {
		UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(GetActor());
		projectilesShootedThroughMe.Add(NewComponent);
	}
}

void AProjectileShootable::SetupProjectile(FRotator rotator, FVector scale, UStaticMesh* mesh, UMaterialInterface* material, FVector offset) {

	if (projectileShootedThroughMeCacheIndex >= projectilesShootedThroughMe.Num()) {
		UE_LOG(LogTemp, Warning, TEXT("no cached projectile elements in me"));
	}

	UStaticMeshComponent* cachedShootedProjectile = projectilesShootedThroughMe[projectileShootedThroughMeCacheIndex];
	cachedShootedProjectile->RegisterComponent();
	cachedShootedProjectile->SetStaticMesh(mesh);
	cachedShootedProjectile->AttachToComponent(GetActor()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	cachedShootedProjectile->SetRelativeScale3D(scale);

	cachedShootedProjectile->SetWorldLocation(offset);

	cachedShootedProjectile->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	cachedShootedProjectile->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	cachedShootedProjectile->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	cachedShootedProjectile->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	cachedShootedProjectile->SetRelativeRotation(rotator);
	cachedShootedProjectile->SetMaterial(0, material);

	FName NewName = MakeUniqueObjectName(GetActor(), UStaticMeshComponent::StaticClass(), TEXT("InnerProjectile"));
	cachedShootedProjectile->Rename(*NewName.ToString());

	projectileShootedThroughMeCacheIndex += 1;

	if (projectileShootedThroughMeCacheIndex >= projectilesShootedThroughMe.Num()) {
		UStaticMeshComponent* createdComp = NewObject<UStaticMeshComponent>(GetActor());
		projectilesShootedThroughMe.Add(createdComp);
	}
}

AActor* AProjectileShootable::GetActor() {
	return this;
}