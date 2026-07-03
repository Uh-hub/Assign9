#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StartMenuGMode.generated.h"


UCLASS()
class NETWORKMINIGAME_API AStartMenuGMode : public AGameModeBase
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="UI")
	TSubclassOf<class UUserWidget> DisconnectPopupClass;
};
