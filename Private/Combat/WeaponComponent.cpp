// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/WeaponComponent.h"
#include "Interfaces/AmmoCarrier.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// WeaponIndex = 0; 
}

FString UWeaponComponent::GetWeaponName() const
{
	return UEnum::GetDisplayValueAsText(WeaponName).ToString();
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UWeaponComponent::AssaultRifle()
{
	TraceComponent = GetOwner()->FindComponentByClass<UTraceComponent>();
	GetWorld()->DeltaTimeSeconds;
}

void UWeaponComponent::CheckAmmo()
{
	AActor* CurrentActor = GetOwner();
	if (!CurrentActor) { return; }
	
	if (CurrentActor->Implements<UAmmoCarrier>())
	{
		if (IAmmoCarrier* AmmoInterface = Cast<IAmmoCarrier>(CurrentActor))
		{
			CurrentMagAmmo = AmmoInterface->GetCurrentMagAmmo();
			CurrentAmmoReserve = AmmoInterface->GetCurrentAmmoReserve();

			UE_LOG(LogTemp, Warning, TEXT("MagAmmo: %d, ReserveAmmo: %d"), CurrentMagAmmo, CurrentAmmoReserve);
		}
	}
}



