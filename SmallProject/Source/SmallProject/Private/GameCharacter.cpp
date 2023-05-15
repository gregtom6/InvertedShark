// Fill out your copyright notice in the Description page of Project Settings.


#include "GameCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <GameCharacterUserWidget.h>
#include "Components/AudioComponent.h"
#include "Enemy.h"
#include "BossEnemy.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetStringLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Creature.h"
#include <Sound/SoundCue.h >
#include <PauseUserWidget.h>
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "ActorSequenceComponent.h"
#include "ActorSequencePlayer.h"
#include "GameFramework/PlayerController.h"
#include "Materials/MaterialInterface.h"
#include <Kismet/KismetMathLibrary.h>
#include "ProjectileCompPositioner.h"
#include "ResourceDataAsset.h"

// Sets default values
AGameCharacter::AGameCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	SetRootComponent(CameraMesh);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	LeftLeft = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("LeftLeft"));
	RightArm = CreateDefaultSubobject< UPhysicsConstraintComponent>(TEXT("RightArm"));

	Tongue = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Tongue"));

	AudioComp = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComp"));

	TongueAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("TongueAudio"));

	MetalScratchAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("MetalScratchAudio"));

	DashAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("DashAudio"));

	SneezeAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("SneezeAudio"));

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

	MetalScratchAudio->SetupAttachment(CameraMesh);

	DashAudio->SetupAttachment(CameraMesh);

	SneezeAudio->SetupAttachment(CameraMesh);

	Camera->Deactivate();

	isHugging = false;
	ProjectilePositioner = CreateDefaultSubobject<UProjectileCompPositioner>(TEXT("ProjectilePositioner"));

	globalSettings = NewObject<UResourceDataAsset>(GetTransientPackage(), FName("globalSettings"));
}

/*
left right rotations of game character
*/
void AGameCharacter::RotateLR(float rotateDelta) {

	if (isHugging || actualStatus == EGameCharacterStatus::Dead) { return; }

	FRotator actualRotation = GetActorRotation();
	actualRotation.Yaw += rotateDelta * RotateSpeed;
	CameraMesh->SetRelativeRotation(actualRotation);
}


/*
wing beat movement of game character
*/
void AGameCharacter::WingBeat() {

	if (isHugging || actualStatus == EGameCharacterStatus::Dead || actualStatus == EGameCharacterStatus::UpDash) { return; }

	if (actualEnergy < energyDecreaseAfterWingBeat) { return; }

	FVector actorForwardVector = GetActorForwardVector() * ForwardFlyStrength;
	FVector actorUpVector = GetActorUpVector() * WingStrength;
	FVector impulseDirection = actorUpVector + actorForwardVector;

	CameraMesh->AddImpulse(impulseDirection);

	actualEnergy -= energyDecreaseAfterWingBeat;

	AudioComp->Play(0.f);

	wingPlayer->Stop();
	FRotator rot = CameraMesh->GetComponentRotation();
	rot.Roll = 0.f;
	CameraMesh->SetWorldRotation(rot);

	wingPlayer->Play();
}

/*
creature fur hugging ability. When under the creature, player can hug its fur for faster energy reloading
*/
void AGameCharacter::HugCreature() {

	if (!isHugging) {

		if (creature != nullptr && creature->IsCharacterInFur()) {

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

		CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
		CameraMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);

		CameraMesh->SetAngularDamping(normalAngularDampling);

		isHugging = false;

		startArmLength = SpringArm->TargetArmLength;
		targetArmLength = defaultArmLength;
		startTimeForSpringArm = GetWorld()->GetTimeSeconds();
	}

}

/*
helper method for hugging action
*/
void AGameCharacter::SetRotationLocks(bool X, bool Y, bool Z) {
	if (CameraMesh->GetBodyInstance())
	{
		CameraMesh->GetBodyInstance()->bLockXRotation = X;
		CameraMesh->GetBodyInstance()->bLockYRotation = Y;
		CameraMesh->GetBodyInstance()->bLockZRotation = Z;
		CameraMesh->GetBodyInstance()->SetDOFLock(EDOFMode::SixDOF);
	}
}

