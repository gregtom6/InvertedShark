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


// Sets default values
AGameCharacter::AGameCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));

	CameraMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CameraMesh"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SetRootComponent(CameraMesh);

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
}

/*
left right rotations of game character
*/
void AGameCharacter::RotateLR(float rotateDelta) {

	if (isHugging || actualStatus == GameCharacterStatus::Dead) { return; }

	FRotator actualRotation = GetActorRotation();
	actualRotation.Yaw += rotateDelta * RotateSpeed;
	CameraMesh-> SetRelativeRotation(actualRotation);
}


/*
wing beat movement of game character
*/
void AGameCharacter::WingBeat() {

	if (isHugging || actualStatus == GameCharacterStatus::Dead || actualStatus == GameCharacterStatus::UpDash) { return; }

	if (actualEnergy < energyDecreaseAfterWingBeat) { return; }

	FVector actorForwardVector = GetActorForwardVector() * ForwardFlyStrength;
	FVector actorUpVector = GetActorUpVector() * WingStrength;
	FVector impulseDirection = actorUpVector + actorForwardVector;

	CameraMesh->AddImpulse(impulseDirection);

	actualEnergy -= energyDecreaseAfterWingBeat;

	if (AudioComp && wingBeat) {
		AudioComp->Play(0.f);
	}

	wingPlayer->Play();
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

	if (actualStatus == GameCharacterStatus::Attack || isHugging || actualStatus == GameCharacterStatus::Dead || actualStatus == GameCharacterStatus::UpDash) { return; }

	UE_LOG(LogTemp, Warning, TEXT("attack happened"));

	Tongue->SetVisibility(true);
	startTime = GetWorld()->GetTimeSeconds();
	prevStatus = actualStatus;
	actualStatus = GameCharacterStatus::Attack;

	TongueAudio->Play(0.f);
}

/*
dash ability
*/
void AGameCharacter::Dash() {
	if (isHugging || actualStatus == GameCharacterStatus::Dead || actualStatus == GameCharacterStatus::UpDash || actualStatus == GameCharacterStatus::Attack) { return; }


	APlayerController* thisCont = GetWorld()->GetFirstPlayerController();
	if (thisCont->IsInputKeyDown(EKeys::S)) {
		actualStatus = GameCharacterStatus::DownDash;
		DownDash();
	}
	else if (thisCont->IsInputKeyDown(EKeys::Q)) {
		actualStatus = GameCharacterStatus::LeftDash;
		LeftDash();
	}
	else if (thisCont->IsInputKeyDown(EKeys::E)) {
		actualStatus = GameCharacterStatus::RightDash;
		RightDash();
	}
	else {
		actualStatus = GameCharacterStatus::UpDash;
		UpDash();
	}
}

void AGameCharacter::DownDash() {

	FVector actorDownVector = -GetActorUpVector() * dashStrength;
	FVector impulseDirection = actorDownVector;

	positionBeforeDash = GetActorLocation();

	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	CameraMesh->AddImpulse(impulseDirection);

	startTime = GetWorld()->GetTimeSeconds();
}

void AGameCharacter::LeftDash() {
	FVector actorUpVector = (GetActorUpVector() + -GetActorRightVector()) * (dashStrength/2.f);
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
	FVector actorUpVector = (GetActorUpVector() + GetActorRightVector()) * (dashStrength/2.f);
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
	FVector actorUpVector = GetActorUpVector() * dashStrength;
	FVector impulseDirection = actorUpVector;

	positionBeforeDash = GetActorLocation();

	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	CameraMesh->AddImpulse(impulseDirection);

	actualEnergy -= energyDecreaseAfterDash;
	if (actualEnergy < 0.f)
		actualEnergy = 0.f;

	startTime = GetWorld()->GetTimeSeconds();

	if (DashAudio && dashSound) {
		DashAudio->Play(0.f);
	}

	if (SneezeAudio && sneezeSound) {
		SneezeAudio->Play(0.f);
	}

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

	//FFrameTime FirstFrameTime(0);
	//C:\WORK\Sajat\SmallProject\SmallProject\Source\SmallProject\Private\GameCharacter.cpp(274): warning C4996: 'UMovieSceneSequencePlayer::JumpToFrame': JumpToFrame is deprecated, use SetPlaybackPosition. Please update your code to the new API before upgrading to the next release, otherwise your project will no longer compile.
	//loopedEyePlayer->JumpToFrame(FirstFrameTime);
	FMovieSceneSequencePlaybackParams FirstFrame(0.f, EUpdatePositionMethod::Play);
	loopedEyePlayer->SetPlaybackPosition(FirstFrame);
	loopedEyePlayer->Stop();
	sneezeBlinkPlayer->Play();

	leftNoseSneezeNiagara->Activate(true);
	rightNoseSneezeNiagara->Activate(true);
}

