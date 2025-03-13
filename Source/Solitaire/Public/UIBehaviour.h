// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UIBehaviour.generated.h"

/**
 * 
 */
UCLASS()
class SOLITAIRE_API UUIBehaviour : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetPassCount(int count);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowWinText(int moveCount);
	UFUNCTION(BlueprintImplementableEvent)
	void ShowAutoComplete();
};
