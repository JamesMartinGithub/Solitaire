// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SolitaireController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"
#include "PlayerPawn.generated.h"

UCLASS()
class SOLITAIRE_API APlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APlayerPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void AssignController();
	bool interactable = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void Mouse0Pressed();
	void Mouse0Released();
	UFUNCTION(BlueprintCallable)
	void SetInteractable(bool canInteract);
	UPROPERTY(EditAnywhere, Category = "Variables")
	ASolitaireController* solController;
};
