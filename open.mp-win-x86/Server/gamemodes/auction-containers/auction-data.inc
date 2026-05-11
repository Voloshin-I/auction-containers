new auctionTimerFunction;
new auctionTimerTime = AUCTION_COOLDOWN_SECONDS;
new bool:timerStarted = false;

new stakes[MAX_CONTAINERS][E_STAKE];

forward StartAuction();
StartAuction()
{
    if (!timerStarted)
    {
        timerStarted = true;
        auctionTimerFunction = SetTimer("AuctionTimerTick", 1000, true);
    }
    auctionTimerTime = AUCTION_COOLDOWN_SECONDS;
    for(new i = 0; i < MAX_CONTAINERS; i++)
    {
        stakes[i][playerId] = -1;
        stakes[i][value] = 0;
        OnStakeUpdated(i, stakes[i]);
    }
}

forward AuctionTimerTick();
public AuctionTimerTick()
{
    auctionTimerTime--;
    OnAuctionTimerTick(auctionTimerTime);
    if (auctionTimerTime <= 0)
    {
        StartAuction()
    }
}
