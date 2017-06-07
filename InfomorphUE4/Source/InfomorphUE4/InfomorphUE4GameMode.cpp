// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "InfomorphUE4GameMode.h"
#include "InfomorphUE4Character.h"
#include "UObject/ConstructorHelpers.h"

AInfomorphUE4GameMode::AInfomorphUE4GameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
