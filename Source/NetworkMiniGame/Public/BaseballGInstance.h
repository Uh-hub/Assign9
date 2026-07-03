#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Net/Core/Connection/NetEnums.h"
#include "BaseballGInstance.generated.h"


UCLASS()
class NETWORKMINIGAME_API UBaseballGInstance : public UGameInstance
{
	GENERATED_BODY()
	

public:
	//게임 인스턴스 시작 시 초기화 되는 함수
	virtual void Init() override;

	//탈주 감지 깃발
	UPROPERTY(BlueprintReadWrite, Category = "Network")
	bool bWasDisconnected = false;



	void HandleNetworkError(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString);

};
