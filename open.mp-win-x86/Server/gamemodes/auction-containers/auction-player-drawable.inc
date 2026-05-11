#include <open.mp>
#include <a_mysql>
#include <streamer>
#include "constants.inc"

new PlayerText:drawables[MAX_PLAYERS][MAX_CONTAINERS];
new bool:drawableVisible[MAX_PLAYERS][MAX_CONTAINERS];

new auctionTimeLeft;

forward UpdateAuctionTimeLeftForDrawables(timeLeft);
UpdateAuctionTimeLeftForDrawables(timeLeft)
{
    auctionTimeLeft = timeLeft;
}

forward CreateContainerDrawableForPlayer(playerid, containerid);
CreateContainerDrawableForPlayer(playerid, containerid)
{
    // создаётся в OnPlayerConnect
    new PlayerText:playerTextDrawID = CreatePlayerTextDraw(playerid, 320.0, 200.0, "Auction");
    drawables[playerid][containerid] = playerTextDrawID;
    PlayerTextDrawAlignment(playerid, playerTextDrawID, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, playerTextDrawID, 0xFFFFFFFF);
    PlayerTextDrawBackgroundColour(playerid, playerTextDrawID, 0x000000FF);
    PlayerTextDrawUseBox(playerid, playerTextDrawID, true);
    PlayerTextDrawBoxColour(playerid, playerTextDrawID, 0x00000088);
    PlayerTextDrawTextSize(playerid,playerTextDrawID, 200.0, 20.0);

    drawableVisible[playerid][containerid] = false;
    return 1;
}

forward ShowContainerDrawableForPlayer(playerid, containerid, timer);
ShowContainerDrawableForPlayer(playerid, containerid, timer)
{
    drawableVisible[playerid][containerid] = true;
    UpdateContainerDrawableForPlayer(playerid, containerid, timer)
    PlayerTextDrawShow(playerid, drawables[playerid][containerid]);

    return 1;
}

forward UpdateContainerDrawableForPlayer(playerid, containerid, timer);
UpdateContainerDrawableForPlayer(playerid, containerid, timer)
{
    if (drawableVisible[playerid][containerid] == false)
    {
        return 1;
    }

    new str[TIMER_STRING_LENGTH];
    FormatTimer(str);
    PlayerTextDrawSetString(playerid, drawables[playerid][containerid], str);
    return 1;
}

forward HideContainerDrawableForPlayer(playerid, containerid);
HideContainerDrawableForPlayer(playerid, containerid)
{
    drawableVisible[playerid][containerid] = false;
    PlayerTextDrawHide(playerid, drawables[playerid][containerid]);
    return 1;
}

FormatTimer(output[])
{
    new seconds = auctionTimeLeft;
    new h = seconds / 3600;
    new m = (seconds % 3600) / 60;
    new s = seconds % 60;
    format(output, TIMER_STRING_LENGTH, "%02d:%02d:%02d", h, m, s);
}