// Engineering Thesis Mateusz Osipa 2018/19

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPawn.generated.h"

UENUM()
enum class EHelperMode : uint8
{
	On,
	Off
};

UCLASS()
class ENGINECONSTRUCT_API AMyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AMyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	class UMyPawnMovementComponent* MovementComponent;

	virtual UPawnMovementComponent* GetMovementComponent() const override;

	void MoveForward(float AxisValue);
	void MoveRight(float AxisValue);
	void Turn(float AxisValue);
	void LookUpDown(float AxisValue);

	void Grab();
	void Release();
	void Reattach();

	void RotateXAxle();
	void RotateYAxle();
	void RotateZAxle();
	void RotateToZeros();
	void RotateXAxleNeg();
	void RotateYAxleNeg();
	void RotateZAxleNeg();

	class UEnginePart* GetOverlappingPart() const;
	void SetOverlappedPart(class UEnginePart* Part);

	UFUNCTION(BlueprintCallable)
		bool IsPawnOverlappedByPart() const;

	FORCEINLINE void SetOverlappingPart(bool bOverlap) { bOverlappingPart = bOverlap; }

	UFUNCTION(BlueprintCallable)
		bool IsPartGrabbed() const;

	class UEnginePart* GetGrabbedPart() const;
	void SetGrabbedPart(class UEnginePart* Part);

private:
	UPROPERTY(EditAnywhere, Category = "Socket-Setup")
		double SocketRadius = 5;

	UPROPERTY(EditAnywhere, Category = "Socket-Setup")
		int32 SocketSegments = 25;

	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* Collision;

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* Mesh;
	
	class UEnginePart* OverlappedPart; 
	class UEnginePart* GrabbedPart;

	bool bOverlappingPart;
	bool bGrabbed;

	FRotator CurrentComponentRotation = FRotator(0, 0, 0);

	void MoveAndRotateGrabbedComponent();
	void DrawSocketLocationOfGrabbedObject();
	void TurnOnHelperMode();

	float GetDistanceToSocket();

	EHelperMode HelperMode;
};
