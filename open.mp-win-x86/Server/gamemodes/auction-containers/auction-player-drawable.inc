#include <open.mp>
#include <a_mysql>
#include <streamer>
#include "constants.inc"

new PlayerText:pAuctionTD[MAX_PLAYERS][MAX_CONTAINERS];
forward ShowContainerDrawableForPlayer(playerid, containerid);
ShowContainerDrawableForPlayer(playerid, containerid)
{

    // создаётся в OnPlayerConnect
    new PlayerText:playerTextDrawID = CreatePlayerTextDraw(playerid, 320.0, 200.0, "Auction");
    pAuctionTD[playerid][containerid] = playerTextDrawID;
    PlayerTextDrawAlignment(playerid, playerTextDrawID, TEXT_DRAW_ALIGN_CENTER);
    PlayerTextDrawColour(playerid, playerTextDrawID, 0xFFFFFFFF);
    PlayerTextDrawBackgroundColour(playerid, playerTextDrawID, 0x000000FF);
    PlayerTextDrawUseBox(playerid, playerTextDrawID, true);
    PlayerTextDrawBoxColour(playerid, playerTextDrawID, 0x00000088);
    PlayerTextDrawTextSize(playerid,playerTextDrawID, 200.0, 20.0);

    // показать игроку
    PlayerTextDrawShow(playerid, playerTextDrawID);

    // обновить текст только у этого игрока
    PlayerTextDrawSetString(playerid, playerTextDrawID, "Timer: 59");
    PlayerTextDrawShow(playerid, playerTextDrawID); // после SetString нужно перепоказать

    return 1;
}

forward UpdateContainerDrawableForPlayer(playerid, containerid);
UpdateContainerDrawableForPlayer(playerid, containerid)
{
    return 1;
}


forward HideContainerDrawableForPlayer(playerid, containerid);
HideContainerDrawableForPlayer(playerid, containerid)
{
    PlayerTextDrawHide(playerid, pAuctionTD[playerid][containerid]);
    return 1;
}