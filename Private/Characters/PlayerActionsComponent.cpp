// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PlayerActionsComponent.h"

#include "SWarningOrErrorBox.h"
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
    	
        // GetWorld()->SpawnActor<AWeaponPickup>(
        //     MainCharacter->CurrentWeapon->PickupClass,
        //     GetSpawnLocation(),
        //     FRotator::ZeroRotator
        // );
		// For Dropping weapon^
    	
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

	if (WeaponToSpawn)
	{
		AttachWeaponToSocket(WeaponToSpawn, WeaponToSpawn->WeaponSocketName);
		CurrentMagAmmo =  WeaponToSpawn->StartingMagAmmo;
		CurrentAmmoReserve = WeaponToSpawn->StartingAmmoReserve;
	}
}

// Needs Refactoring
void UPlayerActionsComponent::HandleWeaponPickUp(AWeaponPickup* WeaponPickup)
{
	if (!WeaponPickup)
	{
		UE_LOG(LogTemp, Error, TEXT("HandleWeaponPickUp failed: WeaponPickup is null"));
		return;
	}
	// if (!WeaponPickup->WeaponToSpawn)
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("HandleWeaponPickUp failed: WeaponToSpawn is null."));
	// 	return;
	// }

	// Spawns Weapon on Character
	AWeaponMaster* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponMaster>(
		WeaponPickup->WeaponToSpawn,
		CharacterRef->GetActorLocation(),
		FRotator::ZeroRotator
	);
	
	WeaponPickup->SetActorHiddenInGame(true);

	// ELoadoutSlot TargetSlot = ELoadoutSlot::Primary;

	if (LoadoutComp->HasSameWeapon(WeaponPickup->WeaponName))
	{
		// Add Ammo Accordingly
		SpawnedWeapon->Destroy();
		WeaponPickup->Destroy();
		return;
	}
	if (LoadoutComp->IsSecondarySlotFree())
	{
		LoadoutComp->AddToLoadout(ELoadoutSlot::Secondary, SpawnedWeapon);
		LoadoutComp->SetCurrentActiveSlot(ELoadoutSlot::Secondary);
	}
	else if (LoadoutComp->IsPrimarySlotFree())
	{
		LoadoutComp->AddToLoadout(ELoadoutSlot::Primary, SpawnedWeapon);
		LoadoutComp->SetCurrentActiveSlot(ELoadoutSlot::Primary);
	}
	else
	{
		LoadoutComp->RemoveFromLoadout(CurrentSlot);
		LoadoutComp->AddToLoadout(CurrentSlot, SpawnedWeapon);
		LoadoutComp->SetCurrentActiveSlot(CurrentSlot);
	}

	AttachWeaponToSocket(SpawnedWeapon, SpawnedWeapon->WeaponSocketName);

	CharacterRef->SetCurrentWeapon(SpawnedWeapon);
	InitializeAmmoFromWeapon(SpawnedWeapon);

	WeaponPickup->Destroy();
	UE_LOG(LogTemp, Warning, TEXT("Current Active Slot: %d"), static_cast<int32>(CurrentSlot));
	UE_LOG(LogTemp, Log, TEXT("Weapon %s successfully picked up and equipped."), *SpawnedWeapon->GetName());

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

void UPlayerActionsComponent::Sprint()
{
	if (!IPlayerRef->HasEnoughStamina(SprintCost)){ return; }

	MovementComp->MaxWalkSpeed = SprintSpeed;
}

void UPlayerActionsComponent::Walk()
{
	MovementComp->MaxWalkSpeed = WalkSpeed;
}

// Move out ammo logic
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

void UPlayerActionsComponent::SwitchToWeaponSlot(ELoadoutSlot Slot)
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
	
    if (LoadoutComp->Loadout.Contains(Slot) && LoadoutComp->Loadout[Slot])
    {
        if (NewWeapon)
        {
            // Unequip the current weapon
            if (CharacterRef->CurrentWeapon)
            {
            	UE_LOG(LogTemp, Warning, TEXT("Setting current weapon %s to invisible"), 
		   *CharacterRef->CurrentWeapon->GetName());
            	CurrentWeaponMesh->SetVisibility(false);
                CurrentWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                CurrentWeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
            }
        	
            CharacterRef->CurrentWeapon = NewWeapon;
            InitializeAmmoFromWeapon(NewWeapon);

            if (!NewWeapon->WeaponModel)
            {
            	UE_LOG(LogTemp, Error, TEXT("Weapon model is null for %s"), *NewWeapon->GetName());
            	bIsSwapping = false;
            	return;
            }

        	USkeletalMeshComponent* NewWeaponMesh = NewWeapon->WeaponModel;

        	NewWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        	NewWeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
        	NewWeaponMesh->SetSimulatePhysics(false);

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
}

void UPlayerActionsComponent::EquipWeapon(ELoadoutSlot Slot)
{
	if (LoadoutComp->Loadout.Contains(Slot))
	{
		if (AWeaponMaster* NewWeapon = LoadoutComp->Loadout[Slot])
		{
			// Dequip the current weapon (detach, disable it)
			if (CharacterRef->CurrentWeapon)
			{
				CharacterRef->CurrentWeapon->WeaponModel->SetVisibility(false); 
				CharacterRef->CurrentWeapon->WeaponModel->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				CharacterRef->CurrentWeapon->WeaponModel->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
			}
			
			CharacterRef->CurrentWeapon = NewWeapon;
			InitializeAmmoFromWeapon(NewWeapon);
			
			AttachWeaponToSocket(NewWeapon, NewWeapon->WeaponSocketName);
			NewWeapon->WeaponModel->SetVisibility(true);
			NewWeapon->WeaponModel->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			LoadoutComp->SetCurrentActiveSlot(Slot);
		}
	}
}

void UPlayerActionsComponent::ToggleWeapon()
{
	if (CurrentSlot == ELoadoutSlot::Primary)
		SwitchToWeaponSlot(ELoadoutSlot::Secondary);
	else
		SwitchToWeaponSlot(ELoadoutSlot::Primary);
}
