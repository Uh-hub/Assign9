#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "BaseballGameUI.generated.h"


UCLASS()
class NETWORKMINIGAME_API UBaseballGameUI : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, Category = "Baseball UI")
	bool bIsMyTurnUI = false;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RoundText = nullptr;
	


	void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	ESlateVisibility GetInputVisibility() const;

	UFUNCTION(BlueprintCallable)
	void UpdateRoundCount(int32 CurrentRound, int32 MaxRound);

	void SetIsMyTurn(bool bNewTurn);
};
