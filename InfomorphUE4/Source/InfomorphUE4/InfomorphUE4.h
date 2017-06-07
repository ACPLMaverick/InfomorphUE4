// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core.h"

void LogOnScreen(const FString& Message);
void LogOnScreen(int32 Key, const FString& Message);
void LogOnScreen(float Duration, const FString& Message);
void LogOnScreen(const FColor& Color, const FString& Message);
void LogOnScreen(int32 Key, const FColor& Color, const FString& Message);
void LogOnScreen(float Duration, const FColor& Color, const FString& Message);
void LogOnScreen(int32 Key, float Duration, const FString& Message);
void LogOnScreen(int32 Key, float Duration, const FColor& Color, const FString& Message);
