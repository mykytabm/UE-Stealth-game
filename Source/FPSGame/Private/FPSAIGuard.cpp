// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSAIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "AIController.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

// Sets default values
AFPSAIGuard::AFPSAIGuard()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AFPSAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AFPSAIGuard::OnNoiseHeard);

	GuardState = EAIState::Idle;
	UE_LOG(LogTemp,Log,TEXT("ctor"));

	
}

// Called when the game starts or when spawned
void AFPSAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();
	CurrentPatrolPoint = 0;

	if (bPatrol)
	{
		WaitBeforePatrolling();
	}
}

// Called every frame
void AFPSAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(bPatrol)
	{
		FVector Delta=GetActorLocation()-Waypoints[CurrentPatrolPoint]->GetActorLocation();
		float DistanceToGoal=Delta.Size();
		if(DistanceToGoal<WaypointAcceptanceRange)
		{
			WaypointReached();
		}
	}
}

void AFPSAIGuard::Patrol()
{	
	if(Waypoints[CurrentPatrolPoint]==nullptr)
	{
		return;
	}
	SetGuardState(EAIState::Patrolling);
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(),Waypoints[CurrentPatrolPoint]);
}

void AFPSAIGuard::WaitBeforePatrolling()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_WaitBeforePatrolling);
	GetWorldTimerManager().SetTimer(TimerHandle_WaitBeforePatrolling, this, &AFPSAIGuard::Patrol, WaitOnPatrolPointTime, false);
}

void AFPSAIGuard::WaypointReached()
{
	if (CurrentPatrolPoint+1 > Waypoints.Num()-1)
	{
		CurrentPatrolPoint = 0;
	}
	else
	{
		CurrentPatrolPoint++;
	}

	WaitBeforePatrolling();
}

void AFPSAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alert)
	{
		return;
	}

	SetGuardState(EAIState::Idle);
	SetActorRotation(OriginalRotation);

	if(bPatrol)
	{
		Patrol();
	}
}

void AFPSAIGuard::OnNoiseHeard(APawn *NoiseInstigator, const FVector &Location, float Volume)
{
	// UE_LOG(LogTemp,Log,TEXT("Guard heard something!"));
	if (GuardState == EAIState::Alert)
	{
		return;
	}

	// DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Blue, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	Direction.Normalize();

	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();
	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;
	SetActorRotation(NewLookAt);
	SetGuardState(EAIState::Investigating);

	AController* Controller=GetController();
	if(Controller)
	{
		Controller->StopMovement();
	}


	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AFPSAIGuard::ResetOrientation, 3.0f, false);
}

void AFPSAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState)
	{
		return;
	}
	GuardState = NewState;
	OnStateChanged(GuardState);
}

void AFPSAIGuard::OnPawnSeen(APawn *SeenPawn)
{
	if (SeenPawn == nullptr)
	{
		return;
	}
	// DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Red, false, 10.0f);

	AFPSGameMode *GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}
	SetGuardState(EAIState::Alert);
	AController* Controller=GetController();
	if(Controller)
	{
		Controller->StopMovement();
	}
}
