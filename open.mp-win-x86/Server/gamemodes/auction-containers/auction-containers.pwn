#include <open.mp>
#include <a_mysql>
#include <streamer>
#include "constants.inc"
#include "auction-player-drawable.inc"

// Array length should be defined before compilation
// Update MAX_CONTAINERS according to recors count in the database



new MySQL:connId;

#define Host "127.0.0.1"
#define User "root"
#define DataBase "sa_db"
#define Password ""

#define ContainerTextColor 0xAA00DDFF

enum E_CONTAINER
{
    id,
    type,
    initial_price,
    Float:x,
    Float:y,
    Float:z,
    objectId,    // ID объекта из CreateDynamicObject
    areaId,      // ID зоны из CreateDynamicSphere
    Text3D:labelId      // ID 3D текста
}

new containers[MAX_CONTAINERS][E_CONTAINER];


main()
{
    print("My gamemode loaded");
}

public OnGameModeInit()
{
	new MySQLOpt:opt = mysql_init_options();
	mysql_set_option(opt, POOL_SIZE, 2);
	connId = mysql_connect(Host,User, Password, DataBase);
    print("Gamemode initialized");
	//printf(dbHandle);
	
	if(mysql_errno())
    {
        printf("FAIL connect to db %s", DataBase);
    }
    else
    {
        printf("SUCCESS connect to db %s", DataBase);
    }	
	
	GetContainers();
	
    return 1;
}

public OnPlayerConnect(playerid)
{
    SendClientMessage(playerid, -1, "Hello from PAWN!");
    return 1;
}

forward OnPlayerEnterDynamicObject(playerid, objectid);
public OnPlayerEnterDynamicObject(playerid, objectid)
{
    SendClientMessage(playerid, -1, "OnPlayerEnterDynamicObject");
    return 1;
}

forward OnPlayerLeaveDynamicObject(playerid, objectid);
public OnPlayerLeaveDynamicObject(playerid, objectid)
{
    SendClientMessage(playerid, -1, "OnPlayerLeaveDynamicObject");
    return 1;
}

forward OnPlayerEnterDynamicArea(playerid, areaid);
public OnPlayerEnterDynamicArea(playerid, areaid){
    SendClientMessage(playerid, -1, "OnPlayerEnterDynamicArea");
    for(new i = 0; i < MAX_CONTAINERS; i++){
        if(containers[i][areaId] == areaid){
            SendClientMessage(playerid, -1, "You are in container area");
            ShowContainerDrawableForPlayer(playerid, i);
            return 1;
        }
    }
    return 1;
}

forward OnPlayerLeaveDynamicArea(playerid, areaid);
public OnPlayerLeaveDynamicArea(playerid, areaid)
{
    SendClientMessage(playerid, -1, "OnPlayerLeaveDynamicArea");
    return 1;
}

forward OnPlayerEnterDynamicCP(playerid, checkpointid);
public OnPlayerEnterDynamicCP(playerid, checkpointid)
{
    SendClientMessage(playerid, -1, "OnPlayerEnterDynamicCP");
    return 1;
}

forward OnPlayerLeaveDynamicArea(playerid, areaid);
public OnPlayerLeaveDynamicCP(playerid, checkpointid)
{
    SendClientMessage(playerid, -1, "OnPlayerLeaveDynamicCP");
    return 1;
}

forward GetContainers();
GetContainers()
{
	//new qString[];
	//format(qString, sizeof(qString), "select * from 'spawn_containers'");
	new Cache:result = mysql_query(connId, "SELECT * FROM `spawn_containers`");
    
    new rows;
    cache_get_row_count(rows);
    
    for(new i = 0; i < rows; i++)
    {        
        // Store container data to array
        cache_get_value_name_int(i, "id", containers[i][id]);
        cache_get_value_name_float(i, "x", containers[i][x]);
        cache_get_value_name_float(i, "y", containers[i][y]);
        cache_get_value_name_float(i, "z", containers[i][z]);
        cache_get_value_name_int(i, "type", containers[i][type]);
        cache_get_value_name_int(i, "initial_price", containers[i][initial_price]);

        // Spawn container object
        containers[i][objectId] = CreateObject(containers[i][type], containers[i][x], containers[i][y], containers[i][z], 0.0, 0.0, 0.0);

        // Spawn container area
        containers[i][areaId] = CreateDynamicSphere(containers[i][x], containers[i][y], containers[i][z], 5.0);

        containers[i][labelId] = Create3DTextLabel("Auction container", ContainerTextColor, containers[i][x], containers[i][y], containers[i][z] + 2, 50, 0);

        printf("Container %d: area id = %d, pos = %f %f %f", i, containers[i][areaId], containers[i][x], containers[i][y], containers[i][z]);
    }
    
    cache_delete(result);
	return 1;
}