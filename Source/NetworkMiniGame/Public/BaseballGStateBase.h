#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BaseballGStateBase.generated.h"


UCLASS()
class NETWORKMINIGAME_API ABaseballGStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:

	//최대 시도 가능 횟수
	UPROPERTY(Replicated)
	int32 MaxTries = 3;

	//결과창
	UFUNCTION(NetMulticast, Reliable)
	void MulticastShowResult(const FString& choice, const FString& result);

	UFUNCTION(Client, Reliable)
	void ClientShowResult(const FString& choice, const FString& result);
	
	
	//Getter : 클라이언트에서 데이터를 읽어가기 위함
	UFUNCTION(BlueprintCallable)
	int32 GetRoundCnt() const { return RoundCnt; }
	//Setter : 서버에서 데이터를 변경하기 위함
	void SetRoundCnt(int32 NewCnt);

	UFUNCTION()
	void OnRep_RoundCnt();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	//라운드 횟수 : RoundCnt
	UPROPERTY(ReplicatedUsing = OnRep_RoundCnt)
	int32 RoundCnt = 1;
};
