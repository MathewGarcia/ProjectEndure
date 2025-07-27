// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Ability.h"
#include "AbilityHealOverTime.h"
#include "Ability_Reflect_Damage.h"
#include "Ability_Second_Wind.h"
#include "AoE_Heal.h"
#include "Enemy.h"
#include "EquipmentUIWidget.h"
#include "GearSlotWidget.h"
#include "InGamePlayerHUD.h"
#include "InputConfigData.h"
#include "ItemData.h"
#include "MainPlayerWidget.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Weapon.h"
#include "GameFramework/HUD.h"
#include "PlayerCharacterState.h"
#include "BaseItem.h"
#include "BleedDebuff.h"
#include "ConsumeableItem.h"
#include "DebuffActor.h"
#include "Drink_Heal.h"
#include "DungeonCrawlerGM.h"
#include "GearArmorItem.h"
#include "PlayerAnimInstance.h"
#include "MainGameInstance.h"
#include "NiagaraFunctionLibrary.h"
#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FireDebuff.h"
#include "GearItemInstance.h"
#include "InteractableActor.h"
#include "ItemDataObject.h"
#include "LevelUpWidget.h"
#include "LighteningDebuff.h"
#include "PoisonDebuff.h"
#include "ShadowDebuff.h"
#include "WaterDebuff.h"
#include "Engine/DamageEvents.h"
#include "LiquidDataAsset.h"
#include "LivesWidget.h"
#include "NiagaraComponent.h"
#include "NPC.h"
#include "PlayerCharacterController.h"
#include "PlayerSaveGame.h"
#include "Quest.h"
#include "WeaponInstance.h"
#include "Widget_Pause_Menu.h"
#include "Components/HorizontalBox.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#define SURFACE_Default		SurfaceType_Default
#define SURFACE_Water		SurfaceType1
#define SURFACE_Lava		SurfaceType2
#define SURFACE_Poison		SurfaceType3
#define SURFACE_Blood		SurfaceType4
#define SURFACE_Wood		SurfaceType5
#define SURFACE_Cobble		SurfaceType6
#define SURFACE_Grass		SurfaceType7

// Sets default values
APlayerCharacter::APlayerCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Component"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 10.f;
	SpringArm->bEnableCameraRotationLag = true;
	SpringArm->CameraRotationLagSpeed = 12.f;



	Cam = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Component"));
	Cam->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Cam->bUsePawnControlRotation = false;


	GetCharacterMovement()->bEnablePhysicsInteraction = false;
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);

	Chest = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Chest SkeletalMesh"));
	Chest->SetCollisionObjectType(ECC_GameTraceChannel2);
	Chest->SetupAttachment(GetMesh());

	Legs = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Legs SkeletalMesh"));
	Legs->SetCollisionObjectType(ECC_GameTraceChannel2);
	Legs->SetupAttachment(GetMesh());

	Helm = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Helm SkeletalMesh"));
	Helm->SetCollisionObjectType(ECC_GameTraceChannel2);
	Helm->SetupAttachment(GetMesh());

	CurrentLivesWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("Current Lives Widget"));
	CurrentLivesWidget->SetupAttachment(RootComponent);
	CurrentLivesWidget->SetWidgetSpace(EWidgetSpace::Screen);
	CurrentLivesWidget->SetDrawSize(FVector2D(200.f, 100.f));
	CurrentLivesWidget->SetRelativeLocation(FVector(0.f, 0.f, 200.f));
	CurrentLivesWidget->SetVisibility(false);


}

void APlayerCharacter::SetPlayerState(PlayerStates NewPlayerStates)
{
	ps = NewPlayerStates;
}

void APlayerCharacter::IncrementAttackCount()
{
	if(AttackingWeapon)
	{
		if (AttackingWeapon->WeaponType == EWeaponType::TwoHand) {
			if (AttackingWeapon->TwohandMoveSet) {
				attackCount = (attackCount + 1) % AttackingWeapon->TwohandMoveSet->GetNumSections();
			}
		}
		else
		{
			if (AttackingWeapon->OnehandMoveSet) {
				attackCount = (attackCount + 1) % AttackingWeapon->OnehandMoveSet->GetNumSections();
			}

		}

	}
}

void APlayerCharacter::ResetAttackCount()
{

	attackCount = 0;
}

void APlayerCharacter::Dodge()
{
	UE_LOG(LogTemp, Warning, TEXT("Dodging"));

	APlayerCharacterState* PCS = SoftPCS.Get();

	if (PCS && CanPlayerDoAction(EResourceTypes::Stamina,DodgeReduction)) {
		ps = PlayerStates::DODGING;
		if (TargetedEnemy) {
			if (InputDirection.X > 0) // Right
			{
				PlayAnimMontage(DodgeMontages, 1.0, "Dodge_O_Right");

			}
			else if (InputDirection.X < 0) // Left
			{
				PlayAnimMontage(DodgeMontages, 1.0, "Dodge_O_Left");

			}
			else if (InputDirection.Y > 0) // Forward
			{
				PlayAnimMontage(DodgeMontages, 1.0, "Dodge_O_Forward");
			}
			else if (InputDirection.Y <= 0) // Backward
			{
				PlayAnimMontage(DodgeMontages, 1.0, "Dodge_O_Back");
			}
		}
		else
		{
			if (InputDirection.IsNearlyZero())
			{
				PlayAnimMontage(DodgeMontages, 1.0, "Dodge_O_Back");

			}
			else {
				PlayAnimMontage(DodgeMontages, 1.0, "Dodge_O_Forward");
			}
		}
	}

}


void APlayerCharacter::ResetLastInputDirection()
{
	InputDirection = FVector2D(0);
}

void APlayerCharacter::SetAttackingWeapon(AWeapon* AttackWeapon)
{
	AttackingWeapon = AttackWeapon;
}

void APlayerCharacter::DecreaseElementalProgression()
{
    if (!SoftPCS.IsValid() || !PlayerHUD || !PlayerHUD->GetMainUIWidget()) return;

    TWeakObjectPtr<APlayerCharacter> SafePlayer = this;
    GetWorld()->GetTimerManager().SetTimer(FDecreaseElementalTimerHandle, [SafePlayer]
    {
        if (!SafePlayer.IsValid()) return;
        if (APlayerCharacter* localthis = SafePlayer.Get()) {
            if (!localthis->PlayerHUD || !localthis->PlayerHUD->GetMainUIWidget()) return;
            if (APlayerCharacterState* PCS = localthis->SoftPCS.Get())
            {
                int32 max = 6;
                int32 currentSkips = 0;

                for (auto& pair : PCS->playerStats.ElementProgression)
                {
                    if (pair.Value <= 0)
                    {
                        ++currentSkips;
                        localthis->PlayerHUD->GetMainUIWidget()->SetHorizontalBoxVisibility(pair.Key, ESlateVisibility::Hidden);
                        continue;
                    }

                    if (localthis->PlayerHUD && localthis->PlayerHUD->GetMainUIWidget())
                    {
                        localthis->PlayerHUD->GetMainUIWidget()->SetHorizontalBoxVisibility(pair.Key, ESlateVisibility::Visible);

                        pair.Value -= 0.05;
                        pair.Value = FMath::Clamp(pair.Value, 0, 1.f);

                        FName ProgressBarName = "";
                        switch (pair.Key)
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
                        if (localthis->PlayerHUD && localthis->PlayerHUD->GetMainUIWidget())
                        {
                            localthis->PlayerHUD->GetMainUIWidget()->UpdateProgressBar(ProgressBarName, pair.Value);
                        }
                    }
                }

                if (currentSkips >= max)
                {
                    localthis->GetWorld()->GetTimerManager().ClearTimer(localthis->FDecreaseElementalTimerHandle);
                }
            }
        }

    }, 2.f, true);
}

void APlayerCharacter::DropItem()
{
	//TODO THIS NEEEDS TO BE CHANGED TO DROP ITEM CURRENTLY ONLY DROPS THE CURRENT WEAPON
	FVector Start = GetActorLocation();
	FVector End = Start + FVector(0,0, -1000);
	FHitResult hit;
	FCollisionQueryParams params;
	params.AddIgnoredActor(this);

	

	if(APlayerCharacterState*PCS = SoftPCS.Get())
	{

		if (CurrentWeapon) {

			if (GetWorld()->LineTraceSingleByChannel(hit, Start, End, ECC_WorldStatic, params))
			{
			
					CurrentWeapon->DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
					CurrentWeapon->SetActorLocation(hit.ImpactPoint);
				
			}

			if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(CurrentWeapon->ItemData)) {
				PCS->updateStrength(-WeaponItemData->Strength);
				PCS->updateIntellect(-WeaponItemData->Intellect);
				CurrentWeapon->collisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

				CurrentWeapon->SetOwner(nullptr);
			}
		}
	}

}


void APlayerCharacter::updateGearSlot(UItemDataObject* Item)
{
    if (!Item || !Item->ItemData || !PlayerHUD || !PlayerHUD->GetMainUIWidget() || !PlayerHUD->GetMainUIWidget()->EquipmentUI) return;
    EGearType GearType = EGearType::none;
    if(UGearItemInstance*GearItemInstance = Cast<UGearItemInstance>(Item))
	{
		if (UGearItemData*GearItemData = Cast<UGearItemData>(GearItemInstance->ItemData)) {
			GearType = GearItemData->GearType;
		}
	}
	else if (UWeaponInstance*WeaponInstance = Cast<UWeaponInstance>(Item))
	{
		if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(WeaponInstance->ItemData)) {
			GearType = WeaponItemData->GearType;
		}

	}

	
	switch (GearType)
	{
	case EGearType::none:
		break;
	case EGearType::Head:
		if (PlayerHUD->GetMainUIWidget()->EquipmentUI->HeadGearSlot) {
			PlayerHUD->GetMainUIWidget()->EquipmentUI->HeadGearSlot->UpdateGearSlot(Item);
		}
		break;
	case EGearType::LHand:
		if (PlayerHUD->GetMainUIWidget()->EquipmentUI->LHandGearSlot) {
			PlayerHUD->GetMainUIWidget()->EquipmentUI->LHandGearSlot->UpdateGearSlot(Item);
		}
		break;
	case EGearType::RHand:
		if (PlayerHUD->GetMainUIWidget()->EquipmentUI->RHandGearSlot) {
			PlayerHUD->GetMainUIWidget()->EquipmentUI->RHandGearSlot->UpdateGearSlot(Item);
		}
		break;
	case EGearType::Chest:
		if (PlayerHUD->GetMainUIWidget()->EquipmentUI->ChestGearSlot) {
			PlayerHUD->GetMainUIWidget()->EquipmentUI->ChestGearSlot->UpdateGearSlot(Item);
		}
		break;
	case EGearType::Legs:
		if (PlayerHUD->GetMainUIWidget()->EquipmentUI->LegsGearSlot) {
			PlayerHUD->GetMainUIWidget()->EquipmentUI->LegsGearSlot->UpdateGearSlot(Item);
		}
		break;
	default: break;
	}
}

bool APlayerCharacter::PlayerInterrupted()
{
	if (!CurrentWeapon || !PlayerAnimInstance) return false;


	return 	CurrentWeapon->GetIsSwinging() && (!PlayerAnimInstance->Montage_IsPlaying(CurrentWeapon->OnehandMoveSet) || !PlayerAnimInstance->Montage_IsPlaying(CurrentWeapon->TwohandMoveSet));

}

