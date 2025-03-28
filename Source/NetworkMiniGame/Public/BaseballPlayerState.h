#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "BaseballPlayerState.generated.h"

UCLASS()
class NETWORKMINIGAME_API ABaseballPlayerState : public APlayerState
{
	GENERATED_BODY()

public:

	UPROPERTY(ReplicatedUsing=OnRep_GameResult)
	FString GameResult;

	UFUNCTION()
	void OnRep_GameResult();

	void SetGameResult(const FString& Result);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
