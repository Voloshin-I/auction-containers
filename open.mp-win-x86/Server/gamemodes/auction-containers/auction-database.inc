new MySQL:connId;

#define Host "127.0.0.1"
#define User "root"
#define DataBase "sa_db"
#define Password ""

#define ContainerTextColor 0xAA00DDFF

enum E_CONTAINER
{
    container_id,
    type,
    initial_price,
    Float:x,
    Float:y,
    Float:z,
    object_id,
    area_id,
    Text3D:label_id
}

enum E_LOOT
{
    loot_id,
    container_type,
    name[MAX_LOOT_NAME_LENGTH],
    chance,
    bool:is_model,
    value
}

new containers[MAX_CONTAINERS][E_CONTAINER];
new loots[MAX_LOOTS][E_LOOT];
new containerTypes[MAX_CONTAINER_TYPES];
// Sum of loot.chance per spawn `type` id (DB), not per slot j in containerTypes[]
new LootChanceTotalByType[MAX_CONTAINER_TYPE_ID];
new gLootRowCount;
new gSpawnContainerCount;

// Not the best hierarchy. ConnectToDatabase calls LoadDataFromDatabase.
forward ConnectToDatabase();
ConnectToDatabase()
{
    printf("[DB] ConnectToDatabase enter db=%s host=%s", DataBase, Host);
    new MySQLOpt:opt = mysql_init_options();
	mysql_set_option(opt, POOL_SIZE, 2);
	connId = mysql_connect(Host,User, Password, DataBase);
	
	if(mysql_errno())
    {
        printf("[DB] ConnectToDatabase FAIL errno=%d", mysql_errno());
        printf("FAIL connect to db %s", DataBase);
    }
    else
    {
        printf("[DB] ConnectToDatabase SUCCESS");
        printf("SUCCESS connect to db %s", DataBase);
        LoadDataFromDatabase();
    }
	
    printf("[DB] ConnectToDatabase exit");
    return 1;
}

forward LoadDataFromDatabase();
LoadDataFromDatabase()
{
    printf("[DB] LoadDataFromDatabase enter");
    LoadContainersFromDatabase();
    LoadLootsFromDatabase();
    printf("[DB] LoadDataFromDatabase exit");
    return 1;
}

