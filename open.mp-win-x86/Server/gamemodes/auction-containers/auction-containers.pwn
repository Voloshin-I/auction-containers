#include <open.mp>
#include <a_mysql>
#include <streamer>
//Global primary includes
#include "constants.inc"
#include "contracts.inc"
// Rest includes depending on primary ingludes
#include "auction-database.inc"
#include "auction-data.inc"
#include "auction-player-drawable.inc"


main()
{
    printf("[GM] main()");
    print("My gamemode loaded");
}

public OnGameModeInit()
{
    printf("[GM] OnGameModeInit enter");
    print("Gamemode initialized");
	ConnectToDatabase();
    StartAuction();
    printf("[GM] OnGameModeInit exit (StartAuction scheduled)");
    return 1;
}

public OnPlayerConnect(playerid)
{
    printf("[PendingRewards] OnPlayerConnect start playerid=%d", playerid);
    playerCurrentContainer[playerid] = -1;
    SendClientMessage(playerid, -1, "Hello from PAWN!");
    for(new i = 0; i < gSpawnContainerCount; i++)
    {
        CreateContainerDrawableForPlayer(playerid, i);
    }
    printf("[PendingRewards] OnPlayerConnect drawables created count=%d playerid=%d", gSpawnContainerCount, playerid);

    GetPlayerName(playerid, gPlayerNames[playerid], MAX_PLAYER_NAME);
    printf("[PendingRewards] OnPlayerConnect gPlayerNames cached='%s' playerid=%d (pending rewards load on spawn + short delay)", gPlayerNames[playerid], playerid);

    new participantId = ResolveOrCreateParticipant(gPlayerNames[playerid]);
    playerParticipant[playerid] = participantId;
    if (participantId == -1)
    {
        printf("[Auction] OnPlayerConnect WARN playerid=%d no participant slot (max participants)", playerid);
        SendClientMessage(playerid, 0xFF6666FF, "Auction participant list is full. You cannot bid this round.");
    }
    else
    {
        printf("[Auction] OnPlayerConnect playerid=%d -> participantId=%d nick='%s'", playerid, participantId, gPlayerNames[playerid]);
    }

    GivePlayerMoney(playerid, 9999);
    printf("[PendingRewards] OnPlayerConnect exit playerid=%d", playerid);
    return 1;
}

forward ApplyPendingRewardsTimer(playerid);
public ApplyPendingRewardsTimer(playerid)
{
    printf("[PendingRewards] ApplyPendingRewardsTimer fired playerid=%d connected=%d", playerid, IsPlayerConnected(playerid));
    if (!IsPlayerConnected(playerid))
    {
        return 0;
    }
    LoadAndApplyPendingRewardsForPlayer(playerid);
    return 0;
}

public OnPlayerSpawn(playerid)
{
    printf("[PendingRewards] OnPlayerSpawn playerid=%d -> schedule pending rewards apply in 750ms", playerid);
    if (playerParticipant[playerid] == -1 && IsPlayerConnected(playerid))
    {
        new tmpNick[MAX_PLAYER_NAME];
        GetPlayerName(playerid, tmpNick, MAX_PLAYER_NAME);
        new partId = ResolveOrCreateParticipant(tmpNick);
        playerParticipant[playerid] = partId;
        printf("[Auction] OnPlayerSpawn late-bind playerid=%d -> participantId=%d nick='%s'", playerid, partId, tmpNick);
    }
    SetTimerEx("ApplyPendingRewardsTimer", 750, false, "i", playerid);
    return 1;
}

public OnPlayerDisconnect(playerid)
{
    printf("[GM] OnPlayerDisconnect playerid=%d", playerid);
    playerParticipant[playerid] = -1;
    OnPlayerDisconnectForDrawables(playerid);
    return 1;
}

