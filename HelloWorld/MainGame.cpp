#define PLAY_IMPLEMENTATION
#define PLAY_USING_GAMEOBJECT_MANAGER
#include "Play.h"
#include "GameUtilities.h"

bool testMode = false;

GameState game_state;

DisplayInfo displayInfo;

std::vector<Room*> Rooms;

Room room1;
Room room2;
Room room3;
Room room4;
Room room5;
Room room6;

std::vector<int> room1Objects;
std::vector<int> room2Objects;
std::vector<int> room3Objects;
std::vector<int> room4Objects;
std::vector<int> room5Objects;
std::vector<int> room6Objects;

std::vector<int> tempObjects;

PlayerProperties playerProperties;

void StartGame(float);

void CreateRoom(int, Vector2D);

std::vector<int>* GetRoomObjectArray();

void CreateShapeshiftEffect(GameObject&);

int CreateBoss(Vector2D);

void CreateCoin(std::vector<int>*, Vector2D);

void CreateLongPlatformWithCoins(std::vector<int>*, Vector2D);

int CreateNPC(Vector2D, int);

void UpdateCoin(GameObject&);

void UpdateCrate(GameObject&);

void UpdateNPC(GameObject&);

void UpdateFood(GameObject&);

void UpdateDrone(GameObject&, float);

void UpdateHammer(GameObject&, float);

void UpdateOrb(GameObject&);

void UpdateScout(GameObject&, float);

void UpdateBoss(GameObject&, float);

void UpdateFanatic(GameObject&, float);

void UpdateShooter(GameObject&, float);

void UpdateBullets(GameObject&, float);

void UpdateAttack(GameObject&, float);

void UpdateDestroyed();

void ReloadRoom(std::vector<int>*);

void ReloadCollectibles(std::vector<int>*);

void UnloadRoom();

void DrawKeybinds();

void DrawShapeshiftEffect();

int GetCurrentSpriteId();

void UpdatePlayerDeath(GameObject&);

void PlayerControls(GameObject&);

void TransformCat(GameObject&, int*);

void TransformDog(GameObject&, int*);

void TransformRat(GameObject&, int*);

void TransformBird(GameObject&, int*);

void UpdateCollisions(GameObject&);

void SetupPlatformSizes();

void SetupPickups();

void DrawHealthBar(GameObject&);

void DrawPlayerIcon();

void UpdateCharacterSize(GameObject*);

void StartMenu(float);

// The entry point for a PlayBuffer program
void MainGameEntry(PLAY_IGNORE_COMMAND_LINE)
{
	Play::CreateManager(displayInfo.DISPLAY_WIDTH, displayInfo.DISPLAY_HEIGHT, displayInfo.DISPLAY_SCALE);

	Play::CentreAllSpriteOrigins();

	//Creates the rooms
	Room room0 = Room();
	room1 = Room(Vector2D(2040, 2040), Vector2D(1000, 1000));
	room2 = Room(Vector2D(2040, 2040), Vector2D(1000, -1000));
	room3 = Room(Vector2D(3060, 2040), Vector2D(3500, 1000));
	room4 = Room(Vector2D(3060, 2040), Vector2D(6500, 1000));
	room5 = Room(Vector2D(2040, 2040), Vector2D(9000, 1000));
	room6 = Room(Vector2D(3060, 2550), Vector2D(3500, -1000));

	int player_id = Play::CreateGameObject(TYPE_PLAYER, room1.pos + Vector2D(-100, 700), 25, "dogA_idle_right");
	UpdateCharacterSize(&Play::GetGameObject(player_id));
	Play::GetGameObject(player_id).acceleration = { 0, 0.6F };
	Play::GetGameObject(player_id).animSpeed = 0.125f;

	//Create room links
	room1.roomTop = 2;
	room2.roomBottom = 1;
	room2.roomRight = 6;

	room1.roomRight = 3;
	room3.roomLeft = 1;

	room3.roomRight = 4;
	room4.roomLeft = 3;

	room4.roomRight = 5;
	room5.roomLeft = 4;

	room6.roomLeft = 2;
	room6.roomBottom = 3;

	//Add the rooms to a vector
	Rooms = std::vector<Room*>();
	Rooms.push_back(&room0);
	Rooms.push_back(&room1);
	Rooms.push_back(&room2);
	Rooms.push_back(&room3);
	Rooms.push_back(&room4);
	Rooms.push_back(&room5);
	Rooms.push_back(&room6);

	//Create the first room
	CreateRoom(1, room1.pos);
	playerProperties.direction = true;
	Play::LoadBackground("Data\\Backgrounds\\dawn.png");
	Play::LoadBackground("Data\\Backgrounds\\day.png");
	Play::LoadBackground("Data\\Backgrounds\\evening.png");
	Play::LoadBackground("Data\\Backgrounds\\night.png");
	Play::LoadBackground("Data\\Backgrounds\\noon.png");

	if(testMode)
	{
		playerProperties.animal_forms.isBirdUnlocked = true;
		playerProperties.animal_forms.isRatUnlocked = true;
		playerProperties.animal_forms.isCatUnlocked = true;
	}
}

// Called by PlayBuffer every frame (60 times a second!)
bool MainGameUpdate(float elapsedTime)
{
	if (elapsedTime < 0.02)
	{
		game_state.FPSMulti = 1;
	}
	else if (elapsedTime < 0.04 && elapsedTime > 0.025)
	{
		game_state.FPSMulti = 2;
	}
	else if (elapsedTime < 0.055 && elapsedTime > 0.045)
	{
		game_state.FPSMulti = 3;
	}

	StartMenu(elapsedTime);

	Play::PresentDrawingBuffer();

	if (Play::KeyPressed(VK_TAB) && game_state.animalState == STATE_PLAY)
	{
		if (!game_state.paused)
		{
			game_state.paused = true;
			Play::DrawFontText("132px", "PAUSED", Play::cameraPos + Vector2D{displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2}, Play::CENTRE);
			Play::DrawFontText("64px", "Coins Collected: " + std::to_string(playerProperties.coinsCollected) + " / " + std::to_string(playerProperties.levelCoins), Play::cameraPos + Vector2D{displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 + 150}, Play::CENTRE);
		}
		else
		{
			game_state.paused = false;
		}
	}
	return Play::KeyDown(VK_ESCAPE);
}

// Gets called once when the player quits the game 
int MainGameExit(void)
{
	Play::DestroyManager();
	return PLAY_OK;
}

/// <summary>
/// Draws the main menu for the game.
/// </summary>
/// <param name="elapsedTime"></param>
void StartMenu(float elapsedTime)
{
	if (game_state.animalState == STATE_MENU)
	{
		Play::ClearDrawingBuffer(Play::cOrange);
		Play::DrawBackground(0);
		//Play::StopAudioLoop("cityTheme");
		//Play::StartAudioLoop("mainTheme");

		Play::DrawFontText("132px", "PixelProwl", Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 }, Play::CENTRE);
		Play::DrawFontText("64px", "Press 'enter' to start", Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 + 275 }, Play::CENTRE);
		if (Play::KeyPressed(VK_RETURN))
		{
			game_state.animalState = STATE_PLAY;
			//Play::StopAudioLoop("mainTheme");
		}
	}
	else
	{
		StartGame(elapsedTime);
	}
}

/// <summary>
/// Draws the objective messages at the start of the game.
/// </summary>
/// <param name="player"></param>
/// <param name="elapsedTime"></param>
void DrawObjectiveMessage(GameObject& player, float elapsedTime)
{
	game_state.timer += elapsedTime;
	int message_id = Play::CreateGameObject(TYPE_SPEECH, Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 4 }, 0, "objective_first");
	GameObject& message_object = Play::GetGameObject(message_id);
	message_object.internalTimer += elapsedTime;

	if (game_state.timer < 5 && playerProperties.animal_forms.isCatUnlocked == false)
	{
		Play::SetSprite(message_object, "objective_first", 0.0f);
		Play::UpdateGameObject(message_object);
		Play::DrawObjectRotated(message_object);
		//Play::DrawSprite("objective_first", Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 4 }, 0);
	} else if (game_state.timer > 5 && game_state.timer < 11 && playerProperties.animal_forms.isCatUnlocked == false)
	{
		Play::SetSprite(message_object, "objective_second", 0.0f);
		Play::UpdateGameObject(message_object);
		Play::DrawObjectRotated(message_object);
		//Play::DrawSprite("objective_second", Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 4 }, 0);
	}
	else if (game_state.timer < 5 && playerProperties.roomNum == 1 && playerProperties.animal_forms.isCatUnlocked == true)
	{
		Play::SetSprite(message_object, "objective_cat", 0.0f);
		Play::UpdateGameObject(message_object);
		Play::DrawObjectRotated(message_object);
	}
	else if (game_state.timer < 5 && playerProperties.roomNum == 6 && playerProperties.animal_forms.isBirdUnlocked == true)
	{
		Play::SetSprite(message_object, "objective_bird", 0.0f);
		Play::UpdateGameObject(message_object);
		Play::DrawObjectRotated(message_object);
	}

	else if (game_state.timer < 5 && playerProperties.roomNum == 5 && playerProperties.animal_forms.isRatUnlocked == true)
	{
		Play::SetSprite(message_object, "objective_rat", 0.0f);
		Play::UpdateGameObject(message_object);
		Play::DrawObjectRotated(message_object);
	}

	Play::DestroyGameObject(message_id);
}