void APlayerCharacter::PlaySoundBasedOnSurface()
{
	if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
		USoundBase* Sound = nullptr;

		if (MostRecentSurfaceType == SURFACE_Water || MostRecentSurfaceType == SURFACE_Lava || MostRecentSurfaceType == SURFACE_Poison || MostRecentSurfaceType == SURFACE_Blood)
		{
			Sound = MGI->Liquid[FMath::RandRange(0, MGI->Liquid.Num() - 1)];

		}
		else if (MostRecentSurfaceType == SURFACE_Cobble)
		{
			Sound = MGI->Cobble[FMath::RandRange(0, MGI->Cobble.Num() - 1)];
		}
		else if (MostRecentSurfaceType == SURFACE_Grass)
		{
			Sound = MGI->Grass[FMath::RandRange(0, MGI->Grass.Num() - 1)];

		}
		else if (MostRecentSurfaceType == SURFACE_Wood)
		{
			Sound = MGI->Wood[FMath::RandRange(0, MGI->Wood.Num() - 1)];

		}
		else
		{
			Sound = MGI->Cobble[FMath::RandRange(0, MGI->Cobble.Num() - 1)];
		}

		if (Sound) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation(),FRotator::ZeroRotator,1,1,0,MGI->Attenuation);
		}


	}
}

void APlayerCharacter::PlayPickUpSound()
{
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		UGameplayStatics::PlaySound2D(GetWorld(), MGI->PickUpSound);
	}
}

void APlayerCharacter::ResetCurrentLivesWidget()
{
	if(CurrentLivesWidget)
	{
		CurrentLivesWidget->SetWidgetClass(LivesWidgetClass);
	}
}

void APlayerCharacter::GiveStarterItems()
{
	if (StarterWeapon)
	{
		if (AWeapon* SWeapon = Cast<AWeapon>(GetWorld()->SpawnActor(StarterWeapon)))
		{
			if (SWeapon->WeaponInstance && SWeapon->WeaponInstance->ItemData) {
				if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(SWeapon->WeaponInstance->ItemData)) {
					tryEquipGear(SWeapon, WeaponItemData->GearType, SWeapon->WeaponInstance);
				}
			}
		}
	}
}

void APlayerCharacter::RemoveGearStats(const EGearType& Slot)
{
	if (APlayerCharacterState* PCS = SoftPCS.Get()) {
		switch (Slot)
		{
		case EGearType::Head:
		case EGearType::Chest:
		case EGearType::Legs:

			if (UGearItemInstance** GearItemInstance = PCS->PlayerGearInstances.Find(Slot))
			{
				PCS->updateStrength(-(*GearItemInstance)->Strength);
				PCS->updateIntellect(-(*GearItemInstance)->Intellect);
			}
			break;

		case EGearType::LHand:
		case EGearType::RHand:
			if (GetCurrentWeapon() && GetCurrentWeapon()->WeaponInstance) {
				if (UWeaponInstance* CurrentWeaponInstance = GetCurrentWeapon()->WeaponInstance)
				{
					PCS->updateStrength(-CurrentWeaponInstance->Strength);
					PCS->updateIntellect(-CurrentWeaponInstance->Intellect);
				}
			}
			break;
		default:
			break;
		}

	}
}

void APlayerCharacter::SetHealAbility(TSubclassOf<UAbility> NewHealAbility)
{
	if (APlayerCharacterState* PCS = SoftPCS.Get()) {
		if (UAbility* CreatedAbility = NewObject<UAbility>(this, NewHealAbility))
		{
			EquippedHealAbility = CreatedAbility;
			PCS->HealAbility = NewHealAbility;
		}

	}
}

void APlayerCharacter::PauseGame()
{
	if(UMainGameInstance*MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (PC) {
			if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(PC)) {
				if (AInGamePlayerHUD* InGamePlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD())) {
					if (InGamePlayerHUD->PauseWidget) {
						PCC->ShowHideCursor(!MGI->bGamePaused);
						UGameplayStatics::SetGamePaused(GetWorld(), !MGI->bGamePaused);
						MGI->bGamePaused = !MGI->bGamePaused;
						MGI->bGamePaused ? InGamePlayerHUD->PauseWidget->SetVisibility(ESlateVisibility::Visible) : InGamePlayerHUD->PauseWidget->SetVisibility(ESlateVisibility::Hidden);

					}
				}
			}
		}
	}
}

void APlayerCharacter::PlayLevelUpWidgetNiagara()
{
	if(LevelUpPressedWidget)
	UNiagaraFunctionLibrary::SpawnSystemAttached(LevelUpPressedWidget, GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true);
}

bool APlayerCharacter::bIsTargetedEnemy(AEnemy* Enemy)
{
	return TargetedEnemy == Enemy;
}

void APlayerCharacter::ResetTargetedEnemy()
{
	TargetedEnemy = nullptr;
}

void APlayerCharacter::UpdateEXPWidget()
{
	if(PlayerHUD)
	{
		if (APlayerCharacterState* pcs = GetPlayerCharacterState()) {
			if (ULevelUpWidget* LevelUpWidget = PlayerHUD->LevelUpWidget)
			{
				LevelUpWidget->UpdateEXPProgressBar(pcs->playerStats.GetEXP() / pcs->playerStats.GetNextLevelEXP());
			}
		}
	}
}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();


	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		PC = PlayerController;
		PlayerHUD = Cast<AInGamePlayerHUD>(PC->GetHUD());

	}


	PlayerAnimInstance = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	if (!PlayerAnimInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("Player Anim Instance failed"));
	}


	if(!PlayerHUD)
	{
		UE_LOG(LogTemp, Error, TEXT("PLAYER HUD FAILED"));
	}


	SoftPCS = Cast<APlayerCharacterState>(GetPlayerState());

	if (CurrentLivesWidget)
	{
		if (LivesWidgetClass) {
			CurrentLivesWidget->SetWidgetClass(LivesWidgetClass);
			CurrentLivesWidget->InitWidget();
		}
	}

	if (SoftPCS) {

		if (APlayerCharacterState* PCS = SoftPCS.Get())
		{
			InitializeActionNameMap(); //this function will have to be recreated to get the current bindings a player puts.

			if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance()))
			{
				if (!GI->localPlayer) {
					GI->localPlayer = this;
				}

				UE_LOG(LogTemp, Warning, TEXT("Load Type %s"), *UEnum::GetValueAsString(GI->LoadType));

				switch (GI->LoadType)
				{
				case ELoadType::SaveGame:
					GI->LoadGameData();

					if (UPlayerSaveGame* Save = GI->GetSavedGame())
					{
						if (GI->bLoadingSucceeded) {
							LoadFromSavedGame(PCS, GI, Save);
						}
						else
						{
							GiveStarterItems();
						}
					}
					break;
				case ELoadType::GameInstance:
					LoadFromGameInstance();
					break;
				case ELoadType::Tutorial:
					GiveStarterItems();
					break;
				}
			}


			PCS->playerStats.Owner = Cast<APlayerController>(Controller);
			if (PCS->HealAbility)
			{
				EquippedHealAbility = NewObject<UAbility>(this, PCS->HealAbility);
			}

			FTimerHandle KillTimer;
			GetWorld()->GetTimerManager().SetTimer(KillTimer, [this,PCS]
			{
				if(PCS && PCS->bInDungeon)
				{
					TakeDamage(MAX_flt, FDamageEvent(), nullptr, nullptr);
				}
			}, 2.5f, false);
		}
	}

	TargetCamLocation = SpringArm->GetRelativeLocation() + CamHeightOffset;

	OriginalCamLocation = SpringArm->GetRelativeLocation();

	GetCharacterMovement()->bEnablePhysicsInteraction = false;
	GetCharacterMovement()->bPushForceUsingZOffset = false;
	GetCharacterMovement()->PushForceFactor = 0.f;

	OnPlayerReady.Broadcast();

}

