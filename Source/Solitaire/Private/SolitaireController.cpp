// Fill out your copyright notice in the Description page of Project Settings.


#include "SolitaireController.h"

// Sets default values
ASolitaireController::ASolitaireController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASolitaireController::BeginPlay()
{
	Super::BeginPlay();
	
	for (int i = 0; i < 7; i++) {
		columns.Add(FColumn());
		if (i < 4) {
			rightPiles.Add(FColumn());
		}
	}
}

// Called every frame
void ASolitaireController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (isMoving) {
		// Move held card to mouse pointer
		FVector worldLocation;
		FVector worldDirection;
		GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(worldLocation, worldDirection);
		movingCard->SetActorLocation(FVector((worldLocation.X - mouseStartPos.X) + cardStartPos.X, 9, (worldLocation.Z - mouseStartPos.Z) + cardStartPos.Z));
	}
}

void ASolitaireController::AssignWidget(UUIBehaviour* widget) {
	ui = widget;
}

void ASolitaireController::StartGame(bool replay) {
	if (replay) {
		FMath::RandInit(seed);
	}
	else {
		seed = FDateTime::Now().GetTicks() % INT_MAX;
		//seed = 1803062870; // DEBUG SEED
		FMath::RandInit(seed);
	}
	ClearBoard();
	GenerateBoard();
}

void ASolitaireController::SetGameRules(int allowedPasses, int cardTurns) {
	maxPasses = allowedPasses;
	turns = cardTurns;
}

void ASolitaireController::ClearBoard() {
	pile.Empty();
	for (int i = 0; i < 7; i++) {
		columns[i].cards.Empty();
		if (i < 4) {
			rightPiles[i].cards.Empty();
		}
	}
	isMoving = false;
	movingCard = NULL;
	pilePointer = 0;
	TArray<AActor*> cards;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayingCard::StaticClass(), cards);
	for (AActor* card : cards) {
		card->Destroy();
	}
	passesLeft = maxPasses;
	moveCount = 0;
	ui->SetPassCount(maxPasses);
}

void ASolitaireController::GenerateBoard() {
	// Create array of possible card indexes 0->51
	TArray<int> cardIndexes;
	for (int i = 0; i < 52; i++) {
		cardIndexes.Add(i);
	}
	// Populate left pile
	for (int i = 0; i < 24; i++) {
		int randomIndex = FMath::RandRange(0, cardIndexes.Num() - 1);
		FTransform pos(FQuat::Identity, FVector(350, 33 - i, 140), FVector(1));
		APlayingCard* card = GetWorld()->SpawnActor<APlayingCard>(cardBP, pos, params);
		card->SetValue(cardIndexes[randomIndex]);
		card->SetFaceUp(false);
		pile.Add(card);
		cardIndexes.RemoveAt(randomIndex);
	}
	// Populate columns
	for (int cardNum = 1; cardNum < 8; cardNum++) {
		for (int i = 0; i < cardNum; i++) {
			int randomIndex = FMath::RandRange(0, cardIndexes.Num() - 1);
			FTransform pos(FQuat::Identity, FVector(0), FVector(1));
			APlayingCard* card = GetWorld()->SpawnActor<APlayingCard>(cardBP, pos, params);
			card->MoveFromTo(FVector(350, 33, 140), FVector((320 - (80 * cardNum)), (33 - i), (140 - (12 * i))));
			card->SetValue(cardIndexes[randomIndex]);
			if (i == (cardNum - 1)) {
				card->SetFaceUp(true);
			}
			else {
				card->SetFaceUp(false);
			}
			columns[cardNum - 1].Add(card);
			cardIndexes.RemoveAt(randomIndex);
		}
	}
}

void ASolitaireController::ActorClicked(AActor* actor) {
	if (APlayingCard* card = Cast<APlayingCard>(actor)) {
		if (card->IsFaceUp()) {
			movingCard = card;
			FVector worldLocation;
			FVector worldDirection;
			GetWorld()->GetFirstPlayerController()->DeprojectMousePositionToWorld(worldLocation, worldDirection);
			mouseStartPos = FVector(worldLocation.X, 0, worldLocation.Z);
			cardStartPos = movingCard->GetActorLocation();
			isMoving = true;
		}
	}
}

void ASolitaireController::MouseReleased() {
	if (isMoving) {
		// Find closest card place point
		FVector location = movingCard->GetActorLocation();
		if (location.X < 295) {
			if (location.X < -295) {
				// Over right piles
				float maxDist = 999;
				int pileIndex = 0;
				for (int i = 0; i < 4; i++) {
					float newDist = abs(location.Z - pileZs[i]);
					if (newDist < maxDist) {
						maxDist = newDist;
						pileIndex = i;
					}
				}
				// Check if card suit matches and value 1 above
				if (IsRightPileIncrement(movingCard, pileIndex)) {
					// Detatch from parent
					if (movingCard->GetAttachParentActor() != NULL) {
						movingCard->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
					}
					// Move Card
					movingCard->SetActorLocation(FVector(-350, (22 - rightPiles[pileIndex].Num()), pileZs[pileIndex]));
					// Update card lists
					RemoveFromLists(movingCard);
					rightPiles[pileIndex].Add(movingCard);
					FlipBottomCards();
					moveCount++;
					CheckForCompletable();
					// Check if game complete
					if (rightPiles[0].Num() + rightPiles[1].Num() + rightPiles[2].Num() + rightPiles[3].Num() == 52) {
						// Game won, update ui
						ui->ShowWinText(moveCount);
					}
				}
				else {
					movingCard->SetActorLocation(cardStartPos);
				}
			}
			else {
				// Over columns
				float maxDist = 999;
				int columnIndex = 0;
				int columnXs[7] = { 240, 160, 80, 0, -80, -160, -240 };
				for (int i = 0; i < 7; i++) {
					float newDist = abs(location.X - columnXs[i]);
					if (newDist < maxDist) {
						maxDist = newDist;
						columnIndex = i;
					}
				}
				// Check if card opposite colour and value 1 below
				if (IsColumnDecrement(movingCard, columnIndex)) {
					// Detatch from parent
					if (movingCard->GetAttachParentActor() != NULL) {
						movingCard->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
					}
					// Move Card
					movingCard->SetActorLocation(FVector(columnXs[columnIndex], (33 - columns[columnIndex].Num()), (140 - (12 * columns[columnIndex].Num()))));
					// Attatch to new parent
					if (columns[columnIndex].Num() > 0) {
						movingCard->AttachToActor(columns[columnIndex].cards.Last(0), FAttachmentTransformRules::KeepWorldTransform);
					}
					// Update card lists
					RemoveFromLists(movingCard);
					columns[columnIndex].Add(movingCard);
					TArray<AActor*> children;
					movingCard->GetAttachedActors(children, true, true);
					for (AActor* child : children) {
						if (APlayingCard* childCard = Cast<APlayingCard>(child)) {
							columns[columnIndex].Add(childCard);
						}
					}
					FlipBottomCards();
					moveCount++;
					CheckForCompletable();
				}
				else {
					movingCard->SetActorLocation(cardStartPos);
				}
			}
		}
		else {
			movingCard->SetActorLocation(cardStartPos);
		}
		isMoving = false;
		movingCard = NULL;
	}
}