/// <summary>
/// Runs the games logic systems
/// </summary>
/// <param name="elapsedTime"></param>
void StartGame(float elapsedTime)
{
	GameObject& player = Play::GetGameObjectByType(TYPE_PLAYER);
	if (!game_state.paused) {
		Play::ClearDrawingBuffer(Play::cOrange);
		Play::DrawBackground(0);
		
		player.velocity.x = 0;
		if (player.velocity.y > 9.8)
		{
			player.velocity.y = 9.6F;
		}

		UpdateCollisions(player);
		UpdateDrone(player, elapsedTime);
		UpdateFanatic(player, elapsedTime);
		UpdateScout(player, elapsedTime);
		UpdateHammer(player, elapsedTime);
		UpdateShooter(player, elapsedTime);
		UpdateCoin(player);
		UpdateFood(player);
		UpdateBoss(player, elapsedTime);
		UpdateNPC(player);
		UpdateCrate(player);
		UpdateOrb(player);
		UpdateBullets(player, elapsedTime);
		if (game_state.animalState == STATE_ATTACKING) {
			UpdateAttack(player, elapsedTime);
		}
		UpdateDestroyed();
		DrawHealthBar(player);
		DrawPlayerIcon();
		player.internalTimer += elapsedTime;
		if (game_state.animalState != STATE_WON) {
			game_state.levelTime += elapsedTime;
		}
		if (player.internalTimer > 4) {
			player.radius = 25;
		}
		PlayerControls(player);
		DrawObjectiveMessage(player, elapsedTime);

		Play::UpdateGameObject(player);
		Play::DrawObjectRotated(player);
		DrawShapeshiftEffect();
		if (game_state.animalState == STATE_DEAD || game_state.animalState == STATE_WON)
		{
			UpdatePlayerDeath(player);
		}
		Play::cameraPos = player.pos - Vector2D(displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 1.7F);

		//Play::DrawFontText("64px", "HEALTH: " + std::to_string(playerProperties.health), player.pos - Vector2D{displayInfo.DISPLAY_WIDTH / 2, 400}, Play::LEFT);
		Play::DrawFontText("64px", "SCORE: " + std::to_string(game_state.score), player.pos - Vector2D{ displayInfo.DISPLAY_WIDTH / 2 - 10, 310 }, Play::LEFT);

		if (game_state.animalState == STATE_PLAY || game_state.animalState == STATE_ATTACKING)
		{
			DrawKeybinds();
		}

		if (Play::cameraPos.x < Rooms[playerProperties.roomNum]->left)
		{
			Play::cameraPos.x = Rooms[playerProperties.roomNum]->left;
		}

		if (Play::cameraPos.x > Rooms[playerProperties.roomNum]->right - displayInfo.DISPLAY_WIDTH)
		{
			Play::cameraPos.x = Rooms[playerProperties.roomNum]->right - displayInfo.DISPLAY_WIDTH;
		}

		if (Play::cameraPos.y < Rooms[playerProperties.roomNum]->top)
		{
			Play::cameraPos.y = Rooms[playerProperties.roomNum]->top;
		}

		if (Play::cameraPos.y > Rooms[playerProperties.roomNum]->bottom - displayInfo.DISPLAY_HEIGHT)
		{
			Play::cameraPos.y = Rooms[playerProperties.roomNum]->bottom - displayInfo.DISPLAY_HEIGHT;
		}

		//Play::DrawFontText("64px", "" + std::to_string(player.pos.y), player.pos - Vector2D( 100,300 ), Play::CENTRE);
		/*Play::DrawFontText("64px", "" + std::to_string(elapsedTime), player.pos - Vector2D(100, 250), Play::CENTRE);*/
	}
}

/// <summary>
/// Draw the players icon
/// </summary>
void DrawPlayerIcon()
{
	switch (playerProperties.playerCharacter)
	{
	case TYPE_DOG_A:
		Play::DrawSprite("icon_dog", Play::cameraPos + Vector2D{ 50, 50 }, 0);
		break;
	case TYPE_CAT_A:
		Play::DrawSprite("icon_cat", Play::cameraPos + Vector2D{ 50, 50 }, 0);
		break;
	case TYPE_RAT_A:
		Play::DrawSprite("icon_rat", Play::cameraPos + Vector2D{ 50, 50 }, 0);
		break;
	case TYPE_BIRD_A:
		Play::DrawSprite("icon_bird", Play::cameraPos + Vector2D{ 50, 50 }, 0);
		break;
	}
}

/// <summary>
/// Creates a room and all its objects, platforms and enimies
/// </summary>
/// <param name="roomNum">The room number</param>
/// <param name="roomPos">The room position</param>
void CreateRoom(int roomNum, Vector2D roomPos)
{
	srand(time(NULL));
	int randNum = rand() % 4;

	std::vector<int>* roomObjects = GetRoomObjectArray();

	switch (roomNum)
	{
	case 1:
		if (roomObjects->size() == 0) {
			roomObjects->push_back(Play::CreateGameObject(TYPE_LONGPLATFORM, roomPos + Vector2D(0, 870), 0, "null"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_LONGPLATFORM, roomPos + Vector2D(0, 934), 0, "room_bottom_A"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_LONGPLATFORM, roomPos + Vector2D(0, 998), 0, "room_bottom_A"));
			for (int i = -934; i < 1056; i += 64)
			{
				randNum = rand() % 4;
				switch (randNum)
				{
				default: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_A")); break;
				case 1: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_B")); break;
				case 2: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_C")); break;
				case 3: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_D")); break;
				}

			}
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(600, 680), 0, "platformA"));

			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-116, 392), 0, "platformLong"));
			CreateLongPlatformWithCoins(&room1Objects, roomPos + Vector2D(550, 100));

			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, 680), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, 424), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, 168), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, -88), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, -344), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, -600), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, -856), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, -1112), 0, "wall"));

			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-628, 600), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-628, 350), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-628, 100), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-628, -150), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-628, -400), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-628, -650), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-628, -900), 0, "platformB"));
			CreateCoin(roomObjects, roomPos + Vector2D(-300, 332));
			CreateCoin(roomObjects, roomPos + Vector2D(-100, 332));
			CreateCoin(roomObjects, roomPos + Vector2D(100, 332));
			CreateCoin(roomObjects, roomPos + Vector2D(500, 810));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(-400, 810), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SPIKES, roomPos + Vector2D(580, 624), 5, "spikes"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_ROOM_SIDE, roomPos + Vector2D(-998, 0), 0, "room_side_A"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_ORB, roomPos + Vector2D(-435, 332), 20, "orb_cat"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_FANATIC, roomPos + Vector2D(-170, 316), 25, "fanatic_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-200, 680), 0, "billboard1"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-200, 650), 0, "pizzaAd"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(100, 788), 0, "smallbillboard"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(100, 785), 0, "testAdSmall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(900, 788), 0, "smallbillboard"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(900, 785), 0, "muscleAdSmall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NPC, roomPos + Vector2D(320, 790), 80, "human_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NPC, roomPos + Vector2D(-300, 790), 80, "human2_idle_6"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NPC, roomPos + Vector2D(600, 790), 80, "human3_idle_4"));
			roomObjects->push_back(CreateNPC(roomPos + Vector2D(320, 790), 0));
			Play::StartAudioLoop("cityTheme");
		}
		else
		{
			ReloadRoom(&room1Objects);
		}
		break;
	case 2:

		if (roomObjects->size() == 0) {
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-628, 816), 0, "platformB"));
			CreateLongPlatformWithCoins(roomObjects, roomPos + Vector2D(-180, 832));
			CreateLongPlatformWithCoins(roomObjects, roomPos + Vector2D(-50, 0));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(350, 680), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(650, 480), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(750, 300), 0, "bigBillboard"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(750, 270), 0, "rooftopAd"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(842, 480), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(450, 160), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_DRONE, roomPos + Vector2D(300, 440), 25, "drone_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(100, 770), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(-260, -64), 30, "chest_closedA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(160, -64), 30, "chest_closedA"));

			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, 928), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, 672), 0, "wall"));

			roomObjects->push_back(Play::CreateGameObject(TYPE_ROOM_SIDE, roomPos + Vector2D(-998, 0), 0, "null"));
			Play::StopAudioLoop("cityTheme");
			Play::StartAudioLoop("rooftops");
		}
		else
		{
			ReloadRoom(&room2Objects);
		}
		break;
	case 3:
		if (roomObjects->size() == 0) {

			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-700, 680), 0, "bigBillboard"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-700, 650), 0, "gymAd"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(1200, 680), 0, "bigBillboard"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(1200, 650), 0, "cardAdBig"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-1300, 680), 0, "billboard1"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-1300, 651), 0, "stonksAd"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(600, 788), 0, "foodTruck"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(0, 788), 0, "foodTruck"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCOUT, roomPos + Vector2D(0, 790), 25, "scout_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(620, 700), 0, "hotDog"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(300, 788), 0, "house"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(375, 800), 0, "graffiti"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_EXTRALONGPLATFORM, roomPos + Vector2D(0, 870), 0, "null"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_EXTRALONGPLATFORM, roomPos + Vector2D(0, 934), 0, "room_bottom_long"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_EXTRALONGPLATFORM, roomPos + Vector2D(0, 998), 0, "room_bottom_long"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(600, 810), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(-600, 810), 30, "crate"));
			CreateCoin(roomObjects, roomPos + Vector2D(400, 810));

			for (int i = -1504; i < 1524; i += 64)
			{
				randNum = rand() % 4;
				switch (randNum)
				{
				default: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_A")); break;
				case 1: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_B")); break;
				case 2: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_C")); break;
				case 3: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_D")); break;
				}
			}
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(-900, 680), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(180, 320), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(-1100, 140), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-400, 500), 0, "platformLong"));
			CreateLongPlatformWithCoins(roomObjects, roomPos + Vector2D(-400, 140));
			roomObjects->push_back(Play::CreateGameObject(TYPE_FANATIC, roomPos + Vector2D(-520, 420), 25, "fanatic_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_FANATIC, roomPos + Vector2D(-550, 60), 25, "fanatic_idle_4"));

			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(900, 710), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(900, 454), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(1028, 650), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(1028, 450), 0, "platformB"));
		}
		else
		{
			ReloadRoom(&room3Objects);
		}
		break;
	case 4:
		if (roomObjects->size() == 0) {
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-700, 680), 0, "bigBillboard"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-700, 650), 0, "futureAd"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_EXTRALONGPLATFORM, roomPos + Vector2D(0, 870), 0, "null"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_EXTRALONGPLATFORM, roomPos + Vector2D(0, 934), 0, "room_bottom_long"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_EXTRALONGPLATFORM, roomPos + Vector2D(0, 998), 0, "room_bottom_long"));
			for (int i = -1504; i < 1524; i += 64)
			{
				randNum = rand() % 4;
				switch (randNum)
				{
				default: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_A")); break;
				case 1: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_B")); break;
				case 2: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_C")); break;
				case 3: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_D")); break;
				}

			}
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(900, 680), 0, "bigBillboard"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(900, 650), 0, "noodleAd"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(-1000, 640), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-1000, 440), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-500, 400), 0, "platformMedium"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(300, 400), 0, "platformMedium"));
			CreateCoin(roomObjects, roomPos + Vector2D(-600, 340));
			CreateCoin(roomObjects, roomPos + Vector2D(-400, 340));
			CreateCoin(roomObjects, roomPos + Vector2D(200, 340));
			CreateCoin(roomObjects, roomPos + Vector2D(400, 340));
			roomObjects->push_back(Play::CreateGameObject(TYPE_FANATIC, roomPos + Vector2D(300, 790), 25, "fanatic_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_FANATIC, roomPos + Vector2D(-300, 790), 25, "fanatic_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_DRONE, roomPos + Vector2D(-160, 0), 25, "drone_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_DRONE, roomPos + Vector2D(-40, 0), 25, "drone_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_DRONE, roomPos + Vector2D(670, 0), 25, "drone_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(1228, 400), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_DRONE, roomPos + Vector2D(1000, 0), 25, "drone_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCOUT, roomPos + Vector2D(300, 790), 35, "scout_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(450, 810), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(-600, 810), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(-1000, 640), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMB, roomPos + Vector2D(-1000, 440), 0, "platformB"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(1500, 680), 0, "wall"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(1500, 430), 0, "wall"));
		}
		else
		{
			ReloadRoom(&room4Objects);
		}
		break;
	case 5:

		if (roomObjects->size() == 0) {

			Play::StartAudioLoop("boss1");
			Play::StopAudioLoop("cityTheme");
			roomObjects->push_back(Play::CreateGameObject(TYPE_LONGPLATFORM, roomPos + Vector2D(0, 870), 0, "null"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_LONGPLATFORM, roomPos + Vector2D(0, 934), 0, "room_bottom_A"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_LONGPLATFORM, roomPos + Vector2D(0, 998), 0, "room_bottom_A"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(600, 810), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(-600, 810), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-20, 680), 0, "bigBillboard"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(-20, 650), 0, "burgerAd"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(200, 795), 30, "car"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_DRONE, roomPos + Vector2D(-150, 400), 25, "drone_idle_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_DRONE, roomPos + Vector2D(250, 400), 25, "drone_idle_4"));


			roomObjects->push_back(CreateBoss(roomPos + Vector2D(50, 765)));

			for (int i = -998; i < 932; i += 64)
			{
				randNum = rand() % 4;
				switch (randNum)
				{
				default: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_A")); break;
				case 1: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_B")); break;
				case 2: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_C")); break;
				case 3: roomObjects->push_back(Play::CreateGameObject(TYPE_NO_COLLISION, roomPos + Vector2D(i, 870), 0, "tile_road_D")); break;
				}

			}
			room5Objects.push_back(Play::CreateGameObject(TYPE_ROOM_SIDE, roomPos + Vector2D(988, 0), 0, "room_side_A"));
			room5Objects.push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-1000, 680), 0, "wall"));
			room5Objects.push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, roomPos + Vector2D(-1000, 430), 0, "wall"));
		}
		else
		{
			ReloadRoom(&room5Objects);

		}
		break;
	case 6:
		if (roomObjects->size() == 0) {
			
			CreateLongPlatformWithCoins(roomObjects, roomPos + Vector2D(-1050, 480));
			CreateLongPlatformWithCoins(roomObjects, roomPos + Vector2D(-150, 480));
			CreateLongPlatformWithCoins(roomObjects, roomPos + Vector2D(-900, -150));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(-900, -210), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_CRATE, roomPos + Vector2D(100, 420), 30, "crate"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(100, 100), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(-550, -500), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(-250, -820), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SHOOTER, roomPos + Vector2D(-550, -700), 60, "shooter_idle_left_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_PLATFORMA, roomPos + Vector2D(150, -850), 0, "platformA"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SHOOTER, roomPos + Vector2D(-150, -100), 60, "shooter_idle_left_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_SHOOTER, roomPos + Vector2D(-200, -400), 60, "shooter_idle_left_4"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_ORB, roomPos + Vector2D(-400, 410), 20, "orb_bird"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_ORB, roomPos + Vector2D(160, -920), 20, "orb_bird"));
			roomObjects->push_back(CreateNPC(roomPos + Vector2D(-1100, -230), 1));
			roomObjects->push_back(Play::CreateGameObject(TYPE_ROOM_SIDE, roomPos + Vector2D(1490, 0), 0, "null"));
			roomObjects->push_back(Play::CreateGameObject(TYPE_LONGPLATFORM, roomPos + Vector2D(0, -1200), 0, "null"));

		}
		else
		{
			ReloadRoom(&room6Objects);
		}
		break;
	}
	SetupPlatformSizes();
	SetupPickups();
}