void APlayerCharacter::Move(const FInputActionValue& Val)
{
	if(Controller != nullptr)
	{

		const FVector2D MoveValue = Val.Get<FVector2D>();
		const FRotator MovementRotation(0, Controller->GetControlRotation().Yaw, 0);
		InputDirection = MoveValue;

		if(TargetedEnemy)
		{
			FVector DirectionToEnemy = (TargetedEnemy->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			// Forward/Backward direction
			if (MoveValue.Y != 0.f)
			{
				// Get forward vector
				AddMovementInput(DirectionToEnemy, MoveValue.Y);
			}

			// Right/Left direction
			if (MoveValue.X != 0.f)
			{
				// Get right vector
				const FVector Direction = FVector::CrossProduct(-DirectionToEnemy, FVector::UpVector);
				AddMovementInput(Direction, MoveValue.X);
				
			}
		}
		else {
			// Forward/Backward direction
			if (MoveValue.Y != 0.f)
			{
				// Get forward vector
				const FVector Direction = MovementRotation.RotateVector(FVector::ForwardVector);
				AddMovementInput(Direction, MoveValue.Y);
			}

			// Right/Left direction
			if (MoveValue.X != 0.f)
			{
				// Get right vector
				const FVector Direction = MovementRotation.RotateVector(FVector::RightVector);

				AddMovementInput(Direction, MoveValue.X);
			}
		}

		GetWorld()->GetTimerManager().SetTimer(FResetLastInputDirectionTimer, this, &APlayerCharacter::ResetLastInputDirection, 0.1, false);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Val)
{
	if (Controller != nullptr)
	{
		const FVector2D LookValue = Val.Get<FVector2D>();

		if (LookValue.X != 0.f)
		{
			AddControllerYawInput(LookValue.X * MouseSens);
		}

		if (LookValue.Y != 0.f)
		{
			AddControllerPitchInput(LookValue.Y * MouseSens);
		}
	}
}

void APlayerCharacter::Attack()
{

	if (APlayerCharacterState* PCS = SoftPCS.Get())
	{
		if (AItem** FoundWeapon = PCS->PlayerGear.Find(EGearType::RHand)) {

			AttackingWeapon = Cast<AWeapon>((*FoundWeapon));
			if (AttackingWeapon)
			{
				float deci = log2f(attackCount + 1.0f) * 0.3f;
				float StaminaCost = AttackingWeapon->StaminaReduction * (1 + deci);
				if (CanPlayerDoAction(EResourceTypes::Stamina, StaminaCost) && CanPlayerDoAction(EResourceTypes::Mana, AttackingWeapon->ManaReduction)) {
					ps = PlayerStates::ATTACKING;
					if (AttackingWeapon->WeaponType == EWeaponType::Bow)
					{
						bIsHolding = true;
						if (AttackingWeapon->Projectile)
						{
							FActorSpawnParameters params;
							params.Owner = this;
							AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(AttackingWeapon->Projectile, params);
							Projectile->AttachToComponent(AttackingWeapon->SkeletalMeshComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("Projectile"));
							AttackingWeapon->CurrentProjectile = Projectile;
						}
					}
					else if (AttackingWeapon->WeaponType == EWeaponType::Mage)
					{
						if (AttackingWeapon->Projectile)
						{
							SpawnProjectile();
						}
					}
				
						FString AttackString = "Attack" + FString::FromInt(attackCount + 1);
						UE_LOG(LogTemp, Warning, TEXT("%s"), *AttackString);

						switch (AttackingWeapon->WeaponType)
						{
						case EWeaponType::OneHand:
							if (AttackingWeapon->OnehandMoveSet)
								PlayAnimMontage(AttackingWeapon->OnehandMoveSet, MontageSpeed, FName(AttackString));
							break;
						case EWeaponType::TwoHand:
							if (AttackingWeapon->TwohandMoveSet)
								PlayAnimMontage(AttackingWeapon->TwohandMoveSet, MontageSpeed, FName(AttackString));
							break;

						case EWeaponType::Mage:
							if (AttackingWeapon->OnehandMoveSet)
								PlayAnimMontage(AttackingWeapon->OnehandMoveSet, MontageSpeed, FName(AttackString));
						default:
							break;
						}
					
				}
				GetWorld()->GetTimerManager().SetTimer(FResetAttackFallBack, this, &APlayerCharacter::ResetState, ResetAttackFallBackTime, false);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("PCS NULL"));
			}

		}
	}

}

void APlayerCharacter::Target(const FInputActionValue& Val)
{

	if(TargetedEnemy)
	{
		TargetedEnemy = nullptr;
		return;
	}

	FVector Start = GetActorLocation();
	TArray angles = { -15.f,-7.5f,0.f,7.5f, 15.f };
	TArray<AActor*> actorsToIgnore;
	actorsToIgnore.Add(this);

	if(	APlayerCharacterState* PCS = SoftPCS.Get())
	{
		actorsToIgnore.Add(PCS->PlayerGear[EGearType::RHand]);
	}

	float ClosestEnemy = 100000;
	for (auto angle : angles) {
		FVector Direction = Cam->GetForwardVector().RotateAngleAxis(angle, GetActorUpVector());
		FVector End = Start + Direction * 1000.f;
		FHitResult Hit;
		if(UKismetSystemLibrary::SphereTraceSingle(GetWorld(), Start, End, 50.0, UEngineTypes::ConvertToTraceType(ECC_Pawn), false, actorsToIgnore, EDrawDebugTrace::Persistent, Hit, true, FLinearColor::Red, FLinearColor::Green, 25.f))
		{
			if(AEnemy*enemy = Cast<AEnemy>(Hit.GetActor()))
			{
				float Distance = FVector::Dist(Start, enemy->GetActorLocation());
				if(Distance < ClosestEnemy)
				{
					ClosestEnemy = Distance;
					TargetedEnemy = enemy;

					if (PC)
					{
						SpringArm->bUsePawnControlRotation = false;
						CamTargeted = true;
						if (!PC->IsLookInputIgnored()) {
						PC->SetIgnoreLookInput(true);
						}
					}
					UE_LOG(LogTemp, Warning, TEXT("Closest Target %s"), *TargetedEnemy->GetName());
				}
			}
		}

	}
}

void APlayerCharacter::UseAbility1(const FInputActionValue& Val)
{
	if (ps != PlayerStates::NONE) return;

	APlayerCharacterState* PCS = SoftPCS.Get();
	if (AItem** FoundWeapon = PCS->PlayerGear.Find(EGearType::RHand)) {
		AttackingWeapon = Cast<AWeapon>((*FoundWeapon));
		if (PCS && PCS->EquippedAbilities.IsValidIndex(0) && PCS->EquippedAbilities[0])
		{
			if (PCS->EquippedAbilities[0]->bShouldExecute_Implementation()) {
				PCS->EquippedAbilities[0]->execute_Implementation();
				RecentlyPressedAbility = 0;
				OnAbilityUsed.Broadcast(0);
			}
		}
	}
}

void APlayerCharacter::UseAbility2(const FInputActionValue& Val)
{
	if (ps != PlayerStates::NONE) return;

	APlayerCharacterState* PCS = SoftPCS.Get();

	if (AItem** FoundWeapon = PCS->PlayerGear.Find(EGearType::RHand)) {
		AttackingWeapon = Cast<AWeapon>((*FoundWeapon));
		if (PCS && PCS->EquippedAbilities.IsValidIndex(1) && PCS->EquippedAbilities[1])
		{
			if (PCS->EquippedAbilities[1]->bShouldExecute_Implementation()) {
				PCS->EquippedAbilities[1]->execute_Implementation();
				RecentlyPressedAbility = 1;
				OnAbilityUsed.Broadcast(1);

			}
		}
	}
}

void APlayerCharacter::UseAbility3(const FInputActionValue& Val)
{
	if (ps != PlayerStates::NONE) return;

	APlayerCharacterState* PCS = SoftPCS.Get();

	if (AItem** FoundWeapon = PCS->PlayerGear.Find(EGearType::RHand)) {
		AttackingWeapon = Cast<AWeapon>((*FoundWeapon));
		if (PCS && PCS->EquippedAbilities.IsValidIndex(2) && PCS->EquippedAbilities[2])
		{
			if (PCS->EquippedAbilities[2]->bShouldExecute_Implementation()) {
				PCS->EquippedAbilities[2]->execute_Implementation();
				RecentlyPressedAbility = 2;
				OnAbilityUsed.Broadcast(2);

			}
		}
	}
}

void APlayerCharacter::UseAbility4(const FInputActionValue& Val)
{
	if (ps != PlayerStates::NONE) return;

	APlayerCharacterState* PCS = SoftPCS.Get();
	if (AItem** FoundWeapon = PCS->PlayerGear.Find(EGearType::RHand)) {
		AttackingWeapon = Cast<AWeapon>((*FoundWeapon));
		if (PCS && PCS->EquippedAbilities.IsValidIndex(3) && PCS->EquippedAbilities[3])
		{
			if (PCS->EquippedAbilities[3]->bShouldExecute_Implementation()) {
				PCS->EquippedAbilities[3]->execute_Implementation();
				RecentlyPressedAbility = 3;
				OnAbilityUsed.Broadcast(3);

			}
		}
	}
}

void APlayerCharacter::UseAbility5(const FInputActionValue& Val)
{
	if (ps != PlayerStates::NONE) return;

	APlayerCharacterState* PCS = SoftPCS.Get();
	if (AItem** FoundWeapon = PCS->PlayerGear.Find(EGearType::RHand)) {
		AttackingWeapon = Cast<AWeapon>((*FoundWeapon));
		if (PCS && PCS->EquippedAbilities.IsValidIndex(4) && PCS->EquippedAbilities[4])
		{
			if (PCS->EquippedAbilities[4]->bShouldExecute_Implementation()) {
				PCS->EquippedAbilities[4]->execute_Implementation();
				RecentlyPressedAbility = 4;
				OnAbilityUsed.Broadcast(4);

			}
		}
	}
}

void APlayerCharacter::UseAbility6(const FInputActionValue& Val)
{
	if (ps != PlayerStates::NONE) return;

	APlayerCharacterState* PCS = SoftPCS.Get();
	if (AItem** FoundWeapon = PCS->PlayerGear.Find(EGearType::RHand)) {
		AttackingWeapon = Cast<AWeapon>((*FoundWeapon));
		if (PCS && PCS->EquippedAbilities.IsValidIndex(5) && PCS->EquippedAbilities[5])
		{
			if (PCS->EquippedAbilities[5]->bShouldExecute_Implementation()) {
				PCS->EquippedAbilities[5]->execute_Implementation();
				RecentlyPressedAbility = 5;
				OnAbilityUsed.Broadcast(5);

			}
		}
	}
}

void APlayerCharacter::Release(const FInputActionValue& Val)
{

	if(APlayerCharacterState* PCS = SoftPCS.Get())
	{
		if (CurrentWeapon)
		{
			if (CurrentWeapon->WeaponType != EWeaponType::Bow) {
				return;
			}

			UE_LOG(LogTemp, Warning, TEXT("Bow released %f"), BowPower);
			if(CurrentWeapon->CurrentProjectile)
			{

				FVector Direction = DeprojectScreen(CurrentWeapon);
				CurrentWeapon->CurrentProjectile->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				CurrentWeapon->CurrentProjectile->LaunchProjectile(BowPower,Direction);
			}
			bIsHolding = false;
			ps = PlayerStates::NONE;
			BowPower = 0.0f;
		}
	}
}

void APlayerCharacter::ApplyElementDebuff(EElementTypes ElementType,AActor*DamageCauser)
{
	if(UMainGameInstance*GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{

		if (TSubclassOf<UElementDebuff>* FoundDebuff = GI->ElementDebuffs.Find(ElementType))
		{
			switch (ElementType)
			{
			case EElementTypes::Physical:
				break;
			case EElementTypes::Fire:
				if(UFireDebuff*FireDebuff = NewObject<UFireDebuff>(this,*FoundDebuff))
				{
					FireDebuff->ExecuteDebuff_Implementation(this,DamageCauser);
				}
				break;
			case EElementTypes::Water:
				if (UWaterDebuff* WaterDebuff = NewObject<UWaterDebuff>(this, *FoundDebuff))
				{
					WaterDebuff->ExecuteDebuff_Implementation(this,DamageCauser);
				}
				break;
			case EElementTypes::Shadow:

				if (UShadowDebuff* ShadowDebuff = NewObject<UShadowDebuff>(this, *FoundDebuff))
				{
					ShadowDebuff->ExecuteDebuff_Implementation(this,DamageCauser);
				}
				break;
			case EElementTypes::Lightening:
				if (ULighteningDebuff* LighteningDebuff = NewObject<ULighteningDebuff>(this, *FoundDebuff))
				{
					LighteningDebuff->ExecuteDebuff_Implementation(this,DamageCauser);
				}
				break;
			case EElementTypes::Bleed:
				if (UBleedDebuff* BleedDebuff = NewObject<UBleedDebuff>(this, *FoundDebuff))
				{
					BleedDebuff->ExecuteDebuff_Implementation(this,DamageCauser);
				}
				break;
			case EElementTypes::Poison:
				if (UPoisonDebuff* PoisonDebuff = NewObject<UPoisonDebuff>(this, *FoundDebuff))
				{
					PoisonDebuff->ExecuteDebuff_Implementation(this,DamageCauser);
				}
				break;
			case EElementTypes::NONE:
				break;
			}
		}
	}
}

void APlayerCharacter::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp,
	bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);

	if (OtherComp && OtherComp->IsSimulatingPhysics())
	{
		FVector Direction = GetVelocity().GetSafeNormal();
		OtherComp->AddImpulse(Direction * 100.f, NAME_None, true);
	}
}

void APlayerCharacter::InteractBag(const FInputActionValue& Val)
{

	if(UMainPlayerWidget*MainPlayerWidget = GetMainPlayerWidget().Get())
	{
		MainPlayerWidget->ExpandBag(!bIsBagOpen);
		if(APlayerCharacterController*PlayerController = Cast<APlayerCharacterController>(Controller))
		{
			PlayerController->ShowHideCursor(!bIsBagOpen);
		}
		bIsBagOpen = !bIsBagOpen;
	}
}

//TODO add show mouse and change input mode from game to UI.
void APlayerCharacter::InteractEquipment(const FInputActionValue& Val)
{
	if (UMainPlayerWidget* MainPlayerWidget = GetMainPlayerWidget().Get())
	{
		MainPlayerWidget->ExpandEquipment(!bIsEquipmentOpen);
		if (APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(Controller))
		{
			PlayerController->ShowHideCursor(!bIsEquipmentOpen);
		}
		bIsEquipmentOpen = !bIsEquipmentOpen;
	}
}

