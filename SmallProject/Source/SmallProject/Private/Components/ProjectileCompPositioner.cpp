// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/ProjectileCompPositioner.h"

// Sets default values for this component's properties
UProjectileCompPositioner::UProjectileCompPositioner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UProjectileCompPositioner::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < projectilesShotThroughMeCacheSize; i++) {
		UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(GetOwner());
		projectilesShotThroughMe.Add(NewComponent);
	}

}


void UProjectileCompPositioner::SetupProjectile(const FRotator rotator, const FVector scale, UStaticMesh* mesh, UMaterialInterface* material, const FVector offset) {

	if (projectileShotThroughMeCacheIndex >= projectilesShotThroughMe.Num()) {
		UE_LOG(LogTemp, Warning, TEXT("no cached projectile elements in me"));
	}

	UStaticMeshComponent* cachedShootedProjectile = projectilesShotThroughMe[projectileShotThroughMeCacheIndex];
	cachedShootedProjectile->RegisterComponent();
	cachedShootedProjectile->SetStaticMesh(mesh);
	cachedShootedProjectile->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	cachedShootedProjectile->SetRelativeScale3D(scale);

	cachedShootedProjectile->SetWorldLocation(offset);

	cachedShootedProjectile->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	cachedShootedProjectile->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	cachedShootedProjectile->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	cachedShootedProjectile->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	cachedShootedProjectile->SetRelativeRotation(rotator);
	cachedShootedProjectile->SetMaterial(0, material);

	FName NewName = MakeUniqueObjectName(GetOwner(), UStaticMeshComponent::StaticClass(), TEXT("InnerProjectile"));
	cachedShootedProjectile->Rename(*NewName.ToString());

	projectileShotThroughMeCacheIndex += 1;

	if (projectileShotThroughMeCacheIndex >= projectilesShotThroughMe.Num()) {
		UStaticMeshComponent* createdComp = NewObject<UStaticMeshComponent>(GetOwner());
		projectilesShotThroughMe.Add(createdComp);
	}
}

