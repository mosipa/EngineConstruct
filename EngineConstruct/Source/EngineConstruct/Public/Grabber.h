// Engineering Thesis Mateusz Osipa 2018/19

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

class UPhysicsHandleComponent;
class UEnginePart;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ENGINECONSTRUCT_API UGrabber : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
		void Grab();

	UFUNCTION(BlueprintCallable)
		void Release();

	UFUNCTION(BlueprintCallable)
		void RotateXAxle();

	UFUNCTION(BlueprintCallable)
		void RotateYAxle();

	UFUNCTION(BlueprintCallable)
		void RotateZAxle();

	UFUNCTION(BlueprintCallable)
		void RotateToZeros();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	UPhysicsHandleComponent * PhysicsHandle = nullptr;
	
	void SetupPhysicsHandleComponent();

	const FHitResult GetFirstPhysicsBodyInReach();

	FHitResult HitResult;

	FVector CreateLineTraceEnd();

	FRotator CurrentComponentRotation = FRotator(0, 0, 0);
};
