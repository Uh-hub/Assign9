#include "BaseballGameUI.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBallPlayerController.h"

void UBaseballGameUI::SetIsMyTurn(bool bNewTurn)
{
	bIsMyTurnUI = bNewTurn;
	GetInputVisibility();
}


//testestestestsetstsetstestsetsetetstesteetesetestetestetest
//test one more 
ESlateVisibility UBaseballGameUI::GetInputVisibility() const
{
	return bIsMyTurnUI ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
}