/// <summary>
/// Get the array of objects for the current room
/// </summary>
/// <returns></returns>
std::vector<int>* GetRoomObjectArray()
{
	std::vector<int>* objects = &room1Objects;
	switch (playerProperties.roomNum)
	{
	default: break;
	case 2: objects = &room2Objects; break;
	case 3: objects = &room3Objects; break;
	case 4: objects = &room4Objects; break;
	case 5: objects = &room5Objects; break;
	case 6: objects = &room6Objects; break;
	}
	return objects;
}

void CreateShapeshiftEffect(GameObject& player)
{
	int shapeshift_id = Play::CreateGameObject(TYPE_SHAPESHIFT, player.pos, 0, "shapeshift");
	GameObject& obj_shapeshift = Play::GetGameObject(shapeshift_id);
	Play::SetSprite(obj_shapeshift, "shapeshift", 0.65f);
}

int CreateBoss(Vector2D pos)
{
	int boss = Play::CreateGameObject(TYPE_BOSS, pos, 80, "boss_idle_left_4");
	Play::GetGameObject(boss).health = 4;
	return boss;
}

/// <summary>
/// Creates a coin object at the given cords
/// </summary>
/// <param name="roomObjects"></param>
/// <param name="pos"></param>
void CreateCoin(std::vector<int>* roomObjects, Vector2D pos)
{
	roomObjects->push_back(Play::CreateGameObject(TYPE_COIN, pos, 8, "coin_A"));
	playerProperties.levelCoins += 1;
}

/// <summary>
/// Creates a long platform with coins on at the given cords
/// </summary>
/// <param name="roomObjects"></param>
/// <param name="pos"></param>
void CreateLongPlatformWithCoins(std::vector<int>* roomObjects, Vector2D pos)
{
	roomObjects->push_back(Play::CreateGameObject(TYPE_SCALE_FROM_SPRITE, pos, 0, "platformLong"));
	roomObjects->push_back(Play::CreateGameObject(TYPE_COIN, pos + Vector2D(120, -60), 8, "coin_A"));
	roomObjects->push_back(Play::CreateGameObject(TYPE_COIN, pos + Vector2D(-120, -60), 8, "coin_A"));
	playerProperties.levelCoins += 2;
}

/// <summary>
/// Creates a NPC at the cords with the specfic dialog wanted.
/// </summary>
/// <param name="pos"></param>
/// <param name="dialogNum"></param>
/// <returns></returns>
int CreateNPC (Vector2D pos, int dialogNum)
{
	int NPC = Play::CreateGameObject(TYPE_NPC, pos, 80, "human_idle_4");
	Play::GetGameObject(NPC).points = dialogNum;
	return NPC;
}

void UpdateCoin(GameObject& player) {
	std::vector<int> v_coins = Play::CollectGameObjectIDsByType(TYPE_COIN);
	//update the state of all the coins
	for (int id : v_coins) {

		GameObject& obj_coin = Play::GetGameObject(id);
		if (obj_coin.active)
		{
			if (Play::IsColliding(player, obj_coin))
			{
				game_state.score += obj_coin.points;
				playerProperties.coinsCollected += 1;
				obj_coin.collected = true;
				Play::PlayAudio("collect");
			}
			Play::UpdateGameObject(obj_coin);
			Play::DrawObjectRotated(obj_coin);
		}
		if (obj_coin.collected)
		{
			obj_coin.active = false;
		}
	}
}

void UpdateNPC(GameObject& player) {
	std::vector<int> v_human = Play::CollectGameObjectIDsByType(TYPE_NPC);


	for (int id : v_human) {
		GameObject& human = Play::GetGameObject(id);

		if (human.pos == Vector2D(1000,1000) + Vector2D{ -300, 790 }) {

			Play::SetSprite(human, "human2_idle_6", 0.10f);
			Play::UpdateGameObject(human);
			Play::DrawObjectRotated(human);

			if (Play::IsColliding(human, player) == true && playerProperties.playerCharacter != TYPE_RAT_A)
			{
				int speech;
				speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_bub");
	
				GameObject& bubble = Play::GetGameObject(speech);
				Play::UpdateGameObject(bubble);
				Play::DrawObjectRotated(bubble);
			}
		}
		else if (human.pos == Vector2D(1000, 1000) + Vector2D{600, 790}) {

			Play::SetSprite(human, "human3_idle_4", 0.15f);
			Play::UpdateGameObject(human);
			Play::DrawObjectRotated(human);

			if (Play::IsColliding(human, player) == true)
			{
				int speech;
				speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_bot");

				GameObject& bubble = Play::GetGameObject(speech);
				Play::UpdateGameObject(bubble);
				Play::DrawObjectRotated(bubble);

			}

		}
		else {
			Play::SetSprite(human, "human_idle_4", 0.15f);
			Play::UpdateGameObject(human);
			Play::DrawObjectRotated(human);

			if (Play::IsColliding(human, player) == true)
			{
				int speech;
				if (human.points == 0) {			{
				switch(playerProperties.playerCharacter)
				{
				case TYPE_BIRD_A: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_jokeB"); break;
				case TYPE_CAT_A: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_jokeC"); break;
				case TYPE_DOG_A: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_jokeD"); break;
				case TYPE_RAT_A: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_jokeR"); break;
				default: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_joke"); break;
				}
				
			}
					speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_help");
				}
				else {
					switch (playerProperties.playerCharacter)
					{
					case TYPE_BIRD_A: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_jokeB"); break;
					case TYPE_CAT_A: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_jokeC"); break;
					case TYPE_DOG_A: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_jokeD"); break;
					case TYPE_RAT_A: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_jokeR"); break;
					default: speech = Play::CreateGameObject(TYPE_SPEECH, human.pos + Vector2D{ 100, -60 }, 40, "pixel_speech_joke"); break;
					}

				}
				GameObject& bubble = Play::GetGameObject(speech);
				Play::UpdateGameObject(bubble);
				Play::DrawObjectRotated(bubble);

			}
		}
	}
}

void UpdateCrate(GameObject& player) {
	std::vector<int> crates = Play::CollectGameObjectIDsByType(TYPE_CRATE);

	//Cycles though each platform
	for (int crate_id : crates)
	{
		GameObject& crate = Play::GetGameObject(crate_id);
		if (crate.active) {
			Play::DrawObjectRotated(crate);
		}
	}
}

void UpdateFood(GameObject& player)
{
	std::vector<int> v_foods = Play::CollectGameObjectIDsByType(TYPE_FOOD);
	//update the state of all the coins
	for (int id : v_foods) {
		GameObject& obj_food = Play::GetGameObject(id);
		if (obj_food.active) {
			if (Play::IsColliding(player, obj_food)) {

				obj_food.collected = true;
				playerProperties.health += obj_food.points;
				Play::PlayAudio("collect");
				if (playerProperties.health > playerProperties.maxHealth)
				{
					playerProperties.health = playerProperties.maxHealth;
				}

			}
			Play::UpdateGameObject(obj_food);
			Play::DrawObjectRotated(obj_food);

			if (obj_food.collected)
			{
				obj_food.active = false;
			}
		}
	}
}

