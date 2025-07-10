// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelMusicActor.h"
#include <Kismet/GameplayStatics.h>

#include "Components/AudioComponent.h"

// Sets default values
ALevelMusicActor::ALevelMusicActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.

	MusicManagerComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Music Manager Comp"));
	MusicManagerComponent->SetAutoActivate(true);
	MusicManagerComponent->SetUISound(true);
	MusicManagerComponent->SetupAttachment(RootComponent);


}

void ALevelMusicActor::SetAndPlayMusic()
{
	if (LevelSong) {
		MusicManagerComponent->SetSound(LevelSong);
		MusicManagerComponent->Play();
	}
}

// Called when the game starts or when spawned
void ALevelMusicActor::BeginPlay()
{
	Super::BeginPlay();

	if (LevelSong && bShouldPlay) {

		MusicManagerComponent->SetSound(LevelSong);
		MusicManagerComponent->Play();
	}
}

// Called every frame
void ALevelMusicActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

