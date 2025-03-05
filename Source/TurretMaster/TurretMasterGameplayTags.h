// // CG Spectrum, Nic 2025

#pragma once

#include "NativeGameplayTags.h"

namespace TMGameplayTags
{
	/*
	 *	Turret States
	 */
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TurretState_Disabled)
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TurretState_Idle)
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TurretState_Firing)

	/**
	 *	Target Tags
	 */
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TargetType)
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TargetType_Projectile_Gravity)
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TargetType_Projectile_NoGravity)
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(TargetType_Stationary)

	// TODO: Potentially replace with Teams for multiplayer or versus
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Target_Enemy)
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Target_Neutral)
	TURRETMASTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Target_Friendly)

	// TODO: add projectile states, like being targeted or being free, and targeting priority levels, but that might be an int
}