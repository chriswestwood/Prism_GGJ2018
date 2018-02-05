// Fill out your copyright notice in the Description page of Project Settings.

#include "Defractor.h"




void ADefractor::DistortLight(FLightInfo InLight, FVector InNormal, FVector InDirection)
{
	FLightInfo NewR = InLight;
	FLightInfo NewG = InLight;
	FLightInfo NewB = InLight;

	NewR.Colour = FColor(InLight.Colour.R, 0, 0);
	FVector RedDirection = (GetActorRotation() + FRotator(0, 45, 0)).Vector();
	NewR.LightSource = this;

	NewG.Colour = FColor(0, InLight.Colour.G,0);
	FVector GreenDirection = (GetActorRotation() + FRotator(0, 0, 0)).Vector();
	NewG.LightSource = this;

	NewB.Colour = FColor(0, 0, InLight.Colour.B);
	FVector BlueDirection = (GetActorRotation() + FRotator(0, -45, 0)).Vector();
	NewB.LightSource = this;

	if (NewR.Colour.R > 0)
	{
		EmitLight(RedDirection, NewR);
	}
	
	if (NewG.Colour.G > 0)
	{
		EmitLight(GreenDirection, NewG);
	}
	
	if (NewB.Colour.B > 0)
	{
		EmitLight(BlueDirection, NewB);
	}
}
