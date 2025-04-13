// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LoadoutComponent.h"

// Sets default values for this component's properties
ULoadoutComponent::ULoadoutComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void ULoadoutComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void ULoadoutComponent::AddWeapon(ELoadoutSlot Slot, AWeaponMaster* Weapon)
{
	if (!Weapon){ return; }
	Loadout.Add(Slot, Weapon);
}

AWeaponMaster* ULoadoutComponent::GetWeapon(ELoadoutSlot Slot) const
{
	AWeaponMaster* const* FoundWeapon = Loadout.Find(Slot);
	return FoundWeapon ? *FoundWeapon : nullptr;
}
