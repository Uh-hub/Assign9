#include "BaseballGStateBase.h"
#include "BaseBallPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

//변수 Replicate 처리
//GStateBase에서 서버와 클라이언트 모두 참고해야 하는 변수 선언 후
//서버인 GModeBase에서 Setter로 RoundCnt(라운드 수)를 세팅하고, 
//클라이언트는 Getter 함수로 해당 변수들의 값을 받아 UI를 변경함
void ABaseballGStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABaseballGStateBase, RoundCnt);
}

//서버가 RoundCnt를 바꾸면 OnRep_RoundCnt 함수를 호출해서 UI를 업데이트함
void ABaseballGStateBase::SetRoundCnt(int32 NewCnt)
{
	//서버인 경우에만 가능
	if (HasAuthority())
	{
		RoundCnt = NewCnt;
		OnRep_RoundCnt();
	}
}

void ABaseballGStateBase::OnRep_RoundCnt()
{
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (ABaseBallPlayerController* MyPC = Cast<ABaseBallPlayerController>(PC))
	{
		MyPC->UpdateUI(RoundCnt, MaxTries);
	}
}


void ABaseballGStateBase::MulticastShowResult_Implementation(const FString& choice, const FString& result)
{
	if (GEngine)
	{
		if (result.Contains("WIN") || result.Contains("LOSE") || result.Contains("DRAW"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("[GS공지] 게임 결과: %s"), *result));
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("[GS공지] 입력 :%s, 결과: %s"), *choice, *result));

		}


	}
}

void ABaseballGStateBase::ClientShowResult_Implementation(const FString& choice, const FString& result)
{
	if (GEngine)
	{
        if (result.Contains("WIN") || result.Contains("LOSE") || result.Contains("DRAW"))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Game Result: %s"), *result));
        }
        else {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Choice: %s, Result: %s"), *choice, *result));
        }
	}
	UE_LOG(LogTemp, Warning, TEXT("GStateBase_ClientShowResult_Choice: %s, Result: %s"), *choice, *result);
}



