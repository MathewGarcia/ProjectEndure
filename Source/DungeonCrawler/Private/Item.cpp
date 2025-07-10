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
			if(UMainPlayerWidget*MPW = Player->GetMainPlayerWidget().Get())
			{
				if(UInteractableTextWidget*InteractableTextWidget = MPW->InteractionsWidget)
				{
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
			if (UMainPlayerWidget* MPW = Player->GetMainPlayerWidget().Get())
			{
				if (UInteractableTextWidget* InteractableTextWidget = MPW->InteractionsWidget)
				{
					
					InteractableTextWidget->SetVisibility(ESlateVisibility::Collapsed);
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
			if (AWeapon* Weapon= Cast<AWeapon>(this))
			{
				if(!Weapon->WeaponInstance)
				{
					Weapon->InitWeapon();
				}
				PlayerCharacterState->AddItemToInventory(Weapon->WeaponInstance);

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

	collisionBox->OnComponentBeginOverlap.AddDynamic(this, &AItem::ItemOverlapBegin);
	collisionBox->OnComponentEndOverlap.AddDynamic(this, &AItem::OverlapEnd);

	if(collisionBox->GetCollisionEnabled() == ECollisionEnabled::Type::NoCollision)
	{
		collisionBox->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	}

}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	// Toggle visibility based on the selected mesh type
	StaticMeshComponent->SetVisibility(!bIsSkeletal);
	StaticMeshComponent->SetCollisionEnabled(bIsSkeletal ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryOnly);

	SkeletalMeshComponent->SetVisibility(bIsSkeletal);
	SkeletalMeshComponent->SetCollisionEnabled(bIsSkeletal ? ECollisionEnabled::QueryOnly : ECollisionEnabled::NoCollision);
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	

}

