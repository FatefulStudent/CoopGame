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

bool FNetworkHelper::IsLocallyControlled(const APawn* Pawn)
{
	if (Pawn)
		return Pawn->IsLocallyControlled();

	return false;
}

bool FNetworkHelper::IsLocallyControlled(const AActor* Actor)
{
	if (Actor)
	{
		if (const APawn* Pawn = Cast<APawn>(Actor))
			return IsLocallyControlled(Pawn);
		else if (const APawn* PawnOwner = Cast<APawn>(Actor->GetOwner()))
			return IsLocallyControlled(PawnOwner);
	}

	return false;
}

bool FNetworkHelper::IsLocallyControlled(const UActorComponent* ActorComponent)
{
	if (ActorComponent)
		return IsLocallyControlled(ActorComponent->GetOwner());

	return false;
}

bool FNetworkHelper::IsAutonomousProxy(const APawn* Pawn)
{
	if (Pawn)
		return Pawn->GetLocalRole() == ROLE_AutonomousProxy;

	return false;
}

bool FNetworkHelper::IsAutonomousProxy(const AActor* Actor)
{
	if (Actor)
	{
		if (const APawn* Pawn = Cast<APawn>(Actor))
			return IsAutonomousProxy(Pawn);
		else if (const APawn* PawnOwner = Cast<APawn>(Actor->GetOwner()))
			return IsAutonomousProxy(PawnOwner);
	}
	
	return false;
}

bool FNetworkHelper::IsAutonomousProxy(const UActorComponent* ActorComponent)
{
	if (ActorComponent)
		return IsAutonomousProxy(ActorComponent->GetOwner());

	return false;
}
