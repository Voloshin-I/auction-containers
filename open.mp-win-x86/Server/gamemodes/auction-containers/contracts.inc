// Global cross-module methods (callbacks)
forward OnAuctionTimerTick();
forward OnStakeUpdated(containerId);
//forward GetStake(containerId, stake[E_STAKE]);

// Global data
new gAuctionTimeLeft = AUCTION_COOLDOWN_SECONDS;
new gStakes[MAX_CONTAINERS][MAX_PLAYERS];