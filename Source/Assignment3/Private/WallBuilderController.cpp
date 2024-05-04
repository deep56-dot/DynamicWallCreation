// Fill out your copyright notice in the Description page of Project Settings.


#include "WallBuilderController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"

static void KeyMap(UInputMappingContext* InputMappingContext, UInputAction* InputAction, FKey Key,
	bool bNegate = false,
	bool bSwizzle = false, EInputAxisSwizzle SwizzleOrder = EInputAxisSwizzle::YXZ)
{
	FEnhancedActionKeyMapping& Mapping = InputMappingContext->MapKey(InputAction, Key);
	UObject* Outer = InputMappingContext->GetOuter();

	if (bNegate) {
		UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(Outer);
		Mapping.Modifiers.Add(Negate);
	}

	if (bSwizzle) {
		UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(Outer);
		Swizzle->Order = SwizzleOrder;
		Mapping.Modifiers.Add(Swizzle);
	}
}
void AWallBuilderController::BeginPlay()
{
	Super::BeginPlay();
	AWallSpline* WallObj = NewObject<AWallSpline>(this);
	WallSplineArr.Add(WallObj);
	/*CurrWall = WallSplineArr.Num() - 1;*/
	SetShowMouseCursor(true);

}
AWallBuilderController::AWallBuilderController()
{
	WallConstructionDelegate.BindUFunction(this, "ChangeText"); 

}
void AWallBuilderController::SetupInputComponent()
{

	Super::SetupInputComponent();

	SplineMappingContext = NewObject<UInputMappingContext>(this);

	LeftClickAction = NewObject<UInputAction>(this);
	LeftClickAction->ValueType = EInputActionValueType::Boolean;
	KeyMap(SplineMappingContext, LeftClickAction, EKeys::LeftMouseButton);

	RightClickAction = NewObject<UInputAction>(this);
	RightClickAction->ValueType = EInputActionValueType::Boolean;
	KeyMap(SplineMappingContext, RightClickAction, EKeys::RightMouseButton);

	UndoAction = NewObject<UInputAction>(this);
	UndoAction->ValueType = EInputActionValueType::Boolean;
	KeyMap(SplineMappingContext, UndoAction, EKeys::Z);

	LeftArrow = NewObject<UInputAction>(this);
	LeftArrow->ValueType = EInputActionValueType::Boolean;
	KeyMap(SplineMappingContext, LeftArrow, EKeys::Left);

	RightArrow = NewObject<UInputAction>(this);
	RightArrow->ValueType = EInputActionValueType::Boolean;
	KeyMap(SplineMappingContext, RightArrow, EKeys::Right);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent);


	if (EIC) {

		EIC->BindAction(LeftClickAction, ETriggerEvent::Completed, this, &AWallBuilderController::OnLeftClick);
		EIC->BindAction(RightClickAction, ETriggerEvent::Completed, this, &AWallBuilderController::OnRightClick);
		EIC->BindAction(UndoAction, ETriggerEvent::Completed, this, &AWallBuilderController::Undo);
		EIC->BindAction(LeftArrow, ETriggerEvent::Completed, this, &AWallBuilderController::OnLeft);
		EIC->BindAction(RightArrow, ETriggerEvent::Completed, this, &AWallBuilderController::OnRight);


	
		ULocalPlayer* LocalPlayer = GetLocalPlayer();
		check(LocalPlayer);

		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check(Subsystem);

		Subsystem->AddMappingContext(SplineMappingContext, 0);
	}
	
}


void AWallBuilderController::Delete()
{
	if (WallSplineArr.Num()>0) {
			WallSplineArr[WallSplineArr.Num()-1]->Destroy();
			WallSplineArr.RemoveAt(WallSplineArr.Num() - 1);
	}
	if(WallSplineArr.Num()<=0)
	WallSplineArr.Add(NewObject<AWallSpline>(this));

}

void AWallBuilderController::DeleteAll()
{
	for (int i = 0; i < WallSplineArr.Num(); i++) {
		WallSplineArr[i]->Destroy();
	}
	WallSplineArr.Empty();
	WallSplineArr.Add(NewObject<AWallSpline>(this));
}

void AWallBuilderController::OnLeftClick(const FInputActionValue& ActionValue)
{
	FHitResult HitResult;
	GetHitResultUnderCursor(ECC_Visibility, true, HitResult);

	if (HitResult.bBlockingHit)
	{
		FVector ClickLocation = HitResult.Location;
		/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Click Location: %s"), *ClickLocation.ToString()));*/

		WallSplineArr[WallSplineArr.Num() - 1]->SetPointLocation(ClickLocation);
		if(WallSplineArr[WallSplineArr.Num() - 1]->SpileArr.Num()==1)
			WallConstructionDelegate.ExecuteIfBound("Construction Statrted");
	}
}

void AWallBuilderController::OnRightClick(const FInputActionValue& ActionValue)
{
	if (WallSplineArr[WallSplineArr.Num() - 1]->NoOfSplinePoints>0) {
		AWallSpline* WallObj = NewObject<AWallSpline>(this);
		WallSplineArr.Add(WallObj);
		WallConstructionDelegate.ExecuteIfBound(FString("Construction Completed"));
	}
}

void AWallBuilderController::Undo(const FInputActionValue& ActionValue)
{
	if(WallSplineArr.Num()){
		if (WallSplineArr[WallSplineArr.Num() - 1]->SpileArr.Num() > 0) {
			WallSplineArr[WallSplineArr.Num() - 1]->SplineComponent->RemoveSplinePoint(WallSplineArr[WallSplineArr.Num() - 1]->SpileArr.Num());
			WallSplineArr[WallSplineArr.Num() - 1]->SpileArr[WallSplineArr[WallSplineArr.Num() - 1]->SpileArr.Num() - 1]->DestroyComponent();
			WallSplineArr[WallSplineArr.Num() - 1]->SpileArr.RemoveAt(WallSplineArr[WallSplineArr.Num() - 1]->SpileArr.Num() - 1);
		}
		else {
			WallSplineArr[WallSplineArr.Num() - 1]->SplineComponent->ClearSplinePoints();
			WallSplineArr[WallSplineArr.Num() - 1]->SpileArr.Empty();
			WallSplineArr.RemoveAt(WallSplineArr.Num() - 1);
		}
	}
	else {
		WallSplineArr.Add(NewObject<AWallSpline>(this));
	}
}

void AWallBuilderController::OnLeft(const FInputActionValue& ActionValue)
{
	if (CurrWall != 0) {
		CurrWall--;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, "No more prwevious wall press right");
	}
}

void AWallBuilderController::OnRight(const FInputActionValue& ActionValue)
{
	if (CurrWall != WallSplineArr.Num()-1) {
		CurrWall++;
	}
	else {
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Emerald, "No more next wall press left");
	}
}


