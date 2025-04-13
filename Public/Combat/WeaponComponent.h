// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TraceComponent.h"
#include "Components/ActorComponent.h"
#include "EWeaponType.h"
#include "WeaponComponent.generated.h"


class AWeaponMaster;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ZOMBIES_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	double TraceRate{ 0.0 };


public:	
	// Sets default values for this component's properties
	UWeaponComponent();

	UFUNCTION(BlueprintPure, Category="Weapons")
	FString GetWeaponName() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<USkeletalMesh*> WeaponMeshes;
	
	UPROPERTY(VisibleAnywhere, Category="Ammo")
	int CurrentMagAmmo{ 0 };

	UPROPERTY(EditAnywhere, Category="Ammo")
	int CurrentAmmoReserve{ 0 };

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UTraceComponent* TraceComponent;

	UPROPERTY(EditAnywhere, Category="Weapon")
	EWeaponName WeaponName;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void AssaultRifle();

	void CheckAmmo();

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	// int32 WeaponIndex;
	//
	// UFUNCTION(BlueprintCallable, Category = "Weapon")
	// void SetWeaponIndex(int32 NewIndex);
	//
	// UFUNCTION(BlueprintCallable, Category = "Weapon")
	// int32 GetWeaponIndex() const { return WeaponIndex; }

	// UFUNCTION()
	// void GetWeaponType();
	//
	// UFUNCTION()
	// void SetWeaponType();
	//
	// UFUNCTION()
	// void SetWeapon();
	
};
