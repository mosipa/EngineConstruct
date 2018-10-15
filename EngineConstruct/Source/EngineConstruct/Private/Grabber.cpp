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
	
	MoveAndRotateGrabbedComponent();
}

void UGrabber::TurnOnHelperMode()
{
	if (HelperMode == EHelperMode::Off) { HelperMode = EHelperMode::On; }
	else if (HelperMode == EHelperMode::On) { HelperMode = EHelperMode::Off; }
}

void UGrabber::MoveAndRotateGrabbedComponent()
{
	if (!ensure(PhysicsHandle)) { return; }

	if (PhysicsHandle->GrabbedComponent)
	{
		PhysicsHandle->SetTargetLocation(CreateLineTraceEnd());
		PhysicsHandle->GrabbedComponent->SetSimulatePhysics(false);
		PhysicsHandle->GrabbedComponent->SetRelativeRotation(FRotator(0, 0, 0) + CurrentComponentRotation);
		PhysicsHandle->GrabbedComponent->SetSimulatePhysics(true);

		if (HelperMode == EHelperMode::On)
		{
			DrawSocketLocationOfGrabbedObject();
		}
	}
}

void UGrabber::DrawSocketLocationOfGrabbedObject()
{
	//Get and draw its socket's location while object is grabbed
	auto GrabbedPart = PhysicsHandle->GrabbedComponent;
	auto SocketWorldLocation = Cast<UEnginePart>(GrabbedPart)->PreviouslyAttachedParent()->GetSocketLocation(Cast<UEnginePart>(GrabbedPart)->PreviouslyAttachedToSocket());
	FColor SocketColor;

	auto Parent = Cast<UEnginePart>(GrabbedPart)->PreviouslyAttachedParent();

	if (GetDistanceToSocket() > 100.f) { SocketColor = FColor::Red; }
	else 
	{ 
		//Parent is attached to the model
			//or Parent is base of the model
		if (Parent->GetAttachParent() != nullptr
			|| Parent->GetClass()->IsChildOf<UEnginePart>() == false)
		{
			SocketColor = FColor::Green;
		}
		else { SocketColor = FColor::Red; }
	}

	DrawDebugSphere(GetWorld(), SocketWorldLocation, SocketRadius, SocketSegments, SocketColor);
}

void UGrabber::Grab()
{
	if (!ensure(PhysicsHandle)) { return; }
	
	//Prevent grabbing if already grabbed something
	if (PhysicsHandle->GrabbedComponent) { return; }

	UE_LOG(LogTemp, Warning, TEXT("Grabbing"));

	FHitResult Hit = GetFirstPhysicsBodyInReach();
	CurrentComponentRotation = FRotator(0.f, 0.f, 0.f);

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
		GrabbedPart->SetSimulatePhysics(false);
	}
}

void UGrabber::ReattachGrabbedComponent()
{
	if (!ensure(PhysicsHandle)) { return; }

	//If there's nothing to reattach - quit
	if (!(PhysicsHandle->GrabbedComponent)) { return; }

	auto GrabbedPart = PhysicsHandle->GrabbedComponent;
	auto Parent = Cast<UEnginePart>(GrabbedPart)->PreviouslyAttachedParent();

	//Check if parent is UEnginePart class AND it is attached to its parent
		//Or parent is not UEnginePart class (so it dont have parent)
	if ((Parent->GetClass()->IsChildOf<UEnginePart>() && Parent->GetAttachParent() != nullptr)
		|| (Parent->GetClass()->IsChildOf<UEnginePart>() == false))
	{
		if (GetDistanceToSocket() < 100.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("Reattaching component"));

			PhysicsHandle->ReleaseComponent();

			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget, //Location
				EAttachmentRule::SnapToTarget, //Rotation
				EAttachmentRule::SnapToTarget, //Scale
				true
			);

			GrabbedPart->AttachToComponent(
				Cast<UEnginePart>(GrabbedPart)->PreviouslyAttachedParent(),
				AttachmentRules,
				Cast<UEnginePart>(GrabbedPart)->PreviouslyAttachedToSocket()
			);

			GrabbedPart->SetRelativeRotation(Cast<UEnginePart>(GrabbedPart)->GetPartRotation());
			GrabbedPart->SetRelativeLocation(Cast<UEnginePart>(GrabbedPart)->GetPartLocation());
			GrabbedPart->SetSimulatePhysics(false);

			UE_LOG(LogTemp, Warning, TEXT("Parent: %s"), *(GrabbedPart->GetAttachParent()->GetName()));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Too far from socket - unable to attach part. Get closer."));
		}
	}	
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Parent is not attached to its parent"));
	}
}

float UGrabber::GetDistanceToSocket()
{
	auto GrabbedPart = PhysicsHandle->GrabbedComponent;

	FVector SocketWorldLocation = Cast<UEnginePart>(GrabbedPart)->PreviouslyAttachedParent()->GetSocketLocation(Cast<UEnginePart>(GrabbedPart)->PreviouslyAttachedToSocket());

	FVector PlayerWorldLocation = GetOwner()->GetActorLocation();

	FVector VectorLength = SocketWorldLocation - PlayerWorldLocation;

	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));

	return Distance;
}

void UGrabber::RotateXAxle()
{
	if (!ensure(PhysicsHandle)) { return; }

	//If there's nothing to rotate - quit
	if (!(PhysicsHandle->GrabbedComponent)) { return; }

	CurrentComponentRotation.Roll += 8.f;
}

void UGrabber::RotateYAxle()
{
	if (!ensure(PhysicsHandle)) { return; }

	//If there's nothing to rotate - quit
	if (!(PhysicsHandle->GrabbedComponent)) { return; }

	CurrentComponentRotation.Pitch += 8.f;
}

void UGrabber::RotateZAxle()
{
	if (!ensure(PhysicsHandle)) { return; }

	//If there's nothing to rotate - quit
	if (!(PhysicsHandle->GrabbedComponent)) { return; }

	CurrentComponentRotation.Yaw += 8.f;
}

void UGrabber::RotateToZeros()
{
	if (!ensure(PhysicsHandle)) { return; }

	//If there's nothing to rotate - quit
	if (!(PhysicsHandle->GrabbedComponent)) { return; }

	CurrentComponentRotation.Roll = 0.f;
	CurrentComponentRotation.Pitch = 0.f;
	CurrentComponentRotation.Yaw = 0.f;
}

FVector UGrabber::CreateLineTraceEnd()
{
	FVector PlayerLocation;
	FRotator PlayerRotation;
	float Reach = 100.f;

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

	//Player's viewpoint
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		PlayerLocation,
		PlayerRotation
	);

	FVector LineTraceEnd = CreateLineTraceEnd();

	//Player's current location and rotation
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