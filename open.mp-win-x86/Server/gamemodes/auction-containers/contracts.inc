// Global cross-module methods (callbacks)
forward OnAuctionTimerTick();
forward OnBidUpdated(containerId);
forward MakeBid(playerId, containerId, amount);
forward CreateContainerObject(containerId);
forward DestroyContainerObject(containerId);
forward GetRandomLootIdForContainerType(containerType);
forward OnPlayerWon(containerId, playerId, lootId)

// Global data
new gAuctionTimeLeft = AUCTION_COOLDOWN_SECONDS;
new gBids[MAX_CONTAINERS][MAX_PLAYERS];
new gPlayerNames[MAX_PLAYERS][MAX_PLAYER_NAME];
new gTotalBids[MAX_CONTAINERS];
new gDrawableStr[BID_DRAWABLE_STRING_LENGTH];