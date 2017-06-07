// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "InfomorphUE4.h"
#include "Modules/ModuleManager.h"
#include "Engine.h"

IMPLEMENT_PRIMARY_GAME_MODULE( FDefaultGameModuleImpl, InfomorphUE4, "InfomorphUE4" );

static const int32 DefaultKey = -1;
static const float DefaultDuration = 5.0f;
static const FColor DefaultColor = FColor::Red;

void LogOnScreen(const FString& Message)
{
	LogOnScreen(DefaultKey, DefaultDuration, DefaultColor, Message);
}

void LogOnScreen(int32 Key, const FString& Message)
{
	LogOnScreen(Key, DefaultDuration, DefaultColor, Message);
}

void LogOnScreen(float Duration, const FString& Message)
{
	LogOnScreen(DefaultKey, Duration, DefaultColor, Message);
}

void LogOnScreen(const FColor& Color, const FString& Message)
{
	LogOnScreen(DefaultKey, DefaultDuration, Color, Message);
}

void LogOnScreen(int32 Key, const FColor& Color, const FString& Message)
{
	LogOnScreen(Key, DefaultDuration, Color, Message);
}

void LogOnScreen(float Duration, const FColor& Color, const FString& Message)
{
	LogOnScreen(DefaultKey, Duration, Color, Message);
}

void LogOnScreen(int32 Key, float Duration, const FString& Message)
{
	LogOnScreen(Key, Duration, DefaultColor, Message);
}

void LogOnScreen(int32 Key, float Duration, const FColor& Color, const FString& Message)
{
	if(GEngine != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(Key, Duration, Color, Message);
	}
}