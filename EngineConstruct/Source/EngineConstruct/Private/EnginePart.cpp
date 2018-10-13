// Engineering Thesis Mateusz Osipa 2018/19

#include "EnginePart.h"

void UEnginePart::BeginPlay()
{
	Super::BeginPlay();

	Parent = this->GetAttachParent();

	SocketName = this->GetAttachSocketName();

	if (Parent)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s have parent %s"), *(this->GetName()), *(Parent->GetName()));
	}

	if (SocketName != EName::NAME_None)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s is attached at %s"), *(this->GetName()), *(SocketName.ToString()));
	}
}

USceneComponent* UEnginePart::PreviouslyAttachedParent() const
{
	return Parent;
}

FName UEnginePart::PreviouslyAttachedToSocket() const
{
	return SocketName;
}
