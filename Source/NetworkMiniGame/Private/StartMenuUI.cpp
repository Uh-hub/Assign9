#include "StartMenuUI.h"
#include "Components/Button.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Kismet/GameplayStatics.h"


void UStartMenuUI::NativeConstruct()
{
	Super::NativeConstruct();
	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &UStartMenuUI::OnHostBClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &UStartMenuUI::OnJoinBClicked);
	}


	//델리게이트에 만든 콜백 함수(OnCreateSessionComplete)를 연결해 둠
	//세션 생성이 완료되었다는 신호가 오면 this(StartMenuUI)의 OnCreateSessionComplete 함수를 실행해라
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UStartMenuUI::OnCreateSessionComplete);

}

void UStartMenuUI::OnHostBClicked()
{	
	//현재 켜져 있는 온라인 서브시스템을 가져옴
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{	
		//세션을 관리하는 인터페이스를 가져옴
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{

			if (Sessions->GetNamedSession(FName("BaseballSession")) != nullptr)
			{
				Sessions->DestroySession(FName("BaseballSession"));
				UE_LOG(LogTemp, Warning, TEXT("기존에 남아있던 유령 세션을 파괴했습니다. "));
			}


			//세션이 유효하다면
			//방의 규칙을 세팅함
			FOnlineSessionSettings SessionSettings;
			SessionSettings.bIsLANMatch = true; //같은 공유기 / 로컬만 허용하도록 함
			SessionSettings.NumPublicConnections = 2; //최대 2명 접속 가능
			SessionSettings.bShouldAdvertise = true; //다른 사람이 찾을 수 있게 허용

			//세션 생성 요청이 완료되면 델리게이트가 발송됨(방 생성 완료된 경우 델리게이트를 부르는 규칙 등록)
			CreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate); 
			
			//세션 생성 명령
			Sessions->CreateSession(0, FName("BaseballSession"), SessionSettings);

			UE_LOG(LogTemp, Warning, TEXT("방 생성 요청 보냄..."));
		}
	}
}

//엔진이 방 생성을 끝마쳤을 때 자동으로 호출함
//세션이 끝났을 때 자동으로 호출해 줌
void UStartMenuUI::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			//세션 생성이 완료되었으므로 델리게이트 연결은 깔끔하게 해제함
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegateHandle);

		}

		if (bWasSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("방 생성 성공! 게임 레벨로 이동합니다. "));
			//현재 레벨을 'listen' 서버 모드로 다시 열음
			UGameplayStatics::OpenLevel(GetWorld(), TEXT("MainMap"), true, TEXT("listen"));
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("방 생성 실패..."));
		}
	}
}


void UStartMenuUI::OnJoinBClicked()
{
}
