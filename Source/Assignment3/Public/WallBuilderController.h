// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "WallSpline.h"
#include "Delegates/Delegate.h"    
#include "WallBuilderController.generated.h"


DECLARE_DELEGATE_OneParam(FWallDelegate, FString); 

UCLASS()
class ASSIGNMENT3_API AWallBuilderController : public APlayerController
{
	GENERATED_BODY()
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:

	AWallBuilderController();
	virtual void SetupInputComponent() override;

	/** Mapping context used for pawn control. */
	UPROPERTY()
	class UInputMappingContext* SplineMappingContext;

	/** Action to update location. */
	UPROPERTY()
	class UInputAction* LeftClickAction;

	UPROPERTY()
	class UInputAction* RightClickAction;

	/*UPROPERTY()
	class UInputAction* LeftArrow;

	UPROPERTY()
	class UInputAction* RightArrow;*/

	UPROPERTY()
	class UInputAction* UndoAction;

	UPROPERTY()
	TArray<AWallSpline*> WallSplineArr;

	//UPROPERTY()
	//int CurrWall;

	UFUNCTION(BlueprintCallable)
	void Delete();

	UFUNCTION(BlueprintCallable)
	void DeleteAll();

	void OnLeftClick(const struct FInputActionValue& ActionValue);
	void OnRightClick(const struct FInputActionValue& ActionValue);
	void Undo(const struct FInputActionValue& ActionValue);
	//void OnLeft(const struct FInputActionValue& ActionValue);
	//void OnRight(const struct FInputActionValue& ActionValue);


	FWallDelegate WallConstructionDelegate;

	UFUNCTION(BlueprintImplementableEvent)
	void ChangeText(const FString& text);

};
