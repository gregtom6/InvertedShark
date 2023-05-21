// Copyright Epic Games, Inc. All Rights Reserved.


#include "SmallProjectGameModeBase.h"
#include "Public/Allies/GameCharacter.h"

ASmallProjectGameModeBase::ASmallProjectGameModeBase() {
	DefaultPawnClass = AGameCharacter::StaticClass();
}