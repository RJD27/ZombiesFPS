// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MainCharacter.h"
#include "Components/ActorComponent.h"
#include "Interfaces/AmmoCarrier.h"
#include "Interfaces/Interact.h"
#include "PlayerActionsComponent.generated.h"

class UPlayMontageCallbackProxy;
class UTraceComponent;
class AWeaponPickup;
class AWeaponMaster;
class AMainCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponSpawned, AWeaponMaster*, NewWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIES_API UPlayerActionsComponent : public UActorComponent, public IInteract, public IAmmoCarrier
{
	GENERATED_BODY()
	
	TWeakObjectPtr<AMainCharacter> CharacterRef;
	
	class IMainPlayer* IPlayerRef;

	class UCharacterMovementComponent* MovementComp;

	class ULoadoutComponent* LoadoutComp;

	UPROPERTY(EditAnywhere)
	float SprintCost{ 0.1f };

	UPROPERTY(EditAnywhere)
	float SprintSpeed{ 1000.0f };

	UPROPERTY(EditAnywhere)
	float WalkSpeed{ 500.0f };
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* ReloadAnimMontage;
	
	UPROPERTY(EditAnywhere)
	UAnimMontage* FireAnimMontage;
	
	bool bIsFiring{ false };
	
	FTimerHandle ReloadTimerHandle;
	
	FTimerHandle FireTimerHandle;
	
	FVector GetSpawnLocation() const;

	UTraceComponent* GetTraceComponent();

	UPROPERTY()
	ELoadoutSlot CurrentActiveSlot;
	
public:
	UPlayerActionsComponent();

	UPROPERTY(BlueprintReadOnly)
	bool bIsReloadActive{ false };

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentMagAmmo{ 0 };

	UPROPERTY(BlueprintReadOnly)
	int32 CurrentAmmoReserve{ 0 };


protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void SpawnWeapon(AWeaponMaster* WeaponToSpawn);

	void HandleWeaponPickUp(AWeaponPickup* WeaponPickup);

	void AttachWeaponToSocket(AWeaponMaster* WeaponToAttach, const FName& SocketName) const;
	
	UFUNCTION(BlueprintCallable)
	virtual void Interact(AActor* InteractingActor) override;

	UFUNCTION(BlueprintCallable)
	void Reload();

	UFUNCTION()
	void FinishReload();

	UFUNCTION()
	void HandleFire();

	UFUNCTION(BlueprintCallable)
	void StartFire();

	UFUNCTION(BlueprintCallable)
	void StopFire();

	UFUNCTION(BlueprintCallable)
	void Sprint();

	UFUNCTION(BlueprintCallable)
	void Walk();

	UFUNCTION(BlueprintCallable)
	virtual int32 GetCurrentMagAmmo() const override { return CurrentMagAmmo; }

	UFUNCTION(BlueprintCallable)
	virtual int32 GetCurrentAmmoReserve() const override { return CurrentAmmoReserve; }
	
	virtual void ModifyMagAmmo(int32 Amount) override;
	
	virtual void ModifyReserveAmmo(int32 Amount) override;
	
	void InitializeAmmoFromWeapon(AWeaponMaster* WeaponMaster);
    
    UFUNCTION(BlueprintCallable)
	void SwapWeapon(ELoadoutSlot Slot);

	void AddAndEquip(ELoadoutSlot Slot, AWeaponMaster* Weapon);

	void EquipWeapon(ELoadoutSlot Slot);
};
