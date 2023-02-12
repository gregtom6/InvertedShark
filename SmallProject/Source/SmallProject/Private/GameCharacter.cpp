// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <GameCharacterUserWidget.h>
#include "Components/AudioComponent.h"
#include "Enemy.h"
#include "BossEnemy.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
AGameCharacter::AGameCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	LeftLeft = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LeftLeft"));
	RightArm = CreateDefaultSubobject< UPhysicsConstraintComponent>(TEXT("RightArm"));

	Tongue = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tongue"));

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));

	TongueAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("TongueAudio"));

	Spark = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Spark"));

	Spark->SetupAttachment(CameraMesh);

	SpringArm->SetupAttachment(CameraMesh);

	Tongue->SetupAttachment(CameraMesh);

	Camera->FieldOfView = 120.f;
	Camera->SetupAttachment(SpringArm);

	LeftLeft->SetupAttachment(CameraMesh);
	RightArm->SetupAttachment(CameraMesh);

	AudioComp->SetupAttachment(CameraMesh);

	TongueAudio->SetupAttachment(CameraMesh);

	Camera->Deactivate();

	isHugging = false;
}

void AGameCharacter::RotateLR(float rotateDelta) {

	if (isHugging || actualStatus == GameCharacterStatus::Dead) { return; }

	FRotator actualRotation = GetActorRotation();
	actualRotation.Yaw += rotateDelta * RotateSpeed;
	SetActorRotation(actualRotation);

	SpringArm->SetWorldRotation(actualRotation);
}

void AGameCharacter::StrafeLR(float movementDelta) {

	if (isHugging || actualStatus == GameCharacterStatus::Dead) { return; }

	FVector rightVector = GetActorRightVector();
	FVector upVector = GetActorUpVector();
	FVector forwardVector = GetActorForwardVector();

	FVector newLocation = GetActorLocation();

	SetActorLocation(newLocation + (rightVector * movementDelta * MovementSpeed));
}

void AGameCharacter::WingBeat() {

	if (isHugging || actualStatus == GameCharacterStatus::Dead) { return; }

	if (actualEnergy < energyDecreaseAfterWingBeat) { return; }

	FVector actorForwardVector = GetActorForwardVector() * ForwardFlyStrength;
	FVector actorUpVector = GetActorUpVector() * WingStrength;
	FVector impulseDirection = actorUpVector + actorForwardVector;

	CameraMesh->AddImpulse(impulseDirection);

	actualEnergy -= energyDecreaseAfterWingBeat;

	if (AudioComp && wingBeat) {
		AudioComp->Play(0.f);
	}
}

void AGameCharacter::HugCreature() {

	if (!isHugging) {

		if (creature != nullptr && creature->IsCharacterInFur()) {

			SetRotationLocks(false, false, false);

			creature->GetHugged();

			RightArm->ConstraintActor1 = this;
			FConstrainComponentPropName name1;
			name1.ComponentName = CameraMesh->GetFName();
			RightArm->ComponentName1 = name1;

			RightArm->ConstraintActor2 = creature;
			FConstrainComponentPropName name2;
			name2.ComponentName = "StaticMesh";
			RightArm->ComponentName2 = name2;

			RightArm->SetWorldLocation(creature->GetLocation());

			RightArm->InitComponentConstraint();

			CameraMesh->SetAngularDamping(angularDamplingWhenHuggingFur);
			FVector actorForwardVector = GetActorForwardVector() * ForwardFlyStrength;
			FVector actorUpVector = GetActorUpVector() * WingStrength;
			FVector impulseDirection = actorUpVector + actorForwardVector;

			CameraMesh->AddImpulse(impulseDirection);

			isHugging = true;

			startArmLength = SpringArm->TargetArmLength;
			targetArmLength = zoomedOutArmLength;
			startTimeForSpringArm = GetWorld()->GetTimeSeconds();
		}
	}
	else {

		RightArm->BreakConstraint();

		FRotator rotation = GetActorRotation();
		rotation.Roll = 0.f;
		rotation.Pitch = 0.f;
		SetActorRotation(rotation);

		SetRotationLocks(true, true, false);

		CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		CameraMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		CameraMesh->SetAngularDamping(normalAngularDampling);

		isHugging = false;

		startArmLength = SpringArm->TargetArmLength;
		targetArmLength = defaultArmLength;
		startTimeForSpringArm = GetWorld()->GetTimeSeconds();
	}

}

void AGameCharacter::SetRotationLocks(bool X, bool Y, bool Z) {
	if (CameraMesh->GetBodyInstance())
	{
		CameraMesh->GetBodyInstance()->bLockXRotation = X;
		CameraMesh->GetBodyInstance()->bLockYRotation = Y;
		CameraMesh->GetBodyInstance()->bLockZRotation = Z;
		CameraMesh->GetBodyInstance()->SetDOFLock(EDOFMode::SixDOF);
	}
}

