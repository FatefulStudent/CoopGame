#include "LevelObjects/SPowerUp.h"

void ASPowerUp::ActivatePowerUp()
{
	OnPowerUpActivated();

	if (TickInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(Tick_TimerHandle, this, &ASPowerUp::OnTicked,
			TickInterval, true);
	}
	else
	{
		OnTicked();
	}
}

void ASPowerUp::OnTicked()
{
	OnPowerUpTicked();
	NumberOfTicksProcessed++;

	if (NumberOfTicksProcessed >= TotalNumberOfTicks)
	{
		OnPowerUpExpired();

		GetWorldTimerManager().ClearTimer(Tick_TimerHandle);
		
	}
}
