// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"

#include "BaseItem.h"
#include "GearArmorItem.h"
#include "GearItemInstance.h"
#include "InteractableTextWidget.h"
#include "ItemDataObject.h"
#include "MainPlayerWidget.h"
#include "Components/BoxComponent.h"
#include "PlayerCharacter.h"
#include "PlayerCharacterState.h"
#include "Weapon.h"
#include "WeaponInstance.h"
// Sets default values
AItem::AItem()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    StaticMeshComponent->SetupAttachment(Root);
    StaticMeshComponent->SetCollisionProfileName("ActualNoCollision");
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));
    SkeletalMeshComponent->SetupAttachment(Root);
    SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
    collisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));
    collisionBox->SetGenerateOverlapEvents(true);
    collisionBox->SetupAttachment(Root);
    collisionBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
}

void AItem::ItemOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if(OtherActor && OtherActor != this)
    {
        if(APlayerCharacter*Player = Cast<APlayerCharacter>(OtherActor))
        {
            Player->OverlappingActor = this;
            TWeakObjectPtr<UMainPlayerWidget> MPWPtr = Player->GetMainPlayerWidget();
            if (MPWPtr.IsValid())
            {
                UMainPlayerWidget* MPW = MPWPtr.Get();
                if(MPW && MPW->InteractionsWidget)
                {
                    UInteractableTextWidget*InteractableTextWidget = MPW->InteractionsWidget;
                    if (APlayerCharacterState* PlayerCharacterState = Player->GetPlayerCharacterState()) {
                        if (FKey* InputKey = PlayerCharacterState->InputMap.Find("InteractIA")) {
                            FKey Key = (*InputKey);
                            FString String = FString::Printf(TEXT("Press %s to Pick Up"),*Key.ToString());
                            InteractableTextWidget->UpdateInteractTextBlock(String);
                            InteractableTextWidget->SetVisibility(ESlateVisibility::Visible);
                        }
                    }
                }
            }
        }
    }
}

void AItem::OverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    if (OtherActor && OtherActor != this)
    {
        if (APlayerCharacter* Player = Cast<APlayerCharacter>(OtherActor))
        {
            UE_LOG(LogTemp, Warning, TEXT("We have left the range to pick me up :("));
            Player->OverlappingActor = nullptr;
            TWeakObjectPtr<UMainPlayerWidget> MPWPtr = Player->GetMainPlayerWidget();
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

void AItem::PickUp(APlayerCharacter*player)
{
    if (!player || !ItemData) return;
    player->PlayPickUpSound();
    if(APlayerCharacterState*PlayerCharacterState = Cast<APlayerCharacterState>(player->GetPlayerState()))
    {
        //create a new object and assign it to the item data so we can dynamically set which class to spawn.
        if (!ItemData->ItemToSpawn) {
            ItemData->ItemToSpawn = GetClass();
        }
        switch(ItemType)
        {
        case EItemType::Armor:
            if (AGearArmorItem* GearArmorItem = Cast<AGearArmorItem>(this)) {
                if (!GearArmorItem->GearItemInstance) {
                    if (UGearItemInstance* GearItemDataObject = NewObject<UGearItemInstance>(PlayerCharacterState, UGearItemInstance::StaticClass()))
                    {
                        GearItemDataObject->ItemData = ItemData;
                        GearItemDataObject->InitializeStats();
                        PlayerCharacterState->AddItemToInventory(GearItemDataObject);
                    }
                }
                else
                {
                    PlayerCharacterState->AddItemToInventory(GearArmorItem->GearItemInstance);
                }
            }
            break;
        case EItemType::Weapon:
            if (AWeapon* Weapon = Cast<AWeapon>(this))
            {
                UE_LOG(LogTemp, Warning, TEXT("=== PICKUP WEAPON ==="));
                UE_LOG(LogTemp, Warning, TEXT("Weapon: %s"), *Weapon->GetName());
                UE_LOG(LogTemp, Warning, TEXT("WeaponInstance exists: %s"), Weapon->WeaponInstance ? TEXT("YES") : TEXT("NO"));

                if (Weapon->WeaponInstance)
                {
                    UE_LOG(LogTemp, Warning, TEXT("BEFORE pickup - Element: %d, Level: %d, Damage: %d"),
                        (int32)Weapon->WeaponInstance->WeaponElementType,
                        Weapon->WeaponInstance->CurrentWeaponLevel,
                        Weapon->WeaponInstance->BaseDamage);
                }

                if (!Weapon->WeaponInstance)
                {
                    UE_LOG(LogTemp, Warning, TEXT("Pickup: WeaponInstance is NULL, calling InitWeapon"));
                    Weapon->InitWeapon();
                }

                if (Weapon->WeaponInstance)
                {
                    UE_LOG(LogTemp, Warning, TEXT("AFTER InitWeapon check - Element: %d, Level: %d, Damage: %d"),
                        (int32)Weapon->WeaponInstance->WeaponElementType,
                        Weapon->WeaponInstance->CurrentWeaponLevel,
                        Weapon->WeaponInstance->BaseDamage);

                    UE_LOG(LogTemp, Warning, TEXT("Adding WeaponInstance to inventory..."));
                    PlayerCharacterState->AddItemToInventory(Weapon->WeaponInstance);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("WeaponInstance is STILL NULL after InitWeapon!"));
                }
            }
            break;
        default:
            if (UItemDataObject* ItemDataObject = NewObject<UItemDataObject>(PlayerCharacterState, UItemDataObject::StaticClass()))
            {
                ItemDataObject->ItemData = ItemData;
                ItemDataObject->Quantity = ItemData->Quantity;
                PlayerCharacterState->AddItemToInventory(ItemDataObject);
            }
            break;
        }
        UE_LOG(LogTemp, Warning, TEXT("Added item to inventory"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed player character state"));
    }
    Destroy();
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
    Super::BeginPlay();
    if (collisionBox) {
        collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AItem::ItemOverlapBegin);
        collisionBox->OnComponentEndOverlap.AddDynamic(this, &AItem::OverlapEnd);
        if(collisionBox->GetCollisionEnabled() == ECollisionEnabled::Type::NoCollision)
        {
            collisionBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
        }
    }
}

void AItem::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);
    // Toggle visibility based on the selected mesh type
    if (StaticMeshComponent) {
        StaticMeshComponent->SetVisibility(!bIsSkeletal);
        StaticMeshComponent->SetCollisionEnabled(bIsSkeletal ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);
    }
    if (SkeletalMeshComponent) {
        SkeletalMeshComponent->SetVisibility(bIsSkeletal);
        SkeletalMeshComponent->SetCollisionEnabled(bIsSkeletal ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
    }
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