forward LoadContainersFromDatabase();
LoadContainersFromDatabase()
{
    printf("[DB] LoadContainersFromDatabase enter");
    // Invalidate all container types
    for (new i = 0; i < MAX_CONTAINER_TYPES; i++)
    {
        containerTypes[i] = -1;
    }

    new Cache:result = mysql_query(connId, "SELECT * FROM `spawn_containers`");
    new rows;
    cache_get_row_count(rows);
    if (rows == 0)
    {
        print("No containers found in database");
        gSpawnContainerCount = 0;
        cache_delete(result);
        printf("[DB] LoadContainersFromDatabase exit early: zero rows");
        return 1;
    }
    if (rows > MAX_CONTAINERS)
    {
        printf("WARNING: spawn_containers rows %d exceed MAX_CONTAINERS %d, truncating", rows, MAX_CONTAINERS);
        rows = MAX_CONTAINERS;
    }
    gSpawnContainerCount = rows;
    printf("%d containers loaded", rows);
    for(new i = 0; i < rows; i++)
    {
        cache_get_value_name_int(i, "id", containers[i][container_id]);
        cache_get_value_name_float(i, "x", containers[i][x]);
        cache_get_value_name_float(i, "y", containers[i][y]);
        cache_get_value_name_float(i, "z", containers[i][z]);
        cache_get_value_name_int(i, "type", containers[i][type]);
        cache_get_value_name_int(i, "initial_price", containers[i][initial_price]);

        // Register container type if new
        for (new j = 0; j < MAX_CONTAINER_TYPES; j++)
        {
            if (containerTypes[j] == containers[i][type])
            {
                break;
            }
            if (containerTypes[j] == -1)
            {
                containerTypes[j] = containers[i][type];
                break;
            }
        }
        
    }
    for (new i = gSpawnContainerCount; i < MAX_CONTAINERS; i++)
    {
        containers[i][container_id] = -1;
    }
    cache_delete(result);
    printf("[DB] LoadContainersFromDatabase exit gSpawnContainerCount=%d", gSpawnContainerCount);
    return 1;
}
LoadLootsFromDatabase()
{
    printf("[DB] LoadLootsFromDatabase enter");
    new i;
    for (i = 0; i < MAX_CONTAINER_TYPE_ID; i++)
    {
        LootChanceTotalByType[i] = 0;
    }

    new Cache:result = mysql_query(connId, "SELECT * FROM `loot`");
    new rows;
    cache_get_row_count(rows);
    if (rows == 0)
    {
        print("No loot found in database");
        gLootRowCount = 0;
        cache_delete(result);
        printf("[DB] LoadLootsFromDatabase exit early: zero rows");
        return 1;
    }
    if (rows > MAX_LOOTS)
    {
        printf("WARNING: loot rows %d exceed MAX_LOOTS %d, truncating", rows, MAX_LOOTS);
        rows = MAX_LOOTS;
    }
    gLootRowCount = rows;
    printf("%d loots loaded", rows);
    for (i = 0; i < gLootRowCount; i++)
    {
        printf("Loot %d: %s", i, loots[i][name]);
        cache_get_value_name_int(i, "id", loots[i][loot_id]);
        printf("Loot id: %d", loots[i][loot_id]);
        cache_get_value_name_int(i, "type", loots[i][container_type]);
        printf("Loot type: %d", loots[i][container_type]);
        cache_get_value_name(i, "name", loots[i][name]);
        printf("Loot name: %s", loots[i][name]);
        cache_get_value_name_int(i, "chance", loots[i][chance]);
        printf("Loot chance: %d", loots[i][chance]);
        cache_get_value_name_bool(i, "is_model", loots[i][is_model]);
        printf("Loot is model: %d", loots[i][is_model]);
        cache_get_value_name_int(i, "value", loots[i][value]);
        printf("Loot value: %d", loots[i][value]);

        new ct = loots[i][container_type];
        new slot = -1;
        for (new j = 0; j < MAX_CONTAINER_TYPES; j++)
        {
            if (containerTypes[j] == ct)
            {
                slot = j;
                break;
            }
        }
        if (slot != -1)
        {
            LootChanceTotalByType[slot] += loots[i][chance];
            printf("Loot chance total for model %d (slot %d): %d", ct, slot, LootChanceTotalByType[slot]);
        }
        else
        {
            printf("WARNING: loot row %d type %d does not match any loaded spawn_containers.type (model id)", i, ct);
        }
    }

    cache_delete(result);
    printf("[DB] LoadLootsFromDatabase exit gLootRowCount=%d", gLootRowCount);
    return 1;
}

// MySQL: CREATE TABLE IF NOT EXISTS `pending_rewards` (
//   `player_nick` varchar(24) NOT NULL,
//   `is_model` tinyint(1) NOT NULL,
//   `value` int NOT NULL,
//   KEY `player_nick` (`player_nick`)
// ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

forward EnqueuePendingReward(const nick[], bool:rewardIsModel, rewardValue);
EnqueuePendingReward(const nick[], bool:rewardIsModel, rewardValue)
{
    if (nick[0] == '\0')
    {
        printf("[EnqueuePendingReward] empty nick, is_model=%d value=%d", _:rewardIsModel, rewardValue);
        return 0;
    }
    new q[192];
    mysql_format(connId, q, sizeof(q),
        "INSERT INTO `pending_rewards` (`player_nick`,`is_model`,`value`) VALUES ('%e', %d, %d)",
        nick, _:rewardIsModel, rewardValue);
    printf("[EnqueuePendingReward] enter nick='%s' is_model=%d value=%d", nick, _:rewardIsModel, rewardValue);
    mysql_query(connId, q);
    new err = mysql_errno(connId);
    if (err)
    {
        new errstr[144];
        mysql_error(errstr, sizeof(errstr), connId);
        printf("[EnqueuePendingReward] mysql_errno=%d after INSERT: %s (errno 1146 = table `pending_rewards` missing in DB %s)", err, errstr, DataBase);
    }
    else
    {
        printf("[EnqueuePendingReward] INSERT ok nick='%s'", nick);
    }
    return 1;
}

