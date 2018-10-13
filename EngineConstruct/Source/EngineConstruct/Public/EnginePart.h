// Engineering Thesis Mateusz Osipa 2018/19

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "EnginePart.generated.h"

/**
 * 
 */
UCLASS(meta = (BlueprintSpawnableComponent))
class ENGINECONSTRUCT_API UEnginePart : public UStaticMeshComponent
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;
	
private:
	USceneComponent* Parent = nullptr;

	FName SocketName = EName::NAME_None;

	FVector PartLocation = FVector(0, 0, 0);

	FRotator PartRotation = FRotator(0, 0, 0);

public:
	USceneComponent* PreviouslyAttachedParent() const;

	FName PreviouslyAttachedToSocket() const;

	FVector GetPartLocation() const;

	FRotator GetPartRotation() const;
};