/*
attack ability
*/
void AGameCharacter::Attack() {

	if (actualStatus == EGameCharacterStatus::Attack || isHugging || actualStatus == EGameCharacterStatus::Dead || actualStatus == EGameCharacterStatus::UpDash) { return; }

	UE_LOG(LogTemp, Warning, TEXT("attack happened"));

	Tongue->SetVisibility(true);
	startTime = GetWorld()->GetTimeSeconds();
	prevStatus = actualStatus;
	actualStatus = EGameCharacterStatus::Attack;

	TongueAudio->Play(0.f);
}

/*
dash ability
*/
void AGameCharacter::Dash() {
	if (isHugging || actualStatus == EGameCharacterStatus::Dead || actualStatus == EGameCharacterStatus::UpDash || actualStatus == EGameCharacterStatus::Attack) { return; }


	APlayerController* thisCont = GetWorld()->GetFirstPlayerController();
	if (thisCont->IsInputKeyDown(EKeys::S)) {
		actualStatus = EGameCharacterStatus::DownDash;
		DownDash();
	}
	else if (thisCont->IsInputKeyDown(EKeys::Q)) {
		actualStatus = EGameCharacterStatus::LeftDash;
		LeftDash();
	}
	else if (thisCont->IsInputKeyDown(EKeys::E)) {
		actualStatus = EGameCharacterStatus::RightDash;
		RightDash();
	}
	else {
		actualStatus = EGameCharacterStatus::UpDash;
		UpDash();
	}
}

void AGameCharacter::DownDash() {

	FVector actorDownVector = -GetActorUpVector() * upDownDashStrength;
	FVector impulseDirection = actorDownVector;

	positionBeforeDash = GetActorLocation();

	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	CameraMesh->AddImpulse(impulseDirection);

	startTime = GetWorld()->GetTimeSeconds();
}

void AGameCharacter::LeftDash() {
	FVector actorUpVector = (GetActorUpVector() + -GetActorRightVector()) * leftRightDashStrength;
	FVector impulseDirection = actorUpVector;

	positionBeforeDash = GetActorLocation();

	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	CameraMesh->AddImpulse(impulseDirection);

	if (!amITargeted) {

		actualEnergy -= energyDecreaseAfterDash;
		if (actualEnergy < 0.f)
			actualEnergy = 0.f;
	}
	else {
		SlowdownTime();
	}

	SpringArm->bInheritYaw = false;
	FRotator rotator = CameraMesh->GetComponentRotation();
	SpringArm->SetRelativeRotation(rotator);
	leftDashPlayer->Play();

	startTime = GetWorld()->GetTimeSeconds();
}

void AGameCharacter::RightDash() {
	FVector actorUpVector = (GetActorUpVector() + GetActorRightVector()) * leftRightDashStrength;
	FVector impulseDirection = actorUpVector;

	positionBeforeDash = GetActorLocation();

	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	CameraMesh->AddImpulse(impulseDirection);

	if (!amITargeted) {

		actualEnergy -= energyDecreaseAfterDash;
		if (actualEnergy < 0.f)
			actualEnergy = 0.f;
	}
	else {
		SlowdownTime();
	}

	SpringArm->bInheritYaw = false;
	FRotator rotator = CameraMesh->GetComponentRotation();
	SpringArm->SetRelativeRotation(rotator);
	rightDashPlayer->Play();

	startTime = GetWorld()->GetTimeSeconds();
}

