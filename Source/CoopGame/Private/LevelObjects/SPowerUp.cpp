#include "LevelObjects/SPowerUp.h"

#include "Helpers/NetworkHelper.h"

ASPowerUp::ASPowerUp()
{
	SetReplicates(true);
}

void ASPowerUp::ActivatePowerUp(APawn* InInteractionInstigator)
{
	check(FNetworkHelper::HasAuthority(this));
	
	NetMulticastPowerUpActivated(InInteractionInstigator);

	if (TickInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(Tick_TimerHandle, this, &ASPowerUp::NetMulticastPowerUpTick,
            TickInterval, true);
	}
	else
	{
		NetMulticastPowerUpTick();
	}
}

void ASPowerUp::NetMulticastPowerUpExpired_Implementation()
{
	OnPowerUpExpired(InteractionInstigator.Get());

	if (FNetworkHelper::HasAuthority(this))
	{
		GetWorldTimerManager().ClearTimer(Tick_TimerHandle);
		SetLifeSpan(1.0f);
	}
}

void ASPowerUp::NetMulticastPowerUpTick_Implementation()
{
	OnPowerUpTicked(InteractionInstigator.Get());
	
	if (FNetworkHelper::HasAuthority(this))
	{
		NumberOfTicksProcessed++;
		if (NumberOfTicksProcessed >= TotalNumberOfTicks)
		{
			NetMulticastPowerUpExpired();
		}
	}
}

void ASPowerUp::NetMulticastPowerUpActivated_Implementation(APawn* InInteractionInstigator)
{
	InteractionInstigator = InInteractionInstigator;
	
	OnPowerUpActivated(InteractionInstigator.Get());
}
