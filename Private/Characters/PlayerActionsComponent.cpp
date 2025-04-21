// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerActionsComponent.h"
#include "Characters/ELoadoutSlot.h"
#include "Characters/LoadoutComponent.h"
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

UTraceComponent* UPlayerActionsComponent::GetTraceComponent() const
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

	if (CharacterRef.IsValid())
	{
		LoadoutComp = CharacterRef->FindComponentByClass<ULoadoutComponent>();
        
		// Log whether we found the component or not
		if (!LoadoutComp)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find LoadoutComponent on character %s"), *GetOwner()->GetName());
		}
	}
	CurrentSlot = LoadoutComp->GetCurrentActiveSlot();
}

// Called every frame
void UPlayerActionsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UPlayerActionsComponent::HandleWeaponPickUp(AWeaponPickup* WeaponPickup)
{
	if (!WeaponPickup || !LoadoutComp)
	{
		UE_LOG(LogTemp, Error, TEXT("HandleWeaponPickUp failed: WeaponPickup or LoadoutComp is null"));
		return;
	}
	
	if (LoadoutComp->HasSameWeapon(WeaponPickup->WeaponName))
	{
		CurrentAmmoReserve += 20;
		WeaponPickup->Destroy();
		return;
	}
    
	AWeaponMaster* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponMaster>(
		WeaponPickup->WeaponToSpawn,
		CharacterRef->GetActorLocation(),
		FRotator::ZeroRotator
	);
    
	if (!SpawnedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to spawn weapon"));
		return;
	}
    
	WeaponPickup->SetActorHiddenInGame(true);
    
	ELoadoutSlot TargetSlot = CurrentSlot;
    
	if (LoadoutComp->IsSecondarySlotFree())
	{
		TargetSlot = ELoadoutSlot::Secondary;
	}
	else if (LoadoutComp->IsPrimarySlotFree())
	{
		TargetSlot = ELoadoutSlot::Primary;
	}
	
	LoadoutComp->AddToLoadout(TargetSlot, SpawnedWeapon);
    
	EquipWeapon(TargetSlot);
	WeaponPickup->Destroy();
    
	UE_LOG(LogTemp, Log, TEXT("Weapon %s successfully picked up and equipped in slot %d."), 
		*SpawnedWeapon->GetName(), static_cast<int32>(TargetSlot));
}