forward LoadAndApplyPendingRewardsForPlayer(playerid);
LoadAndApplyPendingRewardsForPlayer(playerid)
{
    printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer enter playerid=%d", playerid);
    if (!IsPlayerConnected(playerid))
    {
        printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer abort: not connected playerid=%d", playerid);
        return 0;
    }
    new nick[MAX_PLAYER_NAME];
    GetPlayerName(playerid, nick, MAX_PLAYER_NAME);
    if (nick[0] == '\0')
    {
        printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer abort: empty nick from GetPlayerName playerid=%d", playerid);
        return 0;
    }
    printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer nick='%s' playerid=%d", nick, playerid);

    new q[160];
    mysql_format(connId, q, sizeof(q),
        "SELECT `is_model`, `value` FROM `pending_rewards` WHERE `player_nick` = '%e'",
        nick);

    printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer executing SELECT");
    new Cache:result = mysql_query(connId, q);
    new err = mysql_errno(connId);
    if (err)
    {
        new errstr[144];
        mysql_error(errstr, sizeof(errstr), connId);
        printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer mysql_errno=%d after SELECT: %s (1146 = no table `pending_rewards` in DB %s)", err, errstr, DataBase);
        cache_delete(result);
        return 0;
    }
    new rows;
    cache_get_row_count(rows);
    printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer SELECT rows=%d nick='%s' playerid=%d", rows, nick, playerid);
    if (rows == 0)
    {
        cache_delete(result);
        printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer no rows, exit playerid=%d", playerid);
        return 1;
    }

    new bool:isModel;
    new prizeValue;
    for (new i = 0; i < rows; i++)
    {
        cache_get_value_name_bool(i, "is_model", isModel);
        cache_get_value_name_int(i, "value", prizeValue);
        printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer row %d/%d is_model=%d value=%d -> DeliverAuctionRewardToPlayer playerid=%d",
            i, rows, _:isModel, prizeValue, playerid);
        DeliverAuctionRewardToPlayer(playerid, isModel, prizeValue);
        printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer row %d DeliverAuctionRewardToPlayer returned playerid=%d", i, playerid);
    }
    cache_delete(result);

    mysql_format(connId, q, sizeof(q),
        "DELETE FROM `pending_rewards` WHERE `player_nick` = '%e'",
        nick);
    printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer DELETE for nick='%s'", nick);
    mysql_query(connId, q);
    err = mysql_errno(connId);
    if (err)
    {
        new errstr[144];
        mysql_error(errstr, sizeof(errstr), connId);
        printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer mysql_errno=%d after DELETE: %s", err, errstr);
    }
    else
    {
        printf("[PendingRewards] LoadAndApplyPendingRewardsForPlayer DELETE ok playerid=%d", playerid);
    }
    return 1;
}

