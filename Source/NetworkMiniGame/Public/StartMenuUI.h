#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
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

	UFUNCTION()
	void OnHostBClicked();
	
	UFUNCTION()
	void OnJoinBClicked();
};