void UPlayerActionsComponent::AttachWeaponToSocket(AWeaponMaster* WeaponToAttach, const FName& SocketName) const
{
	
	// Check if the weapon and socket name are valid
	if (!WeaponToAttach || SocketName.IsNone())
	{
		UE_LOG(LogTemp, Error, TEXT("AttachWeaponToSocket failed: WeaponToAttach or SocketName is null."));
		return;
	}

	if (CharacterRef.IsValid())
	{
        	WeaponToAttach->AttachToComponent(CharacterRef->GetMesh(),
        		FAttachmentTransformRules::SnapToTargetIncludingScale, SocketName);
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
	if (bIsFiring || bIsReloadActive || !IsValid(CharacterRef->CurrentWeapon)) return;
	bIsFiring = true;

	HandleFire();
	GetWorld()->GetTimerManager().SetTimer(FireTimerHandle, this, &UPlayerActionsComponent::HandleFire, 0.1f, true);
}

void UPlayerActionsComponent::StopFire()
{
	bIsFiring = false;
	GetWorld()->GetTimerManager().ClearTimer(FireTimerHandle);
}

void UPlayerActionsComponent::Sprint() const
{
	if (!IPlayerRef->HasEnoughStamina(SprintCost)){ return; }

	MovementComp->MaxWalkSpeed = SprintSpeed;
}

void UPlayerActionsComponent::Walk() const
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

void UPlayerActionsComponent::SwitchWeaponSlot(ELoadoutSlot Slot)
{
	if (!LoadoutComp->Loadout.Contains(Slot) || !LoadoutComp->Loadout[Slot])
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon found in Loadout Slot %d."), static_cast<uint8>(Slot));
		return;
	}

	if (bIsSwapping)
	{
		UE_LOG(LogTemp, Warning, TEXT("Already swapping weapons, cannot swap now."));
		return;
	}

	bIsSwapping = true;
	AWeaponMaster* NewWeapon = LoadoutComp->Loadout[Slot];

	USkeletalMeshComponent* CurrentWeaponMesh = CharacterRef->CurrentWeapon
		? CharacterRef->CurrentWeapon->WeaponModel : nullptr;
	
    if (LoadoutComp->Loadout.Contains(Slot) && NewWeapon)
    {
        // Unequip the current weapon
    	if (CharacterRef->CurrentWeapon)
    	{
    		CharacterRef->CurrentWeapon->CurrentMagAmmo = CurrentMagAmmo;
    		CharacterRef->CurrentWeapon->CurrentAmmoReserve = CurrentAmmoReserve;
    		CurrentWeaponMesh->SetVisibility(false);
    	}
        	
    	CharacterRef->CurrentWeapon = NewWeapon;

    	CurrentMagAmmo = NewWeapon->CurrentMagAmmo;
    	CurrentAmmoReserve = NewWeapon->CurrentAmmoReserve;

    	if (!NewWeapon->WeaponModel)
    	{
    		UE_LOG(LogTemp, Error, TEXT("Weapon model is null for %s"), *NewWeapon->GetName());
    		bIsSwapping = false;
    		return;
    	}

    	USkeletalMeshComponent* NewWeaponMesh = NewWeapon->WeaponModel;

    	AttachWeaponToSocket(NewWeapon, NewWeapon->WeaponSocketName);

    	UE_LOG(LogTemp, Warning, TEXT("Setting new weapon %s to visible"), *NewWeapon->GetName());

    	NewWeaponMesh->SetVisibility(true);

    	LoadoutComp->SetCurrentActiveSlot(Slot);
    	CurrentSlot = Slot;
        	
    	UE_LOG(LogTemp, Warning, TEXT("Current Active Slot: %d %hhd"),
    		static_cast<int32>(CurrentSlot),
    		CharacterRef->CurrentWeapon->WeaponName);
        }
	bIsSwapping = false;
}

void UPlayerActionsComponent::EquipWeapon(const ELoadoutSlot Slot)
{
	if (LoadoutComp->Loadout.Contains(Slot) && LoadoutComp->Loadout[Slot])
	{
		AWeaponMaster* NewWeapon = LoadoutComp->Loadout[Slot];
        
		// Unequip the current weapon
		if (CharacterRef->CurrentWeapon)
		{
			USkeletalMeshComponent* CurrentWeaponMesh = CharacterRef->CurrentWeapon->WeaponModel;
			if (CurrentWeaponMesh)
			{
				CharacterRef->CurrentWeapon->CurrentMagAmmo = CurrentMagAmmo;
				CharacterRef->CurrentWeapon->CurrentAmmoReserve = CurrentAmmoReserve;
				CurrentWeaponMesh->SetVisibility(false);
			}
		}
        
		// Equip the new weapon
		CharacterRef->CurrentWeapon = NewWeapon;
		InitializeAmmoFromWeapon(NewWeapon);
        
		if (NewWeapon->WeaponModel)
		{
			AttachWeaponToSocket(NewWeapon, NewWeapon->WeaponSocketName);
			NewWeapon->WeaponModel->SetVisibility(true);
		}
        
		LoadoutComp->SetCurrentActiveSlot(Slot);
		CurrentSlot = Slot;
        
		UE_LOG(LogTemp, Warning, TEXT("Equipped weapon: %s in slot: %d"), 
			*NewWeapon->GetName(), CurrentSlot);
	}
}

void UPlayerActionsComponent::ToggleWeapon()
{
	if (CurrentSlot == ELoadoutSlot::Primary)
		SwitchWeaponSlot(ELoadoutSlot::Secondary);
	else
		SwitchWeaponSlot(ELoadoutSlot::Primary);
}
