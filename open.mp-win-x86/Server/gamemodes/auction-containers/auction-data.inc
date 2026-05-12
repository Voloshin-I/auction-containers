new auctionTimerFunction;
new bool:timerStarted = false;

forward StartAuction();
StartAuction()
{
    if (!timerStarted)
    {
        timerStarted = true;
        auctionTimerFunction = SetTimer("AuctionTimerTick", 1000, true);
    }
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
}

forward AuctionTimerTick();
public AuctionTimerTick()
{
    gAuctionTimeLeft--;
    OnAuctionTimerTick();
    if (gAuctionTimeLeft <= 0)
    {
        StartAuction()
    }
}

MakeBid(playerid, containerid, amount)
{
    gBids[containerid][playerid] += amount;
    gTotalBids[containerid] += amount;
    OnBidUpdated(containerid);
    return 1;
}
