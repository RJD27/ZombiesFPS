// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LoadoutComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIES_API ULoadoutComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY()
	TMap<ELoadoutSlot, AWeaponMaster*> Loadout;

public:	
	// Sets default values for this component's properties
	ULoadoutComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	void AddWeapon(ELoadoutSlot Slot, AWeaponMaster* Weapon);
	AWeaponMaster* GetWeapon(ELoadoutSlot Slot) const;
};
