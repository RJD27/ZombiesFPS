// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Combat/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Combat/WeaponComponent.h"
#include "Interfaces/MainPlayer.h"
#include "Pickups/WeaponMaster.h"
#include "MainCharacter.generated.h"

class ULoadoutComponent;
class UPlayerActionsComponent;

UCLASS()
class ZOMBIES_API AMainCharacter : public ACharacter, public IMainPlayer
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMainCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStatsComponent* StatsComp;
	
	TArray<AActor*>& GetNearbyInteractables();
	TArray<AActor*> NearbyInteractables;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTraceComponent* TraceComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCombatComponent* CombatComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UWeaponComponent* WeaponComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPlayerActionsComponent* PlayerActionsComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ULoadoutComponent* LoadoutComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponName CurrentWeaponName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AWeaponMaster* CurrentWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EFireMode FireMode;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAnimMontage* FireAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAnimMontage* ReloadAnimation;

	void SetCurrentWeapon(AWeaponMaster* NewWeapon);
	AWeaponMaster* GetCurrentWeapon() const;

	virtual bool HasEnoughStamina(float Cost) override;

	virtual void EquipWeapon(ELoadoutSlot Slot) override;

	virtual void SwapWeapon(ELoadoutSlot Slot) override;
	
};
