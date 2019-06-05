// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Gameframework/Actor.h"
#define OUT
// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	if (!PressurePlate)
	{
		UE_LOG(LogTemp, Error, TEXT("Component %s is missing PressurePlate"), *(GetOwner()->GetName()))
	}
}

void UOpenDoor::OpenDoor()
{
	Owner->SetActorRotation(FRotator(0.0f, OpenAngle, 0.0f));
}


void UOpenDoor::CloseDoor()
{
	Owner->SetActorRotation(FRotator(0.0f, 0.0f, 0.0f));
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	//Poll the Trigger Volume
	if (GetTotalMassOfActorsOnPlate()>= 30.f) { // TODO make  into parameters
		OpenDoor();
		LastDoorOpenTime = GetWorld()->GetTimeSeconds();
	}

	//Check if its time  to close door
	
	if (GetWorld()->GetTimeSeconds() >= LastDoorOpenTime + DoorCloseDelay)
	{
		CloseDoor();
	}

}
float UOpenDoor::GetTotalMassOfActorsOnPlate()
{
	float TotalMass = 0.f;
	TArray<AActor*> OverlappingActors;
	//Find all the overlapping actors
	if (!PressurePlate) { return TotalMass; }
	
	PressurePlate->GetOverlappingActors(OUT OverlappingActors);
	
	///Iterate through Actors adding their masses
	for (const auto* OActors : OverlappingActors)
	{
		TotalMass += OActors->FindComponentByClass<UPrimitiveComponent>()->GetMass();
	}
	///UE_LOG(LogTemp, Error, TEXT("Weight on PP %.2f"), (TotalMass))
	return TotalMass;
}
