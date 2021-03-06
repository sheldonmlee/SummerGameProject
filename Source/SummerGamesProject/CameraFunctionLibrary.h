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
	static void SetTrackingTargets(TSubclassOf<AActor> subclass);

	/*
	 * Point and adjust camera FOV to (hopefully) fit targets within view.
	 */
	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject"))
	static void TrackTargets(
		const UObject* WorldContextObject,
		UCameraComponent* camera,
		float delta_seconds,
		float padding = 100,
		float min_fov = 30, 
		float max_fov = 170
	);

private:
	static TArray<AActor*> tracking_targets;
// To solve the package issue, this line is disabled
//	static TSubclassOf<AActor> tracking_class;
	
	static float t_fov;
	static FRotator t_rotation;

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

	/*
	 * Update the target camera rotation.
	 */
	static void UpdateCameraTargetRotationFOV(
		const UObject* WorldContextObject,
		FMinimalViewInfo view_info,
		float padding,
		float min_fov, 
		float max_fov
	);
	
	static void GetRotationStep(FMinimalViewInfo view_info, FRotator& rotation_step, float delta_seconds);

	static void GetFOVStep(FMinimalViewInfo view_info, float& rotation_step, float delta_seconds);

	/*
	 * Update the list of actors.
	 */
	static void UpdateActors(const UObject* WorldContextObject);
};