void APlayerCharacter::InteractStats(const FInputActionValue& Val)
{

	if (UMainPlayerWidget* MainPlayerWidget = GetMainPlayerWidget().Get())
	{
		MainPlayerWidget->ExpandStats(!bIsStatsOpen);
		if (APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(Controller))
		{
			PlayerController->ShowHideCursor(!bIsStatsOpen);
		}
		bIsStatsOpen = !bIsStatsOpen;
	}
}

void APlayerCharacter::InteractTalent(const FInputActionValue& Val)
{

	if (UMainPlayerWidget* MainPlayerWidget = GetMainPlayerWidget().Get())
	{
		MainPlayerWidget->ExpandTalents(!bIsTalentsOpen);
		if (APlayerCharacterController* PlayerController = Cast<APlayerCharacterController>(Controller))
		{
			PlayerController->ShowHideCursor(!bIsTalentsOpen);
		}
		bIsTalentsOpen = !bIsTalentsOpen;
	}
}


void APlayerCharacter::Interact(const FInputActionValue& InputActionValue)
{
	WantsToInteract = true;

	if(AItem*Item = Cast<AItem>(OverlappingActor))
	{
		Item->PickUp(this);
	}
	else if(AInteractableActor*InteractableActor = Cast<AInteractableActor>(OverlappingActor))
	{
		InteractableActor->Interact_Implementation();
	}
	else if (ANPC* NPC = Cast<ANPC>(OverlappingActor))
	{
		NPC->Interact_Implementation();
	}
}

void APlayerCharacter::FinishedInteract()
{
	WantsToInteract = false;
}

void APlayerCharacter::UseItem(UItemDataObject* item)
{
	if (!item) return;
	if (!item->ItemData || !item->ItemData->ItemLogicClass) return;
	if (PC) {
		UBaseItem* BaseItem = NewObject<UBaseItem>(this,item->ItemData->ItemLogicClass);
		BaseItem->ItemData = item->ItemData;
		BaseItem->ItemDataObject = item;
		BaseItem->useItem(this);
	}

	if (APlayerCharacterState* PCS = SoftPCS.Get())
	{
		PCS->RemoveItemFromInventory(item);
	}

}

void APlayerCharacter::tryEquipGear(AItem* GearToEquip, const EGearType& GearType, UItemDataObject* ItemInstance)
{
	//if we already have gear on, add bring it into the inventory.

	if (!GearToEquip) { UE_LOG(LogTemp, Error, TEXT("Gear To Equip is nullptr")) return; };

	//if it is a bow we are trying to equip (which is the left hand) we need to override it so that the player can't equip a sword in the right hand and a bow in the left hand.

	switch (GearToEquip->ItemType)
	{
	case EItemType::Armor:
			RemoveItemFromSlot(GearType);
			RemoveGearStats(GearType);
			EquipGear(GearToEquip, GearType,ItemInstance);
		
		break;
	case EItemType::Weapon:
		if(AWeapon*Weapon = Cast<AWeapon>(GearToEquip))
		{
			TArray<EGearType> validSlots = GetValidSlotsForWeapon(Weapon->WeaponType);

			EGearType FinalSlot = EGearType::none;

			if (validSlots.Contains(GearType))
			{
				FinalSlot = GearType;
			}
			else if (validSlots.Num() > 0)
			{
				FinalSlot = validSlots[0];
			}

			if (FinalSlot == EGearType::none)
			{
				return;
			}

			RemoveItemFromSlot(FinalSlot);

			if (Weapon->WeaponType == EWeaponType::TwoHand || Weapon->WeaponType == EWeaponType::Bow)
			{
				RemoveItemFromSlot(EGearType::RHand);
				RemoveItemFromSlot(EGearType::LHand);
			}
			RemoveGearStats(GearType);
			EquipGear(GearToEquip, FinalSlot,ItemInstance);
		}
		break;

	default: break;
	}
	
}

void APlayerCharacter::EquipGear(AItem* GearToEquip, const EGearType& GearType, UItemDataObject* ItemInstance)
{
	if (!GearToEquip) return;

	GearToEquip->SetOwner(this);
	GearToEquip->collisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	APlayerCharacterState* PCS = SoftPCS.Get();

	switch (GearToEquip->ItemType)
	{
	case EItemType::Armor:
		if(PCS)
		{
			if (UGearItemInstance* GearItemInstance= Cast<UGearItemInstance>(ItemInstance))
			{

				PCS->updateStrength(GearItemInstance->Strength);
				PCS->updateIntellect(GearItemInstance->Intellect);
				PCS->PlayerGearInstances.Add(GearType, GearItemInstance);
			}
				
		}
		if (USkeletalMeshComponent* FoundMesh = FindSkeletalMesh(GearType))
		{
			FoundMesh->SetSkeletalMesh(GearToEquip->SkeletalMeshComponent->GetSkeletalMeshAsset());
			FoundMesh->SetMaterial(0,GearToEquip->SkeletalMeshComponent->GetMaterial(0));
		}
		break;
	case EItemType::Weapon:
		if(AWeapon*Weapon = Cast<AWeapon>(GearToEquip))
		{
			if (PCS)
			{
				PCS->OnWeaponUpdated.Broadcast(Weapon);
			}

			FName GearSocket = GetValidSocket(GearType, Weapon);

			if (PlayerAnimInstance)
			{
				switch (Weapon->WeaponType)
				{
				case EWeaponType::OneHand:
					if (Weapon->OneHandBlendSpace)
						PlayerAnimInstance->CurrentWeaponBlendSpace = Weapon->OneHandBlendSpace;
					break;
				case EWeaponType::TwoHand:
					if (Weapon->TwoHandBlendSpace)
						PlayerAnimInstance->CurrentWeaponBlendSpace = Weapon->TwoHandBlendSpace;
					break;

				default:
					break;
				}
			}
		
			GearToEquip->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, GearSocket);
			if (UWeaponInstance* WeaponItemInstance = Cast<UWeaponInstance>(ItemInstance)) {
				PCS->updateStrength(WeaponItemInstance->Strength);
				PCS->updateIntellect(WeaponItemInstance->Intellect);
				Weapon->WeaponInstance = WeaponItemInstance;
			}
			Weapon->AttachElementType();
			CurrentWeapon = Weapon;
			CurrentWeapon->OriginalTransform = CurrentWeapon->GetActorTransform();
		}

		break;


	default: break;
	}
	if (PCS)
	{
		PCS->PlayerGear.Add(GearType, GearToEquip);
		updateGearSlot(ItemInstance);
	}



}

FName APlayerCharacter::GetValidSocket(EGearType Slot, AWeapon* Weapon)
{
	if(Weapon)
	{
		switch (Weapon->WeaponType)
		{
		case EWeaponType::OneHand:
		case EWeaponType::TwoHand:
			bUseControllerRotationYaw = false;
			return FName("hand_r_socket");
		case EWeaponType::Bow:
			bUseControllerRotationYaw = true;
			return  FName("hand_l");
		case EWeaponType::Mage:
			return FName("hand_r_socket");
		}
	}
	else {
		switch (Slot) {
		case EGearType::none:
			break;
		case EGearType::Head:
			return FName("HeadGear");
		case EGearType::LHand:
		return	FName("hand_l");

		case EGearType::RHand:
		return FName( "hand_r_socket");
		case EGearType::Chest:
			return FName("ChestGear");
		case EGearType::Legs:
		return	FName("LegGear");
		}
	}
	return NAME_None;
}

//this equips the weapon to that hand.
TArray<EGearType> APlayerCharacter::GetValidSlotsForWeapon(EWeaponType WeaponType)
{
	TArray<EGearType> Result;
	switch (WeaponType)
	{
	case EWeaponType::OneHand:
	case EWeaponType::Mage:
		Result.Add(EGearType::RHand);
		Result.Add(EGearType::LHand);
		break;

	case EWeaponType::TwoHand:
		Result.Add(EGearType::RHand);
		break;

	case EWeaponType::Bow:
		Result.Add(EGearType::RHand);
	}

	return Result;
}

void APlayerCharacter::RemoveItemFromSlot(EGearType Slot)
{
    if (APlayerCharacterState* PCS = SoftPCS.Get())
    {
        if (AItem** itemReturned = PCS->PlayerGear.Find(Slot))
        {
            if (AItem* Gear = *itemReturned)
            {
                // Add the item instance back to inventory if possible
                if (Slot == EGearType::Head || Slot == EGearType::Chest || Slot == EGearType::Legs)
                {
                    if (UGearItemInstance** GearItemInstance = PCS->PlayerGearInstances.Find(Slot))
                    {
                        PCS->AddItemToInventory(*GearItemInstance);
                        PCS->PlayerGearInstances.Remove(Slot);
                    }
                }
                else if (Slot == EGearType::RHand || Slot == EGearType::LHand)
                {
                    if (AWeapon* Weapon = Cast<AWeapon>(Gear))
                    {

						if (Weapon->WeaponInstance)
						{
							PCS->AddItemToInventory(Weapon->WeaponInstance);
							Weapon->Destroy();

						}
                    }
                }
                PCS->PlayerGear.Remove(Slot);
            }
        }
    }
}

float APlayerCharacter::GetPlayerMontageTime()
{
    float CurrentMontagePosition = -1.f;
    if(GetMesh() && GetMesh()->GetAnimInstance())
	{
        if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(OneHandMontages))
        {
            CurrentMontagePosition = GetMesh()->GetAnimInstance()->Montage_GetPosition(OneHandMontages);
        }
        else if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(TwoHandMontages))
        {
            CurrentMontagePosition = GetMesh()->GetAnimInstance()->Montage_GetPosition(TwoHandMontages);
        }
        else if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(DodgeMontages))
        {
            CurrentMontagePosition = GetMesh()->GetAnimInstance()->Montage_GetPosition(DodgeMontages);
        }
        else if (GetMesh()->GetAnimInstance()->Montage_IsPlaying(MiscMontages))
        {
            CurrentMontagePosition = GetMesh()->GetAnimInstance()->Montage_GetPosition(MiscMontages);
        }
    }
    return CurrentMontagePosition;
}

void APlayerCharacter::PlayerStunned()
{
	ps = PlayerStates::STUNNED;
	PlayAnimMontage(MiscMontages, 1, "Stunned");
}

