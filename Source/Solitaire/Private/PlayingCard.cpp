// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayingCard.h"

// Sets default values
APlayingCard::APlayingCard()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APlayingCard::BeginPlay()
{
	Super::BeginPlay();

	spriteVisual = GetComponentByClass<UPaperSpriteComponent>();
	spriteVisual->SetSprite(cardFaces[0]);
}

// Called every frame
void APlayingCard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (lerpValue < 1) {
		SetActorLocation(FMath::Lerp(fromVec, toVec, lerpValue));
		lerpValue += 0.07 * (1.1 - lerpValue);
		if (lerpValue >= 1) {
			lerpValue = 2;
			SetActorLocation(toVec);
		}
	}
}

void APlayingCard::MoveFromTo(FVector from, FVector to) {
	fromVec = from;
	toVec = to;
	lerpValue = 0.05;
}

void APlayingCard::SetFace(int index) {
	spriteVisual->SetSprite(cardFaces[index]);
}

int APlayingCard::GetValue() {
	return value;
}

void APlayingCard::SetValue(int index) {
	value = index;
}

bool APlayingCard::IsFaceUp() {
	return isFaceDown ? false : true;
}

void APlayingCard::SetFaceUp(bool faceUp) {
	if (faceUp) {
		SetFace(value);
		isFaceDown = false;
	}
	else {
		SetFace(52);
		isFaceDown = true;
	}
}