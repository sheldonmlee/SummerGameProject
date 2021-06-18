// Fill out your copyright notice in the Description page of Project Settings.


#include "BPFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

float radToDeg(float rad);
void swap(float& a, float& b);

TArray<AActor*> UBPFunctionLibrary::tracking_targets = TArray<AActor*>();

void UBPFunctionLibrary::SetTrackingTargets(TArray<AActor*> actors)
{
	tracking_targets.Empty();
	for (int i = 0; i < actors.Max(); i++) {
		tracking_targets.Add(actors[i]);
	}
	return;
}

void UBPFunctionLibrary::TrackTargets(
	UCameraComponent* camera,
	float padding,
	float min_fov,
	float max_fov
)
{
	if (!camera) return;
	if (tracking_targets.Max() == 0) return;

	FMinimalViewInfo view_info;
	FVector center_point;
		
	// Set camera view_info.
	camera->GetCameraView(1.f, view_info);
	// Get center point of actors that are being tracked.
	center_point = GetTargetCenterLocation();

	// Rotate camera.
	camera->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(view_info.Location, center_point));
	// Set zoom / Field Of View of camera.
	camera->SetFieldOfView(GetCameraFOV(view_info, center_point, padding, min_fov, max_fov));
	return;
}

void UBPFunctionLibrary::GetActorBounds(FVector& min, FVector& max)
{
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

FVector UBPFunctionLibrary::GetTargetCenterLocation()
{
	FVector min, max;
	GetActorBounds(min, max);
	return (min + max) / 2;
}

float UBPFunctionLibrary::GetCameraFOV(
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