bool APlayerCharacter::CanPlayerDoAction(EResourceTypes ResourceType, const float& AmountNeeded)
{
	if (ps != PlayerStates::NONE) return false;
	if (!SoftPCS.IsValid()) return false;


	if (APlayerCharacterState* PCS = SoftPCS.Get()) {
		TWeakObjectPtr<APlayerCharacter> Self = this;

		switch (ResourceType)
		{
		case EResourceTypes::Stamina:
			if (PCS->playerStats.currentStamina < AmountNeeded)
			{
				return false;
			}
				PCS->playerStats.currentStamina -= AmountNeeded;
				if (PlayerHUD && PlayerHUD->GetMainUIWidget())
				{
					PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Stamina", PCS->playerStats.currentStamina / PCS->GetTotalStamina());
				}

			if(GetWorldTimerManager().IsTimerActive(StaminaTimerHandle))
			{
				GetWorldTimerManager().ClearTimer(StaminaTimerHandle);
			}

			GetWorldTimerManager().SetTimer(StaminaTimerHandle, [this,PCS,Self]()
			{

					if (!Self.IsValid() || !IsValid(PCS)) return;

				if(PCS)
				{
					if(PCS->playerStats.currentStamina >= PCS->playerStats.totalStamina)
					{
						GetWorldTimerManager().ClearTimer(Self->StaminaTimerHandle);
						return;
					}
					PCS->playerStats.IncreaseStamina();
					if (PlayerHUD && PlayerHUD->GetMainUIWidget())
					{
						PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Stamina", PCS->playerStats.currentStamina / PCS->GetTotalStamina());
					}
				}
			}, 0.5, true);

			break;
		case EResourceTypes::Mana:
			if (PCS->playerStats.currentMana < AmountNeeded)
			{
				return false;
			}
				PCS->playerStats.currentMana -= AmountNeeded;
				if(PlayerHUD && PlayerHUD->GetMainUIWidget())
				{
				
						PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Mana", PCS->playerStats.currentMana / PCS->GetTotalMana());
				}
			
			break;
		}
	}

	return true;
}


void APlayerCharacter::Block(const FInputActionValue& InputActionValue)
{
	bool isPressed = InputActionValue.Get<bool>();
	if(CanPlayerDoAction(EResourceTypes::Stamina,0.f) && isPressed )
	{
		ps = PlayerStates::BLOCKING;
		if (CurrentWeapon)
		{
			if(CurrentWeapon->Block)
			{
				PlayAnimMontage(CurrentWeapon->Block, 1, "Block1");
			}
		}
	}
	else if(ps == PlayerStates::BLOCKING && !isPressed)
	{
		if (CurrentWeapon)
		{
			if (CurrentWeapon->Block)
			{
				GetMesh()->GetAnimInstance()->Montage_Stop(0.f,CurrentWeapon->Block);
			
				UE_LOG(LogTemp, Warning, TEXT("Attempting to stop blocking"));

			}
		}
		ps = PlayerStates::NONE;

	}
}

void APlayerCharacter::SetParrySuccess(bool bNewValue)
{
	bParrySuccess = bNewValue;
}

void APlayerCharacter::SetHitDirection(const EHitDirection& NewHitDirection)
{
	HitDirection = NewHitDirection;
}

void APlayerCharacter::DetermineHitAnimation(const float& poise)
{

	if (ps == PlayerStates::HIT && poise < 0) {
		switch (HitDirection)
		{
		case EHitDirection::FRONT:
			PlayAnimMontage(MiscMontages, 1, FName("HitFRONT"));
			break;
		case EHitDirection::BACK:
			PlayAnimMontage(MiscMontages, 1, FName("HitBACK"));
			break;
		case EHitDirection::LEFT:
			PlayAnimMontage(MiscMontages, 1, FName("HitLEFT"));
			break;
		case EHitDirection::RIGHT:
			PlayAnimMontage(MiscMontages, 1, FName("HitRIGHT"));
			break;
		}
	}
	else if(ps == PlayerStates::HIT)
	{
		switch (HitDirection)
		{
		case EHitDirection::FRONT:
			PlayAnimMontage(MiscMontages, 1, FName("LightHitFRONT"));
			break;
		case EHitDirection::BACK:
			PlayAnimMontage(MiscMontages, 1, FName("LightHitBACK"));
			break;
		case EHitDirection::LEFT:
			PlayAnimMontage(MiscMontages, 1, FName("LightHitLEFT"));
			break;
		case EHitDirection::RIGHT:
			PlayAnimMontage(MiscMontages, 1, FName("LightHitRIGHT"));
			break;
		}
	}
}

bool APlayerCharacter::IsBackStab(AEnemy* Enemy)
{
	if (!Enemy) return false;

	// Check if backstab timer is still active (prevent rapid backstabs)
	if (Enemy->GetWorld()->GetTimerManager().IsTimerActive(Enemy->FBackstabTimer)) {
		return false;
	}

	FVector EnemyToPlayer = (GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
	float DotResult = FVector::DotProduct(EnemyToPlayer, Enemy->GetActorForwardVector());
	float threshold = FMath::Cos(FMath::DegreesToRadians(150.f)); // About -0.866

	UE_LOG(LogTemp, Warning, TEXT("Backstab check - Dot: %f, Threshold: %f, Behind: %s"),
		DotResult, threshold, (DotResult < threshold) ? TEXT("YES") : TEXT("NO"));

	return DotResult < threshold &&
		Enemy->MiscMontages &&
		Enemy->MiscMontages->IsValidSectionName("BackstabRecieved");
}


bool APlayerCharacter::IsParry(AEnemy* Enemy)
{
	if (!Enemy || Enemy->EnemyState != EEnemyStates::parried) return false;

	FVector EnemyToPlayer = (GetActorLocation() - Enemy->GetActorLocation()).GetSafeNormal();
	float DotResult = FVector::DotProduct(EnemyToPlayer, Enemy->GetActorForwardVector()); //should be positive for in front.
	float threshold = FMath::Cos(FMath::DegreesToRadians(150.f));

	UE_LOG(LogTemp, Warning, TEXT("DOT RESULT (parry) : %f"), DotResult)

		return DotResult > threshold && Enemy->MiscMontages->IsValidSectionName("ParryRecieved");
}

float APlayerCharacter::DeterminePoise(float PoiseDamage)
{
	//rework this.
	if(APlayerCharacterState* PCS = SoftPCS.Get())
	{
		float Protection = 0.f;
		if(UGearItemInstance**ChestItem = PCS->PlayerGearInstances.Find(EGearType::Chest))
		{
			Protection += (*ChestItem)->PoiseProtection;
		}
		if(UGearItemInstance**HelmItem = PCS->PlayerGearInstances.Find(EGearType::Head))
		{
			Protection += (*HelmItem)->PoiseProtection;
		}
		if (UGearItemInstance**LegItem = PCS->PlayerGearInstances.Find(EGearType::Legs))
		{
			Protection += (*LegItem)->PoiseProtection;
		}

		PoiseDamage = PoiseDamage - (PoiseDamage * Protection);
	}
	return PoiseDamage;
}

void APlayerCharacter::ToggleHitBox(ECollisionEnabled::Type NewCollisionChannel) const
{
	GetCapsuleComponent()->SetCollisionEnabled(NewCollisionChannel);
}

void APlayerCharacter::ResetCameraPosition(float DeltaTime)
{
	SpringArm->SetRelativeLocation(OriginalCamLocation);

	if (PC)
	{
		PC->SetControlRotation(GetActorRotation());
		PC->SetIgnoreLookInput(false);
		UE_LOG(LogTemp, Log, TEXT("PC SUCCEED"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("PC FAILED"));
	}

	CamTargeted = false;
	SpringArm->bUsePawnControlRotation = true;
}

APlayerCharacterState* APlayerCharacter::GetPlayerCharacterState() const
{
	if(APlayerCharacterState* PCS = SoftPCS.Get())
	{
		return PCS;
	}
	
	return nullptr;
}

void APlayerCharacter::ToggleWeaponType()
{
	if (!GetPlayerCharacterState()) return;

	if (!GetPlayerCharacterState()->PlayerGear.Find(EGearType::RHand)) return;

	if (AWeapon*Weapon = Cast<AWeapon>(GetPlayerCharacterState()->PlayerGear[EGearType::RHand]))
	{
		if (PlayerAnimInstance) {
			//TODO check if we have the correct stats to swap hands type.
			switch (Weapon->WeaponType)
			{
			case EWeaponType::OneHand:
				Weapon->WeaponType = EWeaponType::TwoHand;
				if (Weapon->OneHandBlendSpace)
					PlayerAnimInstance->CurrentWeaponBlendSpace = Weapon->TwoHandBlendSpace;
				break;
			case EWeaponType::TwoHand:
				Weapon->WeaponType = EWeaponType::OneHand;
				if (Weapon->TwoHandBlendSpace)
					PlayerAnimInstance->CurrentWeaponBlendSpace = Weapon->OneHandBlendSpace;
				break;
			default:
				break;
			}
		}
	}

}

USkeletalMeshComponent* APlayerCharacter::FindSkeletalMesh(EGearType GearType)
{
	switch (GearType)
	{
	case EGearType::Head:
		return Helm;
	case EGearType::Chest:
		return Chest;
	case EGearType::Legs:
		return Legs;
	default:
		break;
	}
	return nullptr;
}

FVector APlayerCharacter::DeprojectScreen(AWeapon*Weapon)
{

	if (!Weapon || !Weapon->CurrentProjectile) return FVector(0.f);

	int32 ViewportSizeX, ViewportSizeY;
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetViewportSize(ViewportSizeX, ViewportSizeY);

	// Center of the screen
	float ScreenCenterX = ViewportSizeX * 0.5f;
	float ScreenCenterY = ViewportSizeY * 0.5f;

	FVector WorldOrigin;
	FVector WorldDirection;

	// Deproject the screen position to a world space ray
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->DeprojectScreenPositionToWorld(
		ScreenCenterX,
		ScreenCenterY,
		WorldOrigin,
		WorldDirection
	);

	FHitResult hitResult;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(AimTrace), true, this);
	FVector TargetLocation;
	FVector End = WorldOrigin + (WorldDirection * 2000.f);
	if (GetWorld()->LineTraceSingleByChannel(hitResult, WorldOrigin, End, ECC_Pawn, TraceParams))
	{
		TargetLocation = hitResult.ImpactPoint;
	}
	else
	{
		TargetLocation = End;
	}

	//DrawDebugLine(GetWorld(), Weapon->CurrentProjectile->GetActorLocation(), TargetLocation, FColor::Red, true);

	return (TargetLocation - Weapon->CurrentProjectile->GetActorLocation()).GetSafeNormal();

}

void APlayerCharacter::UnEquipGear(AItem* GearToUnEquip)
{
	if (!GearToUnEquip) return;

	APlayerCharacterState* PCS = SoftPCS.Get();


	switch(GearToUnEquip->ItemType)
	{
	case EItemType::Armor:
		if(UGearItemData*GearItemData = Cast<UGearItemData>(GearToUnEquip->ItemData))
		{
			if(USkeletalMeshComponent*FoundMesh = FindSkeletalMesh(GearItemData->GearType))
			{
				if(PCS)
				{
					PCS->PlayerGear.Remove(GearItemData->GearType);
				}
				FoundMesh->SetSkeletalMeshAsset(nullptr);
			}
		}
		break;
	case EItemType::Weapon:
		if(UWeaponItemData*WeaponItemData = Cast<UWeaponItemData>(GearToUnEquip->ItemData))
		{
			if (PCS)
			{
				PCS->PlayerGear[WeaponItemData->GearType]->Destroy();
				PCS->PlayerGear.Remove(WeaponItemData->GearType);

			}
		}
		break;

	default: break;
	}
}

void APlayerCharacter::HandleEnemyAttack(AEnemy* Enemy,float& DamageAmount)
{

	APlayerCharacterState* PCS = SoftPCS.Get();
	if (!PCS) return;

	EElementTypes ElementType = PCS->playerStats.IncreaseElementProgression(Enemy->EnemyParameters.ElementType, Enemy->EnemyParameters.ResistanceDamage);

	if (ElementType != EElementTypes::NONE)
	{
		ApplyElementDebuff(ElementType, Enemy);
	}


	if(Enemy->EnemyParameters.ElementType != EElementTypes::NONE && Enemy->EnemyParameters.ElementType != EElementTypes::Physical)
	{
		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			if (UNiagaraSystem** NS = MGI->ElementalDamageMap.Find(Enemy->EnemyParameters.ElementType))
			{
				if (UNiagaraComponent* NC = UNiagaraFunctionLibrary::SpawnSystemAttached(*NS, GetMesh(), "root", FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::SnapToTargetIncludingScale, true))
				{
					NC->SetVariableObject("User.EnemyMesh", GetMesh());
				}
			}
		}
	}

	if (APlayerCharacterController* PCC = Cast<APlayerCharacterController>(Controller))
	{
		PCC->PlayCameraShake();
	}

	switch (ps)
	{
	case PlayerStates::BLOCKING:
		//arbitrary value
		PCS->playerStats.currentHealth -= DamageAmount / 3;


		PCS->playerStats.currentPoise -= DeterminePoise(Enemy->EnemyParameters.poiseDamage) * 2;

		if (CurrentWeapon) {
			if (HitDirection == EHitDirection::RIGHT) {
				PlayAnimMontage(CurrentWeapon->Block, 1, "BlockRight");
			}
			else
			{
				PlayAnimMontage(CurrentWeapon->Block, 1, "BlockLeft");
			}
		}
		if (PCS->playerStats.currentPoise <= 0)
		{
			PlayerStunned();
		}
		UE_LOG(LogTemp, Error, TEXT("BLOCKED"));
		break;
	default:


		if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
		{
			if (GI->BloodSystems.IsValidIndex(0))
			{
				// This spawns a Niagara system once, auto-destroys it when done
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GI->BloodSystems[0], GetActorLocation());
			}
		}

		PCS->playerStats.currentHealth -= DamageAmount;

		if (UAbility** FoundAbility = PCS->LearnedAbilities.FindByPredicate([](UAbility* Ability) {return Ability && Ability->IsA(UAbility_Reflect_Damage::StaticClass()); }))
		{
			if(UAbility_Reflect_Damage*ReflectAbility = Cast<UAbility_Reflect_Damage>(*FoundAbility))
			{
				ReflectAbility->DamageTaken = DamageAmount;
				ReflectAbility->DamageCauser = Enemy;

				if(ReflectAbility->bShouldExecute_Implementation())
				{
					ReflectAbility->execute_Implementation();
				}
			}
		}

		if (PlayerHUD)
		{
			if (PlayerHUD->GetMainUIWidget())
			{
				PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
			}
		}

		PCS->playerStats.currentPoise -= DeterminePoise(Enemy->EnemyParameters.poiseDamage);

		ps = PlayerStates::HIT;
		DetermineHitAnimation(PCS->playerStats.currentPoise);
		UE_LOG(LogTemp, Error, TEXT("NOT BLOCKED"));

		break;
	}

	PCS->playerStats.currentHealth = FMath::Clamp(PCS->playerStats.currentHealth, 0.f, PCS->GetTotalHealth());

	if (GetWorldTimerManager().IsTimerActive(BuildPoise))
	{
		GetWorldTimerManager().ClearTimer(BuildPoise);
	}


	GetWorldTimerManager().SetTimer(BuildPoise, [this]()
		{
			if (APlayerCharacterState*PCS = SoftPCS.Get())
			{
				if (PCS->playerStats.currentPoise >= PCS->playerStats.totalPoise)
				{
					GetWorldTimerManager().ClearTimer(BuildPoise);
					return;
				}

				PCS->playerStats.currentPoise = PCS->playerStats.currentPoise + (PCS->playerStats.totalPoise * 0.2);
				PCS->playerStats.currentPoise = FMath::Clamp(PCS->playerStats.currentPoise, 0.f, PCS->playerStats.totalPoise);
			}
		}, 0.5, true);


}