void AGameCharacter::UpDash() {
	if (actualEnergy < energyDecreaseAfterDash) { return; }
	FVector actorUpVector = GetActorUpVector() * upDownDashStrength;
	FVector impulseDirection = actorUpVector;

	positionBeforeDash = GetActorLocation();

	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	CameraMesh->AddImpulse(impulseDirection);

	actualEnergy -= energyDecreaseAfterDash;
	if (actualEnergy < 0.f)
		actualEnergy = 0.f;

	startTime = GetWorld()->GetTimeSeconds();

	DashAudio->Play(0.f);

	SneezeAudio->Play(0.f);

	if (skeletal) {
		UAnimationAsset* AnimationAsset = skeletal->AnimationData.AnimToPlay;

		if (AnimationAsset) {
			UAnimSequence* AnimSequence = Cast<UAnimSequence>(AnimationAsset);

			if (AnimSequence)
			{
				skeletal->PlayAnimation(AnimSequence, false);
			}
		}
	}

	FMovieSceneSequencePlaybackParams FirstFrame(0.f, EUpdatePositionMethod::Play);
	loopedEyePlayer->SetPlaybackPosition(FirstFrame);
	loopedEyePlayer->Stop();
	sneezeBlinkPlayer->Play();

	leftNoseSneezeNiagara->Activate(true);
	rightNoseSneezeNiagara->Activate(true);
}

void AGameCharacter::NotifyTargeting(bool iAmTargeted) {
	amITargeted = iAmTargeted;
}


/*
game can be paused with this function. Managing cursor state and adding or removing pause widget to viewport
*/
void AGameCharacter::Pause() {

	if (pauseStatus == EPauseStatus::Played) {
		UE_LOG(LogTemp, Warning, TEXT("pause activate"));
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		pauseStatus = EPauseStatus::Paused;

		APlayerController* PC = Cast<APlayerController>(GetController());

		if (PC)
		{
			PC->bShowMouseCursor = true;
			PC->bEnableClickEvents = true;
			PC->bEnableMouseOverEvents = true;
		}

		UE_LOG(LogTemp, Warning, TEXT("pause activate3"));

		InitializePause();

		if (widgetPauseMenuInstance != nullptr) {

			widgetPauseMenuInstance->AddToViewport();
		}

		UE_LOG(LogTemp, Warning, TEXT("pause activate4"));
	}
	else if (pauseStatus == EPauseStatus::Paused) {
		UE_LOG(LogTemp, Warning, TEXT("pause deactivate"));
		UGameplayStatics::SetGamePaused(GetWorld(), false);
		pauseStatus = EPauseStatus::Played;

		APlayerController* PC = Cast<APlayerController>(GetController());

		if (PC)
		{
			PC->bShowMouseCursor = false;
			PC->bEnableClickEvents = false;
			PC->bEnableMouseOverEvents = false;
		}

		widgetPauseMenuInstance->RemoveFromParent();
	}
}

/*
initializing audios, pause widget, energy, status, position, etc.
*/
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializePause();

	startPos = GetActorLocation();

	Tongue->SetVisibility(false);

	startArmLength = defaultArmLength;
	targetArmLength = defaultArmLength;

	actualEnergy = maxEnergy;

	actualStatus = EGameCharacterStatus::Calm;
	prevStatus = EGameCharacterStatus::Calm;

	pauseStatus = EPauseStatus::Played;

	slowdownStatus = ESlowDownStatus::NormalTime;

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

	skeletal = FindComponentByClass<USkeletalMeshComponent>();

	// If the component was found, assign it to the UPROPERTY() variable
	if (skeletal)
	{
		skeletal->SetupAttachment(CameraMesh);
	}

	TArray<UActorSequenceComponent*> ActorSequenceComponents;
	GetComponents<UActorSequenceComponent>(ActorSequenceComponents);

	for (int i = 0; i < ActorSequenceComponents.Num(); i++)
	{
		if (ActorSequenceComponents[i]->GetFName() == FName("loopedeye"))
		{
			loopedEyePlayer = ActorSequenceComponents[i]->GetSequencePlayer();
		}
		else if (ActorSequenceComponents[i]->GetFName() == FName("sneezeblink"))
		{
			sneezeBlinkPlayer = ActorSequenceComponents[i]->GetSequencePlayer();
			FScriptDelegate funcDelegate;
			funcDelegate.BindUFunction(this, FName("SneezeBlinkEnded"));
			sneezeBlinkPlayer->OnFinished.AddUnique(funcDelegate);
		}
		else if (ActorSequenceComponents[i]->GetFName() == FName("leftDash"))
		{
			leftDashPlayer = ActorSequenceComponents[i]->GetSequencePlayer();
		}
		else if (ActorSequenceComponents[i]->GetFName() == FName("rightDash"))
		{
			rightDashPlayer = ActorSequenceComponents[i]->GetSequencePlayer();
		}
		else if (ActorSequenceComponents[i]->GetFName() == FName("wing"))
		{
			wingPlayer = ActorSequenceComponents[i]->GetSequencePlayer();
		}
	}

	TArray<UNiagaraComponent*> niagaraComponents;
	GetComponents<UNiagaraComponent>(niagaraComponents);

	for (int i = 0; i < niagaraComponents.Num(); i++) {
		if (niagaraComponents[i]->GetFName() == FName("LeftNoseSneeze"))
		{
			leftNoseSneezeNiagara = niagaraComponents[i];
		}
		else if (niagaraComponents[i]->GetFName() == FName("RightNoseSneeze")) {
			rightNoseSneezeNiagara = niagaraComponents[i];
		}
	}
}

