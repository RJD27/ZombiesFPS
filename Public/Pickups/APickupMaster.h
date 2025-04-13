// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Engine/Blueprint.h"
#include "Interfaces/Interact.h"
#include "APickupMaster.generated.h"

/**
 * 
 */
UCLASS()
class ZOMBIES_API APickupMaster : public AActor, public IInteract
{
	GENERATED_BODY()

public:
	APickupMaster();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
						class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
					  class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

public:
	virtual void Tick(float DeltaTime) override;
	virtual void Interact(AActor* InteractingActor) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	UStaticMeshComponent* PickupMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	USphereComponent* CollisionSphere;

	bool bCanInteract{ true };
	bool bWeaponAlreadyPickedUp{ false };
	FTimerHandle InteractionCooldownTimer;

	UFUNCTION()
	void ResetInteractionCooldown();
};