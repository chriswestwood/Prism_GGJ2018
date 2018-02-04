// Fill out your copyright notice in the Description page of Project Settings.

#include "LightEmitter.h"
#include "LightReciever.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"



// Sets default values
ALightEmitter::ALightEmitter()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void ALightEmitter::BeginPlay()
{
	Super::BeginPlay();
	
	GetWorldTimerManager().SetTimer(PulseHandle, this, &ALightEmitter::Pulse, 0.05f, true);

	EmittedLight.LightSource = this;
}	

void ALightEmitter::Pulse()
{
	for (TActorIterator<ABlock> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		// Same as with the Object Iterator, access the subclass instance with the * or -> operators.
		ABlock *Block = *ActorItr;
		Block->ClearVisuals();
	}

	EmitLight(GetActorForwardVector(), EmittedLight, LightDistance);
}