void APlayerCharacter::HandleDebuffAttack(ADebuffActor* DebuffActor, float& DamageAmount)
{
	APlayerCharacterState* PCS = SoftPCS.Get();


	if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance()))
	{
		if (GI->BloodSystems.IsValidIndex(0))
		{
			// This spawns a Niagara system once, auto-destroys it when done
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), GI->BloodSystems[0], GetActorLocation());
		}
	}

	PCS->playerStats.currentHealth -= DamageAmount;


	if (PlayerHUD)
	{
		if (PlayerHUD->GetMainUIWidget())
		{
			PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
		}
	}

	ps = PlayerStates::HIT;
	DetermineHitAnimation(PCS->playerStats.currentPoise);
}

void APlayerCharacter::LiquidEntered(ULiquidDataAsset*LiquidDataAsset)
{

	if (!LiquidDataAsset) return;

	APlayerCharacterState* PCS = SoftPCS.Get();
	EElementTypes ElementType = LiquidDataAsset->ElementType;

	if(LiquidDebuffs.Find(ElementType))
	{
		return;
	}

	LiquidDebuffs.Add(ElementType, LiquidDataAsset);

	if(GetCharacterMovement() && GetCharacterMovement()->MaxWalkSpeed != 200.f)
	{
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
	}

	if(PCS)
	{
		TWeakObjectPtr<ULiquidDataAsset> SafeLiquidDA = LiquidDataAsset;
		TWeakObjectPtr<APlayerCharacter>SafePlayer = this;
		TWeakObjectPtr<APlayerCharacterState> safePCS = PCS;
		GetWorldTimerManager().SetTimer(LiquidTimerHandle, [SafePlayer,ElementType,SafeLiquidDA,safePCS]()
		{

				if (!SafeLiquidDA.IsValid() || !SafePlayer.IsValid() || !safePCS.IsValid()) return;

				APlayerCharacterState* LocalPCS = safePCS.Get();
				APlayerCharacter* LocalPlayer = SafePlayer.Get();

				if (!LocalPlayer || !LocalPCS) return;

				if (ULiquidDataAsset* LocalLiquidDA = SafeLiquidDA.Get()) {

					if (ElementType == EElementTypes::Fire)
					{
						LocalPlayer->TakeDamage(LocalLiquidDA->ElementRawDamage, FDamageEvent(), nullptr, nullptr);
					}

					if (LocalPlayer->CurrentDebuffs.Find(ElementType)) return;

					EElementTypes Result = LocalPCS->playerStats.IncreaseElementProgression(ElementType, LocalLiquidDA->ElementResistanceDamageRate);


					if (Result != EElementTypes::NONE)
					{
						LocalPlayer->ApplyElementDebuff(ElementType, nullptr);
					}
				}

		}, .5f, true);
	}
}

