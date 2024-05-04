// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <Components/SplineMeshComponent.h>
#include <Components/SplineComponent.h>
#include "WallSpline.generated.h"

UCLASS()
class ASSIGNMENT3_API AWallSpline : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWallSpline();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	USplineComponent* SplineComponent;

	
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere)
	UStaticMesh* StaticMesh;

	UPROPERTY(EditAnywhere)
	UMaterial* SplineMaterial;



	void GenerateWall();

	void SetPointLocation(FVector);

	int32 NoOfSplinePoints;

	void Undo();

	
	void Delete();

	TArray<USplineMeshComponent*> SpileArr;
};
