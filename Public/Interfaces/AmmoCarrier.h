// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AmmoCarrier.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAmmoCarrier : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ZOMBIES_API IAmmoCarrier
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetCurrentMagAmmo() const = 0;
	virtual int32 GetCurrentAmmoReserve() const = 0;
	
	virtual void ModifyMagAmmo(int32 Amount) = 0;
	virtual void ModifyReserveAmmo(int32 Amount) = 0;
};