void APlayerCharacter::LiquidExited(ULiquidDataAsset* LiquidDataAsset)
{

	if (!LiquidDataAsset) return;

	LiquidDebuffs.Remove(LiquidDataAsset->ElementType);
	CurrentLiquid = nullptr;
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	if(GetWorldTimerManager().IsTimerActive(LiquidTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(LiquidTimerHandle);
	}
	UE_LOG(LogTemp, Warning, TEXT("Liquid Exited"));

}

void APlayerCharacter::AbilityHeal()
{
	if(EquippedHealAbility)
	{
		if(EquippedHealAbility->bShouldExecute())
		{
			SetPlayerState(PlayerStates::HEALING);
			EquippedHealAbility->execute();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Heal called"));
}

AProjectile* APlayerCharacter::SpawnProjectile(FVector Size, float DamageModifier, EProjectileType ProjectileType,float Speed, float GravityScale)
{

	if (!AttackingWeapon) return nullptr;

	FActorSpawnParameters params;
	params.Owner = this;
	if (AProjectile* Projectile = GetWorld()->SpawnActor<AProjectile>(AttackingWeapon->Projectile, GetActorLocation(), FRotator(0.f), params)) {
		Projectile->SetActorScale3D(Size);
		Projectile->DamageModifier = DamageModifier;
		Projectile->ProjectileType = ProjectileType;
		Projectile->ProjectileMovementComponent->ProjectileGravityScale = GravityScale;
		AttackingWeapon->CurrentProjectile = Projectile;
		FVector Direction = DeprojectScreen(AttackingWeapon);
		Projectile->LaunchProjectile(Speed, Direction);
		SetActorRotation(Direction.Rotation());
		return Projectile;
	}
	return nullptr;
}

void APlayerCharacter::SetNewSoftPCS(APlayerCharacterState* NewSoftPCS)
{
	SoftPCS = NewSoftPCS;
}

void APlayerCharacter::ResetPlayer(APlayerCharacterState* playerCharacterState)
{
	if (!playerCharacterState) return;
	SetActorLocation(SpawnLocation);
	playerCharacterState->playerStats.currentHealth = playerCharacterState->GetTotalHealth();
	playerCharacterState->playerStats.currentStamina = playerCharacterState->GetTotalStamina();
	playerCharacterState->playerStats.currentMana = playerCharacterState->GetTotalMana();
	playerCharacterState->playerStats.currentFireProgression = 0;
	playerCharacterState->playerStats.currentLighteningProgression = 0;
 playerCharacterState->playerStats.currentPoisonResistance = 0;
 playerCharacterState->playerStats.currentBleedResistance = 0;
	playerCharacterState->playerStats.currentShadowProgression = 0;
	playerCharacterState->playerStats.currentWaterProgression = 0;
	
	CurrentDebuffs.Empty();
	SetActorEnableCollision(true);

	if (PlayerHUD)
	{
		if (PlayerHUD->GetMainUIWidget())
		{
			PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", playerCharacterState->playerStats.currentHealth / playerCharacterState->GetTotalHealth());
			PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Stamina", playerCharacterState->playerStats.currentStamina / playerCharacterState->GetTotalStamina());
			PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Mana", playerCharacterState->playerStats.currentMana / playerCharacterState->GetTotalMana());
		}
	}
	ps = PlayerStates::NONE;
	FTimerHandle FResetWidgetCompHandle;
	GetWorld()->GetTimerManager().SetTimer(FResetWidgetCompHandle, [this]()
	{
		if(CurrentLivesWidget)
		{
			CurrentLivesWidget->SetVisibility(false);
		}
	}, 1.5f, false);
	bDead = false;

}

void APlayerCharacter::ShowLivesWidget()
{
	if (!CurrentLivesWidget || !IsValid(CurrentLivesWidget->GetWidget()) ||!SoftPCS) return;

	if(ULivesWidget*LivesWidgetCasted = Cast<ULivesWidget>(CurrentLivesWidget->GetWidget()))
	{
		if (APlayerCharacterState* localPCS = SoftPCS.Get()) {
			CurrentLivesWidget->SetVisibility(true);
			FTimerHandle FUpdateTextHandle;
			GetWorldTimerManager().SetTimer(FUpdateTextHandle, [this,localPCS,LivesWidgetCasted]
			{
					if (!IsValid(LivesWidgetCasted) || !IsValid(localPCS)) return;

					LivesWidgetCasted->UpdateLivesText(localPCS->GetCurrentLives());

			}, 1.f, false);
		}
	}
	
}

TWeakObjectPtr<UMainPlayerWidget> APlayerCharacter::GetMainPlayerWidget()
{

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if(AInGamePlayerHUD*PH  = Cast<AInGamePlayerHUD>(PlayerController->GetHUD()))
		{
			if(UMainPlayerWidget*MPW = PH->GetMainUIWidget())
			{
				return MPW;
			}
		}
	}
	return nullptr;
}


void APlayerCharacter::InitializeActionNameMap()
{
	if (!SoftPCS.IsValid()) return;

	APlayerCharacterState* PCS = SoftPCS.Get();

	if (!PCS) return;

	TArray<UInputAction*>AllActions = { InputConfig->InputMove, InputConfig->InputLook, InputConfig->InputAttack, InputConfig->InputTarget, InputConfig->InputDodge, InputConfig->InputInteract, InputConfig->InputAbility1, InputConfig->InputAbility2, InputConfig->InputAbility3, InputConfig->InputAbility4, InputConfig->InputAbility5, InputConfig->InputAbility6, InputConfig->InputBlock, InputConfig->InputHandSwap, InputConfig->InputAbilityHeal };
	for (int i = 0; i < AllActions.Num(); i++) {
		if(UInputAction* Action = AllActions[i]){
		for (const FEnhancedActionKeyMapping& Mapping : InputMapping->GetMappings())
			{

			if (Mapping.Action == Action)
			{
				FKey Key = Mapping.Key;
				FString Name = Mapping.Action->GetName();
				PCS->InputMap.Add(Name, Key);
				break;
			}

			}	
		}
	}

}

void APlayerCharacter::LoadFromGameInstance()
{

	if (!SoftPCS.IsValid()) return;

	if (APlayerCharacterState* PCS = SoftPCS.Get()) {
		if (UMainGameInstance* GI = Cast<UMainGameInstance>(GetGameInstance()))
		{

			PCS->playerStats = GI->GIPlayerStats;
			PCS->HealAbility = GI->GIHealAbility;

			PCS->ReBindDelegates();
			PCS->SetCurrentLives(GI->GICurrentLives);

			if (ADungeonCrawlerGM* GM = GetWorld()->GetAuthGameMode<ADungeonCrawlerGM>())
			{
				GM->SetRoundCount(GI->GIRound);
			}
			for (FQuest_SaveData& QuestSaveData : GI->GICurrentQuestSaveData)
			{
				QuestSaveData.Init(PCS);
			}

			if (PlayerHUD)
			{
				if (ULevelUpWidget* LUW = PlayerHUD->LevelUpWidget)
				{
					LUW->RefreshAllStats();
					LUW->RebindDelegates(PCS);
				}
				if (UMainPlayerWidget* MPI = PlayerHUD->GetMainUIWidget())
				{
					MPI->RebindDelegates();
					MPI->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
					if (!GI->GILearnedAbilities.IsEmpty()) {
						PCS->LearnedAbilitiesSaveData = GI->GILearnedAbilities;
						PCS->DeserializeAbilities();
					}
				}
				if (UTalentTreeWidget* TalentTreeWidget = PlayerHUD->TalentTreeWidget)
				{
					TalentTreeWidget->CheckAllRows();
					TalentTreeWidget->CheckLearnedAbilities();

				}
			}
			//update UI.
			if (!GI->GICurrentWeapon.WeaponID.IsEmpty())
			{
				if (UDataAsset** SavedWeaponDA = GI->AllItems.Find(GI->GICurrentWeapon.WeaponID))
				{
					if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(SavedWeaponDA[0]))
					{
						FActorSpawnParameters params;
						params.Owner = PCS;

						if (AWeapon* SavedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponItemData->ItemToSpawn, FVector::ZeroVector, FRotator::ZeroRotator, params))
						{
							SavedWeapon->InitWeapon();
							GI->GICurrentWeapon.InitWeapon(SavedWeapon->WeaponInstance);
							tryEquipGear(SavedWeapon, WeaponItemData->GearType, SavedWeapon->WeaponInstance);
							UE_LOG(LogTemp, Warning, TEXT("Loaded from Saved Weapon"));
						}
					}
				}
			}

			for (FGearItemInstanceSaveData& Data : GI->GICurrentGear)
			{
				if (UDataAsset** GearDA = GI->AllItems.Find(Data.GearID))
				{
					if (UGearItemData* GearItemData = Cast<UGearItemData>(GearDA[0]))
					{
						if (GearItemData->ItemToSpawn) {
							if (AItem* GearToEquip = GearItemData->ItemToSpawn->GetDefaultObject<AItem>()) {
								if (UGearItemInstance* GearItemInstance = NewObject<UGearItemInstance>(PCS))
								{
									GearItemInstance->ItemData = GearItemData;
									Data.InitGear(GearItemInstance);
									tryEquipGear(GearToEquip, GearItemData->GearType, GearItemInstance);

								}
							}
						}
					}

				}
			}

			//reload inventory
			//check for weapons
			if (!GI->GIInventoryWeapons.IsEmpty())
			{
				for (FWeaponInstanceSaveData& Data : GI->GIInventoryWeapons)
				{
					if (UDataAsset** WeaponDA = GI->AllItems.Find(Data.WeaponID))
					{
						if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(WeaponDA[0]))
						{
							if (UWeaponInstance* WeaponInstance = NewObject<UWeaponInstance>(PCS))
							{
								WeaponInstance->ItemData = WeaponItemData;
								Data.InitWeapon(WeaponInstance);
								PCS->AddItemToInventory(WeaponInstance);
							}

						}

					}
				}
			}
			//check for gear
			if (!GI->GIInventoryGear.IsEmpty())
			{
				for (FGearItemInstanceSaveData& Data : GI->GIInventoryGear)
				{
					if (UDataAsset** GearDA = GI->AllItems.Find(Data.GearID))
					{
						if (UGearItemData* GearItemData = Cast<UGearItemData>(GearDA[0]))
						{
							if (UGearItemInstance* GearItemInstance = NewObject<UGearItemInstance>(PCS))
							{
								GearItemInstance->ItemData = GearItemData;
								Data.InitGear(GearItemInstance);
								PCS->AddItemToInventory(GearItemInstance);

							}

						}

					}
				}

			}
			//check for regular items
			if (!GI->GIInventory.IsEmpty())
			{
				for (FItemInstance& Data : GI->GIInventory)
				{
					if (UDataAsset** ItemDA = GI->AllItems.Find(Data.ItemDataID))
					{
						if (UItemDataAsset* ItemData = Cast<UItemDataAsset>(ItemDA[0]))
						{
							if (UItemDataObject* ItemDataObject = NewObject<UItemDataObject>(PCS))
							{
								ItemDataObject->ItemData = ItemData;
								ItemDataObject->SetQuantity(Data.Quantity);
								PCS->AddItemToInventory(ItemDataObject);

							}
						}
					}
				}
			}

		}

	}
}

void APlayerCharacter::LoadFromSavedGame(APlayerCharacterState* PCS, UMainGameInstance* GI, UPlayerSaveGame* Save)
{

	if (!PCS || !Save) return;

	PCS->playerStats = Save->playerStats;


	PCS->HealAbility = Save->HealAbility;

	PCS->ReBindDelegates();
	PCS->SetCurrentLives(Save->Lives);
	PCS->bInDungeon = Save->bInDungeon;

	if(ADungeonCrawlerGM*GM = GetWorld()->GetAuthGameMode<ADungeonCrawlerGM>())
	{
		GM->SetRoundCount(Save->Round);
	}

	for (FQuest_SaveData& QuestSaveData : Save->CurrentQuestSaveData)
	{
		QuestSaveData.Init(PCS);
	}

	if (PlayerHUD)
	{
		if (ULevelUpWidget* LUW = PlayerHUD->LevelUpWidget)
		{
			LUW->RefreshAllStats();
			LUW->RebindDelegates(PCS);

		}
		if(UMainPlayerWidget* MPI = PlayerHUD->GetMainUIWidget())
		{
			MPI->RebindDelegates();
			MPI->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
			if (!Save->LearnedAbilities.IsEmpty())
			{
				PCS->LearnedAbilitiesSaveData = Save->LearnedAbilities;
				PCS->DeserializeAbilities();
			}

		}
		if (UTalentTreeWidget* TalentTreeWidget = PlayerHUD->TalentTreeWidget)
		{
			TalentTreeWidget->CheckAllRows();
			TalentTreeWidget->CheckLearnedAbilities();
		}
	}
	//update UI.
	if (!Save->SavedWeapon.WeaponID.IsEmpty())
	{
		if (UDataAsset** SavedWeaponDA = GI->AllItems.Find(Save->SavedWeapon.WeaponID))
		{
			if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(SavedWeaponDA[0]))
			{
				FActorSpawnParameters params;
				params.Owner = PCS;

				if (AWeapon* SavedWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponItemData->ItemToSpawn, FVector::ZeroVector, FRotator::ZeroRotator,params))
				{
					SavedWeapon->InitWeapon();
					//SavedWeapon->WeaponInstance->SetQuantity(WeaponItemData->Quantity);
					Save->SavedWeapon.InitWeapon(SavedWeapon->WeaponInstance);
					tryEquipGear(SavedWeapon, WeaponItemData->GearType, SavedWeapon->WeaponInstance);
					UE_LOG(LogTemp, Warning, TEXT("Loaded from Saved Weapon"));
				}
			}
		}
	}

	for (FGearItemInstanceSaveData& Data : Save->SavedGear)
	{
		if (UDataAsset** GearDA = GI->AllItems.Find(Data.GearID))
		{
			if (UGearItemData* GearItemData = Cast<UGearItemData>(GearDA[0]))
			{
				if (GearItemData->ItemToSpawn) {

					if (AItem* GearToEquip = GearItemData->ItemToSpawn->GetDefaultObject<AItem>()) {
						if (UGearItemInstance* GearItemInstance = NewObject<UGearItemInstance>(PCS))
						{
							GearItemInstance->ItemData = GearItemData;
							Data.InitGear(GearItemInstance);
							tryEquipGear(GearToEquip, GearItemData->GearType, GearItemInstance);

						}
					}
				}
			}

		}
	}

	//reload inventory
	//check for weapons
	if (!Save->SavedInventoryWeapons.IsEmpty())
	{
		for (FWeaponInstanceSaveData& Data : Save->SavedInventoryWeapons)
		{
			if (UDataAsset** WeaponDA = GI->AllItems.Find(Data.WeaponID))
			{
				if (UWeaponItemData* WeaponItemData = Cast<UWeaponItemData>(WeaponDA[0]))
				{
					if (UWeaponInstance* WeaponInstance = NewObject<UWeaponInstance>(PCS))
					{
						WeaponInstance->ItemData = WeaponItemData;
						Data.InitWeapon(WeaponInstance);
						PCS->AddItemToInventory(WeaponInstance);
					}

				}

			}
		}
	}
	//check for gear
	if (!Save->SavedInventoryGear.IsEmpty())
	{
		for (FGearItemInstanceSaveData& Data : Save->SavedInventoryGear)
		{
			if (UDataAsset** GearDA = GI->AllItems.Find(Data.GearID))
			{
				if (UGearItemData* GearItemData = Cast<UGearItemData>(GearDA[0]))
				{
					if (UGearItemInstance* GearItemInstance = NewObject<UGearItemInstance>(PCS))
					{
						GearItemInstance->ItemData = GearItemData;
						Data.InitGear(GearItemInstance);
						PCS->AddItemToInventory(GearItemInstance);

					}

				}

			}
		}

	}
	//check for regular items
	if (!Save->SavedInventory.IsEmpty())
	{
		for (FItemInstance& Data : Save->SavedInventory)
		{
			if (UDataAsset** ItemDA = GI->AllItems.Find(Data.ItemDataID))
			{
				if (UItemDataAsset* ItemData = Cast<UItemDataAsset>(ItemDA[0]))
				{
					if (UItemDataObject* ItemDataObject = NewObject<UItemDataObject>(PCS))
					{
						ItemDataObject->ItemData = ItemData;
						PCS->AddItemToInventory(ItemDataObject);

					}
				}
			}
		}
	}


}