void AGameCharacter::Attack() {

	if (actualStatus == GameCharacterStatus::Attack || isHugging || actualStatus == GameCharacterStatus::Dead) { return; }

	UE_LOG(LogTemp, Warning, TEXT("attack happened"));

	Tongue->SetVisibility(true);
	startTime = GetWorld()->GetTimeSeconds();
	prevStatus = actualStatus;
	actualStatus = GameCharacterStatus::Attack;

	TongueAudio->Play(0.f);
}

void AGameCharacter::Pause() {

	if (pauseStatus == PauseStatus::Played) {
		UE_LOG(LogTemp, Warning, TEXT("pause activate"));
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		pauseStatus = PauseStatus::Paused;

		APlayerController* PC = Cast<APlayerController>(GetController());

		if (PC)
		{
			PC->bShowMouseCursor = true;
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;
		}

		widgetPauseMenuInstance->AddToViewport();
	}
	else if (pauseStatus == PauseStatus::Paused) {
		UE_LOG(LogTemp, Warning, TEXT("pause deactivate"));
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		pauseStatus = PauseStatus::Played;

		APlayerController* PC = Cast<APlayerController>(GetController());

		if (PC)
		{
			PC->bShowMouseCursor = false;
			PC->bEnableClickEvents = false;
			PC->bEnableMouseOverEvents = false;
		}

		widgetPauseMenuInstance->RemoveFromViewport();
	}
}

// Called when the game starts or when spawned
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AudioComp && wingBeat) {
		AudioComp->SetSound(wingBeat);
	}

	if (TongueAudio && tongueSound) {
		TongueAudio->SetSound(tongueSound);
	}

	widgetPauseMenuInstance = CreateWidget<UPauseUserWidget>(GetWorld(), widgetPauseMenu);

	if (widgetPauseMenuInstance != nullptr)
		widgetPauseMenuInstance->SetGameCharacter(this);

	startPos = GetActorLocation();

	Tongue->SetVisibility(false);

	actualEnergy = maxEnergy;

	actualStatus = GameCharacterStatus::Calm;
	prevStatus = GameCharacterStatus::Calm;

	pauseStatus = PauseStatus::Played;

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("energy1"));
		energyuserwidget = Cast<UGameCharacterUserWidget>(CreateWidget(GetWorld(), widgetclass));

		if (energyuserwidget != nullptr) {

			energyuserwidget->player = this;

			UE_LOG(LogTemp, Warning, TEXT("energy2"));
			energyuserwidget->AddToViewport(0);
		}
	}

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACreature::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0) {
		ACreature* crea = Cast<ACreature>(FoundActors[0]);

		if (crea != nullptr) {
			creature = crea;
		}
	}

	FoundActors.Empty();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABossEnemy::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0) {
		ABossEnemy* b = Cast<ABossEnemy>(FoundActors[0]);

		if (b != nullptr) {
			bossEnemy = b;
		}
	}

	Tongue->OnComponentBeginOverlap.AddDynamic(this, &AGameCharacter::TriggerEnter);
	Tongue->OnComponentEndOverlap.AddDynamic(this, &AGameCharacter::TriggerExit);
}

void AGameCharacter::TriggerEnter(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) {
	UE_LOG(LogTemp, Warning, TEXT("sword inside enemy1"));
	if (OtherActor && OtherActor != this) {
		UE_LOG(LogTemp, Warning, TEXT("sword inside enemy2"));
		if (OtherActor->IsA(AEnemy::StaticClass()) || OtherActor->IsA(ABossEnemy::StaticClass())) {
			UE_LOG(LogTemp, Warning, TEXT("sword inside enemy3"));

			UE_LOG(LogTemp, Warning, TEXT("sword hit %f %f %f      %f %f %f"), SweepResult.ImpactPoint.X, SweepResult.ImpactPoint.Y, SweepResult.ImpactPoint.Z, SweepResult.Location.X, SweepResult.Location.Y, SweepResult.Location.Z);
		}
	}
}

void AGameCharacter::TriggerExit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) {

}