void AGameCharacter::SneezeBlinkEnded() {
	sneezeBlinkPlayer->Stop();
	loopedEyePlayer->Play();
}

void AGameCharacter::InitializePause() {
	widgetPauseMenuInstance = CreateWidget<UPauseUserWidget>(GetWorld(), widgetPauseMenu);

	if (widgetPauseMenuInstance != nullptr)
		widgetPauseMenuInstance->SetGameCharacter(this);
}

// Called every frame
void AGameCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector currentVelocity = CameraMesh->GetPhysicsLinearVelocity();

	VelocityManagement(currentVelocity);

	StateManagement();

	EnergyManagement(DeltaTime, currentVelocity);

	CameraManagement();

	DeadManagement();

	MetalScratchManagement();

	TimeManagement();
}

void AGameCharacter::SetupProjectile(FRotator rotator, FVector scale, UStaticMesh* mesh, UMaterialInterface* material, FVector offset) {

	ProjectilePositioner->SetupProjectile(rotator, scale, mesh, material, offset);
}

void AGameCharacter::DoAfterGettingHitFromProjectile(FVector direction) {
	if (actualStatus == EGameCharacterStatus::Dead) { return; }

	SetDieState();

	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	CameraMesh->AddImpulse(direction * 150.f);
	CameraMesh->SetConstraintMode(EDOFMode::Type::SixDOF);

	CameraMesh->AddAngularImpulseInRadians(direction * 1000.f);
}

/*
calling delegate, when death happened, setting velocity and state
*/
void AGameCharacter::DeadManagement() {
	FVector actorLocation = GetActorLocation();

	if (actualStatus == EGameCharacterStatus::Dead) { return; }

	if (actorLocation.Z <= heightToDie) {
		SetDieState();
	}
}

void AGameCharacter::SetDieState() {
	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	CameraMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	actualStatus = EGameCharacterStatus::Dead;

	OnDieHappenedDelegate.ExecuteIfBound();
}

