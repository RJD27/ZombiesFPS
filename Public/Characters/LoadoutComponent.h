// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ELoadoutSlot.h"
#include "Combat/EWeaponType.h"
#include "Components/ActorComponent.h"
#include "LoadoutComponent.generated.h"


enum class ELoadoutSlot : uint8;
class AWeaponMaster;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIES_API ULoadoutComponent : public UActorComponent
{
	GENERATED_BODY()

	class AMainCharacter* CharacterRef;
	
	ELoadoutSlot CurrentActiveSlot;
	
public:	
	// Sets default values for this component's properties
	ULoadoutComponent();
	
	TMap<ELoadoutSlot, AWeaponMaster*> Loadout;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual bool IsSecondarySlotFree() const;

	virtual bool IsPrimarySlotFree() const;

	virtual bool HasSameWeapon(EWeaponName WeaponName) const;

	void AddToLoadout(ELoadoutSlot Slot, AWeaponMaster* Weapon);

	void RemoveFromLoadout(ELoadoutSlot Slot);

	bool SetCurrentActiveSlot(ELoadoutSlot NewSlot);

	UFUNCTION(BlueprintCallable)
	ELoadoutSlot GetCurrentActiveSlot() const{ return CurrentActiveSlot; }
	
	AWeaponMaster* GetWeapon(ELoadoutSlot Slot) const;


};
