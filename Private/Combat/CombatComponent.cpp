// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"

#include "Characters/MainCharacter.h"
#include "Combat/EFireMode.h"
#include "Pickups/WeaponMaster.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterRef = Cast<AMainCharacter>(GetOwner());
	if (!CharacterRef.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterRef is not valid!"));
	}
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AWeaponMaster* UCombatComponent::GetCurrentWeapon() const
{
	if (CharacterRef.IsValid())
	{
		return CharacterRef->CurrentWeapon;
	}
	
	return nullptr;
}


void UCombatComponent::Fire()
{
}

