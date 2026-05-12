// Global cross-module methods (callbacks)
forward OnAuctionTimerTick();
forward OnBidUpdated(containerId);
forward MakeBid(playerid, containerid, amount);
//forward GetStake(containerId, stake[E_STAKE]);

// Global data
new gAuctionTimeLeft = AUCTION_COOLDOWN_SECONDS;
new gBids[MAX_CONTAINERS][MAX_PLAYERS];
new gPlayerNames[MAX_PLAYERS][MAX_PLAYER_NAME];
new gTotalBids[MAX_CONTAINERS];
new gDrawableStr[BID_DRAWABLE_STRING_LENGTH];