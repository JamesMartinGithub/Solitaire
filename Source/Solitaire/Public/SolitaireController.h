// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "PlayingCard.h"
#include "UIBehaviour.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

#include "GameFramework/Actor.h"
#include "SolitaireController.generated.h"

USTRUCT()
struct FColumn {
	GENERATED_BODY()
public:
	TArray<APlayingCard*> cards;

	APlayingCard* operator[] (int i) {
		return cards[i];
	}

	void Add(APlayingCard* card) {
		cards.Add(card);
	}

	int Num() {
		return cards.Num();
	}
};

UCLASS()
class SOLITAIRE_API ASolitaireController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASolitaireController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	FActorSpawnParameters params;
	void ClearBoard();
	void GenerateBoard();
	void RemoveFromLists(APlayingCard*);
	void FlipBottomCards();
	void CheckForCompletable();
	bool IsRightPileIncrement(APlayingCard*, int);
	bool IsColumnDecrement(APlayingCard*, int);
	int GetSuit(APlayingCard*);
	int GetNumber(APlayingCard*);
	int GetColour(APlayingCard*);
	UUIBehaviour* ui;
	int seed;
	int maxPasses = -1;
	int passesLeft = -1;
	int turns = 1;
	int moveCount = 0;
	TArray<APlayingCard*> pile;
	TArray<FColumn> columns;
	TArray<FColumn> rightPiles;
	bool isMoving = false;
	APlayingCard* movingCard;
	FVector mouseStartPos;
	FVector cardStartPos;
	int pilePointer = 0;
	int pileZs[4] = { 140, 40, -60, -160 };
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable)
	void AssignWidget(UUIBehaviour* widget);
	void ActorClicked(AActor* actor);
	void MouseReleased();
	UFUNCTION(BlueprintCallable)
	void PileClicked();
	UFUNCTION(BlueprintCallable)
	void StartGame(bool replay);
	UFUNCTION(BlueprintCallable)
	void SetGameRules(int passes, int turns);
	UFUNCTION(BlueprintCallable)
	void AutoComplete();
	UPROPERTY(EditAnywhere, Category = "Variables")
	TSubclassOf<APlayingCard> cardBP;
};