forward SpawnContainer(containerId);
SpawnContainer(containerId)
{
    printf("[DB] SpawnContainer enter containerId=%d", containerId);
    if (containerId == -1)
    {
        printf("[DB] SpawnContainer abort containerId=-1");
        print("Container id is -1");
        return 0;
    }
    if (containers[containerId][container_id] == -1)
    {
        printf("[DB] SpawnContainer abort containerId=%d not loaded", containerId);
        printf("Container %d not found", containerId);
        return 0;

    }

    new containerName[32];
    new Float:cx = containers[containerId][x];
    new Float:cy = containers[containerId][y];
    new Float:cz = containers[containerId][z];
    format(containerName, sizeof(containerName), "Auction container %d", containers[containerId][type]);
    containers[containerId][object_id] = CreateObject(containers[containerId][type], cx, cy, cz, 0.0, 0.0, 0.0);
    containers[containerId][area_id] = CreateDynamicSphere(cx, cy, cz, 7.0);
    containers[containerId][label_id] = Create3DTextLabel(containerName, ContainerTextColor, cx, cy, cz + 2, 50, 0);

    printf("Container %d spawned with object id %d, area id %d, label id %d, name %s", containerId, containers[containerId][object_id], containers[containerId][area_id], containers[containerId][label_id], containerName);
    return 1;
}

forward DestroyContainer(containerId);
DestroyContainer(containerId)
{
    printf("[DB] DestroyContainer enter containerId=%d", containerId);
    DestroyObject(containers[containerId][object_id]);
    DestroyDynamicArea(containers[containerId][area_id]);
    Delete3DTextLabel(containers[containerId][label_id]);
    printf("Container %d destroyed with object id %d, area id %d, label id %d", containerId, containers[containerId][object_id], containers[containerId][area_id], containers[containerId][label_id]);
    return 1;
}

forward SpawnAllContainers();
SpawnAllContainers()
{
    printf("[DB] SpawnAllContainers enter gSpawnContainerCount=%d", gSpawnContainerCount);
    for(new i = 0; i < gSpawnContainerCount; i++)
    {
        SpawnContainer(i);
    }
    printf("[DB] SpawnAllContainers exit");
	return 1;
}

forward GetContainerIdByAreaId(playerId, areaId);
GetContainerIdByAreaId(playerId, areaId)
{
    printf("[DB] GetContainerIdByAreaId enter playerId=%d areaId=%d", playerId, areaId);
    for(new i = 0; i < gSpawnContainerCount; i++){
        if(containers[i][area_id] == areaId){
            printf("[DB] GetContainerIdByAreaId found slot=%d", i);
            return i;
        }
    }

    new str[64];
    format(str, sizeof(str), "Container not found for area id: %d", areaId);
    SendClientMessage(playerId, -1, str);
    printf("[DB] GetContainerIdByAreaId miss areaId=%d", areaId);
    return -1;
}

GetLootChanceTotalByContainerType(containerType)
{
    printf("[DB] GetLootChanceTotalByContainerType enter containerType=%d", containerType);
    for (new i = 0; i < MAX_CONTAINER_TYPES; i++)
    {
        if (containerTypes[i] == containerType)
        {
            printf("[DB] GetLootChanceTotalByContainerType slot=%d total=%d", i, LootChanceTotalByType[i]);
            return LootChanceTotalByType[i];
        }
    }
    printf("[DB] GetLootChanceTotalByContainerType no slot for model %d -> -1", containerType);
    return -1;
}

GetRandomLootIdForContainerType(containerType)
{
    printf("[DB] GetRandomLootIdForContainerType enter containerType=%d", containerType);
    new total = GetLootChanceTotalByContainerType(containerType);
    if (total <= 0)
    {
        printf("[GetRandomLootIdForContainerType] total %d <= 0", total);
        return -1;
    }
    new randomChance = random(total);
    for (new i = 0; i < gLootRowCount; i++)
    {
        if (loots[i][container_type] == containerType)
        {
            randomChance -= loots[i][chance];
            if (randomChance < 0)
            {
                printf("[GetRandomLootIdForContainerType] found loot %d for container type %d", i, containerType);
                return i;
            }
        }
    }
    printf("[GetRandomLootIdForContainerType] no loot found for container type %d, gLootRowCount=%d, randomChance=%d, total=%d", containerType, gLootRowCount, randomChance, total);
    return -1;
}