// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/FallCamera.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include "Camera/CameraComponent.h"
#include <Allies/GameCharacter.h>
#include "Widgets/DeadUserWidget.h"
#include <Kismet/KismetMathLibrary.h>
#include "DataAssets/ResourceDataAsset.h"

// Sets default values
AFallCamera::AFallCamera(const FObjectInitializer& ObjectInitializer):Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Camera->SetupAttachment(Mesh);

	globalSettings = NewObject<UResourceDataAsset>(GetTransientPackage(), FName("globalSettings"));
}

/*
subscribing to gamecharacter's delegate, creating widget
*/
void AFallCamera::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	APawn* pawn = OurPlayerController->GetPawn();

	gameCharacter = Cast<AGameCharacter, APawn>(pawn);
	if (gameCharacter) {
		UE_LOG(LogTemp, Warning, TEXT("game char nem null"));

		gameCharacter->OnDieHappenedDelegate.BindUObject(this, &AFallCamera::DieHappened);
	}

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("dead userwidget1"));
		deaduserwidget = Cast<UDeadUserWidget>(CreateWidget(GetWorld(), widgetclass));
	}
}

void AFallCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!gameCharacter || gameCharacter->GetStatus()!=EGameCharacterStatus::Dead) { return; }

	FRotator targetRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), gameCharacter->GetActorLocation());

	Camera->SetRelativeRotation(targetRotation);

	float currentTime = GetWorld()->GetTimeSeconds() - startTime;

	currentTime /= showTime;

	if (currentTime >= globalSettings->FullPercent && !deaduserwidget->IsInViewport()) {
		deaduserwidget->AddToViewport(0);
	}
}

/*
adding widget to viewport, when player dies
*/
void AFallCamera::DieHappened() 
{
	UE_LOG(LogTemp, Warning, TEXT("delegate !!!!"));

	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	SetActorLocation(gameCharacter->GetCameraLocation());

	//Smoothly transition to our actor on begin play.
	OurPlayerController->SetViewTargetWithBlend(this, 0.5f);

	if (deaduserwidget) {
		UE_LOG(LogTemp, Warning, TEXT("dead userwidget2"));
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());

		OurPlayerController->bShowMouseCursor = true;
		OurPlayerController->bEnableClickEvents = true;
		OurPlayerController->bEnableMouseOverEvents = true;
	}

	startTime = GetWorld()->GetTimeSeconds();
}

