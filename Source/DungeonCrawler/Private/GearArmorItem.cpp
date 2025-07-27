#include "GearArmorItem.h"
#include "GearItemInstance.h"
#include "MainGameInstance.h"
#include "PlayerCharacter.h"

void AGearArmorItem::InitGearItemInstance(int Level)
{
    UWorld* World = GetWorld();
    if (!World) return;
    if (UMainGameInstance* MGI = Cast<UMainGameInstance>(World->GetGameInstance())) {
        if (APlayerCharacter* player = MGI->localPlayer) {
            if (APlayerCharacterState* PCS = player->GetPlayerCharacterState()) {
                if (UGearItemInstance* GearItemDataObject = NewObject<UGearItemInstance>(PCS, UGearItemInstance::StaticClass()))
                {
                    GearItemDataObject->ItemData = ItemData;
                    GearItemDataObject->InitStatsAccordingToLevel(Level);
                }
            }
        }
    }
}
