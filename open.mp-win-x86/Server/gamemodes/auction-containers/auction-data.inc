new auctionTimerFunction;
new auctionStartDelayTimerFunction;
//new bool:timerStarted = false;

forward StartAuction();
public StartAuction()
{
    //if (!timerStarted)
    //{
    //    timerStarted = true;
    //    auctionTimerFunction = SetTimer("AuctionTimerTick", 1000, true);
    //}
    printf("Starting auction");
    KillTimer(auctionStartDelayTimerFunction);
    SpawnAllContainers();
    auctionTimerFunction = SetTimer("AuctionTimerTick", 1000, true);
    gAuctionTimeLeft = AUCTION_COOLDOWN_SECONDS;
    for(new i = 0; i < MAX_CONTAINERS; i++)
    {
        gTotalBids[i] = 0;
        for(new j = 0; j < MAX_PLAYERS; j++)
        {
            gBids[i][j] = 0;
        }
        OnBidUpdated(i);
    }
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
        for(new i = 0; i < MAX_CONTAINERS; i++)
        {
            ProduceAuctionResult(i);
        }
        KillTimer(auctionTimerFunction);
        auctionStartDelayTimerFunction = SetTimer("StartAuction", 1000 * AUCTION_START_DELAY_SECONDS, false);
    }
    return 1;
}

forward ProduceAuctionResult(containerId);
ProduceAuctionResult(containerId)
{
    new winnerId = GetWinner(containerId);
    new lootId = -1;
    if (winnerId != -1)
    {
        lootId = GetRandomLootIdForContainerType(containers[containerId][type]);
    }
    OnPlayerWon(containerId,winnerId, lootId);
    return 1;
}

CanMakeBid(playerid, containerid, amount)
{
    if (containerid < 0 || containerid >= MAX_CONTAINERS)
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
    return 1;
}

MakeBid(playerId, containerId, amount)
{
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
    return 1;
}


GetWinner(containerId)
{
    if (gTotalBids[containerId] == 0) return -1;

    new randomBid = random(gTotalBids[containerId]);
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        randomBid -= gBids[containerId][i];
        if (randomBid <= 0)
        {
            return i;
        }
    }
    return -1;
}

forward AuctionStartDelayTimerTick();
AuctionStartDelayTimerTick()
{
    gAuctionStartDelayTimeLeft--;
    if (gAuctionStartDelayTimeLeft <= 0)
    {
        StartAuction();
    }
    return 1;
}