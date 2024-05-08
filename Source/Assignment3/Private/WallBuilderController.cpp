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
	CurrWall = WallSplineArr.Num() - 1;
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
			WallSplineArr[CurrWall]->Destroy();
			WallSplineArr.RemoveAt(CurrWall);
			WallConstructionDelegate.ExecuteIfBound(FString("Current Wall Deleted"));

			if (CurrWall > 0)
				CurrWall;
	}
	if (WallSplineArr.Num() <= 0) {
	WallSplineArr.Add(NewObject<AWallSpline>(this));
	CurrWall = WallSplineArr.Num() - 1;
	WallConstructionDelegate.ExecuteIfBound(FString("Current Wall Deleted"));

	}
	if (CurrWall >= WallSplineArr.Num() - 1) {
		CurrWall = WallSplineArr.Num() - 1;
	}

}

void AWallBuilderController::DeleteAll()
{
	for (int i = 0; i < WallSplineArr.Num(); i++) {
		WallSplineArr[i]->Destroy();
	}
	WallConstructionDelegate.ExecuteIfBound(FString("All Walls Deleted"));

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

		WallSplineArr[CurrWall]->SetPointLocation(ClickLocation);
		if(WallSplineArr[CurrWall]->SpileArr.Num()==1)
			WallConstructionDelegate.ExecuteIfBound("Construction Statrted");
	}
}

void AWallBuilderController::OnRightClick(const FInputActionValue& ActionValue)
{
	if (WallSplineArr[CurrWall]->NoOfSplinePoints>0) {
		AWallSpline* WallObj = NewObject<AWallSpline>(this);
		WallSplineArr.Add(WallObj);
		CurrWall = WallSplineArr.Num() - 1;

		WallConstructionDelegate.ExecuteIfBound(FString("Construction Completed"));
	}
}

void AWallBuilderController::Undo(const FInputActionValue& ActionValue)
{
	if (CurrWall >= WallSplineArr.Num() - 1) {
		CurrWall = WallSplineArr.Num() - 1;
	}
	if(WallSplineArr.Num()){
		if (WallSplineArr[CurrWall]->SpileArr.Num() > 0) {
			WallSplineArr[CurrWall]->SplineComponent->RemoveSplinePoint(WallSplineArr[CurrWall]->SpileArr.Num());
			WallSplineArr[CurrWall]->SpileArr[WallSplineArr[CurrWall]->SpileArr.Num() - 1]->DestroyComponent();
			WallSplineArr[CurrWall]->SpileArr.RemoveAt(WallSplineArr[CurrWall]->SpileArr.Num() - 1);
		}
		else {
			WallSplineArr[CurrWall]->SplineComponent->ClearSplinePoints();
			WallSplineArr[CurrWall]->SpileArr.Empty();
			WallSplineArr.RemoveAt(CurrWall);
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
		WallConstructionDelegate.ExecuteIfBound(FString::Printf(TEXT("Now You Interact With Wall : %d"), CurrWall+1));

	}
	else {
		WallConstructionDelegate.ExecuteIfBound(FString("No more prwevious wall press right"));
	}
}

void AWallBuilderController::OnRight(const FInputActionValue& ActionValue)
{
	if (CurrWall != WallSplineArr.Num()-1) {
		CurrWall++;
		WallConstructionDelegate.ExecuteIfBound(FString::Printf(TEXT("Now You Interact With Wall : %d"), CurrWall + 1));

	}
	else {
		WallConstructionDelegate.ExecuteIfBound(FString("No more next wall press left"));

	}
}


