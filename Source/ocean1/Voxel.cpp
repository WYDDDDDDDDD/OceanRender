// Fill out your copyright notice in the Description page of Project Settings.


#include "Voxel.h"
#include "CoreMinimal.h"

THIRD_PARTY_INCLUDES_START
#include"openvdb/openvdb.h"
THIRD_PARTY_INCLUDES_END

// Sets default values
AVoxel::AVoxel()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AVoxel::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AVoxel::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

