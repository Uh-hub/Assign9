#include "BaseballGModeBase.h"
#include "BaseballGStateBase.h"
#include "Net/UnrealNetwork.h"
#include "BaseballPlayerState.h"

void ABaseballGModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseballGModeBase, CurrentPlayerIndex);
	DOREPLIFETIME(ABaseballGModeBase, answer);
	DOREPLIFETIME(ABaseballGModeBase, count);
	DOREPLIFETIME(ABaseballGModeBase, bIsGameOver);


}

void ABaseballGModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	ABaseBallPlayerController* PC = Cast<ABaseBallPlayerController>(NewPlayer);
	if (PC)
	{
		PlayerControllers.Add(PC);
		PC->PlayerIndex = PlayerControllers.Num()-1;
		UE_LOG(LogTemp, Warning, TEXT("Player %d joined! Total Players: %d"), PC->PlayerIndex, PlayerControllers.Num());
		if (PlayerControllers.Num() == 2)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABaseballGModeBase::StartTurn);
		}
	}
	
}



void ABaseballGModeBase::BeginPlay()
{
	Super::BeginPlay();
	//서버에서만 실행
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay() 실행됨"));

	answer = GenerateRandomNumber();
	UE_LOG(LogTemp, Warning, TEXT("정답 : %s"), *answer);

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay!"));

	if (PlayerControllers.Num() > 0)
	{
		StartTurn();
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No PlayerControllers available in BeginPlay!"));
	}
	
	
}

void ABaseballGModeBase::StartTurn()
{
	if (PlayerControllers.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("No PlayerControllers available in StartTurn!"));
		return;
	}
	CurrentPC = PlayerControllers[CurrentPlayerIndex];
	if (CurrentPC)
	{
		CurrentPC->SetIsMyTurn(true);
		CurrentPC->OnRep_IsMyTurn();
		UE_LOG(LogTemp, Warning, TEXT("Player %d's turn!"), CurrentPC->PlayerIndex);
	}
	for (auto PC : PlayerControllers)
	{
		if (PC != CurrentPC)
		{
			PC->SetIsMyTurn(false);
			PC->OnRep_IsMyTurn();
		}
	}

	
}

int32 ABaseballGModeBase::GetPlayerIndex(ABaseBallPlayerController* PC) const
{
	return PlayerControllers.Find(PC);
}

int32 ABaseballGModeBase::GetCurrentPlayerIndex() const
{
	return CurrentPlayerIndex;
}

void ABaseballGModeBase::CheckChoice(const FString& choice)
{
	//GameModeBase에서 정답 여부 확인
	FString result = CalculateResult(choice);

	//정답 여부 확인 결과 GameStateBase로 전달
	ABaseballGStateBase* GS = GetGameState<ABaseballGStateBase>();
	if (GS)
	{
		//결과를 multicast 
		//GS->MulticastShowResult(choice, result);
		if (CurrentPC)
		{
			GS->ClientShowResult(choice, result);
		}
	}
	if (result.Contains("3S 0B 0OUT"))
	{
		GameResult(choice, TEXT("WIN"));
		return;
	}

	NextTurn();

}

void ABaseballGModeBase::NextTurn()
{
	CurrentPlayerIndex++;
	

	if (CurrentPlayerIndex >= PlayerControllers.Num())
	{
		CurrentPlayerIndex = 0;
		count++;
		UE_LOG(LogTemp, Warning, TEXT("Current round : %d"), count);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Current Round: %d"), count));
		}
	}
	if (count > MaxTries)
	{
		GameResult("END", "END");
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("NextTurn: CurrentPlayerIndex = %d, PlayerControllers.Num() = %d"), CurrentPlayerIndex, PlayerControllers.Num());
	StartTurn();
	Multicast_UpdateTurnUI();
}


void ABaseballGModeBase::Multicast_UpdateTurnUI_Implementation()
{
	if (ABaseBallPlayerController* BaseballPC = PlayerControllers[CurrentPlayerIndex])
	{
		UE_LOG(LogTemp, Warning, TEXT("IsMyTurn Called from GameMode!"));
		BaseballPC->UpdateUI();

	}
}


FString ABaseballGModeBase::GenerateRandomNumber()
{
	//3자리 숫자 랜덤 생성
	FString result;
	while (result.Len() < 3)
	{
		int32 num = FMath::RandRange(1, 9);
		if (!result.Contains(FString::FromInt(num)))
		{
			result += FString::FromInt(num);
		}

	}
	return result;
}


FString ABaseballGModeBase::CalculateResult(const FString& choice)
{
	//유효하지 않은 값(중복, 3자리 초과 또는 미만 = OUT)
	//유효한 값이라면
	  //S : 자리O, 값O
	  //B : 자리X, 값O
	  //OUT : 자리X, 값X
	  //결과 : 1S2B
	if (choice.Len() != 3)
	{
		return FString::Printf(TEXT("GMB_CalculateResult : 0S 0B 3OUT"));;
	}
	int32 strike = 0, ball = 0, out = 0;
	for (int32 i = 0; i < choice.Len(); i++)
	{
		if (choice[i] == answer[i])
		{
			strike++;
		}
		else if (answer.Contains(FString(1, &choice[i])))
		{
			ball++;
		}
		else
		{
			out++;
		}
	}
	return FString::Printf(TEXT("%dS %dB %dOUT"), strike, ball, out);
}

void ABaseballGModeBase::HandleGameEnd(APlayerController* Winner)
{
	for (ABaseBallPlayerController* PC : PlayerControllers)
	{
		UE_LOG(LogTemp, Warning, TEXT("HandleGameEnd is running on %s"),
			HasAuthority() ? TEXT("SERVER") : TEXT("CLIENT"));
		ABaseballPlayerState* PlayerState = PC->GetPlayerState<ABaseballPlayerState>();
		if (PlayerState)
		{
			if (PC == Winner)
			{
				PlayerState->SetGameResult(TEXT("WIN"));
			}
			else
			{
				PlayerState->SetGameResult(TEXT("LOSE"));
			}
		}
	}
}


void ABaseballGModeBase::GameResult(const FString& choice, const FString& Result)
{
	ABaseballGStateBase* GS = GetGameState<ABaseballGStateBase>();
	if (!GS) return;

	if (Result.Contains("WIN"))
	{
		
		if (PlayerControllers.IsValidIndex(CurrentPlayerIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("[GameResult]playercontroller IsValid!!!!!!"));
			ABaseBallPlayerController* WinnerPlayerController = PlayerControllers[CurrentPlayerIndex];
			if (WinnerPlayerController)
			{
				if (WinnerPlayerController)
				{
					HandleGameEnd(WinnerPlayerController);
					UE_LOG(LogTemp, Warning, TEXT("[GameResult]CurrentPlayerIndex : %d"), CurrentPlayerIndex);
					//WinnerPlayerController->ClientShowResult(choice, TEXT("WIN"));
				}
				
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("[GameResult]WinnerPlayerController Not Valid!!!!!!"));
			}
			
		}
		
	}
	else if (Result.Contains("END"))
	{
		for (auto* PC : PlayerControllers)
		{
			if (PC->IsLocalPlayerController())
			{
				GS->ClientShowResult(choice, TEXT("DRAW"));
			}
		}
	}
	EndGame();
}

void ABaseballGModeBase::EndGame()
{
	bIsGameOver = true;
	for (ABaseBallPlayerController* PC : PlayerControllers)
	{
		PC->SetIsMyTurn(false);
	}
}

