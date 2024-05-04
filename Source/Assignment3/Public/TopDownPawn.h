// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include <Components/CapsuleComponent.h>
#include <GameFramework/SpringArmComponent.h>
#include <GameFramework/FloatingPawnMovement.h>
#include <Camera/CameraComponent.h>
#include "TopDownPawn.generated.h"

UCLASS()
class ASSIGNMENT3_API ATopDownPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATopDownPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY()
	class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY()
	class UInputAction* ZoomAction;

	UPROPERTY()
	class UInputMappingContext* PawnMapping;

	UPROPERTY()
	UFloatingPawnMovement* Movement;

	UPROPERTY(EditAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(EditAnywhere)
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* Capsule;

	void Move(const struct FInputActionValue& ActionValue);

	void Zoom(const struct FInputActionValue& ActionValue);

};
