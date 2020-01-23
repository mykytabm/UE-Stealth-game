// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSAIGuard.generated.h"

class UPawnSensingComponent;

UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle,

	Patrolling,

	Investigating,

	Alert,
};

UCLASS()
class FPSGAME_API AFPSAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSAIGuard();

private:
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, Category = "Components")
	UPawnSensingComponent *PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn *SeenPawn);

	UFUNCTION()
	void OnNoiseHeard(APawn *NoiseInstigator, const FVector &Location, float Volume);

	UFUNCTION()
	void ResetOrientation();

	UFUNCTION()
	void Patrol();

	UFUNCTION()
	void WaypointReached();

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);

	UFUNCTION()
	void WaitBeforePatrolling();

	UFUNCTION()
	void OnRep_GuardState();

	void SetGuardState(EAIState NewState);

	UPROPERTY(ReplicatedUsing = OnRep_GuardState)
	EAIState GuardState;

	UPROPERTY(EditInstanceOnly, Category = "AI")
	bool bPatrol = false;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	float WaitOnPatrolPointTime = 3.0f;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	int WaypointAcceptanceRange = 100;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bPatrol"))
	TArray<AActor *> Waypoints;

	FTimerHandle TimerHandle_ResetOrientation;
	FTimerHandle TimerHandle_WaitBeforePatrolling;

	int CurrentPatrolPoint;

	FRotator OriginalRotation;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
