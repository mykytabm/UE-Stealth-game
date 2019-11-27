// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "FPSBlackHole.h"

// Sets default values
AFPSBlackHole::AFPSBlackHole()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	InnerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComp"));
	InnerSphereComp->SetSphereRadius(100);
	InnerSphereComp->SetupAttachment(MeshComp);

	InnerSphereComp->OnComponentBeginOverlap.AddDynamic(this,&AFPSBlackHole::OverlapInnerSphere);

	OuterSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("OuterSphereComp"));
	OuterSphereComp->SetSphereRadius(3000);
	OuterSphereComp->SetupAttachment(MeshComp);

	BlackHoleRadius=OuterSphereComp->GetScaledSphereRadius();
	ForceStrength=-2000;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFPSBlackHole::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSBlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<UPrimitiveComponent*> OverlappingComponents;
	OuterSphereComp->GetOverlappingComponents(OverlappingComponents);
	for(auto component: OverlappingComponents)
	{
		UPrimitiveComponent* PrimitiveComp=component;
		if(PrimitiveComp&& PrimitiveComp->IsSimulatingPhysics())
		{
			PrimitiveComp->AddRadialForce(GetActorLocation(),BlackHoleRadius,ForceStrength,ERadialImpulseFalloff::RIF_Constant,true);
		}
	}

}


void AFPSBlackHole::OverlapInnerSphere(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult)
	{
		if(OtherActor)
		{
			OtherActor->Destroy();
		}
	}

