#pragma once

struct FNetworkHelper
{
	static bool HasAuthority(const AActor* Actor);
	static bool HasAuthority(const UActorComponent* ActorComponent);

	static bool HasCosmetics(const AActor* Actor);
	static bool HasCosmetics(const UActorComponent* ActorComponent);	
};
