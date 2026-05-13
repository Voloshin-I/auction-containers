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

MakeBid(playerId, containerId, amount)
{
    gBids[containerId][playerId] += amount;
    gTotalBids[containerId] += amount;
    OnBidUpdated(containerId);
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