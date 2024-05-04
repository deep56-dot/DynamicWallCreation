// Fill out your copyright notice in the Description page of Project Settings.


#include "WallSpline.h"

// Sets default values
AWallSpline::AWallSpline()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);



	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/StarterContent/Architecture/Wall_400x200.Wall_400x200'"));
	SplineComponent->ClearSplinePoints();
	if (MeshAsset.Succeeded())
	{
		StaticMesh = MeshAsset.Object;
	}

	static ConstructorHelpers::FObjectFinder<UMaterial> Material(TEXT("/Script/Engine.Material'/Game/StarterContent/Materials/M_Concrete_Grime.M_Concrete_Grime'"));

	if (Material.Succeeded())
	{
		UE_LOG(LogTemp, Warning, TEXT("jklahsxcdkls"));
		SplineMaterial = Material.Object;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("jfail"));

	}
	NoOfSplinePoints = 0;
}

// Called when the game starts or when spawned
void AWallSpline::BeginPlay()
{
	Super::BeginPlay();

}

void AWallSpline::OnConstruction(const FTransform& Transform)
{
	
}



// Called every frame
void AWallSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWallSpline::SetPointLocation(FVector vec)
{
	SplineComponent->AddSplinePoint(vec, ESplineCoordinateSpace::World);
	GenerateWall();
}

void AWallSpline::Undo()
{
	if (SpileArr.Num() >0) {
		SplineComponent->RemoveSplinePoint(SpileArr.Num());
		SpileArr[SpileArr.Num() - 1]->DestroyComponent();
		SpileArr.RemoveAt(SpileArr.Num()-1);
	}
	else {
		SplineComponent->ClearSplinePoints();
		SpileArr.Empty();
	}

}

void AWallSpline::Delete()
{
	for (int i = 0; i < SpileArr.Num(); i++) {
		if (SpileArr[i]) {
			SpileArr[i]->DestroyComponent();
			SpileArr[i] = nullptr;
		}
	}
	SplineComponent->ClearSplinePoints();
	SpileArr.Empty();
}

void AWallSpline::GenerateWall()
{
	/*for (int i = 0; i < SpileArr.Num(); i++) {
		if (SpileArr[i]) {
			SpileArr[i]->DestroyComponent();
			SpileArr[i] = nullptr;
		}
	}
	SpileArr.Empty();*/
	
	NoOfSplinePoints = SplineComponent->GetNumberOfSplinePoints()-1;
	//for (int32 i = 0; i < SplineComponent->GetNumberOfSplinePoints() - 1; ++i)
	if(NoOfSplinePoints>0){

		FVector StartLocation = SplineComponent->GetLocationAtSplinePoint(NoOfSplinePoints-1, ESplineCoordinateSpace::World);
		FVector EndLocation = SplineComponent->GetLocationAtSplinePoint(NoOfSplinePoints , ESplineCoordinateSpace::World);
		FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(NoOfSplinePoints-1, ESplineCoordinateSpace::World);
		FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(NoOfSplinePoints, ESplineCoordinateSpace::World);

		USplineMeshComponent* SplineMesh = NewObject<USplineMeshComponent>(this);
		if (SplineMesh)
		{
			
			SplineMesh->SetMobility(EComponentMobility::Movable);
			SplineMesh->SetStaticMesh(StaticMesh);
			SplineMesh->SetMaterial(0, SplineMaterial);
			SplineMesh->SetStartAndEnd(StartLocation, StartTangent, EndLocation, EndTangent);
		
			SplineMesh->SetupAttachment(SplineComponent);
			SplineMesh->RegisterComponent();
			SpileArr.Add(SplineMesh);
		}
	}
}

