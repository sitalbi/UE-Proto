// Fill out your copyright notice in the Description page of Project Settings.


#include "QuartzItemChest.h"
#include "Components/SphereComponent.h"

AQuartzItemChest::AQuartzItemChest()
{
 	PrimaryActorTick.bCanEverTick = true;

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	RootComponent = BaseMesh;

	LidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LidMesh"));
	LidMesh->SetupAttachment(BaseMesh);

	OpenPitchValue = 120.0f;

	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(BaseMesh);
	InteractionSphere->SetSphereRadius(150.f);
}


void AQuartzItemChest::BeginPlay()
{
	Super::BeginPlay();
	
}

bool AQuartzItemChest::Interact_Implementation(APawn* InstigatorPawn)
{
	OnOwnerInteracted(InstigatorPawn);

	InteractionSphere->SetGenerateOverlapEvents(false);

	return true;
}

