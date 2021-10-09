// Fill out your copyright notice in the Description page of Project Settings.

#include "CameraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

float radToDeg(float rad);
void swap(float& a, float& b);

// Init private class variables;
TArray<AActor*> UCameraFunctionLibrary::tracking_targets = TArray<AActor*>();
TSubclassOf<AActor> UCameraFunctionLibrary::tracking_class = ACharacter::StaticClass();

float UCameraFunctionLibrary::t_fov = 0;
FRotator UCameraFunctionLibrary::t_rotation(0, 0, 0);

// Begin Public functions
void UCameraFunctionLibrary::SetTrackingTargets(TSubclassOf<AActor> subclass)
{
	tracking_class = subclass;
	return;
}

void UCameraFunctionLibrary::TrackTargets(
	const UObject* WorldContextObject,
	UCameraComponent* camera,
	float delta_seconds,
	float padding,
	float min_fov,
	float max_fov
)
{
	if (!camera) return;
	static float t_from_update = -1;

	// Set camera view_info.
	FMinimalViewInfo view_info;
	camera->GetCameraView(1.f, view_info);

	if (t_from_update < 0 || t_from_update > 1.f/10.f) { 
		UpdateCameraTargetRotationFOV(WorldContextObject, view_info, padding, min_fov, max_fov);
		t_from_update = 0;
	}
	// Calculate rotation_step
	FRotator rotation_step(0,0,0);
	GetRotationStep(view_info, rotation_step, delta_seconds);
	
	// Calculate fov step
	float fov_step = 0;
	GetFOVStep(view_info, fov_step, delta_seconds);

	camera->SetWorldRotation(view_info.Rotation + rotation_step);
	camera->SetFieldOfView(view_info.FOV + fov_step);
	
	t_from_update += delta_seconds;
	return;
}
// End public

// begin Private
void UCameraFunctionLibrary::GetActorBounds(FVector& min, FVector& max)
{
	if (tracking_targets.Max() == 0) return;
	min = tracking_targets[0]->GetActorLocation();
	max = tracking_targets[0]->GetActorLocation();
	for (int i = 1; i < tracking_targets.Num(); i++)
	{
		FVector pos;
		pos = tracking_targets[i]->GetActorLocation();

		if (pos.X < min.X) min.X = pos.X;
		if (pos.X > max.X) max.X = pos.X;

		if (pos.Y < min.Y) min.Y = pos.Y;
		if (pos.Y > max.Y) max.Y = pos.Y;

		if (pos.Z < min.Z) min.Z = pos.Z;
		if (pos.Z > max.Z) max.Z = pos.Z;
	}
}

FVector UCameraFunctionLibrary::GetTargetCenterLocation()
{
	FVector min, max;
	GetActorBounds(min, max);
	return (min + max) / 2;
}

float UCameraFunctionLibrary::GetCameraFOV(
	FMinimalViewInfo view_info,
	FVector center_location,
	float padding,
	float min_fov,
	float max_fov
)
{
	// Get bounds for actors.
	FVector min, max;
	GetActorBounds(min, max);

	// Plane represending camera view.
	FVector point, normal;
	point = view_info.Location;
	normal = view_info.Rotation.Vector();

	// Project bounds to camera plane.
	min = FVector::PointPlaneProject(min, point, normal);
	max = FVector::PointPlaneProject(max, point, normal);

	// Translate rotate and bounds.
	FRotator camera_rotation = view_info.Rotation;

	min -= point;
	max -= point;

	min = camera_rotation.UnrotateVector(min);
	max = camera_rotation.UnrotateVector(max);

	// Maintain min/max after transformation.
	if (min.X > max.X) swap(min.X, max.X);
	if (min.Y > max.Y) swap(min.Y, max.Y);
	if (min.Z > max.Z) swap(min.Z, max.Z);

	// Get FOV.
	// dx is actually vertical and dy is horizontal relative to the camera
	// view plane. This is more in line with the coordinate system in Unreal.
	// For some reason, after the trasnformation if min and max, their Y and Z
	// values correspond to vertical and horizontal changes respectively.
	float fov, dx, dy, distance, magnitude;
	dx = max.Y - min.Y;
	dy = max.Z - min.Z;
	// This helps fits the viewport better.
	dx /= view_info.AspectRatio;
	dy *= view_info.AspectRatio;
	magnitude = sqrt(dy*dy + dx*dx);
	distance = FVector::Distance(point, center_location);
	// The main FOV calculation.
	fov = 2 * radToDeg(atan((magnitude / 2 + padding) / distance));
	if (view_info.AspectRatio >= 1) fov *= view_info.AspectRatio;

	// Clamp fov.
	if (fov < min_fov) fov = min_fov;
	if (fov > max_fov) fov = max_fov;

	return fov;
}

void UCameraFunctionLibrary::UpdateCameraTargetRotationFOV(
		const UObject* WorldContextObject,
		FMinimalViewInfo view_info,
		float padding,
		float min_fov, 
		float max_fov
)
{
	UpdateActors(WorldContextObject);
	FVector center_point;
		
	// Get center point of actors that are being tracked.
	center_point = GetTargetCenterLocation();

	// Rotate camera.
	t_rotation = UKismetMathLibrary::FindLookAtRotation(view_info.Location, center_point);
	// Set zoom / Field Of View of camera.
	t_fov = GetCameraFOV(view_info, center_point, padding, min_fov, max_fov);
}

void UCameraFunctionLibrary::GetRotationStep(FMinimalViewInfo view_info, FRotator& rotation_step, float delta_seconds)
{
	float dp = t_rotation.Pitch - view_info.Rotation.Pitch;
	float dy = t_rotation.Yaw - view_info.Rotation.Yaw;
	
	float theta = UKismetMathLibrary::DegAsin(dp/dy);
	
	float mag = (dp*dp + dy*dy)*delta_seconds;
	if (mag > 20) mag = 20;

	rotation_step.Pitch = abs(mag*UKismetMathLibrary::DegSin(theta));
	rotation_step.Yaw = abs(mag*UKismetMathLibrary::DegCos(theta));
	
	if (dp < 0) rotation_step.Pitch = -rotation_step.Pitch;
	if (dy < 0) rotation_step.Yaw = -rotation_step.Yaw;
}

void UCameraFunctionLibrary::GetFOVStep(FMinimalViewInfo view_info, float& fov_step, float delta_seconds)
{
	float dfov = t_fov - view_info.FOV;
	
	fov_step = dfov*delta_seconds;
}

void UCameraFunctionLibrary::UpdateActors(const UObject* WorldContextObject)
{
	if (!WorldContextObject) return;

	tracking_targets.Empty();

	UWorld* _world = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameplayStatics::GetAllActorsOfClass(_world, tracking_class, tracking_targets);
	int length = tracking_targets.Num();
}

float radToDeg(float rad)
{
	return rad * 180/PI;
}

void swap(float& a, float& b)
{
	float temp = a;
	a = b;
	b = temp;
}
