#include "BaseballGInstance.h"
#include "Engine/Engine.h"


void UBaseballGInstance::Init()
{
	Super::Init();
	if (GEngine)
	{
		//게임이 켜질 때 엔진의 네트워크 에러 발생 이벤트에 HandleNetworkError 함수를 바인딩해줌
		GEngine->OnNetworkFailure().AddUObject(this, &UBaseballGInstance::HandleNetworkError);
	}
}

void UBaseballGInstance::HandleNetworkError(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType, const FString& ErrorString)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
			FString::Printf(TEXT("[GI]네트워크 연결이 끊어졌습니다. 사유: %s"), *ErrorString));
	}
	//클라이언트가 튕겼음을 기록하는 깃발
	bWasDisconnected = true;
}
