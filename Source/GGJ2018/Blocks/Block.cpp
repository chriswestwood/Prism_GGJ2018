// Fill out your copyright notice in the Description page of Project Settings.

#include "Block.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LightEmitter.h"
#include "Kismet/KismetSystemLibrary.h"



// Sets default values
ABlock::ABlock()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComp = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	RootComponent = RootComp;

	StaticMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	StaticMeshComp->SetupAttachment(RootComp);

	DirectionComp = CreateDefaultSubobject<UArrowComponent>(TEXT("Direction"));
	DirectionComp->SetupAttachment(RootComp);

	RecieverBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Reciever Overlap"));
	RecieverBox->SetBoxExtent(FVector(50.f));
	RecieverBox->SetCollisionProfileName(TEXT("BlockAll"));
	RecieverBox->SetupAttachment(RootComp);

}

// Called when the game starts or when spawned
void ABlock::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABlock::DistortLight(FLightInfo InLight, FVector HitNormal, FVector InDirection)
{
	UE_LOG(LogTemp, Log, TEXT("Distorting light in empty Block object"));
}

void ABlock::EmitLight(FVector Direction, FLightInfo OutLight)
{
	OutLight.LightStrength-=1;
	if (OutLight.LightStrength > 0)
	{
		FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("RV_Trace")), true, this);
		TraceParams.AddIgnoredActor(OutLight.LightSource);

		FHitResult HitRes;

		FVector TraceStart = GetActorLocation();
		FVector TraceEnd = GetActorLocation() + (Direction * (OutLight.LightStrength * 100));
		FVector HitLocation;

		GetWorld()->LineTraceSingleByChannel(HitRes, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

		if (HitRes.bBlockingHit)
		{
			AActor* HitActor = HitRes.Actor.Get();
			UE_LOG(LogTemp, Log, TEXT("Beam from %s has hit %s, attempting to go to %s"), *OutLight.LightSource->GetName(), *this->GetName(), *HitActor->GetName());

			HitLocation = HitRes.Location;

			if (HitActor)
			{
				if (Cast<ALightReciever>(HitActor))
				{
					VisualiseLight(TraceStart, HitLocation, OutLight);
					Cast<ALightReciever>(HitActor)->LightEntered(OutLight);
				}
				else if (Cast<ABlock>(HitActor))
				{
					ABlock* HitBlock = Cast<ABlock>(HitActor);

					if (this->GetClass()->IsChildOf(ALightEmitter::StaticClass()))
					{
						HitBlock->DistortLight(OutLight, HitRes.Normal, Direction);
					}
					else if(HitBlock)
					{
						HitBlock->DistortLight(OutLight, HitRes.Normal, Direction);
						LastNodeLightSentTo = HitActor;
					}
					VisualiseLight(TraceStart, HitLocation, OutLight);
				}
				else
				{
					VisualiseLight(TraceStart, HitLocation, OutLight);
				}
			}
		}
		else
		{
			VisualiseLight(TraceStart, TraceEnd, OutLight);
		}
	}
}

void ABlock::VisualiseLight(FVector LightStart, FVector LightEnd, FLightInfo LightInfo)
{
	//DrawDebugline yadda yadda

	const FName BeamColorParam = TEXT("BeamColour");

	float MaxThickness = 5;
	float MaxStrength = 100;

	float CurrStrength = LightInfo.LightStrength / MaxStrength;
	float CurrThickness = MaxStrength * CurrStrength;

	DrawDebugLine(GetWorld(), LightStart, LightEnd, LightInfo.Colour, true, -1, 0, 8);

	if (false)
	{
		UParticleSystemComponent* LightVisual = UGameplayStatics::SpawnEmitterAttached(BeamEmitter, RootComp, NAME_None, LightStart, FRotator::ZeroRotator);

		if (LightVisual)
		{
			FParticleSysParam NewParam;
			NewParam.Color = LightInfo.Colour;
			NewParam.ParamType = EParticleSysParamType::PSPT_Color;
			NewParam.Name = TEXT("BeamColor");

			FParticleSysParam StartParam;
			StartParam.ParamType = EParticleSysParamType::PSPT_Vector;
			StartParam.Vector = LightStart;
			StartParam.Name = TEXT("BeamSourceParam");

			FParticleSysParam EndParam;
			EndParam.ParamType = EParticleSysParamType::PSPT_Vector;
			EndParam.Vector = LightEnd;
			EndParam.Name = TEXT("BeamTargetParam");

			LightVisual->InstanceParameters.Add(StartParam);
			LightVisual->InstanceParameters.Add(EndParam);
			LightVisual->InstanceParameters.Add(NewParam);


			LightVisual->SetBeamSourcePoint(0, LightStart, 0);
			LightVisual->SetBeamTargetPoint(0, LightEnd, 0);
			LightVisual->ActivateSystem(true);

			LightVisual->RegisterComponent();
			Particles.Add(LightVisual);
		}	
		
	}
}

bool ABlock::IsValidBounce(AActor* LightSource, AActor* TraceHit)
{
	if (LightSource == TraceHit || LightSource == this)
	{
		return false;
	}

	return true;
}

void ABlock::ClearVisuals()
{
	UKismetSystemLibrary::FlushPersistentDebugLines(GetWorld());
	
	for (UParticleSystemComponent* Particle : Particles)
	{
		Particle->UnregisterComponent();
		if (Particle)
		{
		Particle->DestroyComponent();
		}
	}
}

