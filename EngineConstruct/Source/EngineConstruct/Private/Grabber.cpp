// Engineering Thesis Mateusz Osipa 2018/19

#include "Grabber.h"
#include "Engine/World.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Public/DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"
#include "EnginePart.h"

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
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!ensure(PhysicsHandle)) { return; }

	
	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(CreateLineTraceEnd());
		PhysicsHandle->GrabbedComponent->SetRelativeRotation(FRotator(0, 0, 0));
	}
}

void UGrabber::Grab()
{
	if (!ensure(PhysicsHandle)) { return; }
	
	//Prevent grabbing if already grabbed something
	if (PhysicsHandle->GrabbedComponent) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Grabbing"));

	FHitResult Hit = GetFirstPhysicsBodyInReach();

	auto HitComponent = Hit.GetComponent();

	//Prevent crashing
	if (!HitComponent) { return; }

	//Check if grabbed object is a EnginePart type
		//And it has no children (cannot detach more than 1 object from engine at one go)
	if (HitComponent->GetClass()->IsChildOf<UEnginePart>() && HitComponent->GetAttachChildren().Num() == 0)
	{
		PhysicsHandle->GrabComponentAtLocation(
			HitComponent,
			NAME_None,
			HitComponent->GetComponentLocation()
		);
		
		HitComponent->UnWeldFromParent();
		HitComponent->SetSimulatePhysics(true);

		//TODO later use it in method that reattaches engine part
		UE_LOG(LogTemp, Warning, TEXT("%s had parent %s and was attached at %s"),
			*(HitComponent->GetName()),
			*(Cast<UEnginePart>(HitComponent)->PreviouslyAttachedParent()->GetName()),
			*(Cast<UEnginePart>(HitComponent)->PreviouslyAttachedToSocket().ToString())
		);	
	}

}

void UGrabber::Release()
{
	if (!ensure(PhysicsHandle)) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Releasing"));

	if (PhysicsHandle->GrabbedComponent)
	{
		auto GrabbedPart = PhysicsHandle->GrabbedComponent;
		PhysicsHandle->ReleaseComponent();

		//TODO decide if we want to let it freely move around or turn it's physics off
		GrabbedPart->SetSimulatePhysics(false);
	}
}

FVector UGrabber::CreateLineTraceEnd()
{
	FVector PlayerLocation;
	FRotator PlayerRotation;
	float Reach = 200.f;

	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerLocation,
		PlayerRotation
	);

	return PlayerLocation + PlayerRotation.Vector() * Reach;
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	FVector PlayerLocation;
	FRotator PlayerRotation;

	// Player viewpoint
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerLocation,
		PlayerRotation
	);

	FVector LineTraceEnd = CreateLineTraceEnd();

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

	DrawDebugLine(
		GetWorld(),
		PlayerLocation - FVector(0.f, 0.f, 30.f),
		LineTraceEnd,
		FColor::Red,
		false,
		0.f,
		0.f,
		10.f
	);

	UPrimitiveComponent* ActorHit = HitResult.GetComponent();
	if (ActorHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("Detected: %s"), *(Cast<USceneComponent>(ActorHit)->GetName()));
	}

	return HitResult;
}