void AGameCharacter::TimeManagement() {
	if (!canSlowdownTimeStarted) { return; }

	if (slowdownStatus == ESlowDownStatus::SlowDownTime) {
		float currentTime = GetWorld()->GetTimeSeconds() - slowdownStartTime;

		float percentage = currentTime / timeOfSlowDown;

		if (percentage > globalSettings->FullPercent)
			percentage = globalSettings->FullPercent;

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), FMath::Lerp(1.f, minSlowdownTimeMultiplier, percentage));
		actualSoundPitchMultiplier = globalSettings->FullPercent - percentage;
		AudioComp->SetPitchMultiplier(actualSoundPitchMultiplier);
		TongueAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		MetalScratchAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		DashAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		SneezeAudio->SetPitchMultiplier(actualSoundPitchMultiplier);

		if (percentage >= globalSettings->FullPercent) {
			slowdownStatus = ESlowDownStatus::StaySlowedDownTime;
			slowdownStartTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (slowdownStatus == ESlowDownStatus::StaySlowedDownTime) {
		float currentTime = GetWorld()->GetTimeSeconds() - slowdownStartTime;
		if (currentTime >= timeOfStaySlowedDown) {
			slowdownStatus = ESlowDownStatus::RestoreTime;
			slowdownStartTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (slowdownStatus == ESlowDownStatus::RestoreTime) {
		float currentTime = GetWorld()->GetTimeSeconds() - slowdownStartTime;

		float percentage = currentTime / timeOfSlowDown;

		if (percentage > globalSettings->FullPercent)
			percentage = globalSettings->FullPercent;

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), FMath::Lerp(minSlowdownTimeMultiplier, 1.f, percentage));
		actualSoundPitchMultiplier = percentage;
		AudioComp->SetPitchMultiplier(actualSoundPitchMultiplier);
		TongueAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		MetalScratchAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		DashAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		SneezeAudio->SetPitchMultiplier(actualSoundPitchMultiplier);

		if (percentage >= globalSettings->FullPercent) {
			slowdownStatus = ESlowDownStatus::NormalTime;
			canSlowdownTimeStarted = false;
		}
	}
}

/*
slowing down velocity of player, when it needs
*/
void AGameCharacter::VelocityManagement(FVector& currentVelocity) {
	if (actualStatus != EGameCharacterStatus::Dead &&
		actualStatus != EGameCharacterStatus::UpDash && actualStatus != EGameCharacterStatus::DownDash
		&& actualStatus != EGameCharacterStatus::LeftDash && actualStatus != EGameCharacterStatus::RightDash) {

		FVector clampedVelocity = currentVelocity.GetClampedToMaxSize(velocityLimit);
		CameraMesh->SetPhysicsLinearVelocity(clampedVelocity);
	}
}

/*
attack: when player draws sword
calm: when player is just flying around
dash: when player dashes upwards
*/
void AGameCharacter::StateManagement() {
	if (actualStatus == EGameCharacterStatus::Attack) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= attackTime) {
			Tongue->SetVisibility(false);
			prevStatus = actualStatus;
			actualStatus = EGameCharacterStatus::Calm;
		}
	}

	else if (actualStatus == EGameCharacterStatus::UpDash) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= dashCooldownTime) {
			actualStatus = EGameCharacterStatus::Calm;
		}
	}

	else if (actualStatus == EGameCharacterStatus::LeftDash) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= dashCooldownTime / 2.f) {
			actualStatus = EGameCharacterStatus::Calm;
			SpringArm->bInheritYaw = true;
			FRotator rotator = FRotator::ZeroRotator;
			SpringArm->SetRelativeRotation(rotator);
		}
	}

	else if (actualStatus == EGameCharacterStatus::RightDash) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= dashCooldownTime / 2.f) {
			actualStatus = EGameCharacterStatus::Calm;
			SpringArm->bInheritYaw = true;
			FRotator rotator = FRotator::ZeroRotator;
			SpringArm->SetRelativeRotation(rotator);
		}
	}

	else if (actualStatus == EGameCharacterStatus::DownDash) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= dashCooldownTime) {
			actualStatus = EGameCharacterStatus::Calm;
		}
	}
}

/*
energy management of player, based on current restmultiplier
*/
void AGameCharacter::EnergyManagement(float DeltaTime, FVector& currentVelocity) {
	float newEnergy = actualEnergy + energyRegeneration * DeltaTime;
	float restMult = 1.f;

	if (isHugging || (FMath::Abs(currentVelocity.X) < restVelocity && FMath::Abs(currentVelocity.Y) < restVelocity && FMath::Abs(currentVelocity.Z) < restVelocity)) {
		restMult = restingMultiplier;
	}

	if (newEnergy <= maxEnergy)
		actualEnergy += energyRegeneration * restMult * DeltaTime;
	else
		actualEnergy = maxEnergy;
}

