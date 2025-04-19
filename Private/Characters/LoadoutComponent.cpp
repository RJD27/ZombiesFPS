// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LoadoutComponent.h"
#include "Characters/ELoadoutSlot.h"
#include "Characters/MainCharacter.h"

// Sets default values for this component's properties
ULoadoutComponent::ULoadoutComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void ULoadoutComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = Cast<AMainCharacter>(GetOwner());
	if (!CharacterRef)
	{
		UE_LOG(LogTemp, Warning, TEXT("ULoadoutComponent: Failed to get owner as AMainCharacter."));
	}
}

bool ULoadoutComponent::IsSecondarySlotFree() const
{
	return !Loadout.Contains(ELoadoutSlot::Secondary)
		|| Loadout[ELoadoutSlot::Secondary] == nullptr;
}

bool ULoadoutComponent::IsPrimarySlotFree() const
{
	return !Loadout.Contains(ELoadoutSlot::Primary)
		|| Loadout[ELoadoutSlot::Primary] == nullptr;
}

bool ULoadoutComponent::HasSameWeapon(EWeaponName WeaponName) const
{
	if (Loadout.Contains(ELoadoutSlot::Primary))
	{
		AWeaponMaster* PrimaryWeapon = Loadout[ELoadoutSlot::Primary];
		if (PrimaryWeapon && PrimaryWeapon->WeaponName == WeaponName)
		{
			return true;
		}
	}

	if (Loadout.Contains(ELoadoutSlot::Secondary))
	{
		AWeaponMaster* SecondaryWeapon = Loadout[ELoadoutSlot::Secondary];
		if (SecondaryWeapon && SecondaryWeapon->WeaponName == WeaponName)
		{
			return true;
		}
	}
	return false;
}

void ULoadoutComponent::AddToLoadout(ELoadoutSlot Slot, AWeaponMaster* Weapon)
{
	if (!Weapon || !CharacterRef) return;

	if (Loadout.Contains(Slot))
	{
		AWeaponMaster* OldWeapon = Loadout[Slot];
		if (OldWeapon != Weapon)
		{
			RemoveFromLoadout(Slot);
		}
	}

	Loadout.Emplace(Slot, Weapon);

	UE_LOG(LogTemp, Log, TEXT("Weapon %s added to slot %d"),
		*Weapon->GetName(), static_cast<int32>(Slot));
}

void ULoadoutComponent::RemoveFromLoadout(ELoadoutSlot Slot)
{
	if (!Loadout.Contains(Slot) || !Loadout[Slot]) { return; }
       
	AWeaponMaster* WeaponToRemove = Loadout[Slot];
    
	Loadout.Remove(Slot);
    
	if (WeaponToRemove)
	{
		WeaponToRemove->Destroy();
	}
    
	UE_LOG(LogTemp, Log, TEXT("Removed weapon from slot %d"), static_cast<int32>(Slot));
}

bool ULoadoutComponent::SetCurrentActiveSlot(ELoadoutSlot NewSlot)
{
	if (!Loadout.Contains(NewSlot) || !Loadout[NewSlot])
	{
		return false;
	}
    
	CurrentActiveSlot = NewSlot;
    
	AWeaponMaster* NewWeapon = Loadout[NewSlot];
    
	if (IsValid(CharacterRef))
	{
		CharacterRef->SetCurrentWeapon(NewWeapon);
	}
	return true;
	
}

AWeaponMaster* ULoadoutComponent::GetWeapon(ELoadoutSlot Slot) const
{
	return Loadout.Contains(Slot) ? Loadout[Slot] : nullptr;
}

