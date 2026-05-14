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
    print("My gamemode loaded");
}

public OnGameModeInit()
{
    print("Gamemode initialized");
	ConnectToDatabase();
    StartAuction();
	
    return 1;
}

public OnPlayerConnect(playerid)
{
    playerCurrentContainer[playerid] = -1;
    SendClientMessage(playerid, -1, "Hello from PAWN!");
    for(new i = 0; i < MAX_CONTAINERS; i++)
    {
        CreateContainerDrawableForPlayer(playerid, i);
    }

    GetPlayerName(playerid, gPlayerNames[playerid], MAX_PLAYER_NAME);
    GivePlayerMoney(playerid, 9999);
    return 1;
}

public OnPlayerDisconnect(playerid)
{
    OnPlayerDisconnectForDrawables(playerid);
    return 1;
}

forward OnPlayerEnterDynamicArea(playerid, areaid);
public OnPlayerEnterDynamicArea(playerid, areaid){
    SendClientMessage(playerid, -1, "OnPlayerEnterDynamicArea");
    new containerid = GetContainerIdByAreaId(playerid, areaid);
    if (containerid == -1)
    {
        return 1;
    }

    ShowContainerDrawableForPlayer(playerid, containerid);
    return 1;
}

forward OnPlayerLeaveDynamicArea(playerid, areaid);
public OnPlayerLeaveDynamicArea(playerid, areaid)
{
    HideContainerDrawableForPlayer(playerid);
    return 1;
}


// ================================Dialogs===============================
public OnPlayerClickPlayerTextDraw(playerid, PlayerText:playertextid)
{
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
    if (containerid >= 0 && containerid < MAX_CONTAINERS && containers[containerid][container_id] != -1)
    {
        ShowContainerDrawableForPlayer(playerid, containerid);
    }
    return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    if (dialogid == DIALOG_BID_ACCEPTED || dialogid == DIALOG_BID_REJECTED)
    {
        RestoreAuctionTextDrawAfterBidDialog(playerid);
        return 1;
    }

    if (dialogid == DIALOG_BID)
    {
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
    return 1;
}

public OnPlayerBidSuccess(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_BID_ACCEPTED, DIALOG_STYLE_MSGBOX, "Bid",
        "Your bid was successfully accepted.", "OK", "");
    return 1;
}

public OnPlayerBidFail(playerid)
{
    ShowPlayerDialog(playerid, DIALOG_BID_REJECTED, DIALOG_STYLE_MSGBOX, "Bid",
        "Your bid was not accepted because it is invalid. See the chat for details.", "OK", "");
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

OnPlayerWon(containerId, playerId, lootId)
{
    

    DestroyContainer(containerId);
    return 1;
}