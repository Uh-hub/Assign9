#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "BaseBallPlayerController.generated.h"

class UBaseballGameUI;
//���� �Է� �ޱ� + �Է��� ���ڸ� ������ ���� + ��� UI ǥ��
UCLASS()
class NETWORKMINIGAME_API ABaseBallPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="UI")
	TSubclassOf<UUserWidget> GameUIClass;
	UPROPERTY(Replicated)
	int32 PlayerIndex;
	UPROPERTY(ReplicatedUsing = OnRep_IsMyTurn)
	bool bIsMyTurn;
	UFUNCTION()
	void OnRep_IsMyTurn();

	void SetIsMyTurn(bool bNewTurn);




	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	bool IsMyTurn() const;
	UFUNCTION(Server, Reliable)
	void ServerSubmitChoice(const FString& choice);
	UFUNCTION(BlueprintCallable)
	void SubmitChoice(FString choice);

	void UpdateUI();
	void CreateBaseballGameUI();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	UBaseballGameUI* BaseballGameUI;
};
