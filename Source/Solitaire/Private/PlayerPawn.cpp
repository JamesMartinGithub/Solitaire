// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawn.h"

// Sets default values
APlayerPawn::APlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Get controller after delay
	FTimerHandle unusedHandle;
	GetWorldTimerManager().SetTimer(unusedHandle, this, &APlayerPawn::AssignController, 0.2f, false);
}

void APlayerPawn::AssignController() {
	if (AActor* controllerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ASolitaireController::StaticClass())) {
		solController = Cast<ASolitaireController>(controllerActor);
	}
}

// Called every frame
void APlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAction("Mouse0", IE_Pressed, this, &APlayerPawn::Mouse0Pressed);
	InputComponent->BindAction("Mouse0", IE_Released, this, &APlayerPawn::Mouse0Released);
	EnableInput(GetWorld()->GetFirstPlayerController());
}

void APlayerPawn::Mouse0Pressed()
{
	if (solController && interactable == true) {
		FHitResult resultt;
		GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, resultt);
		if (resultt.GetActor()) {
			solController->ActorClicked(resultt.GetActor());
		}
	}
}

void APlayerPawn::Mouse0Released() {
	if (solController && interactable == true) {
		solController->MouseReleased();
	}
}

void APlayerPawn::SetInteractable(bool canInteract) {
	interactable = canInteract;
}