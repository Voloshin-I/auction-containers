enum E_STAKE
{
    playerId,
    value
}


forward OnAuctionTimerTick(secondsLeft);
forward OnStakeUpdated(containerId, stake[E_STAKE]);
//forward GetStake(containerId, stake[E_STAKE]);