FVector AGameCharacter::GetBackBeforeDashLocation() {
	return positionBeforeDash;
}

void AGameCharacter::NotifyTargeting(bool iAmTargeted) {
	amITargeted = iAmTargeted;
}

USkeletalMeshComponent* AGameCharacter::GetSkeletalMeshComponent() {
	return skeletal;
}

UStaticMeshComponent* AGameCharacter::GetStaticMeshComponent() {
	return CameraMesh;
}

/*
game can be paused with this function. Managing cursor state and adding or removing pause widget to viewport
*/
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

		UE_LOG(LogTemp, Warning, TEXT("pause activate3"));

		InitializePause();

		if (widgetPauseMenuInstance != nullptr) {

			widgetPauseMenuInstance->AddToViewport();
		}

		UE_LOG(LogTemp, Warning, TEXT("pause activate4"));
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

		widgetPauseMenuInstance->RemoveFromParent();
	}
}

/*
initializing audios, pause widget, energy, status, position, etc.
*/
void AGameCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AudioComp && wingBeat) {
		AudioComp->SetSound(wingBeat);
	}

	if (TongueAudio && tongueSound) {
		TongueAudio->SetSound(tongueSound);
	}

	InitializePause();

	startPos = GetActorLocation();

	Tongue->SetVisibility(false);

	startArmLength = defaultArmLength;
	targetArmLength = defaultArmLength;

	actualEnergy = maxEnergy;

	actualStatus = GameCharacterStatus::Calm;
	prevStatus = GameCharacterStatus::Calm;

	pauseStatus = PauseStatus::Played;

	slowdownStatus = SlowDownStatus::NormalTime;

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

void AGameCharacter::SetupProjectile(FRotator rotator, FVector scale, UStaticMesh* mesh, UMaterialInterface* material, FVector offset, FVector direction) {

	UStaticMeshComponent* NewComponent = NewObject<UStaticMeshComponent>(this);
	NewComponent->RegisterComponent();
	NewComponent->SetStaticMesh(mesh);
	NewComponent->AttachToComponent(CameraMesh, FAttachmentTransformRules::KeepRelativeTransform);
	FVector newScale = scale *1.25f;
	FVector nScale = newScale*1.25f;
	NewComponent->SetRelativeScale3D(nScale);

	NewComponent->SetWorldLocation(offset);

	NewComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	NewComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	NewComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	NewComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);

	NewComponent->SetRelativeRotation(rotator);
	NewComponent->SetMaterial(0, material);

	FName NewName = MakeUniqueObjectName(this, UStaticMeshComponent::StaticClass(), TEXT("InnerProjectile"));
	NewComponent->Rename(*NewName.ToString());

	if (actualStatus == GameCharacterStatus::Dead) { return; }

	SetDieState();

	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);

	CameraMesh->AddImpulse(direction*150.f);
	CameraMesh->SetConstraintMode(EDOFMode::Type::SixDOF);

	CameraMesh->AddAngularImpulseInRadians(direction*1000.f);
}

/*
calling delegate, when death happened, setting velocity and state
*/
void AGameCharacter::DeadManagement() {
	FVector actorLocation = GetActorLocation();

	if (actualStatus == GameCharacterStatus::Dead) { return; }

	if (actorLocation.Z <= heightToDie) {
		SetDieState();
	}
}

FVector AGameCharacter::GetCameraLocation() {
	return Camera->GetComponentLocation();
}

void AGameCharacter::SetDieState() {
	CameraMesh->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	CameraMesh->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	actualStatus = GameCharacterStatus::Dead;

	OnDieHappenedDelegate.ExecuteIfBound();
}