forward OnPlayerEnterDynamicArea(playerid, areaid);
public OnPlayerEnterDynamicArea(playerid, areaid){
    printf("[PendingRewards] OnPlayerEnterDynamicArea playerid=%d areaid=%d (may overlap with connect / reward dialog)", playerid, areaid);
    SendClientMessage(playerid, -1, "OnPlayerEnterDynamicArea");
    new containerid = GetContainerIdByAreaId(playerid, areaid);
    if (containerid == -1)
    {
        return 1;
    }

    printf("[PendingRewards] OnPlayerEnterDynamicArea -> ShowContainerDrawableForPlayer playerid=%d containerid=%d", playerid, containerid);
    ShowContainerDrawableForPlayer(playerid, containerid);
    return 1;
}

forward OnPlayerLeaveDynamicArea(playerid, areaid);
public OnPlayerLeaveDynamicArea(playerid, areaid)
{
    printf("[PendingRewards] OnPlayerLeaveDynamicArea playerid=%d areaid=%d", playerid, areaid);
    HideContainerDrawableForPlayer(playerid);
    return 1;
}


// ================================Dialogs===============================
public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
    printf("[GM] OnPlayerClickPlayerTextDraw playerid=%d", playerid);
    if (IsPlayerBidButton(playerid, playertextid))
    {
        ShowPlayerDialog(playerid, DIALOG_BID, DIALOG_STYLE_INPUT,
            "Make a bid", "Enter bid amount:", "Bid", "Cancel");
    }
    return 1;
}

RestoreAuctionTextDrawAfterBidDialog(playerid)
{
    new containerid = playerCurrentContainer[playerid];
    printf("[GM] RestoreAuctionTextDrawAfterBidDialog playerid=%d containerid=%d", playerid, containerid);
    if (containerid >= 0 && containerid < gSpawnContainerCount && containers[containerid][container_id] != -1)
    {
        ShowContainerDrawableForPlayer(playerid, containerid);
        printf("[GM] RestoreAuctionTextDrawAfterBidDialog restored drawable playerid=%d", playerid);
    }
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if (dialogid == DIALOG_REWARD)
    {
        printf("[PendingRewards] OnDialogResponse DIALOG_REWARD playerid=%d response=%d", playerid, response);
        return 1;
    }

    if (dialogid == DIALOG_BID_ACCEPTED || dialogid == DIALOG_BID_REJECTED)
    {
        printf("[GM] OnDialogResponse bid ack/reject playerid=%d dialogid=%d", playerid, dialogid);
        RestoreAuctionTextDrawAfterBidDialog(playerid);
        return 1;
    }

    if (dialogid == DIALOG_BID)
    {
        printf("[GM] OnDialogResponse DIALOG_BID playerid=%d response=%d", playerid, response);
        if (!response)
        {
            RestoreAuctionTextDrawAfterBidDialog(playerid);
            return 1;
        }

        new amount = strval(inputtext);
        new containerid = playerCurrentContainer[playerid];
        MakeBid(playerid, containerid, amount);
        return 1;
    }
    printf("[GM] OnDialogResponse unhandled dialogid=%d playerid=%d", dialogid, playerid);
    return 1;
}

public OnPlayerBidSuccess(playerid)
{
    printf("[GM] OnPlayerBidSuccess playerid=%d", playerid);
    ShowPlayerDialog(playerid, DIALOG_BID_ACCEPTED, DIALOG_STYLE_MSGBOX, "Bid",
        "Your bid was successfully accepted.", "OK", "");
    return 1;
}

public OnPlayerBidFail(playerid)
{
    printf("[GM] OnPlayerBidFail playerid=%d", playerid);
    ShowPlayerDialog(playerid, DIALOG_BID_REJECTED, DIALOG_STYLE_MSGBOX, "Bid",
        "Your bid was not accepted because it is invalid. See the chat for details.", "OK", "");
    return 1;
}