void UpdateDrone(GameObject& player, float deltaTime) {

	Play::CentreMatchingSpriteOrigins("drone_idle_4");
	std::vector<int> v_drones = Play::CollectGameObjectIDsByType(TYPE_DRONE);
	for (int id : v_drones) {
		GameObject& drone = Play::GetGameObject(id);

		if (drone.active) {
			bool hasCollided = false;
			Play::SetSprite(drone, "drone_idle_4", 0.15f);
			Play::UpdateGameObject(drone);
			Play::DrawObjectRotated(drone);
			drone.internalTimer += deltaTime;


			if (drone.internalTimer >= 5) {

				int bullets = Play::CreateGameObject(TYPE_BULLET, drone.pos + Vector2D{ -5, 110 }, 40, "lightning_down_6");
				GameObject& bullet = Play::GetGameObject(bullets);
				Play::SetSprite(bullet, "lightning_down_6", 0.15f);
				bullet.velocity = Point2f(0, 0);
				Play::DrawObjectRotated(bullet);
				Play::UpdateGameObject(bullet);
				Play::PlayAudio("electric");
				drone.internalTimer = 0;


			}

			if (Play::IsColliding(drone, player) == true)
			{
				player.internalTimer = 0;
				player.radius = 0;
				drone.radius = 0;
				playerProperties.hurt = true;
				playerProperties.health += -1;


			}
			if (Play::IsColliding(drone, player) == false && player.internalTimer > 3) {
				player.radius = 25;
				drone.radius = 25;
			}


			if (playerProperties.health <= 0) {
				game_state.animalState = STATE_DEAD;
			}
		}
	}
}

void UpdateHammer(GameObject& player, float deltaTime) {

	Play::CentreMatchingSpriteOrigins("hammer_active_8");
	std::vector<int> v_hammer = Play::CollectGameObjectIDsByType(TYPE_HAMMER);
	for (int id : v_hammer) {
		GameObject& hammer = Play::GetGameObject(id);

		if (hammer.active) {
			bool hasCollided = false;
			Play::SetSprite(hammer, "hammer_active_8", 0.065f);
			Play::UpdateGameObject(hammer);
			Play::DrawObjectRotated(hammer);
			hammer.internalTimer += deltaTime; 
			if (hammer.internalTimer >=2.13) {

				int crush = Play::CreateGameObject(TYPE_BULLET, hammer.pos + Vector2D{ -5, 200 }, 30, "crush_smoke_4");
				GameObject& crusher = Play::GetGameObject(crush);
				crusher.internalTimer += deltaTime;
				Play::SetSprite(crusher, "crush_smoke_4", 0.05f);
				crusher.velocity = Point2f(0, 0);	
				crusher.type = TYPE_DESTROYED;

				hammer.internalTimer = 0;



			}

			if (Play::IsColliding(hammer, player) == true)
			{
				player.internalTimer = 0;
				player.radius = 0;
				hammer.radius = 0;
				playerProperties.hurt = true;
				playerProperties.health += -1;


			}
			if (Play::IsColliding(hammer, player) == false && player.internalTimer > 3) {
				player.radius = 25;
				hammer.radius = 40;
			}


			if (playerProperties.health <= 0) {
				game_state.animalState = STATE_DEAD;
			}
		}
	}
}

void UpdateOrb(GameObject& player) {
	std::vector<int> v_orbs = Play::CollectGameObjectIDsByType(TYPE_ORB);
	for (int id : v_orbs) {

		GameObject& obj_orb = Play::GetGameObject(id);
		if (obj_orb.active) {
			switch (playerProperties.roomNum)
			{
			case 1:
				Play::SetSprite(obj_orb, "orb_cat", 0.25f);
				break;
			case 6:
				Play::SetSprite(obj_orb, "orb_bird", 0.25f);
				break;
			case 5:
				Play::SetSprite(obj_orb, "orb_rat", 0.25f);
				break;
			}

			if (Play::IsColliding(player, obj_orb))
			{
				obj_orb.collected = true;
				Play::PlayAudio("newForm");
				switch (playerProperties.roomNum)
				{
				case 1:
					playerProperties.animal_forms.isCatUnlocked = true;
					game_state.timer = 0.0f;
					player.pos.y -= 15;
					playerProperties.playerCharacter = TYPE_CAT_A;
					CreateShapeshiftEffect(player);
					break;
				case 6:
					
					if (obj_orb.pos.y < -1000) {
						game_state.animalState = STATE_WON;
					} else
					{
						playerProperties.animal_forms.isBirdUnlocked = true;
						game_state.timer = 0.0f;
						player.pos.y -= 16;
						player.velocity.y = -2;
						playerProperties.playerCharacter = TYPE_BIRD_A;
						CreateShapeshiftEffect(player);
					}
					break;
				case 5:
					playerProperties.animal_forms.isRatUnlocked = true;
					game_state.timer = 0.0f;
					player.pos.y -= 15;
					playerProperties.playerCharacter = TYPE_RAT_A;
					CreateShapeshiftEffect(player);
					break;
				}
			}

			Play::UpdateGameObject(obj_orb);
			Play::DrawObjectRotated(obj_orb);

			if (obj_orb.collected)
			{
				obj_orb.active = false;
			}
		}
	}
}

void UpdateScout(GameObject& player, float deltaTime) {
	std::vector<int> v_scout = Play::CollectGameObjectIDsByType(TYPE_SCOUT);

	for (int id : v_scout) {
		GameObject& scout = Play::GetGameObject(id);
		if (scout.active) {
			bool hasCollided = false;
			if (scout.velocity.x == 0) {
				Play::SetSprite(scout, "scout_idle_4", 0.15f);
			}
			Play::UpdateGameObject(scout);
			Play::DrawObjectRotated(scout);
			//increment timer by deltaTime
			//
			//if outside of timer parameters, reset timer
			scout.internalTimer += deltaTime;
			if (scout.internalTimer >= 2)
			{
				scout.velocity.x = 7;
				Play::SetSprite(scout, "scout_walk_right_4", 0.15f);

				//move accordingly
			}
			if (scout.internalTimer >= 4) {
				scout.velocity.x = -7;
				Play::SetSprite(scout, "scout_walk_left_4", 0.15f);
				scout.internalTimer = -.00000005;
			}

			if (Play::IsColliding(scout, player) == true)
			{
				player.internalTimer = 0;
				player.radius = 0;
				scout.radius = 0;
				playerProperties.hurt = true;
				playerProperties.health -= 1;


			}
			if (Play::IsColliding(scout, player) == false && player.internalTimer > 3) {
				player.radius = 25;
				scout.radius = 25;
			}

		}
	}
}

void UpdateBoss(GameObject& player, float deltaTime) {
	std::vector<int> v_Boss = Play::CollectGameObjectIDsByType(TYPE_BOSS);
	Play::CentreSpriteOrigin("boss_idle_left_4");
	Play::CentreSpriteOrigin("boss_attack_left_6");
	Play::CentreSpriteOrigin("boss_attack_right_6");



		for (int id : v_Boss) {

			GameObject& Boss = Play::GetGameObject(id);
			Boss.internalTimer += deltaTime;

			if (game_state.animalState == STATE_DEAD) {
				Play::SetSprite(Boss, "boss_idle_left_4", 0.05f);
				Play::UpdateGameObject(Boss);
				Play::DrawObjectRotated(Boss);
				int speech;
				Boss.velocity.x = 0;
				speech = Play::CreateGameObject(TYPE_SPEECH, Boss.pos + Vector2D{ 60, -100 }, 40, "pixel_death_boss");
				GameObject& bubble = Play::GetGameObject(speech);
				Play::UpdateGameObject(bubble);
				Play::DrawObjectRotated(bubble);
				Boss.active = false;

			}
			else {

			if (Boss.active) {


				bool hasCollided = false;
				if (Boss.velocity.x == 0) {
					Play::SetSprite(Boss, "boss_idle_left_4", 0.05f);
					int speech;
					speech = Play::CreateGameObject(TYPE_SPEECH, Boss.pos + Vector2D{ 60, -100 }, 40, "pixel_speech_BBQ");

					GameObject& bubble = Play::GetGameObject(speech);
					Play::UpdateGameObject(bubble);
					Play::DrawObjectRotated(bubble);
					if (Boss.internalTimer > 4) {
						bubble.type = TYPE_DESTROYED;
					}
				}
				Play::UpdateGameObject(Boss);
				Play::DrawObjectRotated(Boss);
				//increment timer by deltaTime
				//
				//if outside of timer parameters, reset timer

				if (Boss.internalTimer >= 5)
				{
					if (Boss.health > 1) {
						Boss.velocity.x = 1;
						Play::SetSprite(Boss, "boss_attack_right_6", 0.05f);
						if (Play::RandomRoll(200) == 200) {
							int bullets = Play::CreateGameObject(TYPE_BULLET, Boss.pos + Vector2D{ 140, -70 }, 100, "flame_right_10");
							GameObject& bullet = Play::GetGameObject(bullets);
							Play::SetSprite(bullet, "flame_right_10", 0.15f);
							bullet.velocity = Point2f(2, 0);
							Play::DrawObjectRotated(bullet);
							Play::UpdateGameObject(bullet);
							Play::PlayAudio("flame");
						}
					}

					else if (Boss.health == 1) {
						Boss.velocity.x = 2;
						Play::SetSprite(Boss, "boss_attack_right_6", 0.10f);
						if (Play::RandomRoll(50) == 50) {
							int bullets = Play::CreateGameObject(TYPE_BULLET, Boss.pos + Vector2D{ 140, -70 }, 100, "flame_right_10");
							GameObject& bullet = Play::GetGameObject(bullets);
							Play::SetSprite(bullet, "flame_right_10", 0.15f);
							bullet.velocity = Point2f(4, 0);
							Play::DrawObjectRotated(bullet);
							Play::UpdateGameObject(bullet);
							Play::PlayAudio("flame");
						}
					}
					//move accordingly
				}
				if (Boss.internalTimer >= 10) {
					if (Boss.health > 1) {
						Boss.velocity.x = -1;
						Play::SetSprite(Boss, "boss_attack_left_6", 0.05f);
						Boss.internalTimer = -.00000001;
					}
					else if (Boss.health == 1) {
						Boss.velocity.x = -2;
						Play::SetSprite(Boss, "boss_attack_left_6", 0.10f);
						Boss.internalTimer = -.00000001;
					}
				}
				if (Boss.internalTimer < 5) {
					if (Boss.health > 1) {
						if (Play::RandomRoll(200) == 200) {
							int bullets = Play::CreateGameObject(TYPE_BULLET, Boss.pos + Vector2D{ -250, -70 }, 100, "flame_left_10");
							GameObject& bullet = Play::GetGameObject(bullets);
							Play::SetSprite(bullet, "flame_left_10", 0.15f);
							bullet.velocity = Point2f(-2, 0);
							Play::DrawObjectRotated(bullet);
							Play::UpdateGameObject(bullet);
							Play::PlayAudio("flame");
						}

					}
					else if (Boss.health == 1) {
						if (Play::RandomRoll(50) == 50) {
							int bullets = Play::CreateGameObject(TYPE_BULLET, Boss.pos + Vector2D{ -250, -70 }, 100, "flame_left_10");
							GameObject& bullet = Play::GetGameObject(bullets);
							Play::SetSprite(bullet, "flame_left_10", 0.15f);
							bullet.velocity = Point2f(-4, 0);
							Play::DrawObjectRotated(bullet);
							Play::UpdateGameObject(bullet);
							Play::PlayAudio("flame");
						}

					}
				}
				if (Play::IsColliding(Boss, player) == true)
				{
					player.internalTimer = 0;
					player.radius = 0;
					Boss.radius = 0;
					playerProperties.hurt = true;
					playerProperties.health -= 1;


				}
				if (Play::IsColliding(Boss, player) == false && player.internalTimer > 3) {
					player.radius = 25;
					Boss.radius = 80;
				}

			}

		}
	}
}

