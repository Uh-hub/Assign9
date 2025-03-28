#include "BaseballGStateBase.h"
#include "BaseBallPlayerController.h"
#include "GameFramework/PlayerState.h"


void ABaseballGStateBase::MulticastShowResult_Implementation(const FString& choice, const FString& result)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Choice :%s, Result: %s"), *choice, *result));
	}
	UE_LOG(LogTemp, Warning, TEXT("Choice: %s, Result: %s"), *choice, *result);


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


