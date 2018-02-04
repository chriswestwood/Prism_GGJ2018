// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blocks/Block.h"
#include "Defractor.generated.h"

/**
 * 
 */
UCLASS()
class GGJ2018_API ADefractor : public ABlock
{
	GENERATED_BODY()
	
	void DistortLight(FLightInfo InLight, FVector InNormal, FVector InDirection);
};