void ASolitaireController::PileClicked() {
	int pileNum = pile.Num();
	if (pileNum > 0) {
		if (pilePointer >= pileNum) {
			if (maxPasses == -1 || (maxPasses > 0 && passesLeft > 0)) {
				pilePointer = 0;
				for (int i = 0; i < pileNum; i++) {
					pile[i]->SetActorLocation(FVector(350, 33 - i, 140));
					pile[i]->SetFaceUp(false);
				}
				passesLeft = FMath::Clamp(passesLeft - 1, -1, 3);
				ui->SetPassCount(passesLeft);
				moveCount++;
			}
		}
		else {
			for (int t = 0; t < turns; t++) {
				if (pilePointer < pileNum) {
					pile[pilePointer]->SetActorLocation(FVector(350, pile[pilePointer]->GetActorLocation().Y, 45));
					pile[pilePointer]->SetFaceUp(true);
					pilePointer++;
				}
				else {
					break;
				}
			}
			moveCount++;
		}
	}
}

void ASolitaireController::RemoveFromLists(APlayingCard* card) {
	// Remove card
	if (pile.Remove(card) > 0) {
		pilePointer--;
	}
	for (int c = 0; c < 7; c++) {
		columns[c].cards.Remove(card);
	}
	for (int p = 0; p < 4; p++) {
		rightPiles[p].cards.Remove(card);
	}
	// Remove children
	TArray<AActor*> children;
	card->GetAttachedActors(children);
	if (children.Num() > 0) {
		if (APlayingCard* childCard = Cast<APlayingCard>(children[0])) {
			RemoveFromLists(childCard);
		}
	}
}

void ASolitaireController::FlipBottomCards() {
	for (int c = 0; c < 7; c++) {
		if (columns[c].Num() > 0) {
			if (!columns[c].cards.Last(0)->IsFaceUp()) {
				columns[c].cards.Last(0)->SetFaceUp(true);
			}
		}
	}
}

void ASolitaireController::CheckForCompletable() {
	// Check if pile empty
	if (pile.Num() == 0) {
		// Check if all cards are turned over
		bool anyFaceDown = false;
		for (int c = 0; c < 7; c++) {
			if (columns[c].Num() > 0) {
				if (!columns[c][0]->IsFaceUp()) {
					anyFaceDown = true;
				}
			}
		}
		if (!anyFaceDown) {
			ui->ShowAutoComplete();
		}
	}
}

void ASolitaireController::AutoComplete() {
	APlayingCard* card;
	for (int c = 0; c < 7; c++) {
		for (int i = 0; i < columns[c].Num(); i++) {
			card = columns[c][i];
			// Detatch from parent
			if (card->GetAttachParentActor() != NULL) {
				card->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			}
			// Move to piles
			card->MoveFromTo(card->GetActorLocation(), FVector(-350, 22 - GetNumber(card), pileZs[GetSuit(card)]));
			moveCount++;
		}
	}
	ui->ShowWinText(moveCount);
}

bool ASolitaireController::IsRightPileIncrement(APlayingCard* card, int index) {
	// Check if single card
	if (card->Children.Num() == 0) {
		// Check if right suit
		if (GetSuit(card) == index) {
			// Check if right number
			int lastCardNum = (rightPiles[index].Num() == 0) ? -1 : GetNumber(rightPiles[index].cards.Last(0));
			if (GetNumber(card) - 1 == lastCardNum) {
				return true;
			}
		}
	}
	return false;
}

bool ASolitaireController::IsColumnDecrement(APlayingCard* card, int index) {
	// Check if opposite colour
	if (columns[index].Num() == 0 || GetColour(columns[index].cards.Last(0)) != GetColour(card)) {
		// Check if right number
		int lastCardNum = (columns[index].Num() == 0) ? 13 : GetNumber(columns[index].cards.Last(0));
		if (GetNumber(card) + 1 == lastCardNum) {
			return true;
		}
	}
	return false;
}

int ASolitaireController::GetSuit(APlayingCard* card) {
	return card->GetValue() / 13;
}

int ASolitaireController::GetNumber(APlayingCard* card) {
	return card->GetValue() % 13;
}

int ASolitaireController::GetColour(APlayingCard* card) {
	return card->GetValue() / 26;
}