void UpdateFanatic(GameObject& player, float deltaTime) {
	std::vector<int> v_fanatic = Play::CollectGameObjectIDsByType(TYPE_FANATIC);

	for (int id : v_fanatic) {
		GameObject& fanatic = Play::GetGameObject(id);
		if (fanatic.active) {
			bool hasCollided = false;
			if (fanatic.velocity.x == 0) {
				Play::SetSprite(fanatic, "fanatic_idle_left_4", 0.15f);
			}
			Play::UpdateGameObject(fanatic);
			Play::DrawObjectRotated(fanatic);
			//increment timer by deltaTime
			//
			//if outside of timer parameters, reset timer
			fanatic.internalTimer += deltaTime;
			if (fanatic.internalTimer >= 2)
			{
				fanatic.velocity.x = 3;
				Play::SetSprite(fanatic, "fanatic_walk_right_6", 0.15f);
				if (Play::RandomRoll(200) == 200) {
					int bullets = Play::CreateGameObject(TYPE_BULLET, fanatic.pos, 30, "bullet_right_4");
					GameObject& bullet = Play::GetGameObject(bullets);
					Play::SetSprite(bullet, "bullet_right_4", 0.15f);
					bullet.velocity = Point2f(5, 0);
					Play::DrawObjectRotated(bullet);
					Play::UpdateGameObject(bullet);
					Play::PlayAudio("bullet");
				}

				//move accordingly
			}
			if (fanatic.internalTimer >= 4) {
				fanatic.velocity.x = -3;
				Play::SetSprite(fanatic, "fanatic_walk_left_6", 0.15f);
				fanatic.internalTimer = -.00000001;
			}
			if (fanatic.internalTimer < 2) {
				if (Play::RandomRoll(200) == 200) {
					int bullets = Play::CreateGameObject(TYPE_BULLET, fanatic.pos, 30, "bullet_left_4");
					GameObject& bullet = Play::GetGameObject(bullets);
					Play::SetSprite(bullet, "bullet_left_4", 0.15f);
					bullet.velocity = Point2f(-5, 0);
					Play::DrawObjectRotated(bullet);
					Play::UpdateGameObject(bullet);
					Play::PlayAudio("bullet");
					
				}
			}
			if (Play::IsColliding(fanatic, player) == true)
			{
				player.internalTimer = 0;
				player.radius = 0;
				fanatic.radius = 0;
				playerProperties.hurt = true;
				playerProperties.health -= 1;


			}
			if (Play::IsColliding(fanatic, player) == false && player.internalTimer > 3) {
				player.radius = 25;
				fanatic.radius = 25;
			}

		}

	}
	if (playerProperties.health <= 0) {
		playerProperties.health = 0;
		game_state.animalState = STATE_DEAD;
	}

	std::vector<int> v_bullets = Play::CollectGameObjectIDsByType(TYPE_BULLET);
	for (int id : v_bullets) {
		GameObject& bullet = Play::GetGameObject(id);

		if (Play::IsColliding(player, bullet)) {
			playerProperties.health -= 1;
			playerProperties.hurt = true;
			player.internalTimer = 0;
			player.radius = 0;
			bullet.radius = 0;
			bullet.type = TYPE_DESTROYED;
			if (player.internalTimer > 3) {
				player.radius = 25;
			}
		}

		else if (!Play::IsVisible(bullet) || bullet.internalTimer > 5)
		{

			bullet.type = TYPE_DESTROYED;

		}
	}
	if (player.internalTimer > 0.5F) {
		playerProperties.hurt = false;
	}
}

void UpdateShooter(GameObject& player, float deltaTime) {
	std::vector<int> v_shooter = Play::CollectGameObjectIDsByType(TYPE_SHOOTER);

	for (int id : v_shooter) {
		GameObject& shooter = Play::GetGameObject(id);
		if (shooter.active) {
			bool hasCollided = false;
			if (shooter.velocity.x == 0) {
				Play::SetSprite(shooter, "shooter_idle_left_4", 0.15f);
			}
			Play::UpdateGameObject(shooter);
			Play::DrawObjectRotated(shooter);
			//increment timer by deltaTime
			//
			//if outside of timer parameters, reset timer
			shooter.internalTimer += deltaTime;
			if (shooter.internalTimer >= 4)
			{
				shooter.velocity.x = 1;
				Play::SetSprite(shooter, "shooter_idle_right_4", 0.15f);
				if (Play::RandomRoll(75) == 75) {
					int bullets = Play::CreateGameObject(TYPE_BULLET, shooter.pos + Vector2D{ -5, 18 }, 30, "bullet_shooter_right_4");
					GameObject& bullet = Play::GetGameObject(bullets);
					Play::SetSprite(bullet, "bullet_shooter_right_4", 0.15f);
					bullet.velocity = Point2f(5, 0);
					Play::DrawObjectRotated(bullet);
					Play::UpdateGameObject(bullet);
					Play::PlayAudio("bullet");
				}

				//move accordingly
			}
			if (shooter.internalTimer >= 8) {
				shooter.velocity.x = -1;
				Play::SetSprite(shooter, "shooter_idle_left_4", 0.15f);
				shooter.internalTimer = -.00000001;
			}
			if (shooter.internalTimer < 4) {
				if (Play::RandomRoll(75) == 75) {
					int bullets = Play::CreateGameObject(TYPE_BULLET, shooter.pos + Vector2D{ -5, 18 }, 30, "bullet_shooter_left_4");
					GameObject& bullet = Play::GetGameObject(bullets);
					Play::SetSprite(bullet, "bullet_shooter_left_4", 0.15f);
					bullet.velocity = Point2f(-5, 0);
					Play::DrawObjectRotated(bullet);
					Play::UpdateGameObject(bullet);
					Play::PlayAudio("bullet");

				}
			}
			if (Play::IsColliding(shooter, player) == true)
			{
				player.internalTimer = 0;
				player.radius = 0;
				shooter.radius = 0;
				playerProperties.hurt = true;
				playerProperties.health -= 1;


			}
			if (Play::IsColliding(shooter, player) == false && player.internalTimer > 3) {
				player.radius = 25;
				shooter.radius = 25;
			}

		}

	}
	if (playerProperties.health <= 0) {
		playerProperties.health = 0;
		game_state.animalState = STATE_DEAD;
	}
}



void UpdateBullets(GameObject& player, float deltaTime) {

	std::vector<int> bullets = Play::CollectGameObjectIDsByType(TYPE_BULLET);


	for (int id : bullets) {
		GameObject& bullet = Play::GetGameObject(id);
		bullet.internalTimer += deltaTime;

		Play::UpdateGameObject(bullet);

		if (Play::IsLeavingDisplayArea(bullet, Play::VERTICAL))
		{
			bullet.type = TYPE_DESTROYED;
		}

		if (bullet.internalTimer > 3)
		{
			bullet.type = TYPE_DESTROYED;
		}

		Play::DrawObjectRotated(bullet);
	}
};

