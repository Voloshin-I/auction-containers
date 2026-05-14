new auctionTimerFunction;
new auctionStartDelayTimerFunction;

forward StartAuction();
public StartAuction()
{
    printf("[Auction] StartAuction enter");
    printf("Starting auction");
    KillTimer(auctionStartDelayTimerFunction);
    SpawnAllContainers();
    auctionTimerFunction = SetTimer("AuctionTimerTick", 1000, true);
    gAuctionTimeLeft = AUCTION_COOLDOWN_SECONDS;
    for(new i = 0; i < gSpawnContainerCount; i++)
    {
        gTotalBids[i] = 0;
        for(new j = 0; j < MAX_PLAYERS; j++)
        {
            gBids[i][j] = 0;
        }
        OnBidUpdated(i);
    }
    printf("[Auction] StartAuction exit containers=%d timer=%d timeLeft=%d", gSpawnContainerCount, auctionTimerFunction, gAuctionTimeLeft);
    return 1;
}

forward AuctionTimerTick();
public AuctionTimerTick()
{
    gAuctionTimeLeft--;
    OnAuctionTimerTick();
    if (gAuctionTimeLeft <= 0)
    {
        printf("Auction ended");
        for(new i = 0; i < gSpawnContainerCount; i++)
        {
            ProduceAuctionResult(i);
        }
        KillTimer(auctionTimerFunction);
        auctionStartDelayTimerFunction = SetTimer("StartAuction", 1000 * AUCTION_START_DELAY_SECONDS, false);
        printf("[Auction] AuctionTimerTick scheduled StartAuction delayMs=%d", 1000 * AUCTION_START_DELAY_SECONDS);
    }
    return 1;
}

forward ProduceAuctionResult(containerId);
ProduceAuctionResult(containerId)
{
    printf("[ProduceAuctionResult] enter containerId=%d gSpawnContainerCount=%d", containerId, gSpawnContainerCount);
    new winnerId = GetWinner(containerId);
    printf("[ProduceAuctionResult] containerId=%d winnerId=%d", containerId, winnerId);
    if (winnerId == -1)
        return 1;

    new t = containers[containerId][type];
    printf("[ProduceAuctionResult] containerId=%d type=%d", containerId, t);
    new lootId = GetRandomLootIdForContainerType(t);
    printf("[ProduceAuctionResult] containerId=%d lootId=%d gLootRowCount=%d", containerId, lootId, gLootRowCount);
    if (lootId == -1)
    {
        new total = GetLootChanceTotalByContainerType(t);
        printf("[ProduceAuctionResult] Invalid loot generated. containerId=%d, type=%d, gLootRowCount=%d, totalChance=%d",
            containerId, t, gLootRowCount, total);
        return 1;
    }
    new winnerNick[MAX_PLAYER_NAME];
    winnerNick[0] = '\0';
    printf("[ProduceAuctionResult] before strmid winnerId=%d MAX_PLAYER_NAME=%d", winnerId, MAX_PLAYER_NAME);
    strmid(winnerNick, gPlayerNames[winnerId], 0, MAX_PLAYER_NAME, MAX_PLAYER_NAME);
    printf("[ProduceAuctionResult] after strmid winnerNick='%s'", winnerNick);
    OnPlayerWon(containerId, winnerId, lootId, winnerNick);
    printf("[ProduceAuctionResult] after OnPlayerWon containerId=%d", containerId);
    return 1;
}

CanMakeBid(playerid, containerid, amount)
{
    if (containerid < 0 || containerid >= gSpawnContainerCount)
    {
        printf("[CanMakeBid] player %d invalid container slot %d", playerid, containerid);
        SendClientMessage(playerid, 0xFF6666FF, "This container is not available.");
        return 0;
    }
    if (containers[containerid][container_id] == -1)
    {
        printf("[CanMakeBid] player %d container %d not loaded", playerid, containerid);
        SendClientMessage(playerid, 0xFF6666FF, "This container is not available.");
        return 0;
    }
    if (amount <= 0)
    {
        printf("[CanMakeBid] player %d non-positive amount %d", playerid, amount);
        SendClientMessage(playerid, 0xFF6666FF, "Bid amount must be greater than zero.");
        return 0;
    }
    if (GetPlayerMoney(playerid) < amount)
    {
        printf("[CanMakeBid] player %d insufficient money (have %d, need %d)", playerid, GetPlayerMoney(playerid), amount);
        SendClientMessage(playerid, 0xFF6666FF, "You do not have enough money for this bid.");
        return 0;
    }
    new minFirst = containers[containerid][initial_price];
    if (gTotalBids[containerid] == 0 && amount < minFirst)
    {
        new msg[96];
        printf("[CanMakeBid] player %d first bid %d below starting price %d", playerid, amount, minFirst);
        format(msg, sizeof(msg), "The first bid on this container must be at least $%d (starting price).", minFirst);
        SendClientMessage(playerid, 0xFF6666FF, msg);
        return 0;
    }
    printf("[Auction] CanMakeBid ok playerid=%d containerid=%d amount=%d", playerid, containerid, amount);
    return 1;
}

MakeBid(playerId, containerId, amount)
{
    printf("[Auction] MakeBid enter playerId=%d containerId=%d amount=%d", playerId, containerId, amount);
    if (!CanMakeBid(playerId, containerId, amount))
    {
        OnPlayerBidFail(playerId);
        return 0;
    }

    GivePlayerMoney(playerId, -amount);
    gBids[containerId][playerId] += amount;
    gTotalBids[containerId] += amount;
    OnBidUpdated(containerId);
    OnPlayerBidSuccess(playerId);
    printf("[Auction] MakeBid success playerId=%d totalBids[%d]=%d", playerId, containerId, gTotalBids[containerId]);
    return 1;
}


GetWinner(containerId)
{
    printf("[Auction] GetWinner enter containerId=%d totalBids=%d", containerId, gTotalBids[containerId]);
    if (gTotalBids[containerId] == 0) return -1;

    new randomBid = random(gTotalBids[containerId]);
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        randomBid -= gBids[containerId][i];
        if (randomBid <= 0)
        {
            printf("[Auction] GetWinner winnerId=%d", i);
            return i;
        }
    }
    printf("[Auction] GetWinner fallback -1 (unexpected)");
    return -1;
}