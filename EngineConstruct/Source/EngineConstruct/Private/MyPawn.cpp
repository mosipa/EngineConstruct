// Engineering Thesis Mateusz Osipa 2018/19

#include "MyPawn.h"
#include "Runtime/Engine/Classes/Components/CapsuleComponent.h"
#include "Runtime/Engine/Classes/Components/StaticMeshComponent.h"
#include "MyPawnMovementComponent.h"
#include "Runtime/Engine/Classes/Components/InputComponent.h"
#include "EnginePart.h"
#include "Public/DrawDebugHelpers.h"

// Sets default values
AMyPawn::AMyPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);

	Collision = CreateDefaultSubobject<UCapsuleComponent>("Collision");
	Collision->SetRelativeLocation(FVector(350.f, 0.f, 50.f));
	Collision->InitCapsuleSize(50.f, 10.f);
	Collision->SetupAttachment(Mesh);

	MovementComponent = CreateDefaultSubobject<UMyPawnMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	OverlappedPart = nullptr;
	bOverlappingPart = false;
	bGrabbed = false;
	HelperMode = EHelperMode::Off;
}

UEnginePart* AMyPawn::GetOverlappingPart() const
{
	return OverlappedPart;
}

UEnginePart* AMyPawn::GetGrabbedPart() const
{
	return GrabbedPart;
}

void AMyPawn::SetOverlappedPart(UEnginePart* Part)
{
	OverlappedPart = Part;
}

void AMyPawn::SetGrabbedPart(class UEnginePart* Part)
{
	GrabbedPart = Part;
}

// Called when the game starts or when spawned
void AMyPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	MoveAndRotateGrabbedComponent();
}

void AMyPawn::MoveAndRotateGrabbedComponent()
{
	if (bGrabbed)
	{
		UEnginePart* Part = this->GetGrabbedPart();

		Part->SetWorldLocation(this->Collision->GetComponentLocation());
		Part->SetSimulatePhysics(false);
		Part->SetWorldRotation(FRotator(0, 0, 0) + CurrentComponentRotation);

		if (HelperMode == EHelperMode::On)
		{
			DrawSocketLocationOfGrabbedObject();
		}
	}
}

void AMyPawn::DrawSocketLocationOfGrabbedObject()
{
	//Get and draw its socket's location while object is grabbed
	UEnginePart* Part = this->GetGrabbedPart();
	auto SocketWorldLocation = Part->PreviouslyAttachedParent()->GetSocketLocation(Part->PreviouslyAttachedToSocket());
	FColor SocketColor;

	auto Parent = Part->PreviouslyAttachedParent();

	if (GetDistanceToSocket() > 200.f) { SocketColor = FColor::Red; }
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

void AMyPawn::TurnOnHelperMode()
{
	if (HelperMode == EHelperMode::Off) { HelperMode = EHelperMode::On; }
	else { HelperMode = EHelperMode::Off; }
}


// Called to bind functionality to input
void AMyPawn::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveForward", this, &AMyPawn::MoveForward);
	InputComponent->BindAxis("MoveLeftRight", this, &AMyPawn::MoveRight);
	InputComponent->BindAxis("Turn", this, &AMyPawn::Turn);
	InputComponent->BindAxis("LookUpDown", this, &AMyPawn::LookUpDown);

	InputComponent->BindAction("Grab", IE_Pressed, this, &AMyPawn::Grab);
	InputComponent->BindAction("Release", IE_Pressed, this, &AMyPawn::Release);
	InputComponent->BindAction("Reattach", IE_Pressed, this, &AMyPawn::Reattach);
	InputComponent->BindAction("HelperMode", IE_Pressed, this, &AMyPawn::TurnOnHelperMode);

	InputComponent->BindAction("RotateXAxle", IE_Pressed, this, &AMyPawn::RotateXAxle);
	InputComponent->BindAction("RotateYAxle", IE_Pressed, this, &AMyPawn::RotateYAxle);
	InputComponent->BindAction("RotateZAxle", IE_Pressed, this, &AMyPawn::RotateZAxle);
	InputComponent->BindAction("RotateToZeros", IE_Pressed, this, &AMyPawn::RotateToZeros);
	InputComponent->BindAction("RotateXAxleNeg", IE_Pressed, this, &AMyPawn::RotateXAxleNeg);
	InputComponent->BindAction("RotateYAxleNeg", IE_Pressed, this, &AMyPawn::RotateYAxleNeg);
	InputComponent->BindAction("RotateZAxleNeg", IE_Pressed, this, &AMyPawn::RotateZAxleNeg);
}

bool AMyPawn::IsPawnOverlappedByPart() const
{ 
	return this->bOverlappingPart; 
}

UPawnMovementComponent* AMyPawn::GetMovementComponent() const
{
	return MovementComponent;
}

void AMyPawn::MoveForward(float AxisValue)
{
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		MovementComponent->AddInputVector(GetActorForwardVector() * AxisValue);
	}
}

