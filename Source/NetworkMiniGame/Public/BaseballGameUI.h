#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BaseballGameUI.generated.h"


UCLASS()
class NETWORKMINIGAME_API UBaseballGameUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Baseball UI")
	bool bIsMyTurnUI = false;

	void SetIsMyTurn(bool bNewTurn);

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetInputVisibility() const;


};
