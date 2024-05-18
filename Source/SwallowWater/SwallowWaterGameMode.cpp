// Copyright Epic Games, Inc. All Rights Reserved.

#include "SwallowWaterGameMode.h"
#include "SwallowWaterCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASwallowWaterGameMode::ASwallowWaterGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
