// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractableActor.h"

#include "InteractableTextWidget.h"
#include "MainPlayerWidget.h"
#include "NiagaraComponent.h"
#include "PlayerCharacter.h"
#include "Components/BoxComponent.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    RootComponent = SkeletalMesh;
    BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
    BoxComponent->SetBoxExtent(FVector(10.f));
    BoxComponent->SetupAttachment(SkeletalMesh);
    NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Niagara Comp"));
    NiagaraComponent->SetupAttachment(SkeletalMesh);
}

// Called when the game starts or when spawned
void AInteractableActor::BeginPlay()
{
    Super::BeginPlay();
    if (BoxComponent) {
        BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AInteractableActor::BoxOverlapBegin);
        BoxComponent->OnComponentEndOverlap.AddDynamic(this, &AInteractableActor::OverlapEnd);
    }
}

// Called every frame
void AInteractableActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AInteractableActor::Interact_Implementation()
{
}

void AInteractableActor::BoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(OtherActor && OtherActor != this)
    {
        if(APlayerCharacter*player = Cast<APlayerCharacter>(OtherActor))
        {
            player->OverlappingActor = this;
            TWeakObjectPtr<UMainPlayerWidget> MPWPtr = player->GetMainPlayerWidget();
            if (MPWPtr.IsValid())
            {
                UMainPlayerWidget* MPW = MPWPtr.Get();
                if (MPW && MPW->InteractionsWidget)
                {
                    UInteractableTextWidget* InteractableTextWidget = MPW->InteractionsWidget;
                    if (APlayerCharacterState* PlayerCharacterState = player->GetPlayerCharacterState()) {
                        if (FKey* InputKey = PlayerCharacterState->InputMap.Find("InteractIA")) {
                            FKey Key = (*InputKey);
                            FString String = FString::Printf(TEXT("Press %s to Interact"), *Key.ToString());
                            InteractableTextWidget->UpdateInteractTextBlock(String);
                            InteractableTextWidget->SetVisibility(ESlateVisibility::Visible);
                        }
                    }
                }
            }
        }
    }
}

void AInteractableActor::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {
        if (APlayerCharacter* player = Cast<APlayerCharacter>(OtherActor))
        {
            player->OverlappingActor = nullptr;
            TWeakObjectPtr<UMainPlayerWidget> MPWPtr = player->GetMainPlayerWidget();
            if (MPWPtr.IsValid())
            {
                UMainPlayerWidget* MPW = MPWPtr.Get();
                if (MPW && MPW->InteractionsWidget)
                {
                    MPW->InteractionsWidget->SetVisibility(ESlateVisibility::Collapsed);
                }
            }
        }
    }
}

