// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/WeaponFire.h"
#include "CombatComponent.generated.h"


class AMainCharacter;
class AWeaponMaster;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIES_API UCombatComponent : public UActorComponent, public IWeaponFire
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TWeakObjectPtr<AMainCharacter> CharacterRef;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	AWeaponMaster* GetCurrentWeapon() const;
	
	UFUNCTION(BlueprintCallable)
	virtual void Fire() override;
};
