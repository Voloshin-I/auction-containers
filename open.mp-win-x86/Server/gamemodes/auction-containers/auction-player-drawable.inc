new PlayerText:drawables[MAX_PLAYERS][MAX_CONTAINERS];
new PlayerText:bidButtons[MAX_PLAYERS][MAX_CONTAINERS];

new playerCurrentContainer[MAX_PLAYERS];

forward CreateContainerDrawableForPlayer(playerid, containerid);
CreateContainerDrawableForPlayer(playerid, containerid)
{
    CreateContainerDrawableInfoForPlayer(playerid, containerid);
    CreateContainerBidButtonForPlayer(playerid, containerid);
    return 1;
}

forward CreateContainerDrawableInfoForPlayer(playerid, containerid);
CreateContainerDrawableInfoForPlayer(playerid, containerid)
{
    new PlayerText:td = CreatePlayerTextDraw(playerid, 263.0, 150.0, "00:00:00");
    PlayerTextDrawUseBox(playerid, td, true);
    PlayerTextDrawBoxColour(playerid, td, 0x000000AA);
    PlayerTextDrawTextSize(playerid, td, 377.0, 40.0);
    PlayerTextDrawFont(playerid, td, TEXT_DRAW_FONT_1);
    PlayerTextDrawColour(playerid, td, 0xFFFFFFFF);

    drawables[playerid][containerid] = td;

    return 1;
}

forward CreateContainerBidButtonForPlayer(playerid, containerid);
CreateContainerBidButtonForPlayer(playerid, containerid)
{
    new PlayerText:btn = CreatePlayerTextDraw(playerid, 290.0, 200.0, "[ Make a bid ]");
    PlayerTextDrawUseBox(playerid, btn, true);
    PlayerTextDrawBoxColour(playerid, btn, 0xFF6600FF);
    PlayerTextDrawTextSize(playerid, btn, 450.0, 30.0);
    PlayerTextDrawFont(playerid, btn, TEXT_DRAW_FONT_2);
    PlayerTextDrawColour(playerid, btn, 0xFFFFFFFF);
    PlayerTextDrawSetSelectable(playerid, btn, true);

    bidButtons[playerid][containerid] = btn;

    return 1;
}

forward IsPlayerBidButton(playerid, PlayerText:playertextid);
IsPlayerBidButton(playerid, PlayerText:playertextid)
{
    new containerid = playerCurrentContainer[playerid];
    if (containerid == -1) return 0;
    return playertextid == bidButtons[playerid][containerid];
}

forward OnPlayerDisconnectForDrawables(playerid);
OnPlayerDisconnectForDrawables(playerid)
{
    playerCurrentContainer[playerid] = -1;
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        for(new j = 0; j < MAX_CONTAINERS; j++)
        {
            PlayerTextDrawDestroy(i, drawables[i][j]);
            PlayerTextDrawDestroy(i, bidButtons[i][j]);
        }
    }
    return 1;
}

forward ShowContainerDrawableForPlayer(playerid, containerid);
ShowContainerDrawableForPlayer(playerid, containerid)
{
    if (playerCurrentContainer[playerid] != containerid)
    {
        HideContainerDrawableForPlayer(playerid);
        playerCurrentContainer[playerid] = containerid;
    }
    playerCurrentContainer[playerid] = containerid;
    UpdateContainerDrawableForPlayer(playerid);

    PlayerTextDrawShow(playerid, drawables[playerid][containerid]);
    PlayerTextDrawShow(playerid, bidButtons[playerid][containerid]);
    SelectTextDraw(playerid, 0xFFFFFFAA);
    return 1;
}

forward UpdateContainerDrawableForAllPlayers(containerid);
UpdateContainerDrawableForAllPlayers(containerid)
{
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        if (playerCurrentContainer[i] == containerid)
        {
            UpdateContainerDrawableForPlayer(i);
        }
    }
    return 1;
}

forward UpdateContainerDrawableForPlayer(playerid);
UpdateContainerDrawableForPlayer(playerid)
{
    new containerid = playerCurrentContainer[playerid];
    if (containerid == -1)
    {
        return 1;
    }

    gDrawableStr[0] = EOS;
    FormatTimer();
    FormatStartingPriceLine(containerid);
    FormatBids(containerid);
    PlayerTextDrawSetString(playerid, drawables[playerid][containerid], gDrawableStr);
    return 1;
}

forward HideContainerDrawableForPlayer(playerid);
HideContainerDrawableForPlayer(playerid)
{
    new containerid = playerCurrentContainer[playerid];
    if (containerid == -1)
    {
        return 1;
    }
    playerCurrentContainer[playerid] = -1;
    PlayerTextDrawHide(playerid, drawables[playerid][containerid]);
    PlayerTextDrawHide(playerid, bidButtons[playerid][containerid]);
    CancelSelectTextDraw(playerid);
    return 1;
}

FormatTimer()
{
    new seconds = gAuctionTimeLeft;
    new h = seconds / 3600;
    new m = (seconds % 3600) / 60;
    new s = seconds % 60;
    new tmp[TIMER_STRING_LENGTH];

    format(tmp, sizeof(tmp), "%02d:%02d:%02d~n~", h, m, s);
    strcat(gDrawableStr, tmp, BID_DRAWABLE_STRING_LENGTH);
}

FormatStartingPriceLine(containerid)
{
    new line[96];
    format(line, sizeof(line), "Starting price: $%d~n~", containers[containerid][initial_price]);
    strcat(gDrawableStr, line, BID_DRAWABLE_STRING_LENGTH);
}

FormatBids(containerid)
{
    if (gTotalBids[containerid] == 0)
    {
        strcat(gDrawableStr, "No bids yet.~n~", BID_DRAWABLE_STRING_LENGTH);
        return 1;
    }

    new line[128];
    for(new i = 0; i < MAX_PLAYERS; i++)
    {
        new playerBid = gBids[containerid][i];
        if (playerBid == 0) continue;

        new playerChance = playerBid * 100 / gTotalBids[containerid];

        format(line, sizeof(line), "%s: %d$ (%d%%)~n~", gPlayerNames[i], playerBid, playerChance);
        strcat(gDrawableStr, line, BID_DRAWABLE_STRING_LENGTH);
    }
    return 1;
}