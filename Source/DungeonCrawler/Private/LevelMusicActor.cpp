// Fill out your copyright notice in the Description page of Project Settings.

#include "LevelMusicActor.h"
#include "Components/AudioComponent.h"

// Sets default values
ALevelMusicActor::ALevelMusicActor()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    MusicManagerComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Music Manager Comp"));
    if (MusicManagerComponent) {
        MusicManagerComponent->SetAutoActivate(true);
        MusicManagerComponent->SetUISound(true);
        MusicManagerComponent->SetupAttachment(RootComponent);
    }
}

void ALevelMusicActor::SetAndPlayMusic()
{
    if (MusicManagerComponent && LevelSong) {
        MusicManagerComponent->SetSound(LevelSong);
        MusicManagerComponent->Play();
    }
}

// Called when the game starts or when spawned
void ALevelMusicActor::BeginPlay()
{
    Super::BeginPlay();
    if (MusicManagerComponent && LevelSong && bShouldPlay) {
        MusicManagerComponent->SetSound(LevelSong);
        MusicManagerComponent->Play();
    }
}

// Called every frame
void ALevelMusicActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

