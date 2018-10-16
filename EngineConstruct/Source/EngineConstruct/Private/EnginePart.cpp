// Engineering Thesis Mateusz Osipa 2018/19

#include "EnginePart.h"

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