// Engineering Thesis Mateusz Osipa 2018/19

#include "EnginePart.h"
#include "MyPawn.h"

UEnginePart::UEnginePart()
{
	this->SetCollisionResponseToAllChannels(ECR_Ignore);
	this->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	this->OnComponentBeginOverlap.AddDynamic(this, &UEnginePart::OnOverlapBegin);
	this->OnComponentEndOverlap.AddDynamic(this, &UEnginePart::OnOverlapEnd);

	this->SetCollisionObjectType(ECC_PhysicsBody);
}

void UEnginePart::OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->GetClass()->IsChildOf<AMyPawn>())
	{
		AMyPawn* Pawn = Cast<AMyPawn>(OtherActor);

		if (this->GetAttachChildren().Num() == 0)
		{
			Pawn->SetOverlappingPart(true);
			Pawn->SetOverlappedPart(Cast<UEnginePart>(OverlappedComp));
		}
	}
}

void UEnginePart::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->GetClass()->IsChildOf<AMyPawn>())
	{
		AMyPawn* Pawn = Cast<AMyPawn>(OtherActor);

		if (!Pawn->IsPartGrabbed())
		{
			Pawn->SetOverlappingPart(false);
			Pawn->SetOverlappedPart(nullptr);
		}
	}
}

void UEnginePart::BeginPlay()
{
	Super::BeginPlay();

	this->Parent = this->GetAttachParent();

	this->SocketName = this->GetAttachSocketName();

	FTransform PartLocationAndRotation = this->GetRelativeTransform();
	FQuat QuatPartRotation = PartLocationAndRotation.GetRotation();

	this->PartLocation = PartLocationAndRotation.GetLocation();
	this->PartRotation = QuatPartRotation.Rotator();
}

USceneComponent* UEnginePart::PreviouslyAttachedParent() const
{
	return Parent;
}

FName UEnginePart::PreviouslyAttachedToSocket() const
{
	return SocketName;
}

FVector UEnginePart::GetPartLocation() const
{
	return PartLocation;
}

FRotator UEnginePart::GetPartRotation() const
{
	return PartRotation;
}