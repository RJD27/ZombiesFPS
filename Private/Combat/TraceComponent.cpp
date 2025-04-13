// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/TraceComponent.h"

#include "Camera/CameraComponent.h"
#include "Chaos/Deformable/ChaosDeformableSolverProxy.h"

// Sets default values for this component's properties
UTraceComponent::UTraceComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}


// Called when the game starts
void UTraceComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UTraceComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UTraceComponent::FireTrace()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	APawn* ControlledPawn = PlayerController->GetPawn();
	UCameraComponent* CameraComponent = ControlledPawn->FindComponentByClass<UCameraComponent>();

	FVector StartLocation = CameraComponent->GetComponentLocation();
	FVector ForwardVector = CameraComponent->GetForwardVector();
	FVector EndLocation = StartLocation + (ForwardVector * TraceDistance);

	FHitResult HitResult;

	FCollisionQueryParams IgnoreParams{
		FName{ TEXT("Ignore Params") },
		true,
		GetOwner()
	};
	
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		StartLocation,
		EndLocation,
		ECC_GameTraceChannel1,
		IgnoreParams
	);

	DrawDebugLine(
		GetWorld(),
		StartLocation,
		EndLocation,
		bHit ? FColor::Green : FColor::Red,
		false,
		1.0f,
		0,
		.5f
	);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitResult.GetActor()->GetName());
	}
}

