// Fill out your copyright notice in the Description page of Project Settings.


#include "GoKartMovementReplicator.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGoKartMovementReplicator::UGoKartMovementReplicator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicated(true);
}

// Called when the game starts
void UGoKartMovementReplicator::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = GetOwner()->FindComponentByClass<UGoKartMovementComponent>();
}

// Called every frame
void UGoKartMovementReplicator::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (MovementComponent == nullptr) return;

	FGoKartMove LastMove = MovementComponent->GetLastMove();

	if (GetOwner()->GetLocalRole() == ROLE_AutonomousProxy)
	{
		CachedMoves.Add(LastMove);
		Server_SendMove(LastMove);
	}

	if (GetOwner()->GetLocalRole() == ROLE_Authority)
	{		
		UpdateServerState(LastMove);
	}

	if (GetOwner()->GetLocalRole() == ROLE_SimulatedProxy)
	{
		MovementComponent->SimulateMove(ServerState.LastMove);
	}
}

void UGoKartMovementReplicator::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGoKartMovementReplicator, ServerState);
}

void UGoKartMovementReplicator::OnRep_ServerState()
{
	MovementComponent->SetVelocity(ServerState.Velocity);
	GetOwner()->SetActorTransform(ServerState.Transform);
	ClearCachedMoves(ServerState.LastMove);

	for (const FGoKartMove& Move : CachedMoves)
	{
		MovementComponent->SimulateMove(Move);
	}
}

// Replace this SendMove
void UGoKartMovementReplicator::Server_SendMove_Implementation(FGoKartMove Move)
{
	MovementComponent->SimulateMove(Move);
	UpdateServerState(Move);
}

// Replace this SendMove_Validation
bool UGoKartMovementReplicator::Server_SendMove_Validate(FGoKartMove Move)
{
	// TODO: to make a better validation
	return true;
}

void UGoKartMovementReplicator::ClearCachedMoves(FGoKartMove LastMove)
{
	TArray<FGoKartMove> NewMoves;
	for (const FGoKartMove& Move : CachedMoves)
	{
		if (Move.Timestamp > LastMove.Timestamp)
		{
			NewMoves.Add(Move);
		}
	}
	CachedMoves = NewMoves;
}

void UGoKartMovementReplicator::UpdateServerState(FGoKartMove LastMove)
{
	ServerState.LastMove = LastMove;
	ServerState.Velocity = MovementComponent->GetVelocity();
	ServerState.Transform = GetOwner()->GetActorTransform();
}
