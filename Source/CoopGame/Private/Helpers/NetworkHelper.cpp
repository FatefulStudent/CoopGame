#include "Helpers/NetworkHelper.h"

bool FNetworkHelper::HasAuthority(const AActor* Actor)
{
	if (Actor)
		return Actor->HasAuthority();

	return false;
}

bool FNetworkHelper::HasAuthority(const UActorComponent* ActorComponent)
{
	if (ActorComponent)
		return HasAuthority(ActorComponent->GetOwner());

	return false;
}

bool FNetworkHelper::HasCosmetics(const AActor* Actor)
{
	if (Actor)
		return Actor->GetNetMode() != NM_DedicatedServer;

	return false;
}

bool FNetworkHelper::HasCosmetics(const UActorComponent* ActorComponent)
{
	if (ActorComponent)
		return HasCosmetics(ActorComponent->GetOwner());

	return false;
}
