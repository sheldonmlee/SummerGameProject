// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"


void UBPFunctionLibrary::GetCharacters(const UObject* WorldContextObject, TArray<AActor*>& actors)
{
	if (!WorldContextObject) return;
	UWorld* _world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	TSubclassOf<ACharacter> character_class = ACharacter::StaticClass();
	UGameplayStatics::GetAllActorsOfClass(_world, character_class, actors);
	return;
}

FVector UBPFunctionLibrary::GetPlayerAverageLocation(const UObject* WorldContextObject)
{
	TArray<AActor*> actors;
	GetCharacters(WorldContextObject, actors);
	return UGameplayStatics::GetActorArrayAverageLocation(actors);
}

float UBPFunctionLibrary::GetCameraFOV(const UObject* WorldContextObject, UCameraComponent* camera)
{
	TArray<AActor*> actors;
	GetCharacters(WorldContextObject, actors);
	FPlane cameraplane
	// get bounds for actors.

	return 90.f;
}
