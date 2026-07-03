#include "BaseBallPlayerController.h"
#include "BaseballGModeBase.h"
#include "Net/UnrealNetwork.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "BaseballGameUI.h"
#include "Kismet/GameplayStatics.h"


void ABaseBallPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseBallPlayerController, PlayerIndex);
	DOREPLIFETIME(ABaseBallPlayerController, bIsMyTurn);
}


void ABaseBallPlayerController::BeginPlay()
{
	Super::BeginPlay();


	if (IsLocalPlayerController())
	{
		FString CurrentMapName = GetWorld()->GetName();
		UE_LOG(LogTemp, Warning, TEXT("현재 로드된 맵: %s"), *CurrentMapName);
		
		//로비 맵 (StartMenuMap -> StartMenuUI)
		if (CurrentMapName.Contains(TEXT("StartMenuMap")))
		{
			if (StartMenuClass)
			{
				UUserWidget* StartMenuWidget = CreateWidget<UUserWidget>(this, StartMenuClass);
				if (StartMenuWidget)
				{
					StartMenuWidget->AddToViewport();

					bShowMouseCursor = true;

					FInputModeUIOnly InputMode;
					InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
					SetInputMode(InputMode);
				}
			}
		}
		//게임 맵 (MainMap -> BaseballGameUI)
		else if (CurrentMapName.Contains(TEXT("MainMap")))
		{
			if (GameUIClass)
			{
				UUserWidget* GameUIWidget = CreateWidget<UUserWidget>(this, GameUIClass);
				if (GameUIWidget)
				{
					GameUIWidget->AddToViewport();

					BaseballGameUI = Cast<UBaseballGameUI>(GameUIWidget);
					if (BaseballGameUI == nullptr)
					{
						if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
								FString::Printf(TEXT("UI 캐스팅 실패! BaseballGameUI가 Null 입니다.")));
						}
					}
					bShowMouseCursor = true;
					FInputModeGameAndUI InputMode;
					SetInputMode(InputMode);
				}
			}
		}
		
	}
}

void ABaseBallPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ABaseballGModeBase* GM = Cast<ABaseballGModeBase>(GetWorld()->GetAuthGameMode());
	if (GM && GM->bIsGameOver)
	{
		DisableInput(this);
	}
}


bool ABaseBallPlayerController::IsMyTurn() const
{
	ABaseballGModeBase* GM = Cast<ABaseballGModeBase>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		UE_LOG(LogTemp, Warning, TEXT("IsMyTurn: PlayerIndex=%d, CurrentPlayerIndex=%d, Result=%s"),
			PlayerIndex, GM->GetCurrentPlayerIndex(), bIsMyTurn ? TEXT("True") : TEXT("False"));
		UE_LOG(LogTemp, Warning, TEXT("OR %s"), GM->GetCurrentPlayerIndex() == PlayerIndex ? TEXT("True") : TEXT("False"));
		return GM->GetCurrentPlayerIndex() == PlayerIndex;
	}
	return false;
}


void ABaseBallPlayerController::SubmitChoice(FString choice)
{
	if (HasAuthority())
	{
		//서버
		ServerSubmitChoice(choice);
	}
	else
	{
		//클라이언트
		ServerSubmitChoice(choice);
	}
	
}


void ABaseBallPlayerController::ServerSubmitChoice_Implementation(const FString& choice)
{
	//GetAuthGameMode가 뭐야???
	ABaseballGModeBase* GM = Cast<ABaseballGModeBase>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CheckChoice(choice);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ServerSubmitChoice : GameMode not found!"));
	}
}


void ABaseBallPlayerController::UpdateUI(int32 CurrentRound, int32 MaxRound)
{

	if (BaseballGameUI)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseballGameUI in controller working!"));
		BaseballGameUI->UpdateRoundCount(CurrentRound, MaxRound);
		BaseballGameUI->SetIsMyTurn(bIsMyTurn);

	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("called but BaseballGameUI in controller not working!"));
	}
}

void ABaseBallPlayerController::Client_LeaveGame_Implementation()
{
	//[게임 정상 종료]클라이언트 우선 로비로 복귀
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->DestroySession(FName("BaseballSession"));
		}
	}
	UGameplayStatics::OpenLevel(this, FName("StartMenuMap"));
}


void ABaseBallPlayerController::SetIsMyTurn(bool bNewTurn)
{
	if (HasAuthority())
	{
		bIsMyTurn = bNewTurn;
		OnRep_IsMyTurn();
	}
}

void ABaseBallPlayerController::OnRep_IsMyTurn()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Yellow,
			FString::Printf(TEXT("OnRep_IsMyTurn Called! New Value: %s"), bIsMyTurn ? TEXT("True") : TEXT("False")));
	}
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_IsMyTurn Called! New Value: %s"), bIsMyTurn ? TEXT("True") : TEXT("False"));
	if (BaseballGameUI)
	{
		BaseballGameUI->SetIsMyTurn(bIsMyTurn);
	}
}