void UpdateAttack(GameObject& player, float deltaTime) {

	GameObject& attack = Play::GetGameObjectByType(TYPE_ATTACK);
	
	if (attack.internalTimer > 1)
	{
		attack.type = TYPE_DESTROYED;
		game_state.animalState = STATE_PLAY;
	}
	attack.internalTimer += deltaTime;
	if (!attack.active) { return; }
	int randNum = rand() % 4;

	if (playerProperties.direction)
	{
		attack.pos = player.pos - Vector2D(-32, 0);
	}
	else
	{
		attack.pos = player.pos - Vector2D(32, 0);
	}

	Play::UpdateGameObject(attack);
	if (Play::IsLeavingDisplayArea(attack, Play::VERTICAL))
	{
		attack.type = TYPE_DESTROYED;
		game_state.animalState = STATE_PLAY;
	}
	std::vector<int> v_fanatic = Play::CollectGameObjectIDsByType(TYPE_FANATIC);

	for (int id : v_fanatic) {
		GameObject& fanatic = Play::GetGameObject(id);
		if (fanatic.active) {
			if (Play::IsColliding(attack, fanatic))
			{
				Play::PlayAudio("robotDie");
				fanatic.active = false;
				game_state.score += 1000;
			}
		}
	}

	std::vector<int> v_drone = Play::CollectGameObjectIDsByType(TYPE_DRONE);

	for (int id : v_drone) {
		GameObject& drone = Play::GetGameObject(id);
		if (drone.active) {
			if (Play::IsColliding(attack, drone))
			{
				Play::PlayAudio("robotDie");
				drone.active = false;
				game_state.score += 600;
			}
		}
	}

	std::vector<int> v_crate = Play::CollectGameObjectIDsByType(TYPE_CRATE);

	for (int id : v_crate) {
		GameObject& crate = Play::GetGameObject(id);
		if (crate.active) {
			if (Play::IsColliding(attack, crate))
			{
				randNum = rand() % 6;
				crate.active = false;
				if (crate.spriteId == Play::GetSpriteId("chest_closedA"))
				{
					game_state.score += 350;
					switch (randNum)
					{
					case 0: case 1: case 2: tempObjects.push_back(Play::CreateGameObject(TYPE_COIN, crate.pos, 8, "coin_A")); playerProperties.levelCoins += 1; playerProperties.tempCoins += 1;  break;
					case 3: case 4: tempObjects.push_back(Play::CreateGameObject(TYPE_FOOD, crate.pos, 8, "food_random")); break;
					default: break;
					}
					SetupPickups();
				}
				else 
				{
					game_state.score += 200;
					switch (randNum)
					{
					case 0: tempObjects.push_back(Play::CreateGameObject(TYPE_COIN, crate.pos, 8, "coin_A")); playerProperties.levelCoins += 1; playerProperties.tempCoins += 1;  break;
					case 1: case 2: case 3: case 4: tempObjects.push_back(Play::CreateGameObject(TYPE_FOOD, crate.pos, 8, "food_random")); break;
					default: break;
					}
					SetupPickups();
				}
			}
		}
	}

	std::vector<int> v_scout = Play::CollectGameObjectIDsByType(TYPE_SCOUT);

	for (int id : v_scout) {
		GameObject& scout = Play::GetGameObject(id);
		if (scout.active) {
			if (Play::IsColliding(attack, scout))
			{
				Play::PlayAudio("robotDie");
				scout.active = false;
				game_state.score += 800;
				SetupPickups();
			}
		}
	}

	std::vector<int> v_shooter = Play::CollectGameObjectIDsByType(TYPE_SHOOTER);

	for (int id : v_shooter) {
		GameObject& shooter = Play::GetGameObject(id);
		if (shooter.active) {
			if (Play::IsColliding(attack, shooter))
			{
				Play::PlayAudio("robotDie");
				shooter.active = false;
				game_state.score += 900;
				SetupPickups();
			}
		}
	}

	std::vector<int> v_boss = Play::CollectGameObjectIDsByType(TYPE_BOSS);

	for (int id : v_boss) {
		GameObject& boss = Play::GetGameObject(id);
		if (boss.active) {
			if (Play::IsColliding(attack, boss))
			{
				boss.health -= 1;
				if (boss.health > 0) {
					Play::PlayAudio("bossOw");
				}
				attack.active = false;
				if (boss.health <= 0)
				{
					Play::PlayAudio("robotDie");
					Play::StopAudioLoop("boss1");
					Play::StartAudioLoop("cityTheme");
					Play::CreateGameObject(TYPE_ORB, boss.pos, 20, "orb_rat");
					boss.active = false;
					game_state.score += 2500;
					SetupPickups();
				}

				player.internalTimer = 0;
				player.radius = 0;
				boss.radius = 0;

				if (player.internalTimer > 3) {
					player.radius = 25;
					boss.radius = 80;
				}
			}
		}
	}
};

void UpdateDestroyed()
{
	std::vector<int> v_destroyed = Play::CollectGameObjectIDsByType(TYPE_DESTROYED);

	for (int dead : v_destroyed)
	{
		GameObject& obj_dead = Play::GetGameObject(dead);
		obj_dead.animSpeed = 0.2f;
		Play::UpdateGameObject(obj_dead);

		if (obj_dead.frame % 2)
		{
			Play::DrawObjectRotated(obj_dead, (10 - obj_dead.frame) / 10.0f);
		}
		if (!Play::IsVisible(obj_dead) || obj_dead.frame >= 10)
		{
			Play::DestroyGameObject(dead);
		}
	}
};

/// <summary>
/// Reloads the current room
/// </summary>
void ReloadRoom(std::vector<int>* objects)
{
	for (int object_id : *objects)
	{
		Play::GetGameObject(object_id).active = true;
		//For testing
		//Play::GetGameObject(object_id).collected = false;
	}
}

/// <summary>
/// Reloads collectibles upon restart
/// </summary>
void ReloadCollectibles(std::vector<int>* objects)
{
	for (int object_id : *objects)
	{
		Play::GetGameObject(object_id).active = true;
		Play::GetGameObject(object_id).collected = false;
	}
}

/// <summary>
/// Unloads the current room
/// </summary>
void UnloadRoom()
{
	for (int object_id : tempObjects)
	{
		Play::DestroyGameObject(object_id);
	}
	tempObjects.clear();

	std::vector<int>* objects = GetRoomObjectArray();

	for (int object_id : *objects)
	{
		Play::GetGameObject(object_id).active = false;
	}
	return;
}

/// <summary>
/// Draws the keybind information to the screen
/// </summary>
void DrawKeybinds()
{
	//Draw keybinds
	float opacity_key2 = 0.5;
	float opacity_key3 = 0.5;
	float opacity_key4 = 0.5;
	if (playerProperties.animal_forms.isCatUnlocked == true)
	{
		opacity_key2 = 1;
	}
	if (playerProperties.animal_forms.isRatUnlocked == true)
	{
		opacity_key3 = 1;
	}
	if (playerProperties.animal_forms.isBirdUnlocked == true)
	{
		opacity_key4 = 1;
	}
	Play::DrawSprite("keybind_sprite", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 60, -55 }, 0);
	Play::DrawSprite("string_dog", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 60, -110 }, 0);
	Play::DrawSprite("string_one", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 60, -55 }, 0);

	Play::DrawSpriteTransparent("keybind_sprite", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 140, -55 }, 0, opacity_key2);
	Play::DrawSpriteTransparent("string_cat", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 140, -110 }, 0, opacity_key2);
	Play::DrawSpriteTransparent("string_two", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 140, -55 }, 0, opacity_key2);

	Play::DrawSpriteTransparent("keybind_sprite", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 220, -55 }, 0, opacity_key3);
	Play::DrawSpriteTransparent("string_rat", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 220, -110 }, 0, opacity_key3);
	Play::DrawSpriteTransparent("string_three", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 220, -55 }, 0, opacity_key3);

	Play::DrawSpriteTransparent("keybind_sprite", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 300, -55 }, 0, opacity_key4);
	Play::DrawSpriteTransparent("string_bird", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 300, -110 }, 0, opacity_key4);
	Play::DrawSpriteTransparent("string_four", (Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 300, -55 }), 0, opacity_key4);

	Play::DrawSprite("keybind_sprite", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 380, -55 }, 0);
	Play::DrawSprite("string_use", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 380, -110 }, 0);
	Play::DrawSprite("string_E", (Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 380, -55 }), 0);

	if (playerProperties.playerCharacter == TYPE_DOG_A)
	{
		Play::DrawSprite("keybind_sprite", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 460, -55 }, 0);
		Play::DrawSprite("string_atk", Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 460, -110 }, 0);
		Play::DrawSprite("string_F", (Play::cameraPos + Vector2D{ 0, displayInfo.DISPLAY_HEIGHT } + Vector2D{ 460, -55 }), 0);
	}

}

/// <summary>
/// Updates the shapeshift effect and destroys it when done
/// </summary>
void DrawShapeshiftEffect()
{
	std::vector<int> v_shapeshift = Play::CollectGameObjectIDsByType(TYPE_SHAPESHIFT);
	for (int shapeshift_id : v_shapeshift)
	{
		GameObject& shapeshift = Play::GetGameObject(shapeshift_id);
		Play::UpdateGameObject(shapeshift);
		Play::DrawObjectRotated(shapeshift);

		//delete shapeshift effect once animation is done
		if (Play::IsAnimationComplete(shapeshift))
		{
			Play::DestroyGameObject(shapeshift_id);
		}
	}
}

/// <summary>
/// Gets the current sprite ID for the player
/// </summary>
/// <returns></returns>
int GetCurrentSpriteId()
{
	switch (playerProperties.playerCharacter)
	{
	case TYPE_BIRD_A: return Play::GetSpriteId("birdA_idle_left");
	case TYPE_BIRD_B: return Play::GetSpriteId("birdB_idle_left");
	case TYPE_CAT_A: return Play::GetSpriteId("catA_idle_left");
	case TYPE_CAT_B: return Play::GetSpriteId("catB_idle_left");
	case TYPE_DOG_A: return Play::GetSpriteId("dogA_idle_left");
	case TYPE_DOG_B: return Play::GetSpriteId("dogB_idle_left");
	case TYPE_RAT_A: return Play::GetSpriteId("ratA_idle_left");
	case TYPE_RAT_B: return Play::GetSpriteId("ratB_idle_left");
	}
}

/// <summary>
/// Code for displaying the death and won game menus
/// </summary>
/// <param name="player"></param>
void UpdatePlayerDeath(GameObject& player)
{
	player.velocity.x = 0;
	player.animSpeed = 0.15F;
	player.spriteId = GetCurrentSpriteId() - 3;
	int timeS = round(game_state.levelTime);
	const int timeScore = 50000 - (140 * timeS);
	const int timeM = floor(timeS / 60);
	timeS -= (timeM * 60);

	//update collision box for death sprite
	UpdateCharacterSize(&player);

	//stop animation loop once done
	if (Play::IsAnimationComplete(player))
	{
		player.animSpeed = 0.0f;
		//player.spriteId = GetCurrentSpriteId() + 4;

		//display game over message
		if (game_state.animalState == STATE_DEAD) {

			Play::DrawFontText("64px", "Final Score: " + std::to_string(game_state.score), Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 + 125 }, Play::CENTRE);
			Play::DrawFontText("132px", "GAME OVER", Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 }, Play::CENTRE);
		}
		else
		{
			Play::DrawFontText("64px", "Time Score: " + std::to_string(timeScore), Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 + 125 }, Play::CENTRE);
			Play::DrawFontText("64px", "Completion time: " + std::to_string(timeM) + "m, " + std::to_string(timeS) + "s", Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 + 175 }, Play::CENTRE);
			Play::DrawFontText("64px", "Final Score: " + std::to_string(game_state.score + timeScore), Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 + 225 }, Play::CENTRE);
			Play::DrawFontText("132px", "YOU WIN!!!", Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 }, Play::CENTRE);
		}
		Play::DrawFontText("64px", "Coins Collected: " + std::to_string(playerProperties.coinsCollected) + " / " + std::to_string(playerProperties.levelCoins), Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 + 75 }, Play::CENTRE);
		Play::DrawFontText("64px", "Press 'enter' to restart", Play::cameraPos + Vector2D{ displayInfo.DISPLAY_WIDTH / 2, displayInfo.DISPLAY_HEIGHT / 2 + 275 }, Play::CENTRE);
	}
}

