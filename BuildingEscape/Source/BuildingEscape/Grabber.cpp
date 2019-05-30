// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Gameframework/Actor.h"

#define OUT
// Sets default values for this component's properties CONSTRUCTOR
UGrabber::UGrabber()
{
	PrimaryComponentTick.bCanEverTick = true;
}
// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}
/// Look for attached Physics handle
void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Component %s is missing physics handle component"), *(GetOwner()->GetName()))
	}
}
/// Set up attached input component
void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{	
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Component %s is missing input component"), *(GetOwner()->GetName()))
	}
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab key pressed"))
	/// Line trace and see if we reach any actors with physics body collision channel set
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();// gets the mesh
	auto ActorHit = HitResult.GetActor();
	/// If we hit something then attach a physics handle
	if (ActorHit) 
	{
		PhysicsHandle->GrabComponent(
			ComponentToGrab,
			NAME_None,// no bones needed
			ComponentToGrab->GetOwner()->GetActorLocation(),
			true// allow rotation
		);
	}
}

void UGrabber::Release() 
{
	UE_LOG(LogTemp, Warning, TEXT("Grab key released"))
	PhysicsHandle->ReleaseComponent();
}
// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	// if the physics handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		// move the object that we're holding
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}
		
}


const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	GetReachLineStart();
	/// Setup query parameters
	FHitResult HitResult;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());
	///Raycast out to reach distance
	
	GetWorld()->LineTraceSingleByObjectType(
		OUT HitResult,
		GetReachLineStart(),
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),
		TraceParameters
	);
	return HitResult;
}

FVector UGrabber::GetReachLineStart()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation + PlayerViewPointRotation.Vector() * Reach;
}

FVector UGrabber::GetReachLineEnd()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);

	return PlayerViewPointLocation;
}