#include "GameStats.h"
#include "InGamePlayerHUD.h"
#include "MainPlayerWidget.h"
#include "PlayerCharacter.h"

EElementTypes FGameStats::IncreaseElementProgression(EElementTypes ElementType, float val)
{
    if (float* currentVal = ElementProgression.Find(ElementType))
    {
        if (Owner)
        {
            if (APlayerCharacter* player = Cast<APlayerCharacter>(Owner->GetPawn()))
            {
                UWorld* World = player->GetWorld();
                if (World && World->GetTimerManager().IsTimerActive(player->FDecreaseElementalTimerHandle))
                {
                    World->GetTimerManager().ClearTimer(player->FDecreaseElementalTimerHandle);
                }
            }
            if (AInGamePlayerHUD* PlayerHUD = Cast<AInGamePlayerHUD>(Owner->GetHUD()))
            {
                if (UMainPlayerWidget* MainPlayerWidget = Cast<UMainPlayerWidget>(PlayerHUD->GetMainUIWidget()))
                {
                    FName ProgressBarName = "";
                    switch (ElementType)
                    {
                    case EElementTypes::Fire:
                        ProgressBarName = "Fire";
                        break;
                    case EElementTypes::Water:
                        ProgressBarName = "Water";
                        break;
                    case EElementTypes::Shadow:
                        ProgressBarName = "Shadow";
                        break;
                    case EElementTypes::Lightening:
                        ProgressBarName = "Lightning";
                        break;
                    case EElementTypes::Bleed:
                        ProgressBarName = "Bleed";
                        break;
                    case EElementTypes::Poison:
                        ProgressBarName = "Poison";
                        break;
                    case EElementTypes::NONE:
                        break;
                    default:
                        break;
                    }
                    float resistance = 0.f;
                    if (Resistances.Contains(ElementType)) {
                        resistance = Resistances[ElementType];
                    }
                    *currentVal += val * (1.f - resistance);
                    *currentVal = FMath::Clamp(*currentVal, 0.f, 1.f);
                    MainPlayerWidget->SetHorizontalBoxVisibility(ElementType, ESlateVisibility::Visible);
                    MainPlayerWidget->UpdateProgressBar(ProgressBarName, *currentVal);
                    UE_LOG(LogTemp, Error, TEXT("%s Progression : %f"), *UEnum::GetValueAsString(ElementType), *currentVal)
                    if (*currentVal >= 1.f)
                    {
                        *currentVal = 0.f;
                        return ElementType;
                    }
                    TWeakObjectPtr<UMainPlayerWidget> WeakMainPlayerWidget = MainPlayerWidget;
                    UWorld* World = Owner->GetWorld();
                    if (World) {
                        World->GetTimerManager().SetTimer(FResetTimerHandle, [this, WeakMainPlayerWidget,ElementType]()
                            {
                                if (!WeakMainPlayerWidget.IsValid()) return;
                                UMainPlayerWidget* LocalMainPlayerWidget = WeakMainPlayerWidget.Get();
                                if (!LocalMainPlayerWidget) return;
                                LocalMainPlayerWidget->SetHorizontalBoxVisibility(ElementType, ESlateVisibility::Collapsed);
                            }, 1.0f, false);
                        World->GetTimerManager().SetTimer(FResetResistancesTimerHandle, [this]
                        {
                            StartDecreasingElementalProgression();
                        }, 2.f, false);
                    }
                }
            }
        }
    }
    return EElementTypes::NONE;
}

void FGameStats::StartDecreasingElementalProgression()
{
    if (!Owner) return;
    if(APlayerCharacter*player = Cast<APlayerCharacter>(Owner->GetPawn()))
    {
        player->DecreaseElementalProgression();
    }
}

void FGameStats::EndAllTimers()
{
    if(Owner && Owner->GetWorld())
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(Owner->GetPawn()))
        {
            UWorld* World = Owner->GetWorld();
            if (World) {
                World->GetTimerManager().ClearTimer(FResetResistancesTimerHandle);
                World->GetTimerManager().ClearTimer(FResetTimerHandle);
                World->GetTimerManager().ClearTimer(player->FDecreaseElementalTimerHandle);
            }
        }
    }
}



