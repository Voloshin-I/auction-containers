// Global cross-module methods (callbacks)
forward OnAuctionTimerTick();
forward OnBidUpdated(containerId);
forward MakeBid(playerId, containerId, amount);
forward OnPlayerBidSuccess(playerid);
forward OnPlayerBidFail(playerid);
forward CreateContainerObject(containerId);
forward DestroyContainerObject(containerId);
forward GetRandomLootIdForContainerType(containerType);
forward OnPlayerWon(containerId, winnerParticipantId, lootId);
forward ResolveOrCreateParticipant(const nick[]);
forward DeliverAuctionRewardToPlayer(playerid, bool:rewardIsModel, rewardValue);
forward EnqueuePendingReward(const nick[], bool:rewardIsModel, rewardValue);
forward LoadAndApplyPendingRewardsForPlayer(playerid);

// Global data
new gAuctionTimeLeft = AUCTION_COOLDOWN_SECONDS;
new gBids[MAX_CONTAINERS][MAX_PLAYERS];
new gPlayerNames[MAX_PLAYERS][MAX_PLAYER_NAME];
new gTotalBids[MAX_CONTAINERS];
new gDrawableStr[BID_DRAWABLE_STRING_LENGTH];

// Round participants: stable index for bids across reconnects (same nick -> same slot until StartAuction)
new participantNick[MAX_PLAYERS][MAX_PLAYER_NAME];
new bool:participantActive[MAX_PLAYERS];
new playerParticipant[MAX_PLAYERS];