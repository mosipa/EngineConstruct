// Engineering Thesis Mateusz Osipa 2018/19

#include "EnginePart.h"

void UEnginePart::BeginPlay()
{
	Super::BeginPlay();

	Parent = this->GetAttachParent();

	SocketName = this->GetAttachSocketName();

	FTransform PartLocationAndRotation = this->GetRelativeTransform();
	FQuat QuatPartRotation = PartLocationAndRotation.GetRotation();

	PartLocation = PartLocationAndRotation.GetLocation();
	PartRotation = QuatPartRotation.Rotator();
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