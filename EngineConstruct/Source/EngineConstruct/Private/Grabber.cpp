// Engineering Thesis Mateusz Osipa 2018/19

#include "Grabber.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();

	//Find and get PHComponent
	SetupPhysicsHandleComponent();
}

void UGrabber::SetupPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	if (!ensure(PhysicsHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("Missing PhHandle"));
	}
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	FHitResult kKk = GetFirstPhysicsBodyInReach();
	// ...
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	FVector PlayerLocation;
	FRotator PlayerRotation;
	float Reach = 200.0f;

	// Player viewpoint
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerLocation,
		PlayerRotation
	);

	FVector LineTraceEnd = PlayerLocation + PlayerRotation.Vector() * Reach;

	//Players current location and rotation
	//UE_LOG(LogTemp, Warning, TEXT("%s player location"), *(PlayerLocation.ToString()));
	//UE_LOG(LogTemp, Warning, TEXT("%s player rotation"), *(PlayerRotation.ToString()));
		
	// Ray-cast out to reach distance
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	GetWorld()->LineTraceSingleByObjectType(
		HitResult,
		PlayerLocation,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParams
	);

	UPrimitiveComponent* ActorHit = HitResult.GetComponent();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected: %s"), *(Cast<USceneComponent>(ActorHit)->GetName()));
	}

	return HitResult;
}