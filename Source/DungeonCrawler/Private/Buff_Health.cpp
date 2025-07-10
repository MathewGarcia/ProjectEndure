// Fill out your copyright notice in the Description page of Project Settings.


#include "Buff_Health.h"
#include "PlayerCharacter.h"
#include "MainGameInstance.h"
#include "PlayerCharacterState.h"


//THIS IS REALLY JUST A BUFF STAT... Name should be changed.
void UBuff_Health::Execute() {

	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
		if (APlayerCharacter* player = MGI->localPlayer) {
			if (APlayerCharacterState*PCS = player->GetPlayerCharacterState()) {

				switch (StatType) {
				case EPlayerStatType::Health:
					UE_LOG(LogTemp, Warning, TEXT("Health Before Increased %f"), PCS->playerStats.totalHealth);

					PCS->playerStats.totalHealth += PCS->playerStats.totalHealth * 0.1;
					UE_LOG(LogTemp, Warning, TEXT("Health increased %f"), PCS->playerStats.totalHealth);
					break;
				case EPlayerStatType::Stamina:
					PCS->playerStats.totalStamina += PCS->playerStats.totalStamina * 0.1;
					UE_LOG(LogTemp, Warning, TEXT("Stamina increased"));

					break;

				case EPlayerStatType::Intellect:
					PCS->playerStats.totalIntellect += PCS->playerStats.totalIntellect * 0.1;
					UE_LOG(LogTemp, Warning, TEXT("Intellect increased"));

					break;

				case EPlayerStatType::Strength:
					PCS->playerStats.totalStrength += PCS->playerStats.totalStrength * 0.1;
					UE_LOG(LogTemp, Warning, TEXT("Strength increased"));

					break;

				case EPlayerStatType::Life:
					PCS->UpdateLives(1);
					UE_LOG(LogTemp, Warning, TEXT("Increased Health by 1!"));

					break;
				default:
					PCS->playerStats.totalHealth += PCS->playerStats.totalHealth * 0.1;
					UE_LOG(LogTemp, Warning, TEXT("Health increased"));

					break;

				}
			}
		}
	}

}
