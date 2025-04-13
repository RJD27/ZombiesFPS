#include "Pickups/APickupMaster.h"
#include "Characters/MainCharacter.h"
#include "Components/SphereComponent.h"

// Sets default values
APickupMaster::APickupMaster()
{
	PrimaryActorTick.bCanEverTick = false;

	// Initialize PickupMesh
	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupObject"));
	RootComponent = PickupMesh;
	PickupMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupMesh->SetSimulatePhysics(true);
	PickupMesh->SetMassScale(NAME_None, 60.0f);
	PickupMesh->SetLinearDamping(0.25);
	PickupMesh->SetAngularDamping(0.25);

	// Initialize CollisionSphere
	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickUpRadius"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->SetSphereRadius(100.f);
	CollisionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickupMaster::OnOverlapBegin);
	CollisionSphere->OnComponentEndOverlap.AddDynamic(this, &APickupMaster::OnOverlapEnd);
}

// Called when the game starts or when spawned
void APickupMaster::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APickupMaster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickupMaster::Interact(AActor* InteractingActor)
{
	UE_LOG(LogTemp, Warning, TEXT("Interacted with base pickup!"));
}

void APickupMaster::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bCanInteract) return; 

	AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(OtherActor);
	if (PlayerCharacter && bCanInteract)
	{
		// Add this to the nearby interactables list
		PlayerCharacter->GetNearbyInteractables().Add(this);
		UE_LOG(LogTemp, Warning, TEXT("Player entered pickup range. Added to interactables: %s"), *GetNameSafe(this));
	}
}

void APickupMaster::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(OtherActor))
	{
		if (PlayerCharacter->GetNearbyInteractables().Contains(this))
		{
			UE_LOG(LogTemp, Warning, TEXT("Player left pickup range. Removing interactable: %s"), *GetNameSafe(this));

			PlayerCharacter->GetNearbyInteractables().Remove(this);

			UE_LOG(LogTemp, Warning, TEXT("Total interactables after removal: %d"), PlayerCharacter->GetNearbyInteractables().Num());
		}
	}
}

void APickupMaster::ResetInteractionCooldown()
{
	bCanInteract = true; 
}
