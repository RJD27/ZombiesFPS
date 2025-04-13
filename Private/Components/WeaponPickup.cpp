#include "Pickups/WeaponPickup.h"
#include "Characters/MainCharacter.h"
#include "Characters/PlayerActionsComponent.h"

AWeaponPickup::AWeaponPickup()
{
    PrimaryActorTick.bCanEverTick = false;
}

void AWeaponPickup::BeginPlay()
{
    Super::BeginPlay();
}

void AWeaponPickup::Interact(AActor* InteractingActor)
{
    if (!bCanInteract || bWeaponAlreadyPickedUp)
    {
        return;
    }

    UE_LOG(LogTemp, Warning, TEXT("Weapon Pickup Interacted"));

    AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(InteractingActor);
    if (PlayerCharacter)
    {
        UPlayerActionsComponent* PlayerActions = PlayerCharacter->FindComponentByClass<UPlayerActionsComponent>();
        if (PlayerActions)
        {
            PlayerActions->HandleWeaponPickUp(this);
            bWeaponAlreadyPickedUp = true;
        }
    }
}
