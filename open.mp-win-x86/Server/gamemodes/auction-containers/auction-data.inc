new auctionTimerFunction;
new auctionStartDelayTimerFunction;

forward StartAuction();
public StartAuction()
{
    printf("[Auction] StartAuction enter");
    printf("Starting auction");
    KillTimer(auctionStartDelayTimerFunction);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        participantActive[i] = false;
        participantNick[i][0] = EOS;
        playerParticipant[i] = -1;
    }
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
    new tmpNick[MAX_PLAYER_NAME];
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i))
        {
            GetPlayerName(i, tmpNick, MAX_PLAYER_NAME);
            new partId = ResolveOrCreateParticipant(tmpNick);
            playerParticipant[i] = partId;
            if (partId == -1)
            {
                printf("[Auction] StartAuction WARN playerid=%d could not bind participant (full?)", i);
            }
            else
            {
                printf("[Auction] StartAuction rebind playerid=%d -> participantId=%d nick='%s'", i, partId, tmpNick);
            }
        }
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
    new winnerParticipantId = GetWinner(containerId);
    printf("[ProduceAuctionResult] containerId=%d winnerParticipantId=%d", containerId, winnerParticipantId);
    if (winnerParticipantId == -1)
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
    printf("[ProduceAuctionResult] winnerParticipantId=%d (nick from participantNick[])", winnerParticipantId);
    OnPlayerWon(containerId, winnerParticipantId, lootId);
    printf("[ProduceAuctionResult] after OnPlayerWon containerId=%d", containerId);
    return 1;
}

CanMakeBid(playerid, containerid, amount)
{
    if (playerParticipant[playerid] == -1)
    {
        printf("[CanMakeBid] player %d has no participant slot", playerid);
        SendClientMessage(playerid, 0xFF6666FF, "You are not registered as an auction participant yet. Reconnect if this persists.");
        return 0;
    }
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
    new j = playerParticipant[playerId];
    printf("[Auction] MakeBid enter playerId=%d participantId=%d containerId=%d amount=%d", playerId, j, containerId, amount);
    if (!CanMakeBid(playerId, containerId, amount))
    {
        OnPlayerBidFail(playerId);
        return 0;
    }

    GivePlayerMoney(playerId, -amount);
    gBids[containerId][j] += amount;
    gTotalBids[containerId] += amount;
    OnBidUpdated(containerId);
    OnPlayerBidSuccess(playerId);
    printf("[Auction] MakeBid success playerId=%d participantId=%d totalBids[%d]=%d", playerId, j, containerId, gTotalBids[containerId]);
    return 1;
}


GetWinner(containerId)
{
    printf("[Auction] GetWinner enter containerId=%d totalBids=%d", containerId, gTotalBids[containerId]);
    if (gTotalBids[containerId] == 0) return -1;

    new randomBid = random(gTotalBids[containerId]);
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        randomBid -= gBids[containerId][i];
        if (randomBid <= 0)
        {
            printf("[Auction] GetWinner winnerParticipantId=%d", i);
            return i;
        }
    }
    printf("[Auction] GetWinner fallback -1 (unexpected)");
    return -1;
}

ResolveOrCreateParticipant(const nick[])
{
    if (nick[0] == EOS)
    {
        printf("[Auction] ResolveOrCreateParticipant abort empty nick");
        return -1;
    }
    new i;
    for (i = 0; i < MAX_PLAYERS; i++)
    {
        if (participantActive[i] && strcmp(participantNick[i], nick) == 0)
        {
            printf("[Auction] ResolveOrCreateParticipant existing slot=%d nick='%s'", i, nick);
            return i;
        }
    }
    for (i = 0; i < MAX_PLAYERS; i++)
    {
        if (!participantActive[i])
        {
            format(participantNick[i], MAX_PLAYER_NAME, "%s", nick);
            participantActive[i] = true;
            printf("[Auction] ResolveOrCreateParticipant new slot=%d nick='%s'", i, nick);
            return i;
        }
    }
    printf("[Auction] ResolveOrCreateParticipant FAIL no free slots (max=%d)", MAX_PLAYERS);
    return -1;
}