// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BPFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SUMMERGAMESPROJECT_API UBPFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject"))
	static void GetCharacters(const UObject* WorldContextObject, TArray<AActor*>& actors);

	UFUNCTION(BlueprintPure, meta=(WorldContext = "WorldContextObject"))
	static FVector GetPlayerAverageLocation(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, meta=(WorldContext = "WorldContextObject"))
	static float GetCameraFOV(const UObject* WorldContextObject, UCameraComponent* camera);

};