void AGameCharacter::TimeManagement() {
	if (!canSlowdownTimeStarted) { return; }

	if (slowdownStatus == SlowDownStatus::SlowDownTime) {
		float currentTime = GetWorld()->GetTimeSeconds() - slowdownStartTime;

		float percentage = currentTime / timeOfSlowDown;

		if (percentage > 1.f)
			percentage = 1.f;

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), FMath::Lerp(1.f, minSlowdownTimeMultiplier, percentage));
		actualSoundPitchMultiplier = 1.f - percentage;
		AudioComp->SetPitchMultiplier(actualSoundPitchMultiplier);
		TongueAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		MetalScratchAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		DashAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		SneezeAudio->SetPitchMultiplier(actualSoundPitchMultiplier);

		if (percentage >= 1.f) {
			slowdownStatus = SlowDownStatus::StaySlowedDownTime;
			slowdownStartTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (slowdownStatus == SlowDownStatus::StaySlowedDownTime) {
		float currentTime = GetWorld()->GetTimeSeconds() - slowdownStartTime;
		if (currentTime >= timeOfStaySlowedDown) {
			slowdownStatus = SlowDownStatus::RestoreTime;
			slowdownStartTime = GetWorld()->GetTimeSeconds();
		}
	}
	else if (slowdownStatus == SlowDownStatus::RestoreTime) {
		float currentTime = GetWorld()->GetTimeSeconds() - slowdownStartTime;

		float percentage = currentTime / timeOfSlowDown;

		if (percentage > 1.f)
			percentage = 1.f;

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), FMath::Lerp(minSlowdownTimeMultiplier, 1.f, percentage));
		actualSoundPitchMultiplier = percentage;
		AudioComp->SetPitchMultiplier(actualSoundPitchMultiplier);
		TongueAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		MetalScratchAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		DashAudio->SetPitchMultiplier(actualSoundPitchMultiplier);
		SneezeAudio->SetPitchMultiplier(actualSoundPitchMultiplier);

		if (percentage >= 1.f) {
			slowdownStatus = SlowDownStatus::NormalTime;
			canSlowdownTimeStarted = false;
		}
	}
}

/*
slowing down velocity of player, when it needs
*/
void AGameCharacter::VelocityManagement(FVector& currentVelocity) {
	if (actualStatus != GameCharacterStatus::Dead &&
		actualStatus != GameCharacterStatus::UpDash && actualStatus != GameCharacterStatus::DownDash
		&& actualStatus != GameCharacterStatus::LeftDash && actualStatus != GameCharacterStatus::RightDash) {
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
	if (actualStatus == GameCharacterStatus::Attack) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= attackTime) {
			Tongue->SetVisibility(false);
			prevStatus = actualStatus;
			actualStatus = GameCharacterStatus::Calm;
		}
	}

	else if (actualStatus == GameCharacterStatus::UpDash) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= dashCooldownTime) {
			actualStatus = GameCharacterStatus::Calm;
		}
	}

	else if (actualStatus == GameCharacterStatus::LeftDash) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= dashCooldownTime/2.f) {
			actualStatus = GameCharacterStatus::Calm;
			SpringArm->bInheritYaw = true;
			FRotator rotator = FRotator::ZeroRotator;
			SpringArm->SetRelativeRotation(rotator);
		}
	}

	else if (actualStatus == GameCharacterStatus::RightDash) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= dashCooldownTime/2.f) {
			actualStatus = GameCharacterStatus::Calm;
			SpringArm->bInheritYaw = true;
			FRotator rotator = FRotator::ZeroRotator;
			SpringArm->SetRelativeRotation(rotator);
		}
	}

	else if (actualStatus == GameCharacterStatus::DownDash) {

		float currentTime = GetWorld()->GetTimeSeconds() - startTime;

		if (currentTime >= dashCooldownTime) {
			actualStatus = GameCharacterStatus::Calm;
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
	if (currentTimeForSpringArm > 1.f)
		currentTimeForSpringArm = 1.f;

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

		if (MetalScratchAudio && metalScratchSound && !MetalScratchAudio->IsPlaying()) {

			// Get a random playback time
			float RandomTime = FMath::FRandRange(0.0f, 15.f);

			MetalScratchAudio->Play(RandomTime);
			UE_LOG(LogTemp, Warning, TEXT("bools: play"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("bools: %f  %f"), bOverlap, Tongue->GetVisibleFlag());

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

/*
getter, setter functions
*/
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

void AGameCharacter::SlowdownTime() {
	canSlowdownTimeStarted = true;
	slowdownStartTime = GetWorld()->GetTimeSeconds();
	slowdownStatus = SlowDownStatus::SlowDownTime;
}

float AGameCharacter::GetCurrentSoundPitchMultiplier() {
	return actualSoundPitchMultiplier;
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

