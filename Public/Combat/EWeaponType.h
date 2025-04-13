// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EWeaponType.generated.h" 

UENUM(BlueprintType)
enum class EWeaponType : uint8 
{
	HitScan			UMETA(DisplayName = "Hit Scan"),
	Projectile		UMETA(DisplayName = "Projectile"),
	Melee			UMETA(DisplayName = "Melee")
};

UENUM(BlueprintType)
enum class EWeaponName : uint8 
{
	Pistol			UMETA(DisplayName = "Pistol"),
	AssaultRifle	UMETA(DisplayName = "Assault Rifle"),
	Shotgun			UMETA(DisplayName = "Shotgun")
};
