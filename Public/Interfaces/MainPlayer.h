// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/WeaponMaster.h"
#include "UObject/Interface.h"
#include "MainPlayer.generated.h"

enum class ELoadoutSlot : uint8;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMainPlayer : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIES_API IMainPlayer
{
	GENERATED_BODY()

public:
	virtual bool HasEnoughStamina(float Cost){ return true; }
	virtual void EquipWeapon(ELoadoutSlot Slot) = 0;
	virtual void SwapWeapon(ELoadoutSlot Slot) = 0;
	
	virtual class ULoadoutComponent* GetLoadoutComponent() = 0;

};