/// <summary>
/// Code for restarting the game
/// </summary>
/// <param name="player">The player</param>
/// <param name="elapsedTime">deltaTime</param>
void RestartGame(GameObject& player, float elapsedTime)
{
	if (Play::KeyPressed(VK_RETURN))
	{
		//Reset values
		game_state.score = 0;
		game_state.timer = 0.0f;
		playerProperties.animal_forms.isRatUnlocked = false;
		playerProperties.animal_forms.isCatUnlocked = false;
		playerProperties.animal_forms.isBirdUnlocked = false;
		if (testMode)
		{
			playerProperties.animal_forms.isBirdUnlocked = true;
			playerProperties.animal_forms.isRatUnlocked = true;
			playerProperties.animal_forms.isCatUnlocked = true;
		}
		playerProperties.coinsCollected = 0;
		playerProperties.direction = true;
		playerProperties.health = playerProperties.maxHealth;
		UnloadRoom();
		playerProperties.levelCoins -= playerProperties.tempCoins;
		playerProperties.roomNum = 1;
		playerProperties.playerCharacter = TYPE_DOG_A; 
		player.pos = room1.pos + Vector2D(-100, 700);
		Play::StopAudioLoop("gameOver");

		//Reload first room
		CreateRoom(1, room1.pos);

		//Reload coins and orbs
		std::vector<int> orbs = Play::CollectGameObjectIDsByType(TYPE_ORB);
		std::vector<int> coins = Play::CollectGameObjectIDsByType(TYPE_COIN);

		ReloadCollectibles(&orbs);
		ReloadCollectibles(&coins);

		game_state.animalState = STATE_PLAY;
		game_state.levelTime = 0;
		//StartGame(player, elapsedTime);
	}
}

/// <summary>
/// Handles the player controls
/// </summary>
/// <param name="player">The player pointer</param>
void PlayerControls(GameObject& player)
{
	int spriteID = GetCurrentSpriteId();

	switch (game_state.animalState) {
	case STATE_PLAY:

		break;
	case STATE_DEAD: case STATE_WON:
		RestartGame(player, 0.0f);
		//return statement so that no other controls can be triggered once dead
		return;
	case STATE_ATTACKING:
		player.animSpeed = 0.25F;
		if (playerProperties.direction)
		{
			player.spriteId = spriteID - 4;
		}
		else
		{
			player.spriteId = spriteID - 5;
		}
		if (Play::KeyDown(VK_LEFT) || Play::KeyDown(0x41))
		{
			player.velocity.x = -5.5F * game_state.FPSMulti;
			playerProperties.direction = false;
			if (player.pos.x < Rooms[playerProperties.roomNum]->left + 50)
			{
				player.pos.x -= 50;
				UnloadRoom();
				playerProperties.roomNum = Rooms[playerProperties.roomNum]->roomLeft;
				CreateRoom(playerProperties.roomNum, Rooms[playerProperties.roomNum]->pos);
				playerProperties.hurt = false;
			}
		}

		else if (Play::KeyDown(VK_RIGHT) || Play::KeyDown(0x44))
		{
			player.velocity.x = 5.5F * game_state.FPSMulti;
			playerProperties.direction = true;
			if (player.pos.x > Rooms[playerProperties.roomNum]->right - 50)
			{
				player.pos.x += 50;
				UnloadRoom();
				playerProperties.roomNum = Rooms[playerProperties.roomNum]->roomRight;
				CreateRoom(playerProperties.roomNum, Rooms[playerProperties.roomNum]->pos);
				playerProperties.hurt = false;
			}
		}
		return;
	}

	if (Play::KeyPressed(0x31))
	{
		TransformDog(player, &spriteID);
	}
	else if (Play::KeyPressed(0x32) && playerProperties.animal_forms.isCatUnlocked)
	{
		TransformCat(player, &spriteID);
	}
	else if (Play::KeyPressed(0x33) && playerProperties.animal_forms.isRatUnlocked)
	{
		TransformRat(player, &spriteID);
	}
	else if (Play::KeyPressed(0x34) && playerProperties.animal_forms.isBirdUnlocked)
	{
		TransformBird(player, &spriteID);
	}

	if (Play::KeyPressed(0x46) && playerProperties.playerCharacter == TYPE_DOG_A)
	{
		game_state.animalState = STATE_ATTACKING;
		if (playerProperties.direction)
		{
			Play::CreateGameObject(TYPE_ATTACK, player.pos - Vector2D(-32, 0), 25, "null");
		}
		else
		{
			Play::CreateGameObject(TYPE_ATTACK, player.pos - Vector2D(32, 0), 25, "null");
		}
	}

	if (Play::KeyPressed(VK_SPACE) && playerProperties.onPlatform)
	{
		player.velocity = { 0, -18 };
	}

	else if (Play::KeyPressed(VK_SPACE) && playerProperties.playerCharacter == TYPE_BIRD_A && !playerProperties.doubleJump)
	{
		player.velocity = { 0, -14 };
		playerProperties.doubleJump = true;
	}

	if (Play::KeyDown(VK_LEFT) || Play::KeyDown(0x41))
	{
		player.velocity.x = -7.5F * game_state.FPSMulti;
		if (playerProperties.playerCharacter == TYPE_CAT_A)
		{
			player.velocity.x += -2 * game_state.FPSMulti;
		}
		else if (playerProperties.playerCharacter == TYPE_BIRD_A)
		{
			player.velocity.x += 2 * game_state.FPSMulti;
		}
		player.spriteId = spriteID + 2;
		player.animSpeed = 0.3F;
		playerProperties.direction = false;
		if (player.pos.x < Rooms[playerProperties.roomNum]->left + 50)
		{
			player.pos.x -= 50;
			UnloadRoom();
			playerProperties.roomNum = Rooms[playerProperties.roomNum]->roomLeft;
			CreateRoom(playerProperties.roomNum, Rooms[playerProperties.roomNum]->pos);
			playerProperties.hurt = false;
		}
	}

	else if (Play::KeyDown(VK_RIGHT) || Play::KeyDown(0x44))
	{
		player.velocity.x = 7.5F * game_state.FPSMulti;
		if (playerProperties.playerCharacter == TYPE_CAT_A)
		{
			player.velocity.x += 2 * game_state.FPSMulti;
		}
		else if (playerProperties.playerCharacter == TYPE_BIRD_A)
		{
			player.velocity.x += -2 * game_state.FPSMulti;
		}
		player.spriteId = spriteID + 3;
		player.animSpeed = 0.3F;
		playerProperties.direction = true;
		if (player.pos.x > Rooms[playerProperties.roomNum]->right - 50)
		{
			player.pos.x += 50;
			UnloadRoom();
			playerProperties.roomNum = Rooms[playerProperties.roomNum]->roomRight;
			CreateRoom(playerProperties.roomNum, Rooms[playerProperties.roomNum]->pos);
			playerProperties.hurt = false;
		}
	}
	else
	{
		if (playerProperties.direction)
		{
			player.spriteId = spriteID + 1;
			player.animSpeed = 0.15F;
			UpdateCharacterSize(&player);
		}
		else
		{
			player.spriteId = spriteID;
			player.animSpeed = 0.15F;
			UpdateCharacterSize(&player);
		}
	}

	if (player.pos.y < Rooms[playerProperties.roomNum]->top + 50)
	{
		player.pos.y -= 50;
		UnloadRoom();
		playerProperties.roomNum = Rooms[playerProperties.roomNum]->roomTop;
		CreateRoom(playerProperties.roomNum, Rooms[playerProperties.roomNum]->pos);
	}

	if (player.pos.y > Rooms[playerProperties.roomNum]->bottom - 50)
	{
		if (Rooms[playerProperties.roomNum]->roomBottom == 0)
		{
			player.pos.y -= 600;
			return;
		}
		player.pos.y += 50;
		UnloadRoom();
		playerProperties.roomNum = Rooms[playerProperties.roomNum]->roomBottom;
		CreateRoom(playerProperties.roomNum, Rooms[playerProperties.roomNum]->pos);
	}

	if (Play::KeyPressed(0x31) || Play::KeyPressed(0x32) || Play::KeyPressed(0x33) || Play::KeyPressed(0x34))
	{
		UpdateCharacterSize(&player);
	}

	if (playerProperties.hurt)
	{
		player.animSpeed = 0.1F;
		if (playerProperties.direction)
		{
			player.spriteId = spriteID - 1;
		}
		else
		{
			player.spriteId = spriteID - 2;
		}
		return;
	}
}

/// <summary>
/// Code for transforming into the dog
/// </summary>
/// <param name="player">The player</param>
void TransformDog(GameObject& player, int* spriteID)
{
	if (playerProperties.playerCharacter != TYPE_DOG_A) {
		player.pos.y -= 16;
		playerProperties.playerCharacter = TYPE_DOG_A;
		*spriteID = GetCurrentSpriteId();
		player.spriteId = *spriteID;
		UpdateCharacterSize(&player);
		Play::UpdateGameObject(player);
		CreateShapeshiftEffect(player);
	}
	playerProperties.playerCharacter = TYPE_DOG_A;
}

/// <summary>
/// Code for transforming into the cat
/// </summary>
/// <param name="player">The player</param>
void TransformCat(GameObject& player, int* spriteID)
{
	if (playerProperties.playerCharacter != TYPE_CAT_A) {
		player.pos.y -= 16;
		playerProperties.playerCharacter = TYPE_CAT_A;
		*spriteID = GetCurrentSpriteId();
		player.spriteId = *spriteID;
		UpdateCharacterSize(&player);
		Play::UpdateGameObject(player);
		CreateShapeshiftEffect(player);
	}
	playerProperties.playerCharacter = TYPE_CAT_A;
}

/// <summary>
/// Code for transforming into the rat
/// </summary>
/// <param name="player">The player</param>
void TransformRat(GameObject& player, int* spriteID)
{
	if (playerProperties.playerCharacter != TYPE_RAT_A) {
		player.pos.y += 8;
		playerProperties.playerCharacter = TYPE_RAT_A;
		*spriteID = GetCurrentSpriteId();
		player.spriteId = *spriteID;
		UpdateCharacterSize(&player);
		Play::UpdateGameObject(player);
		CreateShapeshiftEffect(player);
	}
	playerProperties.playerCharacter = TYPE_RAT_A;
}

/// <summary>
/// Code for transforming into the bird
/// </summary>
/// <param name="player">The player</param>
void TransformBird(GameObject& player, int* spriteID)
{
	if (playerProperties.playerCharacter != TYPE_BIRD_A) {
		player.pos.y -= 12;
		playerProperties.playerCharacter = TYPE_BIRD_A;
		*spriteID = GetCurrentSpriteId();
		player.spriteId = *spriteID;
		UpdateCharacterSize(&player);
		Play::UpdateGameObject(player);
		CreateShapeshiftEffect(player);
	}
	playerProperties.playerCharacter = TYPE_BIRD_A;
}

