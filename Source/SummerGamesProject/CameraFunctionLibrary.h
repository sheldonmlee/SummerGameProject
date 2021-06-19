#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CameraFunctionLibrary.generated.h"

/**
 * The Camera function library.
 */
UCLASS()
class SUMMERGAMESPROJECT_API UCameraFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*
	 * Set actors for camera to fit into view.
	 */
	UFUNCTION(BlueprintCallable)
	static void SetTrackingTargets(TArray<AActor*> actors);

	/*
	 * Point and adjust camera FOV to (hopefully) fit targets within view.
	 */
	UFUNCTION(BlueprintCallable)
	static void TrackTargets(
		UCameraComponent* camera,
		float padding = 100,
		float min_fov = 30, 
		float max_fov = 170
	);

private:
	static TArray<AActor*> tracking_targets;

	/*
	 * Gets bounds of current targets (sets min, max).
	 */
	static void GetActorBounds(FVector& min, FVector& max);

	/*
	 * Get center of bounds for camera to face.
	 */
	static FVector GetTargetCenterLocation();
	
	/*
	 * Get camera Field Of View.
	 */
	static float GetCameraFOV(
		FMinimalViewInfo view_info,
		FVector center_location,
		float padding,
		float min_fov, 
		float max_fov
	); 

};
