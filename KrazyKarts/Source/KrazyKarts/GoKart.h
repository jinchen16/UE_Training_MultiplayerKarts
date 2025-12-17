// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

UCLASS()
class KRAZYKARTS_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere)
	float Mass = 1000;

	UPROPERTY(EditAnywhere)
	float MaxDrivingForce = 10000;

	UPROPERTY(EditAnywhere)
	float MinTurningRadius = 10;

	UPROPERTY(EditAnywhere)
	float DragCoefficient = 16;

	UPROPERTY(EditAnywhere)
	float RollingResistanceCoefficient = 0.015;

	void MoveForward(float Value);
	void MoveRight(float Value);

	UPROPERTY(Replicated)
	FVector Velocity;

	UPROPERTY(Replicated)
	float Throttle;

	UPROPERTY(Replicated)
	float SteeringThrow;

	UPROPERTY(ReplicatedUsing = OnRep_ReplicatedTransform)
	FTransform ReplicatedTransform;

	UFUNCTION()
	void OnRep_ReplicatedTransform();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveForward(float Value);

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveRight(float Value);

	void ApplyRotation(float DeltaTime);
	void UpdateLocationFromVelocity(float DeltaTime);
	FVector GetAirResistance();
	FVector GetRollingResistance();







	/*
	* Set the Move.Time = GetWorld()->TimeSeconds;
	* Create a TArray
	* Use the TArray inside the IsLocallyControlled block 
	* Where the FGoKartMove is created
	* Make a function extracting the Move instance.
	* UnacknowledgedMoves.Add(Move); //This is the TArray
	* Prune the Queue 
	* Make a function that Clear the TArray -> param: FGoKartMove LastMove
	* TArray<FGoKartMove> NewMoves;	 
	*	for loop on the queue moves	
	*		if(Move.Time > LastMove.Time)
	*			NewMoves.Add(Move);
	* Set the TArray to the NewMoves;
	* 
	* OnRep_ServerState -> Call Clear the TArray.
	* if(!HasAuthority()) -> Add the move to the TArray
	 */

	/*
	* Replay/Simulate all the AcknowledgedMoves
	* OnRep_ServerState
	* for loop SimulateMove(Move) on the TArray
	* SimulateMove change to a const & param
	*/

	/*
	* SimulateMove(Move) on the Tick move it inside the if check !HasAuthority()
	* 
	* Tick
	* if(Role == ROLE_AutonomousProxy)
	* {
	*	Create the Move
	*	Simulate
	*	TArray add move
	*	Server_SendMove
	* }
	* 
	* // We are the server
	* if(Role == ROLE_Authority && !IsLocallyControlled())
	* {
	*	Create move
	*	Server_SendMove
	* }
	* 
	* Client Prediction
	* if(Role == ROLE_SimulatedProxy)
	* {
	*	SimulateMove(ServerState.LastMove);
	* }
	*/
};
