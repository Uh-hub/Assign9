#include "BaseballGameUI.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBallPlayerController.h"

void UBaseballGameUI::SetIsMyTurn(bool bNewTurn)
{
	bIsMyTurnUI = bNewTurn;
	GetInputVisibility();
}


ESlateVisibility UBaseballGameUI::GetInputVisibility() const
{
	return bIsMyTurnUI ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
}