// Engineering Thesis Mateusz Osipa 2018/19

#pragma once

#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"
#include "EnginePart.generated.h"

/**
 * 
 */
UCLASS()
class ENGINECONSTRUCT_API UEnginePart : public UStaticMeshComponent
{
	GENERATED_BODY()

	UEnginePart();

public:
	UFUNCTION()
		void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


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
