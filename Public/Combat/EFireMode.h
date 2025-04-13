// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EFireMode.generated.h" 

UENUM(BlueprintType)
enum class EFireMode : uint8 
{
	FullAuto		UMETA(DisplayName = "Full Auto"),
	SemiAuto		UMETA(DisplayName = "Semi Auto")
};