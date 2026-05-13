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
new ChancesForContainerTypes[MAX_CONTAINER_TYPES];

// Not the best hierarchy. ConnectToDatabase calls LoadDataFromDatabase.
forward ConnectToDatabase();
ConnectToDatabase()
{
    new MySQLOpt:opt = mysql_init_options();
	mysql_set_option(opt, POOL_SIZE, 2);
	connId = mysql_connect(Host,User, Password, DataBase);
	
	if(mysql_errno())
    {
        printf("FAIL connect to db %s", DataBase);
    }
    else
    {
        printf("SUCCESS connect to db %s", DataBase);
        LoadDataFromDatabase();
    }
	
    return 1;
}

forward LoadDataFromDatabase();
LoadDataFromDatabase()
{
    LoadContainersFromDatabase();
    LoadLootsFromDatabase();
    return 1;
}

forward LoadContainersFromDatabase();
LoadContainersFromDatabase()
{
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
        return 1;
    }
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
    for (new i = rows; i < MAX_CONTAINERS; i++)
    {
        containers[i][container_id] = -1;
    }
    cache_delete(result);
    return 1;
}

forward LoadLootsFromDatabase();
LoadLootsFromDatabase()
{
    // Invalidate all loot chances
    for (new i = 0; i < MAX_CONTAINER_TYPES; i++)
    {
        ChancesForContainerTypes[i] = 0;
    }

    new Cache:result = mysql_query(connId, "SELECT * FROM `loot`");
    new rows;
    cache_get_row_count(rows);
    if (rows == 0)
    {
        print("No loot found in database");
        return 1;
    }
    printf("%d loots loaded", rows);
    for(new i = 0; i < rows; i++)
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

        for (new j = 0; j < MAX_CONTAINER_TYPES; j++)
        {
            if (containerTypes[j] == loots[i][container_type])
            {
                ChancesForContainerTypes[j] += loots[i][chance];
                printf("Chances for container type %d: %d", j, ChancesForContainerTypes[j]);
            }
        }
    }

    cache_delete(result);
    return 1;
}

forward SpawnContainer(containerId);
SpawnContainer(containerId)
{
    if (containerId == -1)
    {
        print("Container id is -1");
        return 0;
    }
    if (containers[containerId][container_id] == -1)
    {
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
    DestroyObject(containers[containerId][object_id]);
    DestroyDynamicArea(containers[containerId][area_id]);
    Delete3DTextLabel(containers[containerId][label_id]);
    printf("Container %d destroyed with object id %d, area id %d, label id %d", containerId, containers[containerId][object_id], containers[containerId][area_id], containers[containerId][label_id]);
    return 1;
}

forward SpawnAllContainers();
SpawnAllContainers()
{    
    for(new i = 0; i < MAX_CONTAINERS; i++)
    {
        SpawnContainer(i);
    }
	return 1;
}

forward GetContainerIdByAreaId(playerId, areaId);
GetContainerIdByAreaId(playerId, areaId)
{
    for(new i = 0; i < MAX_CONTAINERS; i++){
        if(containers[i][area_id] == areaId){
            return i;
        }
    }

    new str[64];
    format(str, sizeof(str), "Container not found for area id: %d", areaId);
    SendClientMessage(playerId, -1, str);
    return -1;
}

GetRandomLootIdForContainerType(containerType)
{
    new randomChance = random(ChancesForContainerTypes[containerType]);
    for(new i = 0; i < MAX_LOOTS; i++)
    {
        if (loots[i][container_type] == containerType)
        {
            randomChance -= loots[i][chance];
            if (randomChance < 0)
            {
                return i;
            }
        }
    }
    return -1;
}