// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Curves/CurveVector.h"
#include "GameFramework/Actor.h"

enum class EWeaponName : uint8;
enum class EWeaponType : uint8;
enum class EFireMode : uint8;
class AWeaponPickup;

#include "WeaponMaster.generated.h"

UCLASS()
class ZOMBIES_API AWeaponMaster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponMaster();

	UPROPERTY(EditAnywhere)
	float Damage{ 0.0f };

	UPROPERTY(EditAnywhere)
	float Penetration{ 0.0f };
	
	UPROPERTY(EditAnywhere)
	float Range{ 0.0f };

	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 MaxAmmoReserve{ 90 };

	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 MagazineSize{ 30 };

	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 StartingMagAmmo{ 30 };

	UPROPERTY(EditAnywhere, Category="Ammo")
	int32 StartingAmmoReserve{ 100 };

	UPROPERTY(VisibleAnywhere, Category="Ammo")
	int32 CurrentAmmoReserve{ StartingAmmoReserve };

	UPROPERTY(VisibleAnywhere, Category="Ammo")
	int32 CurrentMagAmmo{ StartingMagAmmo };

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName WeaponSocketName;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* WeaponModel;
	
	UPROPERTY(EditAnywhere)
	EWeaponType WeaponType{};

	UPROPERTY(EditAnywhere)
	EWeaponName WeaponName{};

	UPROPERTY(EditAnywhere)
	EFireMode FireMode;
	
	UPROPERTY(EditAnywhere)
	UCurveVector* RecoilPattern{};
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeaponPickup> PickupClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* FireAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon Animations")
	UAnimMontage* ReloadAnimation;
	
	static void Fire();

};
