#include "BaseballPlayerState.h"
#include "Net/UnrealNetwork.h"


void ABaseballPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ABaseballPlayerState, GameResult);
}


void ABaseballPlayerState::SetGameResult(const FString& Result)
{
    if (GameResult != Result)
    {
        GameResult = Result;
    }
}

void ABaseballPlayerState::OnRep_GameResult()
{
    APlayerController* OwnerPC = Cast<APlayerController>(GetOwner());

    if (OwnerPC && OwnerPC->IsLocalController())
    {
        UE_LOG(LogTemp, Warning, TEXT("GameResult updated: %s"), *GameResult);
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Game Result: %s"), *GameResult));
        }
    }
}