void AMyPawn::MoveRight(float AxisValue)
{
	if (MovementComponent && (MovementComponent->UpdatedComponent == RootComponent))
	{
		MovementComponent->AddInputVector(GetActorRightVector() * AxisValue);
	}
}

void AMyPawn::Turn(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Yaw += AxisValue;
	SetActorRotation(NewRotation);
}

void AMyPawn::LookUpDown(float AxisValue)
{
	FRotator NewRotation = GetActorRotation();
	NewRotation.Pitch += AxisValue;
	SetActorRotation(NewRotation);
}

void AMyPawn::Grab()
{
	UEnginePart* Part = this->GetOverlappingPart();

	if (this->IsPawnOverlappedByPart()
		&& Part)
	{
		if (Part->GetAttachChildren().Num() == 0)
		{
			this->SetGrabbedPart(Part);
			Part->UnWeldFromParent();
			Part->SetSimulatePhysics(true);
			
			FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(
				EAttachmentRule::SnapToTarget, //Location
				EAttachmentRule::SnapToTarget, //Rotation
				EAttachmentRule::SnapToTarget, //Scale
				true
			);

			Part->AttachToComponent(
				this->Collision,
				AttachmentRules
			);

			bGrabbed = true;
		}
	}
}

void AMyPawn::Release()
{
	UEnginePart* Part = this->GetGrabbedPart();

	if (bGrabbed)
	{
		CurrentComponentRotation = FRotator(0, 0, 0);
		Part->UnWeldFromParent();
		Part->SetSimulatePhysics(true);
		bGrabbed = false;
	}
}

void AMyPawn::Reattach()
{
	if (bGrabbed)
	{
		UEnginePart* Part = this->GetGrabbedPart();
		auto Parent = Part->PreviouslyAttachedParent();

		//Check if parent is UEnginePart class AND it is attached to its parent
			//Or parent is not UEnginePart class (so it dont have parent)
		if ((Parent->GetClass()->IsChildOf<UEnginePart>() && Parent->GetAttachParent() != nullptr)
			|| (Parent->GetClass()->IsChildOf<UEnginePart>() == false))
		{
			if (GetDistanceToSocket() < 200.f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Reattaching component"));

				CurrentComponentRotation = FRotator(0, 0, 0);
				Part->UnWeldFromParent();
				Part->SetSimulatePhysics(true);
				bGrabbed = false;

				FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules(
					EAttachmentRule::SnapToTarget, //Location
					EAttachmentRule::SnapToTarget, //Rotation
					EAttachmentRule::SnapToTarget, //Scale
					true
				);

				Part->AttachToComponent(
					Part->PreviouslyAttachedParent(),
					AttachmentRules,
					Part->PreviouslyAttachedToSocket()
				);

				Part->SetRelativeRotation(Part->GetPartRotation());
				Part->SetRelativeLocation(Part->GetPartLocation());
				Part->SetSimulatePhysics(false);

				UE_LOG(LogTemp, Warning, TEXT("Parent: %s"), *(Part->GetAttachParent()->GetName()));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Too far from socket - unable to attach part. Get closer."));
			}
		}
	}
}

float AMyPawn::GetDistanceToSocket()
{
	UEnginePart* Part = this->GetGrabbedPart();

	FVector SocketWorldLocation = Part->PreviouslyAttachedParent()->GetSocketLocation(Part->PreviouslyAttachedToSocket());
	FVector PlayerWorldLocation = this->Collision->GetComponentLocation();

	FVector VectorLength = SocketWorldLocation - PlayerWorldLocation;
	float Distance = FMath::Sqrt(FMath::Pow(VectorLength.X, 2) + FMath::Pow(VectorLength.Y, 2) + FMath::Pow(VectorLength.Z, 2));

	return Distance;
}

void AMyPawn::RotateXAxle()
{
	if (bGrabbed)
	{
		CurrentComponentRotation.Roll += 8.f;
	}
}

void AMyPawn::RotateYAxle()
{
	if (bGrabbed)
	{
		CurrentComponentRotation.Pitch += 8.f;
	}
}

void AMyPawn::RotateZAxle()
{
	if (bGrabbed)
	{
		CurrentComponentRotation.Yaw += 8.f;
	}
}

void AMyPawn::RotateXAxleNeg()
{
	if (bGrabbed)
	{
		CurrentComponentRotation.Roll -= 8.f;
	}
}

void AMyPawn::RotateYAxleNeg()
{
	if (bGrabbed)
	{
		CurrentComponentRotation.Pitch -= 8.f;
	}
}

void AMyPawn::RotateZAxleNeg()
{
	if (bGrabbed)
	{
		CurrentComponentRotation.Yaw -= 8.f;
	}
}

void AMyPawn::RotateToZeros()
{
	if (bGrabbed)
	{
		CurrentComponentRotation.Roll = 0.f;
		CurrentComponentRotation.Pitch = 0.f;
		CurrentComponentRotation.Yaw = 0.f;
	}
}