void APlayerCharacter::ResetState()
{
	ps = PlayerStates::NONE;
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

		// drive desired offset opposite to input:
		float DesiredY = 100.f + (-InputDirection.X * MaxSideOffset);
		DesiredY = FMath::Min(DesiredY, 100.f);

		// smoothly ease current socket offset toward DesiredY
		FVector Off = SpringArm->SocketOffset;
		Off.Y = FMath::FInterpTo(Off.Y, DesiredY, DeltaTime, LateralRecenterSpeed);
		SpringArm->SocketOffset = Off;
	

	if (TargetedEnemy)
	{
		// From player to targeted enemy location rotation
		FRotator TargetRotation =
			(TargetedEnemy->GetActorLocation() - GetActorLocation()).Rotation();

		// Figure out the rotation relative to the players current rotation
		FRotator ActorRot = GetActorRotation();
		FRotator DesiredRel = (TargetRotation - ActorRot).GetNormalized();

		// interp the spring arms current relative rotation to the new desired rotation
		FRotator CurrRel = SpringArm->GetRelativeRotation();
		FRotator NewRel = FMath::RInterpTo(CurrRel, DesiredRel, DeltaTime, 12.f);
		SpringArm->SetRelativeRotation(NewRel);

		// rotate the player smoothly around the target.
		FRotator NewActor = FMath::RInterpTo(ActorRot, TargetRotation, DeltaTime, 12.f);
		NewActor.Pitch = 0.f;
		NewActor.Roll = 0.f;
		SetActorRotation(NewActor);

		// Interp the camera to follow the location of the spring arm.
		FVector NewCamLoc = FMath::VInterpTo(
			SpringArm->GetRelativeLocation(),
			TargetCamLocation,
			DeltaTime,
			10.f);
		SpringArm->SetRelativeLocation(NewCamLoc);
	}
	else if (!TargetedEnemy && CamTargeted)
	{
		ResetCameraPosition(DeltaTime);
	}
    if(bIsHolding)
	{
		BowPower += 0.08;
		BowPower = FMath::Clamp(BowPower, 0.0f, 1.0f);
	}
	if (GetCharacterMovement()) {
		UPrimitiveComponent* Base = GetCharacterMovement()->CurrentFloor.HitResult.Component.Get();
		if (Base && Base->GetBodyInstance())
		{
			UPhysicalMaterial* PhysMat = Base->GetBodyInstance()->GetSimplePhysicalMaterial();
			if (PhysMat)
			{
				EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(PhysMat);
				MostRecentSurfaceType = SurfaceType;
				if (SurfaceType == SURFACE_Water || SurfaceType == SURFACE_Lava || SurfaceType == SURFACE_Poison || SurfaceType == SURFACE_Blood)
				{
					if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
						ULiquidDataAsset* FinalResult = nullptr;
						switch (SurfaceType)
						{
						case SurfaceType_Default:
							break;
						case SurfaceType1:
							if(ULiquidDataAsset**Result =  MGI->LiquidDataAssetsMap.Find(EElementTypes::Water))
							{
								FinalResult = (*Result);
							}
							break;
						case SurfaceType2:
							if (ULiquidDataAsset** Result = MGI->LiquidDataAssetsMap.Find(EElementTypes::Fire))
							{
								FinalResult = (*Result);
							}
							break;
						case SurfaceType3:
							if (ULiquidDataAsset** Result = MGI->LiquidDataAssetsMap.Find(EElementTypes::Poison))
							{
								FinalResult = (*Result);
							}
							break;
						case SurfaceType4:
							if (ULiquidDataAsset** Result = MGI->LiquidDataAssetsMap.Find(EElementTypes::Bleed))
							{
								FinalResult = (*Result);
							}
							break;
						default:
							break;
						}
						if (FinalResult) {
							CurrentLiquid = FinalResult;
							LiquidEntered(FinalResult);
						}
					}
				}
				else if((SurfaceType != SURFACE_Water && SurfaceType != SURFACE_Lava && SurfaceType != SURFACE_Poison && SurfaceType != SURFACE_Blood) && CurrentLiquid)
				{
					LiquidExited(CurrentLiquid);
				}
			}
		}
	}
}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);



	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}

	UEnhancedInputComponent* PEI = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	PEI->BindAction(InputConfig->InputMove, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
	PEI->BindAction(InputConfig->InputLook, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
	PEI->BindAction(InputConfig->InputAttack, ETriggerEvent::Started, this, &APlayerCharacter::Attack);
	PEI->BindAction(InputConfig->InputAttack, ETriggerEvent::Completed, this, &APlayerCharacter::Release);
	PEI->BindAction(InputConfig->InputTarget, ETriggerEvent::Triggered, this, &APlayerCharacter::Target);
	PEI->BindAction(InputConfig->InputDodge, ETriggerEvent::Triggered, this, &APlayerCharacter::Dodge);
	PEI->BindAction(InputConfig->InputInteract, ETriggerEvent::Triggered, this, &APlayerCharacter::Interact);
	PEI->BindAction(InputConfig->InputInteract, ETriggerEvent::Completed, this, &APlayerCharacter::FinishedInteract);
	PEI->BindAction(InputConfig->InputAbility1, ETriggerEvent::Completed, this, &APlayerCharacter::UseAbility1);
	PEI->BindAction(InputConfig->InputAbility2, ETriggerEvent::Completed, this, &APlayerCharacter::UseAbility2);
	PEI->BindAction(InputConfig->InputAbility3, ETriggerEvent::Completed, this, &APlayerCharacter::UseAbility3);
	PEI->BindAction(InputConfig->InputAbility4, ETriggerEvent::Completed, this, &APlayerCharacter::UseAbility4);
	PEI->BindAction(InputConfig->InputAbility5, ETriggerEvent::Completed, this, &APlayerCharacter::UseAbility5);
	PEI->BindAction(InputConfig->InputAbility6, ETriggerEvent::Completed, this, &APlayerCharacter::UseAbility6);
	PEI->BindAction(InputConfig->InputBlock, ETriggerEvent::Triggered, this, &APlayerCharacter::Block);
	PEI->BindAction(InputConfig->InputHandSwap,ETriggerEvent::Triggered,this,&APlayerCharacter::ToggleWeaponType);
	PEI->BindAction(InputConfig->InputAbilityHeal, ETriggerEvent::Triggered, this, &APlayerCharacter::AbilityHeal);
	PEI->BindAction(InputConfig->InputInteractBag, ETriggerEvent::Triggered, this, &APlayerCharacter::InteractBag);
	PEI->BindAction(InputConfig->InputInteractEquipment, ETriggerEvent::Triggered, this, &APlayerCharacter::InteractEquipment);
	PEI->BindAction(InputConfig->InputInteractStats, ETriggerEvent::Triggered, this, &APlayerCharacter::InteractStats);
	PEI->BindAction(InputConfig->InputInteractTalents, ETriggerEvent::Triggered, this, &APlayerCharacter::InteractTalent);
	PEI->BindAction(InputConfig->InputPauseMenu, ETriggerEvent::Triggered, this, &APlayerCharacter::PauseGame);



}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{

	OnPlayerDamaged.Broadcast(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	if(bParrySuccess)
	{
		bParrySuccess = false;
		return 0;
	}

	APlayerCharacterState* PCS = SoftPCS.Get();

	if (PCS)
	{
		if (AEnemy* Enemy = Cast<AEnemy>(DamageCauser))
		{
			HandleEnemyAttack(Enemy, DamageAmount);
		}
		else if (ADebuffActor*DebuffActor = Cast<ADebuffActor>(DamageCauser))
		{
			HandleDebuffAttack(DebuffActor, DamageAmount);
		}
		else
		{

			PCS->playerStats.currentHealth -= DamageAmount;
			PCS->playerStats.currentHealth = FMath::Clamp(PCS->playerStats.currentHealth, 0.f, PCS->GetTotalHealth());


			if (PlayerHUD)
			{
				if (PlayerHUD->GetMainUIWidget())
				{
					PlayerHUD->GetMainUIWidget()->UpdateProgressBar("Health", PCS->playerStats.currentHealth / PCS->GetTotalHealth());
				}
			}
		}
		if (UMainGameInstance* MGI = Cast<UMainGameInstance>(GetWorld()->GetGameInstance())) {
			USoundBase* Sound = MGI->Hit[FMath::RandRange(0, MGI->Hit.Num() - 1)];
			if (Sound) {
				UGameplayStatics::PlaySound2D(GetWorld(), Sound);
			}
		}
		if(PCS->playerStats.currentHealth <= 0)
		{
			if (UAbility** FoundAbility = PCS->LearnedAbilities.FindByPredicate([](UAbility* Ability) {return Ability && Ability->IsA(UAbility_Second_Wind::StaticClass()); }))
			{
				if (UAbility_Second_Wind* SecondWind = Cast<UAbility_Second_Wind>(*FoundAbility))
				{

					if (SecondWind->bShouldExecute_Implementation())
					{
						SecondWind->execute_Implementation();
						return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
					}
				}
			}
			if (!bDead) {
				if (GetMesh() && GetMesh()->GetAnimInstance()) {
					GetMesh()->GetAnimInstance()->Montage_Stop(0.f);
				}
				SetActorEnableCollision(false);
				ps = PlayerStates::DEAD;
				PCS->UpdateLives(-1);
				ShowLivesWidget();
				if (!CurrentDebuffs.IsEmpty()) {
					for (const auto& pair : CurrentDebuffs)
					{
						if (UElementDebuff* Debuff = pair.Value; IsValid(Debuff))
						{
							Debuff->EndBuff();
						}
					}
				}
				OnPlayerDeath.Broadcast();
				FTimerHandle FDeathTimer;
				TWeakObjectPtr<APlayerCharacterState> WeakPCS = PCS;
				TWeakObjectPtr<APlayerCharacter> safeThis = this;
				GetWorldTimerManager().SetTimer(FDeathTimer, [safeThis, WeakPCS]
					{
						if (!safeThis.IsValid() || !WeakPCS.IsValid()) return;
						if (APlayerCharacter* local = safeThis.Get()) {
							if (APlayerCharacterState* localPCS = WeakPCS.Get()) {
								local->ResetPlayer(localPCS);
							}
						}

					}, 2.5f, false);

		

				if (PCS->GetCurrentLives() <= 0)
				{
					UE_LOG(LogTemp, Warning, TEXT("PLAYER FAILED TO EXFIL, OUT OF LIVES GAME OVER."));
					GetWorldTimerManager().ClearAllTimersForObject(this);
					UGameplayStatics::OpenLevel(this, "MainMenu");
				}
				bDead = true;
			}
		}
	}
	

	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