/// <summary>
/// Handles the rectangle conllisions
/// </summary>
/// <param name="player">The Player</param>
void UpdateCollisions(GameObject& player)
{

	std::vector<int> no_collison = Play::CollectGameObjectIDsByType(TYPE_NO_COLLISION);

	//Cycles though each platform
	for (int platform_id : no_collison)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		if (platform.active) {
			Play::DrawObjectRotated(platform);
		}
	}

	//Gets the player sides and initulizes the platform varibles for later
	int playerLeft = player.pos.x - player.size.x;
	int playerRight = player.pos.x + player.size.x;
	int playerBottom = player.pos.y + player.size.y;
	int playerTop = player.pos.y - player.size.y;
	int left;
	int right;
	int bottom;
	int top;
	playerProperties.onPlatform = false;
	std::vector<int> platforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORM);

	//Cycles though each platform
	for (int platform_id : platforms)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		if (platform.active) {
			left = platform.pos.x - platform.size.x;
			right = platform.pos.x + platform.size.x;
			bottom = platform.pos.y + platform.size.y;
			top = platform.pos.y - platform.size.y;

			//Code for dectectting a general collison
			if (playerRight > left && playerLeft < right)
			{
				if (playerBottom > top && playerTop < bottom)
				{
					player.pos.x = player.oldPos.x;
					player.velocity.x = 0;
					player.acceleration = { 0, 0 };
				}
				//Code for preventing the player from jumping up into a collider
				if (playerTop - 6 > bottom - 16 && playerTop - 6 < bottom && !platform.canJumpThrough)
				{
					if (player.velocity.y < 1)
					{
						player.velocity.y = -(player.velocity.y * 0.4F);
					}
				}
			}
			//Code for detecting if the player is on a platform
			if (playerRight > left + 5 && playerLeft < right - 5 && playerBottom + 11 > top && playerBottom + 11 < top + 16)
			{
				playerProperties.onPlatform = true;
				playerProperties.doubleJump = false;
				player.velocity.y = 0;
				player.pos.y = top + (playerTop - player.pos.y - 1);
				player.acceleration.y = 0;
			}
			//Code to fix walking between 2 platforms on the same height
			/*if (playerRight > left && playerLeft < right && playerBottom + 6 > top && playerBottom + 6 < top + 16)
			{
				playerProperties.onPlatform = true;
				player.velocity.y = 0;
				player.acceleration.y = 0;
			}*/
			//Code for updating and displaying the latforms
			Play::UpdateGameObject(platform);
			Play::DrawObjectRotated(platform);
		}
	}

	platforms = Play::CollectGameObjectIDsByType(TYPE_SPIKES);

	for (int platform_id : platforms)
	{
		GameObject& spikes = Play::GetGameObject(platform_id);
		if (spikes.active) {
			left = spikes.pos.x - spikes.size.x;
			right = spikes.pos.x + spikes.size.x;
			bottom = spikes.pos.y + spikes.size.y;
			top = spikes.pos.y - spikes.size.y;

			//Code for dectectting a general collison
			if (playerRight > left + 8 && playerLeft < right - 8)
			{
				if (playerBottom > top && playerTop < bottom && game_state.animalState != STATE_DEAD) //stops player bouncing on spikes when dead
				{
					player.pos = player.oldPos;
					player.velocity = { 0, -15 };
					player.pos.y = top + (playerTop - player.pos.y - 5);
					playerProperties.health -= 1;
					player.internalTimer = 0;
					playerProperties.hurt = true;
				}
			}
			Play::UpdateGameObject(spikes);
			Play::DrawObjectRotated(spikes);
		}

		if (!playerProperties.onPlatform)
		{
			player.acceleration = { 0, 0.6F };
		}

	}
}

/// <summary>
/// Sets up the sizes for the diffrent platforms and objects
/// </summary>
void SetupPlatformSizes()
{
	std::vector<int> platforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORMA);
	int spriteID;

	//Cycles though each platform
	for (int platform_id : platforms)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		platform.size = { 96, 32 };
		platform.type = TYPE_PLATFORM;
	}

	platforms = Play::CollectGameObjectIDsByType(TYPE_PLATFORMB);

	//Cycles though each platform
	for (int platform_id : platforms)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		platform.size = { 96, 16 };
		platform.canJumpThrough = true;
		platform.type = TYPE_PLATFORM;
	}

	platforms = Play::CollectGameObjectIDsByType(TYPE_LONGPLATFORM);

	//Cycles though each platform
	for (int platform_id : platforms)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		platform.size = { 1024, 32 };
		platform.type = TYPE_PLATFORM;
	}

	platforms = Play::CollectGameObjectIDsByType(TYPE_EXTRALONGPLATFORM);

	//Cycles though each platform
	for (int platform_id : platforms)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		platform.size = { 1536, 32 };
		platform.type = TYPE_PLATFORM;
	}

	platforms = Play::CollectGameObjectIDsByType(TYPE_ROOM_SIDE);

	//Cycles though each platform
	for (int platform_id : platforms)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		platform.size = { 32, 1536 };
		platform.type = TYPE_PLATFORM;
	}

	platforms = Play::CollectGameObjectIDsByType(TYPE_SPIKES);

	//Cycles though each platform
	for (int platform_id : platforms)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		platform.size = { 32, 32 };
	}

	platforms = Play::CollectGameObjectIDsByType(TYPE_SCALE_FROM_SPRITE);

	//Cycles though each platform
	for (int platform_id : platforms)
	{
		GameObject& platform = Play::GetGameObject(platform_id);
		spriteID = platform.spriteId;
		platform.size = { Play::GetSpriteWidth(spriteID) / 2, Play::GetSpriteHeight(spriteID) / 2 };
		platform.type = TYPE_PLATFORM;
	}
}

void SetupPickups()
{
	std::vector<int> pickups = Play::CollectGameObjectIDsByType(TYPE_COIN);
	int randNum = rand() % 6;
	int spriteID;


	//Cycles though each coin
	for (int pickup_id : pickups)
	{
		GameObject& coin = Play::GetGameObject(pickup_id);
		spriteID = Play::GetSpriteId("coin_A");
		if (coin.spriteId == spriteID)
		{
			randNum = rand() % 7;
			switch (randNum)
			{
			default: Play::SetSprite(coin, "coin_copper", 0.1f); coin.points = 200; break;
			case 2: case 3: case 4: Play::SetSprite(coin, "coin_silver", 0.1f); coin.points = 400; break;
			case 5: case 6: Play::SetSprite(coin, "coin_gold", 0.1f); coin.points = 600; break;
			}
		}
		else if (coin.spriteId == spriteID + 1) //copper
		{
			Play::SetSprite(coin, "coin_copper", 0.1f);
			coin.points = 200;
		}
		else if (coin.spriteId == spriteID + 2) //gold
		{
			Play::SetSprite(coin, "coin_gold", 0.1f);
			coin.points = 600;
		}
		else if (coin.spriteId == spriteID + 3) //silver
		{
			Play::SetSprite(coin, "coin_silver", 0.1f);
			coin.points = 400;
		}
	}

	pickups = Play::CollectGameObjectIDsByType(TYPE_FOOD);

	//Cycles though each food
	for (int i = 0; i < pickups.size(); i++)
	{
		GameObject& food = Play::GetGameObject(pickups[i]);
		spriteID = Play::GetSpriteId("food_apple");
		if (food.spriteId == spriteID) //apple
		{
			food.points = 2;
		}
		else if (food.spriteId == spriteID + 1) //chesse
		{
			food.points = 2;
		}
		else if (food.spriteId == spriteID + 2) //fish cooked
		{
			food.points = 5;
		}
		else if (food.spriteId == spriteID + 3) //fish raw
		{
			food.points = 3;
		}
		else if (food.spriteId == spriteID + 4) //fries
		{
			food.points = 4;
		}
		else if (food.spriteId == spriteID + 5) //chicken
		{
			food.points = 5;
		}
		else if (food.spriteId == spriteID + 6) //steak
		{
			food.points = 4;
		}
		else if (food.spriteId == spriteID + 7) //pizza
		{
			food.points = 3;
		}

		else if (food.spriteId == spriteID + 8)
		{
			randNum = rand() % 8;
			food.spriteId = spriteID + randNum;
			i -= 1;
		}
	}
}

void DrawHealthBar(GameObject& player)
{
	if (playerProperties.health > 4)
	{
		Play::DrawSprite("heart_fullB", Play::cameraPos + Vector2D{ 280, 30 }, 0);
		Play::DrawSprite("heart_fullS", player.pos - Vector2D{-40, 40}, 0);
	}
	else
	{
		Play::DrawSprite("heart_empty", Play::cameraPos + Vector2D{ 280, 30 }, 0);
	}
	if (playerProperties.health > 3)
	{
		Play::DrawSprite("heart_fullB", Play::cameraPos + Vector2D{ 240, 30 }, 0);
		Play::DrawSprite("heart_fullS", player.pos - Vector2D{-20, 40}, 0);
	}
	else
	{
		Play::DrawSprite("heart_empty", Play::cameraPos + Vector2D{ 240, 30 }, 0);
	}
	if (playerProperties.health > 2)
	{
		Play::DrawSprite("heart_fullB", Play::cameraPos + Vector2D{ 200, 30 }, 0);
		Play::DrawSprite("heart_fullS", player.pos - Vector2D{-0, 40}, 0);
	}
	else
	{
		Play::DrawSprite("heart_empty", Play::cameraPos + Vector2D{ 200, 30 }, 0);
	}
	if (playerProperties.health > 1)
	{
		Play::DrawSprite("heart_fullB", Play::cameraPos + Vector2D{ 160, 30 }, 0);
		Play::DrawSprite("heart_fullS", player.pos - Vector2D{20, 40}, 0);
	}
	else
	{
		Play::DrawSprite("heart_empty", Play::cameraPos + Vector2D{ 160, 30 }, 0);
	}
	if (playerProperties.health > 0)
	{
		Play::DrawSprite("heart_fullB", Play::cameraPos + Vector2D{ 120, 30 }, 0);
		Play::DrawSprite("heart_fullS", player.pos - Vector2D{40, 40}, 0);
	}
	else
	{
		Play::DrawSprite("heart_empty", Play::cameraPos + Vector2D{ 120, 30 }, 0);
	}
}

/// <summary>
/// Update the characters collision box size
/// </summary>
/// <param name="player">The player pointer</param>
void UpdateCharacterSize(GameObject* player)
{
	int player_sprite_id = player->spriteId;
	player->size = Vector2D((Play::GetSpriteWidth(player_sprite_id) / 2) - 2, (Play::GetSpriteHeight(player_sprite_id) / 2) - 2);
}
