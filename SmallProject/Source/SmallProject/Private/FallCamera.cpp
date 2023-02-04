// Fill out your copyright notice in the Description page of Project Settings.


#include "FallCamera.h"
#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>

// Sets default values
AFallCamera::AFallCamera()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Camera->SetupAttachment(Mesh);
}

// Called when the game starts or when spawned
void AFallCamera::BeginPlay()
{
	Super::BeginPlay();
	APlayerController* OurPlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	APawn* pawn = OurPlayerController->GetPawn();

	gameCharacter = Cast<AGameCharacter, APawn>(pawn);
	if (gameCharacter != nullptr) {
		UE_LOG(LogTemp, Warning, TEXT("game char nem null"));

		gameCharacter->OnScoreChangedDelegate.BindUObject(this, &AFallCamera::DieHappened);
	}

	if (IsValid(widgetclass)) {

		UE_LOG(LogTemp, Warning, TEXT("dead userwidget1"));
		deaduserwidget = Cast<UDeadUserWidget>(CreateWidget(GetWorld(), widgetclass));
	}
}

// Called every frame
void AFallCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

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

