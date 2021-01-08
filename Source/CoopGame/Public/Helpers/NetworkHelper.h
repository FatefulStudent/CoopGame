#pragma once

struct FNetworkHelper
{
	static bool HasAuthority(const AActor* Actor);
	static bool HasAuthority(const UActorComponent* ActorComponent);

	static bool HasCosmetics(const AActor* Actor);
	static bool HasCosmetics(const UActorComponent* ActorComponent);

	static bool IsLocallyControlled(const APawn* Pawn);
	static bool IsLocallyControlled(const AActor* Actor);
	static bool IsLocallyControlled(const UActorComponent* ActorComponent);

	static bool IsAutonomousProxy(const APawn* Pawn);
	static bool IsAutonomousProxy(const AActor* Actor);
	static bool IsAutonomousProxy(const UActorComponent* ActorComponent);
};