// Called every frame
void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector currentVelocity = FVector::ZeroVector;
	if (actualStatus != GameCharacterStatus::Dead) {
		currentVelocity = CameraMesh->GetPhysicsLinearVelocity();
		FVector clampedVelocity = currentVelocity.GetClampedToMaxSize(velocityLimit);
		CameraMesh->SetPhysicsLinearVelocity(clampedVelocity);
	}

	FVector actorLocation = GetActorLocation();

	if (actualStatus == GameCharacterStatus::Attack) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= attackTime) {
			Tongue->SetVisibility(false);
			prevStatus = actualStatus;
			actualStatus = GameCharacterStatus::Calm;
		}
	}

	float newEnergy = actualEnergy + energyRegeneration * DeltaTime;
	float restMult = 1.f;

	if (isHugging || (currentVelocity.X < restVelocity && currentVelocity.Y < restVelocity && currentVelocity.Z < restVelocity)) {
		restMult = restingMultiplier;
	}

	if (newEnergy <= maxEnergy)
		actualEnergy += energyRegeneration * restMult * DeltaTime;

	float currentTimeForSpringArm = GetWorld()->GetTimeSeconds() - startTimeForSpringArm;
	currentTimeForSpringArm *= springArmLengthSpeed;
	if (currentTimeForSpringArm > 1.f)
		currentTimeForSpringArm = 1.f;

	SpringArm->TargetArmLength = FMath::Lerp(startArmLength, targetArmLength, currentTimeForSpringArm);

	if (actualStatus != GameCharacterStatus::Dead && actorLocation.Z <= heightToDie) {
		//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
		CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		CameraMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		actualStatus = GameCharacterStatus::Dead;

		OnScoreChangedDelegate.ExecuteIfBound();
	}

	FVector Actor1ClosestPoint, Actor2ClosestPoint, OverlapPoint;
	bool bOverlap = false;

	if (bossEnemy != nullptr) {

		FVector TempActor1ClosestPoint, TempActor2ClosestPoint;
		if (GetOverlapInfluenceSphere(Tongue, Actor1ClosestPoint, Actor2ClosestPoint))
		{
			if (!bOverlap)
			{
				Actor1ClosestPoint = TempActor1ClosestPoint;
				Actor2ClosestPoint = TempActor2ClosestPoint;
				bOverlap = true;
			}
			else
			{
				FVector TempOverlapPoint = (TempActor1ClosestPoint + TempActor2ClosestPoint) / 2.0f;
				FVector CurrentOverlapPoint = (Actor1ClosestPoint + Actor2ClosestPoint) / 2.0f;
				if (FVector::DistSquared(TempOverlapPoint, GetActorLocation()) <
					FVector::DistSquared(CurrentOverlapPoint, bossEnemy->GetActorLocation()))
				{
					Actor1ClosestPoint = TempActor1ClosestPoint;
					Actor2ClosestPoint = TempActor2ClosestPoint;
				}
			}
		}
	}

	if (bOverlap && Tongue->GetVisibleFlag())
	{
		FVector Center = GetActorLocation();
		float Radius = 10.0f;
		int32 Segments = 32;
		FColor LineColor = FColor::Blue;
		FColor LineColor2 = FColor::Red;
		float Duration = 2.0f;

		ABossEnemy* boss = Cast<ABossEnemy>(bossEnemy);

		//DrawDebugSphere(GetWorld(), Actor1ClosestPoint, Radius, Segments, LineColor, false, Duration);
		//DrawDebugSphere(GetWorld(), boss->GetPositionOfBodyMesh(), boss->GetBodyMeshRadius(), Segments, LineColor2, false, Duration);


		OverlapPoint = (Actor1ClosestPoint + Actor2ClosestPoint) / 2.0f;

		Spark->SetWorldLocation(Actor1ClosestPoint);
		Spark->Activate();
	}
	else {
		Spark->Deactivate();
	}
}

bool AGameCharacter::GetOverlapInfluenceSphere(UStaticMeshComponent* StaticMeshComponent, FVector& OutActor1ClosestPoint, FVector& OutActor2ClosestPoint)
{
	FTransform TransformA = StaticMeshComponent->GetComponentTransform();

	FVector SphereLocationA = TransformA.GetLocation();

	float SphereRadiusA = StaticMeshComponent->Bounds.SphereRadius;

	ABossEnemy* boss = Cast<ABossEnemy>(bossEnemy);
	FVector position = boss->GetPositionOfBodyMesh();
	float radius = boss->GetBodyMeshRadius();


	FVector Difference = position - SphereLocationA;
	float Distance = Difference.Size();

	if (Distance <= SphereRadiusA + radius)
	{
		FVector ClosestPointOnSphereA = SphereLocationA + Difference.GetSafeNormal() * SphereRadiusA;
		FVector ClosestPointOnSphereB = position - Difference.GetSafeNormal() * radius;

		OutActor1ClosestPoint = ClosestPointOnSphereA;
		OutActor2ClosestPoint = ClosestPointOnSphereB;

		return true;
	}

	return false;
}

float AGameCharacter::GetEnergy() {
	return actualEnergy;
}

float AGameCharacter::GetMaxEnergy() {
	return maxEnergy;
}

GameCharacterStatus AGameCharacter::GetStatus() {
	return actualStatus;
}

GameCharacterStatus AGameCharacter::GetPrevStatus() {
	return prevStatus;
}

void AGameCharacter::SetPrevStatusToActualStatus() {
	prevStatus = actualStatus;
}

// Called to bind functionality to input
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//register for LR movement
	PlayerInputComponent->BindAxis(TEXT("StrafeLR"), this, &AGameCharacter::StrafeLR);

	PlayerInputComponent->BindAxis(TEXT("RotateLR"), this, &AGameCharacter::RotateLR);

	PlayerInputComponent->BindAction(TEXT("WingBeat"), IE_Pressed, this, &AGameCharacter::WingBeat);

	PlayerInputComponent->BindAction(TEXT("HugCreature"), IE_Pressed, this, &AGameCharacter::HugCreature);

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AGameCharacter::Attack);

	FInputActionBinding& toggle = PlayerInputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AGameCharacter::Pause);
	toggle.bExecuteWhenPaused = true;
}

