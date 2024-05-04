// Fill out your copyright notice in the Description page of Project Settings.


#include "TopDownPawn.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"


// Sets default values
ATopDownPawn::ATopDownPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	SpringArmComponent->TargetArmLength = -211.599960;
	SpringArmComponent->TargetOffset = FVector(-520, 0, 540);
	SpringArmComponent->SetupAttachment(RootComponent);
	float Angle = FMath::RadiansToDegrees(atan(SpringArmComponent->TargetOffset.Z / SpringArmComponent->TargetArmLength));
	



	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetRelativeRotation(FRotator(Angle, 0, 0));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);


	Movement = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("Floating Movement"));
}

// Called when the game starts or when spawned
void ATopDownPawn::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATopDownPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

static void KeyMap(UInputMappingContext* InputMappingContext, UInputAction* InputAction, FKey Key,
	bool bNegate = false,
	bool bSwizzle = false, EInputAxisSwizzle SwizzleOrder = EInputAxisSwizzle::YXZ)
{
	FEnhancedActionKeyMapping& Mapping = InputMappingContext->MapKey(InputAction, Key);
	UObject* Outer = InputMappingContext->GetOuter();

	if (bNegate)
	{
		UInputModifierNegate* Negate = NewObject<UInputModifierNegate>(Outer);
		Mapping.Modifiers.Add(Negate);
	}

	if (bSwizzle)
	{
		UInputModifierSwizzleAxis* Swizzle = NewObject<UInputModifierSwizzleAxis>(Outer);
		Swizzle->Order = SwizzleOrder;
		Mapping.Modifiers.Add(Swizzle);
	}
}
// Called to bind functionality to input
void ATopDownPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PawnMapping = NewObject<UInputMappingContext>(this);

	MoveAction = NewObject<UInputAction>(this);
	MoveAction->ValueType = EInputActionValueType::Axis2D;
	KeyMap(PawnMapping, MoveAction, EKeys::W);
	KeyMap(PawnMapping, MoveAction, EKeys::S,true);
	KeyMap(PawnMapping, MoveAction, EKeys::A, true,true);
	KeyMap(PawnMapping, MoveAction, EKeys::D, false, true);


	ZoomAction = NewObject<UInputAction>(this);
	ZoomAction->ValueType = EInputActionValueType::Axis1D;
	KeyMap(PawnMapping, ZoomAction, EKeys::MouseWheelAxis);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	APlayerController* TDPC = Cast<APlayerController>(Controller);

	check(EIC)


		EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownPawn::Move);
	    EIC->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &ATopDownPawn::Zoom);


		ULocalPlayer* LocalPlayer = TDPC->GetLocalPlayer();
		check(LocalPlayer);

		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		check(Subsystem);

		
		Subsystem->AddMappingContext(PawnMapping, 0);

}

void ATopDownPawn::Move(const FInputActionValue& ActionValue)
{
	FVector value = ActionValue.Get<FInputActionValue::Axis3D>();
	value.Z = 0;

	AddMovementInput(value, 2);
	

}

void ATopDownPawn::Zoom(const FInputActionValue& ActionValue)
{

	/*float value = ActionValue.Get<float>();

	float length = FMath::Clamp(SpringArmComponent->TargetArmLength - (value * 100), 20, INFINITY);
	SpringArmComponent->TargetArmLength = length;

	float rotation = FMath::Clamp(SpringArmComponent->GetRelativeRotation().Pitch + (value * 5), -80.0f, -10.0f);
	SpringArmComponent->SetRelativeRotation(FRotator(rotation, 0, 0));*/

	float Value = ActionValue.Get<float>();
	float TargetArmLength;
	float TargetOffsetZ;
	if(SpringArmComponent)
	 TargetArmLength = SpringArmComponent->TargetArmLength;
	if(SpringArmComponent)
	 TargetOffsetZ = SpringArmComponent->TargetOffset.Z;

	if (TargetArmLength >= -460) {
		SpringArmComponent->TargetOffset.Z -= Value * 20;
	}
	if (TargetOffsetZ <= 1000) {
		SpringArmComponent->TargetArmLength -= Value * 9.2;
	}

	SpringArmComponent->TargetArmLength = FMath::Clamp(SpringArmComponent->TargetArmLength, -600, -0);
	SpringArmComponent->TargetOffset.Z = FMath::Clamp(SpringArmComponent->TargetOffset.Z, 125, 3500);


	float Angle;
	if (SpringArmComponent->TargetArmLength == 0) {
		Angle = -90;
	}
	else {
		Angle = FMath::RadiansToDegrees(atan(SpringArmComponent->TargetOffset.Z / SpringArmComponent->TargetArmLength));
		Angle = FMath::Clamp(Angle, -90, 0);
	}



	CameraComponent->SetRelativeRotation(FRotator(Angle, 0, 0));

	/*GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("ROTATION: %f"), *Camera->GetRelativeRotation().ToString()));*/
}

