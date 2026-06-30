#include "StartMenuUI.h"
#include "Components/Button.h"
#include "OnlineSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Online/OnlineSessionNames.h"


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
	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UStartMenuUI::OnFindSessionsComplete);
	JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UStartMenuUI::OnJoinSessionComplete);
}

//[Host]

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

			SessionSettings.bUsesPresence = true; //Presence(상태 알림) 켜기
			SessionSettings.bAllowJoinInProgress = true; // 게임 중 난입 허용


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

//[Join]


void UStartMenuUI::OnJoinBClicked()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("방 검색 시작..."));

			//검색 조건 세팅
			SessionSearch = MakeShareable(new FOnlineSessionSearch());
			SessionSearch->bIsLanQuery = true;//LAN에서 찾기(Host의 세팅)
			SessionSearch->MaxSearchResults = 10000; //PIE 환경에서는 쓰레기 세션이 많을 수 있어 넉넉히 잡기

			SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

			//Handle 등록
			FindSessionsCompleteDelegatehandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

			//방 검색 요청(요청 완료 시, 연결 되어 있는 OnFindSessionComplete로 넘어감)
			Sessions->FindSessions(0, SessionSearch.ToSharedRef());
		}
	}



}
//세션(방) 검색 요청에 대한 응답 완료 시 호출됨
void UStartMenuUI::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			//세션 검색 요청을 위해 등록했던 델리게이트 해제 
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegatehandle);

			//세션(방)을 정상적으로 찾았다면
			if (bWasSuccessful && SessionSearch->SearchResults.Num() > 0)
			{
				UE_LOG(LogTemp, Warning, TEXT("방 찾기 성공! 총 %d개의 방 발견. 첫 번째 방에 접속 시도..."), SessionSearch->SearchResults.Num());
				//해당 세션에 접속하기 위해 우선 델리게이트 등록
				JoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
				//검색된 방 목록 중 첫 번째 방에 접속 요청
				//완료 시, OnJoinSessionComplete 함수가 실행됨
				Sessions->JoinSession(0, FName("BaseballSession"), SessionSearch->SearchResults[0]);
				
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("방을 찾지 못했습니다"));
			}
		}
	}
}

void UStartMenuUI::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			//우선 델리게이트 먼저 해제하기
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegateHandle);

			//접속에 성공한 경우,
			if (Result == EOnJoinSessionCompleteResult::Success)
			{
				//접속할 방의 진짜 IP 주소를 가져와서 ConnectString 내에 저장
				FString ConnectString;
				if (Sessions->GetResolvedConnectString(SessionName, ConnectString))
				{
					UE_LOG(LogTemp, Warning, TEXT("방 접속 성공! 맵을 이동합니다. 주소: %s"), *ConnectString);
					
					APlayerController* PC = GetWorld()->GetFirstPlayerController();
					if (PC)
					{
						PC->ClientTravel(ConnectString, ETravelType::TRAVEL_Absolute);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("방 접속 실패..."));
			}


		}
	}

}