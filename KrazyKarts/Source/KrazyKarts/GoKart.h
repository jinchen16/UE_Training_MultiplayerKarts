// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "GoKart.generated.h"

USTRUCT()
struct FGoKartMove
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	float Throttle;

	UPROPERTY()
	float SteeringThrow;

	UPROPERTY()
	float DeltaTime;

	UPROPERTY()
	float Timestamp;
};

USTRUCT()
struct FGoKartState
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	FGoKartMove LastMove;

	UPROPERTY()
	FVector Velocity;

	UPROPERTY()
	FTransform Transform;

};

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

	FVector Velocity;	
	float Throttle;
	float SteeringThrow;

	UPROPERTY(ReplicatedUsing = OnRep_ServerState)
	FGoKartState ServerState;

	UFUNCTION()
	void OnRep_ServerState();

	// RPC to use Server_SendMove using the FStruct FGoKartMove
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendMove(FGoKartMove Move);

	void ApplyRotation(float DeltaTime, float InSteeringThrow);
	void UpdateLocationFromVelocity(float DeltaTime);
	FVector GetAirResistance();
	FVector GetRollingResistance();

	void SimulateMove(FGoKartMove Move);

	TArray<FGoKartMove> CachedMoves;

	FGoKartMove CreateMove(float DeltaTime);
	void ClearCachedMoves(FGoKartMove LastMove);	

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
