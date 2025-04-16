#pragma once

#include "CoreMinimal.h"
#include "Combat/EWeaponType.h"
#include "Pickups/APickupMaster.h"

class AWeaponMaster;

#include "WeaponPickup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FWeaponPickedUp, AActor*, InteractingActor);

UCLASS()
class ZOMBIES_API AWeaponPickup : public APickupMaster
{
	GENERATED_BODY()

public:
	AWeaponPickup();

protected:
	virtual void BeginPlay() override;

	// Handles the weapon-specific interaction logic
	virtual void Interact(AActor* InteractingActor) override;

public:
	UPROPERTY()
	FWeaponPickedUp OnWeaponPickedUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AWeaponMaster> WeaponToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	EWeaponName WeaponName;
};
