#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseBallPlayerController.h"
#include "BaseballGModeBase.generated.h"

//������ ����, ���� ���� + �÷��̾� ���� ���� + ���� ���� �� ��� ����
UCLASS()
class NETWORKMINIGAME_API ABaseballGModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:


	//TArray�� �÷��̾� ��Ʈ�ѷ��� ����
	UPROPERTY()
	TArray<ABaseBallPlayerController*> PlayerControllers;

	ABaseBallPlayerController* CurrentPC;


	//���� ���� �÷��̾��� �ε���

	UPROPERTY(Replicated)
	int32 CurrentPlayerIndex = 0;
	const int32 MaxTries = 3;

	//�÷��̾ �õ� Ƚ��

	UPROPERTY(Replicated)
	FString answer;
	UPROPERTY(Replicated)
	int32 count = 1;
	
	//���� ���Ῡ�� ��Ÿ���� ����
	UPROPERTY(Replicated)
	bool bIsGameOver;

	//Post Login
	virtual void PostLogin(APlayerController* NewPlayer);

	//�ε��� ���� �Լ�
	UFUNCTION(BlueprintCallable)
	int32 GetCurrentPlayerIndex() const;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_UpdateTurnUI();


	int32 GetPlayerIndex(ABaseBallPlayerController* PC) const;
	
	//���� ���� ���� �Լ�
	virtual void BeginPlay() override;
	void StartTurn();
	void NextTurn();
	void CheckChoice(const FString& Choice);
	void GameResult(const FString& choice, const FString& Result);
	void EndGame();
	void HandleGameEnd(APlayerController* Winner);

	//���� �� ���� ���� �Լ�
	FString GenerateRandomNumber();
	FString CalculateResult(const FString& Choice);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
