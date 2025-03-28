#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BaseballGStateBase.generated.h"


UCLASS()
class NETWORKMINIGAME_API ABaseballGStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:


	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowResult(const FString& choice, const FString& result);

	UFUNCTION(Client, Reliable)
	void ClientShowResult(const FString& choice, const FString& result);



};
