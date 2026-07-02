#include "BaseballGameUI.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Border.h"
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
	//GetInputVisibility();


	if (InputBlocker)
	{
		if (bIsMyTurnUI)
		{
			InputBlocker->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			InputBlocker->SetVisibility(ESlateVisibility::Visible);
		}
	}
}


//ESlateVisibility UBaseballGameUI::GetInputVisibility() const
//{
//	//if (GEngine)
//	//{
//	//	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
//	//		FString::Printf(TEXT("GameUI_SlateVisibility :%d"), bIsMyTurnUI ? TEXT("true") : TEXT("false")));
//	//}
//	return bIsMyTurnUI ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
//}

void UBaseballGameUI::UpdateRoundCount(int32 CurrentRound, int32 MaxRound)
{
	if (RoundText)
	{
		FString Str = FString::Printf(TEXT("%d / %d"), CurrentRound, MaxRound);
		RoundText->SetText(FText::FromString(Str));
	}
}


