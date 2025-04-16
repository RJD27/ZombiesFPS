// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainCharacter.h"
#include "Characters/ELoadoutSlot.h"
#include "Characters/LoadoutComponent.h"
#include "Characters/PlayerActionsComponent.h"
#include "Characters/StatsComponent.h"
#include "Combat/CombatComponent.h"
#include "Combat/TraceComponent.h"
#include "Combat/WeaponComponent.h"
#include "Pickups/WeaponMaster.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TraceComp = CreateDefaultSubobject<UTraceComponent>(TEXT("Trace Component"));
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	WeaponComp = CreateDefaultSubobject<UWeaponComponent>(TEXT("Weapon Component"));
	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	PlayerActionsComp = CreateDefaultSubobject<UPlayerActionsComponent>(TEXT("Player Actions Component"));
	LoadoutComp = CreateDefaultSubobject<ULoadoutComponent>(TEXT("Loadout Component"));

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMainCharacter::SetCurrentWeapon(AWeaponMaster* NewWeapon)
{
	CurrentWeapon = NewWeapon;

	if (CurrentWeapon)
	{
		FireMode = CurrentWeapon->FireMode;
		FireAnimation = CurrentWeapon->FireAnimation;
		ReloadAnimation = CurrentWeapon->ReloadAnimation;
		CurrentWeaponName = CurrentWeapon->WeaponName;
	}
	else
	{
		FireAnimation = nullptr;
		ReloadAnimation = nullptr;
		CurrentWeaponName = EWeaponName();
	}
}

TArray<AActor*>& AMainCharacter::GetNearbyInteractables()
{
	return  NearbyInteractables;
}

AWeaponMaster* AMainCharacter::GetCurrentWeapon() const
{
	return CurrentWeapon;
}

bool AMainCharacter::HasEnoughStamina(float Cost)
{
	return StatsComp->Stats[EStat::Stamina] >= Cost;
}

void AMainCharacter::EquipWeapon(ELoadoutSlot Slot)
{
	if (!LoadoutComp) return;
    
	AWeaponMaster* WeaponToEquip = LoadoutComp->GetWeapon(Slot);
    
	if (WeaponToEquip)
	{
		SetCurrentWeapon(WeaponToEquip);
	}
}


void AMainCharacter::SwapWeapon(ELoadoutSlot Slot)
{
	if (!LoadoutComp) return;

	if (!LoadoutComp->Loadout.Contains(Slot) || !LoadoutComp->Loadout[Slot])
	{
		UE_LOG(LogTemp, Warning, TEXT("Attempted to swap to empty slot %d"), static_cast<int32>(Slot));
		return;
	}
	EquipWeapon(Slot);
}