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
	
public:	
	// Sets default values for this component's properties
	ULoadoutComponent();

	UPROPERTY(BlueprintReadOnly)
	TMap<ELoadoutSlot, AWeaponMaster*> Loadout;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual bool IsSecondarySlotFree() const;

	virtual bool HasSameWeapon(EWeaponName WeaponName) const;

	void AddToLoadout(ELoadoutSlot Slot, AWeaponMaster* Weapon);

	void RemoveFromLoadout(ELoadoutSlot Slot);
	
	AWeaponMaster* GetWeapon(ELoadoutSlot Slot) const;


};
