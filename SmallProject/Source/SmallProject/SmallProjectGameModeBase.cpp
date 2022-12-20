// Copyright Epic Games, Inc. All Rights Reserved.


#include "SmallProjectGameModeBase.h"
#include "Public/GameCharacter.h"

ASmallProjectGameModeBase::ASmallProjectGameModeBase() {
	DefaultPawnClass = AGameCharacter::StaticClass();
}