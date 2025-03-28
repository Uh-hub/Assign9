// Copyright Epic Games, Inc. All Rights Reserved.

#include "NetworkMiniGameGameMode.h"
#include "NetworkMiniGameCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANetworkMiniGameGameMode::ANetworkMiniGameGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
