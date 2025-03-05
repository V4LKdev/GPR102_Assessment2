// // CG Spectrum, Nic 2025


#include "TurretMasterGameplayTags.h"


namespace TMGameplayTags
{
	UE_DEFINE_GAMEPLAY_TAG(TurretState_Disabled,					"TurretState.Disabled");
	UE_DEFINE_GAMEPLAY_TAG(TurretState_Idle,						"TurretState.Idle");
	UE_DEFINE_GAMEPLAY_TAG(TurretState_Firing,						"TurretState.Firing");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TargetType_Projectile_Gravity,	"TargetType.Projectile.Gravity",			"Used for targetable projectiles that have gravity enabled");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TargetType_Projectile_NoGravity,	"TargetType.Projectile.NoGravity",			"Used for targetable projectiles that DON'T have gravity enabled");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(TargetType_Stationary,			"TargetType.Stationary",					"Used for targetable Actors that don't move (e.g. other turrets)");

	UE_DEFINE_GAMEPLAY_TAG(Target_Enemy,							"Target.Enemy");
	UE_DEFINE_GAMEPLAY_TAG(Target_Neutral,							"Target.Neutral");
	UE_DEFINE_GAMEPLAY_TAG(Target_Friendly,							"Target.Friendly");
}