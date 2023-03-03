// Fill out your copyright notice in the Description page of Project Settings.


#include "FallCamera.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>

// Sets default values
AFallCamera::AFallCamera()
{
	PrimaryActorTick.bCanEverTick = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Camera->SetupAttachment(Mesh);
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
	if (gameCharacter != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("game char nem null"));

		gameCharacter->OnDieHappenedDelegate.BindUObject(this, &AFallCamera::DieHappened);
	}

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("dead userwidget1"));
		deaduserwidget = Cast<UDeadUserWidget>(CreateWidget(GetWorld(), widgetclass));
	}
}

/*
adding widget to viewport, when player dies
*/
void AFallCamera::DieHappened() 
{
	UE_LOG(LogTemp, Warning, TEXT("delegate !!!!"));


	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	SetActorLocation(gameCharacter->GetActorLocation());

	//Smoothly transition to our actor on begin play.
	OurPlayerController->SetViewTargetWithBlend(this, 0.5f);

	if (deaduserwidget != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("dead userwidget2"));
		UWidgetLayoutLibrary::RemoveAllWidgets(GetWorld());
		deaduserwidget->AddToViewport(0);

		OurPlayerController->bShowMouseCursor = true;
		OurPlayerController->bEnableClickEvents = true;
		OurPlayerController->bEnableMouseOverEvents = true;
	}
}

