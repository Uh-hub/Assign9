#include "BaseBallPlayerController.h"
#include "BaseballGModeBase.h"
#include "Net/UnrealNetwork.h"
#include "BaseballGameUI.h"


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
		UUserWidget* GameUIWidget = CreateWidget<UUserWidget>(this, GameUIClass);
		if (GameUIWidget)
		{
			GameUIWidget->AddToViewport();

			BaseballGameUI = Cast<UBaseballGameUI>(GameUIWidget);

			if (BaseballGameUI)
			{
				UE_LOG(LogTemp, Warning, TEXT("BaseballGameUI successfully created and stored"));
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("BaseballGameUI cast failed!"));
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


void ABaseBallPlayerController::UpdateUI()
{

	if (BaseballGameUI)
	{
		UE_LOG(LogTemp, Warning, TEXT("BaseballGameUI in controller working!"));
		BaseballGameUI->SetIsMyTurn(bIsMyTurn);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("called but BaseballGameUI in controller not working!"));
	}
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
	UE_LOG(LogTemp, Warning, TEXT("OnRep_IsMyTurn Called! New Value: %s"), bIsMyTurn ? TEXT("True") : TEXT("False"));
	if (BaseballGameUI)
	{
		BaseballGameUI->SetIsMyTurn(bIsMyTurn);
	}
}
