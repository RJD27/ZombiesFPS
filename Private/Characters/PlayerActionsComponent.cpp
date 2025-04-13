// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerActionsComponent.h"
#include "Characters/MainCharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/MainPlayer.h"
#include "Pickups/WeaponMaster.h"
#include "Pickups/WeaponPickup.h"

FVector UPlayerActionsComponent::GetSpawnLocation() const
{
	if (!GetOwner()) 
	{
		return FVector::ZeroVector;
	}

	AActor* Owner = GetOwner();
	FVector ForwardVector = Owner->GetActorForwardVector();

	// Multiply each component separately
	FVector Offset(50.f, 50.f, 0.f);
	
	FVector AdjustedForward = FVector(
		ForwardVector.X * Offset.X,
		ForwardVector.Y * Offset.Y,
		ForwardVector.Z * Offset.Z
		);

	return Owner->GetActorLocation() + AdjustedForward;
}

UTraceComponent* UPlayerActionsComponent::GetTraceComponent()
{
	if (!CharacterRef.IsValid())
	{
		return nullptr;
	}

	AMainCharacter* Character = Cast<AMainCharacter>(CharacterRef.Get());
	if (Character)
	{
		return Character->FindComponentByClass<UTraceComponent>();
	}

	return nullptr;
}

UPlayerActionsComponent::UPlayerActionsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UPlayerActionsComponent::BeginPlay()
{
	Super::BeginPlay();
	
	CharacterRef = Cast<AMainCharacter>(GetOwner());
	MovementComp = CharacterRef->GetCharacterMovement();

	if (!CharacterRef->Implements<UMainPlayer>()) { return; }

	IPlayerRef = Cast<IMainPlayer>(CharacterRef);
}

// Called every frame
void UPlayerActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerActionsComponent::SpawnWeapon(AWeaponMaster* WeaponToSpawn)
{
    AMainCharacter* MainCharacter = Cast<AMainCharacter>(CharacterRef.Get());
	if (!MainCharacter || !WeaponToSpawn) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid MainCharacter or WeaponToSpawn"));
		return;
	}

    TSubclassOf<AWeaponPickup> WeaponPickupClass = WeaponToSpawn->PickupClass;

	if (!WeaponPickupClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon Pickup Class is invalid!"));
		return;
	}
	
    if (MainCharacter->CurrentWeapon)
    {
    	CharacterRef->CurrentWeapon->CurrentMagAmmo = CurrentMagAmmo;
    	CharacterRef->CurrentWeapon->CurrentAmmoReserve = CurrentAmmoReserve;
    	
        GetWorld()->SpawnActor<AWeaponPickup>(
            MainCharacter->CurrentWeapon->PickupClass,
            GetSpawnLocation(),
            FRotator::ZeroRotator
        );

        MainCharacter->CurrentWeapon->Destroy();
    }

    FVector DefaultSpawnLocation = GetSpawnLocation(); 
    AWeaponPickup* SpawnedWeaponPickup = GetWorld()->SpawnActor<AWeaponPickup>(
        WeaponToSpawn->PickupClass,
        DefaultSpawnLocation,
        FRotator::ZeroRotator
    );

    MainCharacter->CurrentWeapon = WeaponToSpawn;

	MainCharacter->CurrentWeapon->CurrentMagAmmo = WeaponToSpawn->StartingMagAmmo;
	MainCharacter->CurrentWeapon->CurrentAmmoReserve = WeaponToSpawn->StartingAmmoReserve;

    switch (WeaponToSpawn->WeaponName)
    {
        case EWeaponName::Pistol:
            break;
        
        case EWeaponName::AssaultRifle:
            break;
        
        case EWeaponName::Shotgun:
            break;
        
        default:
            break;
    }
	
    switch (WeaponToSpawn->WeaponType)
    {
        case EWeaponType::HitScan:
            break;
        
        case EWeaponType::Projectile:
            break;
        
        case EWeaponType::Melee:
            break;
        
        default:
            break;
    }

	if (WeaponToSpawn)
	{
		AttachWeaponToSocket(WeaponToSpawn, SpawnedWeaponPickup->WeaponSocketName);
		CurrentMagAmmo =  WeaponToSpawn->StartingMagAmmo;
		CurrentAmmoReserve = WeaponToSpawn->StartingAmmoReserve;
	}
}

void UPlayerActionsComponent::HandleWeaponPickUp(AWeaponPickup* WeaponPickup)
{
	if (!WeaponPickup || !CharacterRef.IsValid())
	{
		return;
	}

	AMainCharacter* PlayerCharacter = CharacterRef.Get();
	if (!PlayerCharacter)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Player is picking up a weapon"));
	
	AWeaponMaster* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponMaster>(
		WeaponPickup->WeaponToSpawn,
		PlayerCharacter->GetActorLocation(),
		FRotator::ZeroRotator
	);

	if (SpawnedWeapon)
	{
		if (PlayerCharacter->CurrentWeapon)
		{
			UE_LOG(LogTemp, Warning, TEXT("Destroying previous weapon: %s"), *PlayerCharacter->CurrentWeapon->GetName());
			PlayerCharacter->CurrentWeapon->Destroy();
		}

		WeaponPickup->SetActorHiddenInGame(true);

		AttachWeaponToSocket(SpawnedWeapon, WeaponPickup->WeaponSocketName);
		
		PlayerCharacter->SetCurrentWeapon(SpawnedWeapon);
		InitializeAmmoFromWeapon(SpawnedWeapon);
		UE_LOG(LogTemp, Warning, TEXT("Weapon attached to socket"));
		
		WeaponPickup->Destroy();
	}
}

