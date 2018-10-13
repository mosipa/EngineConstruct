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

	if (Parent)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s have parent %s"), *(this->GetName()), *(Parent->GetName()));
	}

	if (SocketName != EName::NAME_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is attached at %s"), *(this->GetName()), *(SocketName.ToString()));
	}

	UE_LOG(LogTemp, Warning, TEXT("%s have socket location at %s"), *(this->GetName()), *(PartLocation.ToString()));
	UE_LOG(LogTemp, Warning, TEXT("%s have socket rotation at %s"), *(this->GetName()), *(PartRotation.ToString()));
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