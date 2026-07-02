#include "BaseballGModeBase.h"
#include "BaseballGStateBase.h"
#include "Net/UnrealNetwork.h"
#include "BaseballPlayerState.h"
#include "BaseballGameUI.h"

void ABaseballGModeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseballGModeBase, CurrentPlayerIndex);
	DOREPLIFETIME(ABaseballGModeBase, answer);
	DOREPLIFETIME(ABaseballGModeBase, bIsGameOver);


}

void ABaseballGModeBase::BeginPlay()
{
	Super::BeginPlay();
	if (!HasAuthority()) return;

	UE_LOG(LogTemp, Warning, TEXT("GameMode BeginPlay() - Waiting for players..."));
}


void ABaseballGModeBase::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	

	ABaseBallPlayerController* PC = Cast<ABaseBallPlayerController>(NewPlayer);

	//로그로 누가 들어왔는지 확인
	//GetNumPlayers() : 현재 서버에 정상적으로 로그인한 플레이어 컨트롤러의 총개수를 반환함
	
	if (PC)
	{
		PlayerControllers.Add(PC);
		PC->PlayerIndex = PlayerControllers.Num()-1;
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
				FString::Printf(TEXT("Player %d Joined! Total Players: %d"), PC->PlayerIndex, PlayerControllers.Num()));
		}
		
		if (PlayerControllers.Num() == 2)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
					FString::Printf(TEXT("[GM_PL] 2명 접속 완료! 정답을 생성하고 게임을 시작합니다. ")));
			}

			answer = GenerateRandomNumber();
			UE_LOG(LogTemp, Warning, TEXT("Answer : %s"), *answer);

			CurrentPlayerIndex = 0;
			//안전하게 1초 후에 StartTurn 실행되도록 함
			FTimerHandle StartDelayHandle; 
			GetWorld()->GetTimerManager().SetTimer(StartDelayHandle, this, &ABaseballGModeBase::StartTurn, 1.0f, false);
		}
	}
	
}


void ABaseballGModeBase::StartTurn()
{
	if (PlayerControllers.Num() == 0)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
				FString::Printf(TEXT("No PlayerControllers available in StartTurn! ")));
		}
		//UE_LOG(LogTemp, Error, TEXT("No PlayerControllers available in StartTurn!"));
		return;
	}
	CurrentPC = PlayerControllers[CurrentPlayerIndex];
	if (CurrentPC)
	{
		CurrentPC->SetIsMyTurn(true);
		if (CurrentPC->IsLocalController())
		{
			CurrentPC->OnRep_IsMyTurn();
		}
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
				FString::Printf(TEXT("Player %d's turn!"), CurrentPC->PlayerIndex));
		}
		//UE_LOG(LogTemp, Warning, TEXT("Player %d's turn!"), CurrentPC->PlayerIndex);
	}
	for (auto PC : PlayerControllers)
	{
		if (PC != CurrentPC)
		{
			PC->SetIsMyTurn(false);
			if (PC->IsLocalController())
			{
				PC->OnRep_IsMyTurn();
			}
			
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
	
	FString result = CalculateResult(choice);


	ABaseballGStateBase* GS = GetGameState<ABaseballGStateBase>();
	if (GS)
	{
		//서버가 GameState에서 Multicast 진행
		//호출 시 서버와 모든 클라이언트의 화면에 로그가 출력됨
		GS->MulticastShowResult(choice, result);
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

		if (ABaseballGStateBase* GState = GetGameState<ABaseballGStateBase>())
		{
			int32 NewCount = GState->GetRoundCnt() + 1;
			GState->SetRoundCnt(NewCount);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Current Round: %d"), NewCount));
			}
			if (NewCount > GState->MaxTries)
			{
				GameResult("END", "END");
				return;
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("NextTurn: CurrentPlayerIndex = %d, PlayerControllers.Num() = %d"), CurrentPlayerIndex, PlayerControllers.Num());
	StartTurn();
}


FString ABaseballGModeBase::GenerateRandomNumber()
{
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
	if (choice.Len() != 3)
	{
		return FString::Printf(TEXT("GMB_CalculateResult : 0S 0B 3OUT (Over 3 letters)"));
	}

	if(!choice.IsNumeric())
	{
		return FString::Printf(TEXT("GMB_CalculateResult : 0S 0B 3OUT(Not a Number)"));
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
				GS->MulticastShowResult(choice, TEXT("DRAW"));
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