void UPlayerActionsComponent::AttachWeaponToSocket(AWeaponMaster* WeaponToAttach, const FName& SocketName) const
{
	AMainCharacter* MainCharacter = Cast<AMainCharacter>(CharacterRef.Get());
	if (!MainCharacter || !WeaponToAttach) 
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid MainCharacter or WeaponToAttach"));
		return;
	}

	if (CharacterRef.IsValid())
	{
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
		WeaponToAttach->AttachToComponent(CharacterRef->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterRef is invalid, unable to attach weapon."));
	}
}

void UPlayerActionsComponent::Reload()
{
	if (CurrentMagAmmo == CharacterRef->CurrentWeapon->MagazineSize || CurrentAmmoReserve <= 0) { return; }
	
	if (bIsReloadActive || !IsValid(CharacterRef->CurrentWeapon) || !CharacterRef->ReloadAnimation)
	{
		return;
	}

	bIsReloadActive = true;

	float AnimDuration = CharacterRef->ReloadAnimation->GetPlayLength();

	CharacterRef->CurrentWeapon->WeaponModel->PlayAnimation(CharacterRef->ReloadAnimation, false);
	
	// Set a timer to end reloading after the animation duration
	GetWorld()->GetTimerManager().SetTimer(
		ReloadTimerHandle,
		this,
		&UPlayerActionsComponent::FinishReload,
		AnimDuration,
		false
	);
	int32 MagSize = CharacterRef->CurrentWeapon->MagazineSize;
	int32 MagDifference = MagSize - CurrentMagAmmo;
	int32 AmmoToReload = FMath::Min(MagDifference, CurrentAmmoReserve);
	
	ModifyMagAmmo(AmmoToReload);
	ModifyReserveAmmo(-AmmoToReload);
}

void UPlayerActionsComponent::Interact(AActor* InteractingActor)
{
	if (!CharacterRef.IsValid()) return;

	const TArray<AActor*>& NearbyInteractables = CharacterRef->GetNearbyInteractables();
	
	if (NearbyInteractables.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No interactables nearby."));
		return;
	}
	
	for (AActor* Interactable : NearbyInteractables)
	{
		if (IInteract* InteractableObject = Cast<IInteract>(Interactable))
		{
			UE_LOG(LogTemp, Warning, TEXT("Interacting with: %s"), *Interactable->GetName());
			InteractableObject->Interact(CharacterRef.Get());
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("No valid interactable found."));
}

void UPlayerActionsComponent::FinishReload()
{
	bIsReloadActive = false;
}

void UPlayerActionsComponent::HandleFire()
{
	if (CurrentMagAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No ammo left in mag."));
		StopFire();
		if (CurrentAmmoReserve > 0)
		{
			Reload();
		}
		return;
	}

	if (bIsReloadActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot fire while reloading."));
		return;
	}
	
	UTraceComponent* Trace = GetTraceComponent();
    
	if (CharacterRef->CurrentWeapon && CharacterRef->CurrentWeapon->WeaponModel && CharacterRef->FireAnimation)
	{
		ModifyMagAmmo(-1);
		CharacterRef->CurrentWeapon->WeaponModel->PlayAnimation(CharacterRef->FireAnimation, false);
		Trace->FireTrace();
	}
}

void UPlayerActionsComponent::StartFire()
{
	if (bIsFiring || bIsReloadActive) return;
	bIsFiring = true;

	HandleFire();
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UPlayerActionsComponent::HandleFire, 0.1f, true);
}

void UPlayerActionsComponent::StopFire()
{
	bIsFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UPlayerActionsComponent::Sprint()
{
	if (!IPlayerRef->HasEnoughStamina(SprintCost)){ return; }

	MovementComp->MaxWalkSpeed = SprintSpeed;
}

void UPlayerActionsComponent::Walk()
{
	MovementComp->MaxWalkSpeed = WalkSpeed;
}

void UPlayerActionsComponent::ModifyMagAmmo(int32 Amount)
{
	if (!CharacterRef.IsValid() || !CharacterRef->CurrentWeapon) return;

	const int32 MagSize = CharacterRef->CurrentWeapon->MagazineSize;
	CurrentMagAmmo = FMath::Clamp(CurrentMagAmmo + Amount, 0, MagSize);
	
	CharacterRef->CurrentWeapon->CurrentAmmoReserve = CurrentAmmoReserve;
	
}

void UPlayerActionsComponent::ModifyReserveAmmo(int32 Amount)
{
	if (!CharacterRef.IsValid() || !CharacterRef->CurrentWeapon) return;
	
	CurrentAmmoReserve = FMath::Max(CurrentAmmoReserve + Amount, 0);
	CharacterRef->CurrentWeapon->CurrentAmmoReserve = CurrentAmmoReserve;
}

void UPlayerActionsComponent::InitializeAmmoFromWeapon(AWeaponMaster* WeaponMaster)
{
	if (!WeaponMaster) return;

	CurrentMagAmmo = WeaponMaster->StartingMagAmmo;
	CurrentAmmoReserve = WeaponMaster->StartingAmmoReserve;
}
