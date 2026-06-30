#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "StartMenuUI.generated.h"

class UButton;

UCLASS()
class NETWORKMINIGAME_API UStartMenuUI : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

private:
	//호스트 버튼 누를 시,
	UFUNCTION()
	void OnHostBClicked();
	
	//세션 생성이 완료되면 호출될 콜백 함수
	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	
	//언리얼 내부 시스템과 연결할 델리게이트 변수
	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FDelegateHandle CreateSessionCompleteDelegateHandle;


	//Join 버튼 누를 시,
	UFUNCTION()
	void OnJoinBClicked();
	
	//방 찾는 용 델리게이트와 함수
	void OnFindSessionsComplete(bool bWasSuccessful);
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	FDelegateHandle FindSessionsCompleteDelegatehandle; 

	//방 검색 조건을 담을 스마트 포인터 변수
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	//방 접속 델리게이트와 함수
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;
	FDelegateHandle JoinSessionCompleteDelegateHandle;








};
