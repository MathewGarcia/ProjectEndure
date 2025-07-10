// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ElementDebuff.generated.h"

class UNiagaraSystem;
/**
 * 
 */
UCLASS(Abstract,Blueprintable)
class DUNGEONCRAWLER_API UElementDebuff : public UObject
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Debuff")
    void ExecuteDebuff(AActor* Target,AActor*DamageCauser);

    virtual void ExecuteDebuff_Implementation(AActor* Target,AActor*DamageCauser) {}

    UPROPERTY(EditAnywhere,Category = "Debuff")
    float DebuffTime;

    UPROPERTY(EditAnywhere,Category = "Debuff")
    UNiagaraSystem* NiagaraSystem;

    FTimerHandle FDebuffTimerHandle;

    virtual void EndBuff();
};
