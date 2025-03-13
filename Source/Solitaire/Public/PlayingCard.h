// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PaperSpriteActor.h"
#include "PaperSpriteComponent.h"

#include "GameFramework/Actor.h"
#include "PlayingCard.generated.h"

UCLASS()
class SOLITAIRE_API APlayingCard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayingCard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void SetFace(int index);
	int value;
	bool isFaceDown;
	FVector fromVec;
	FVector toVec;
	float lerpValue = 2;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void MoveFromTo(FVector from, FVector to);
	int GetValue();
	void SetValue(int index);
	bool IsFaceUp();
	void SetFaceUp(bool faceUp);
	UPROPERTY(EditAnywhere, Category = "Variables")
	UPaperSpriteComponent* spriteVisual;
	UPROPERTY(EditAnywhere, Category = "Variables")
	TArray<UPaperSprite*> cardFaces;
};