DeliverAuctionRewardToPlayer(playerid, bool:rewardIsModel, rewardValue)
{
    printf("[Reward] DeliverAuctionRewardToPlayer enter playerid=%d rewardIsModel=%d rewardValue=%d connected=%d currentContainer=%d",
        playerid, _:rewardIsModel, rewardValue, IsPlayerConnected(playerid), playerCurrentContainer[playerid]);
    if (!IsPlayerConnected(playerid))
    {
        printf("[Reward] DeliverAuctionRewardToPlayer abort: not connected playerid=%d", playerid);
        return 0;
    }

    new msg[144];
    if (!rewardIsModel)
    {
        GivePlayerMoney(playerid, rewardValue);
        format(msg, sizeof(msg), "You received $%d from the auction.", rewardValue);
    }
    else
    {
        format(msg, sizeof(msg), "You received a model reward (id %d). It is not spawned automatically on this server.", rewardValue);
    }
    printf("[Reward] DeliverAuctionRewardToPlayer SendClientMessage playerid=%d msg='%s'", playerid, msg);
    SendClientMessage(playerid, 0x22FF22FF, msg);
    printf("[Reward] DeliverAuctionRewardToPlayer ShowPlayerDialog DIALOG_REWARD=%d playerid=%d", DIALOG_REWARD, playerid);
    ShowPlayerDialog(playerid, DIALOG_REWARD, DIALOG_STYLE_MSGBOX, "Auction reward", msg, "OK", "");
    printf("[Reward] DeliverAuctionRewardToPlayer done playerid=%d", playerid);
    return 1;
}
// ================================END Dialogs===============================

OnAuctionTimerTick()
{
    UpdateAllContainerDrawables();
    return 1;
}

OnBidUpdated(containerId)
{
    printf("[GM] OnBidUpdated containerId=%d", containerId);
    UpdateContainerDrawableForAllPlayers(containerId);
    return 1;
}

UpdateAllContainerDrawables()
{
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i))
        {
            UpdateContainerDrawableForPlayer(i);
        }
    }
    return 1;
}

FindConnectedPlayerIdForParticipant(winnerParticipantId)
{
    for (new i = 0; i < MAX_PLAYERS; i++)
    {
        if (IsPlayerConnected(i) && playerParticipant[i] == winnerParticipantId)
        {
            return i;
        }
    }
    return -1;
}

OnPlayerWon(containerId, winnerParticipantId, lootId)
{
    if (winnerParticipantId < 0 || winnerParticipantId >= MAX_PLAYERS)
    {
        printf("[OnPlayerWon] invalid winnerParticipantId=%d", winnerParticipantId);
        DestroyContainer(containerId);
        return 1;
    }
    new winnerNick[MAX_PLAYER_NAME];
    winnerNick[0] = EOS;
    strmid(winnerNick, participantNick[winnerParticipantId], 0, MAX_PLAYER_NAME, MAX_PLAYER_NAME);
    printf("[OnPlayerWon] enter containerId=%d winnerParticipantId=%d lootId=%d nick='%s'", containerId, winnerParticipantId, lootId, winnerNick);
    new bool:isModel = loots[lootId][is_model];
    printf("[OnPlayerWon] isModel=%d", _:isModel);
    new prizeVal = loots[lootId][value];
    printf("[OnPlayerWon] prizeVal=%d", prizeVal);

    new winnerPlayerId = FindConnectedPlayerIdForParticipant(winnerParticipantId);
    if (winnerPlayerId != -1)
    {
        printf("[OnPlayerWon] online winner playerid=%d participantId=%d -> DeliverAuctionRewardToPlayer", winnerPlayerId, winnerParticipantId);
        DeliverAuctionRewardToPlayer(winnerPlayerId, isModel, prizeVal);
    }
    else
    {
        printf("[OnPlayerWon] no connected player for participantId=%d -> pending reward", winnerParticipantId);
        if (winnerNick[0] != '\0')
        {
            printf("[OnPlayerWon] EnqueuePendingReward nick='%s'", winnerNick);
            EnqueuePendingReward(winnerNick, isModel, prizeVal);
        }
        else
        {
            printf("[OnPlayerWon] empty winnerNick for participantId=%d, cannot enqueue pending reward", winnerParticipantId);
        }
    }

    printf("[OnPlayerWon] before DestroyContainer containerId=%d", containerId);
    DestroyContainer(containerId);
    printf("[OnPlayerWon] after DestroyContainer containerId=%d", containerId);
    return 1;
}