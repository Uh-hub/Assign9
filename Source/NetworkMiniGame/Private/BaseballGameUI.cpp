#include "BaseballGameUI.h"
#include "Kismet/GameplayStatics.h"
#include "BaseBallPlayerController.h"


void UBaseballGameUI::NativeConstruct()
{
	Super::NativeConstruct();


	if (RoundText)
	{
		UE_LOG(LogTemp, Warning, TEXT("RoundText is valid!"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("RoundText is NULL! Check widget!"));
	}
}



void UBaseballGameUI::SetIsMyTurn(bool bNewTurn)
{
	bIsMyTurnUI = bNewTurn;
	GetInputVisibility();
}


ESlateVisibility UBaseballGameUI::GetInputVisibility() const
{
	return bIsMyTurnUI ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
}

void UBaseballGameUI::UpdateRoundCount(int32 CurrentRound, int32 MaxRound)
{
	if (RoundText)
	{
		FString Str = FString::Printf(TEXT("%d / %d"), CurrentRound, MaxRound);
		RoundText->SetText(FText::FromString(Str));
	}
}