/*
lerping camera between two spring arm lengths
*/
void AGameCharacter::CameraManagement() {
	float currentTimeForSpringArm = GetWorld()->GetTimeSeconds() - startTimeForSpringArm;
	currentTimeForSpringArm *= springArmLengthSpeed;
	if (currentTimeForSpringArm > globalSettings->FullPercent)
		currentTimeForSpringArm = globalSettings->FullPercent;

	SpringArm->TargetArmLength = FMath::Lerp(startArmLength, targetArmLength, currentTimeForSpringArm);
}

/*
when player sword (tongue) and boss enemy overlaps with each other, we find the intersecting points and placing spark particle to there
*/
void AGameCharacter::MetalScratchManagement() {
	FVector Actor1ClosestPoint, Actor2ClosestPoint, OverlapPoint;
	bool bOverlap = false;

	if (bossEnemy != nullptr) {

		FVector TempActor1ClosestPoint, TempActor2ClosestPoint;
		bool checkMe = GetOverlapInfluenceSphere(Tongue, Actor1ClosestPoint, Actor2ClosestPoint);
		if (checkMe)
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

		OverlapPoint = (Actor1ClosestPoint + Actor2ClosestPoint) / 2.0f;

		Spark->SetWorldLocation(Actor1ClosestPoint);
		Spark->Activate();

		if (MetalScratchAudio && !MetalScratchAudio->IsPlaying()) {

			// Get a random playback time
			float RandomTime = FMath::FRandRange(0.0f, 15.f);

			MetalScratchAudio->Play(RandomTime);
			UE_LOG(LogTemp, Warning, TEXT("bools: play"));
		}
	}
	else {

		Spark->Deactivate();
		MetalScratchAudio->Stop();
	}
}

/*
helper function for detecting the closest intersecting point between the closest static mesh components of two actors
*/
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


void AGameCharacter::SetPrevStatusToActualStatus() {
	prevStatus = actualStatus;
}

void AGameCharacter::SlowdownTime() {
	canSlowdownTimeStarted = true;
	slowdownStartTime = GetWorld()->GetTimeSeconds();
	slowdownStatus = ESlowDownStatus::SlowDownTime;
}


/*
subscribing to controls
*/
void AGameCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("RotateLR"), this, &AGameCharacter::RotateLR);

	PlayerInputComponent->BindAction(TEXT("WingBeat"), IE_Pressed, this, &AGameCharacter::WingBeat);

	PlayerInputComponent->BindAction(TEXT("HugCreature"), IE_Pressed, this, &AGameCharacter::HugCreature);

	PlayerInputComponent->BindAction(TEXT("Attack"), IE_Pressed, this, &AGameCharacter::Attack);

	PlayerInputComponent->BindAction(TEXT("Dash"), IE_Pressed, this, &AGameCharacter::Dash);

	FInputActionBinding& toggle = PlayerInputComponent->BindAction(TEXT("Pause"), IE_Pressed, this, &AGameCharacter::Pause);
	toggle.bExecuteWhenPaused = true;
}

USkeletalMeshComponent* AGameCharacter::GetSkeletalMeshComponent() const {
	return skeletal;
}

UStaticMeshComponent* AGameCharacter::GetStaticMeshComponent() const {
	return CameraMesh;
}

float AGameCharacter::GetCurrentSoundPitchMultiplier() const {
	return actualSoundPitchMultiplier;
}
/*
getter, setter functions
*/
float AGameCharacter::GetEnergy() const {
	return actualEnergy;
}

float AGameCharacter::GetMaxEnergy() const {
	return maxEnergy;
}

EGameCharacterStatus AGameCharacter::GetStatus() const {
	return actualStatus;
}

EGameCharacterStatus AGameCharacter::GetPrevStatus() const {
	return prevStatus;
}

FVector AGameCharacter::GetCameraLocation() const {
	return Camera->GetComponentLocation();
}

bool AGameCharacter::IsHugging() const {
	return isHugging;
}

FVector AGameCharacter::GetBackBeforeDashLocation() const {
	return positionBeforeDash;
}