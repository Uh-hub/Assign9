#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseBallPlayerController.h"
#include "BaseballGModeBase.generated.h"

//게임의 시작, 종료 로직 + 플레이어 순서 관리 + 정답 판정 및 결과 전달
UCLASS()
class NETWORKMINIGAME_API ABaseballGModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:


	//TArray에 플레이어 컨트롤러들 저장
	UPROPERTY()
	TArray<ABaseBallPlayerController*> PlayerControllers;

	ABaseBallPlayerController* CurrentPC;


	//현재 턴인 플레이어의 인덱스

	UPROPERTY(Replicated)
	int32 CurrentPlayerIndex = 0;
	const int32 MaxTries = 3;

	//플레이어별 시도 횟수

	UPROPERTY(Replicated)
	FString answer;
	UPROPERTY(Replicated)
	int32 count = 1;
	
	//게임 종료여부 나타내는 변수
	UPROPERTY(Replicated)
	bool bIsGameOver;

	//Post Login
	virtual void PostLogin(APlayerController* NewPlayer);

	//인덱스 관련 함수
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentPlayerIndex() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateTurnUI();


	int32 GetPlayerIndex(ABaseBallPlayerController* PC) const;
	
	//게임 진행 관련 함수
	virtual void BeginPlay() override;
	void StartTurn();
	void NextTurn();
	void CheckChoice(const FString& Choice);
	void GameResult(const FString& choice, const FString& Result);
	void EndGame();
	void HandleGameEnd(APlayerController* Winner);

	//정답 및 판정 관련 함수
	FString GenerateRandomNumber();
	FString CalculateResult(const FString& Choice);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
