#include "StartMenuGMode.h"
#include "BaseballGinstance.h"
#include "Blueprint/UserWidget.h"



void AStartMenuGMode::BeginPlay()
{
	Super::BeginPlay();

	UBaseballGInstance* MyGI = Cast<UBaseballGInstance>(GetGameInstance());

	if (MyGI && MyGI->bWasDisconnected)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red,
				FString::Printf(TEXT("[StartMenu_GM] 튕김 깃발 확인! 팝업을 띄웁니다.")));
		}
		//튕겨서 로비로 복귀했음을 인식하고 해당 팝업을 띄움(bp에서 등록하기)
		if (DisconnectPopupClass)
		{
			UUserWidget* PopupWidget = CreateWidget<UUserWidget>(GetWorld(), DisconnectPopupClass);
			if (PopupWidget)
			{
				PopupWidget->AddToViewport();
			}
		}
		//관련 로직 완료 했으므로 다음을 위해 깃발을 다시 내림
		MyGI->bWasDisconnected = false;
	}
}