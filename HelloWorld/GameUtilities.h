#pragma once

struct DisplayInfo
{
	int DISPLAY_WIDTH = 1280;
	int DISPLAY_HEIGHT = 720;
	int DISPLAY_SCALE = 1;
};

enum ObjectTypes
{
	TYPE_PLAYER,
	TYPE_SCALE_FROM_SPRITE,
	TYPE_PLATFORM,
	TYPE_PLATFORMA,
	TYPE_PLATFORMB,
	TYPE_LONGPLATFORM,
	TYPE_EXTRALONGPLATFORM,
	TYPE_NO_COLLISION,
	TYPE_ROOM_SIDE,
	TYPE_DRONE,
	TYPE_FANATIC,
	TYPE_SCOUT,
	TYPE_HAMMER,
	TYPE_COIN,
	TYPE_SPIKES,
	TYPE_FOOD,
	TYPE_BULLET,
	TYPE_ORB,
	TYPE_DESTROYED,
	TYPE_ATTACK,
	TYPE_SHAPESHIFT,
	TYPE_CRATE,
	TYPE_SHOOTER,
	TYPE_BOSS,
	TYPE_NPC,
	TYPE_SPEECH,
};
enum AnimalState
{
	STATE_APPEAR = 0,
	STATE_HALT,
	STATE_MENU,
	STATE_PLAY,
	STATE_DEAD,
	STATE_ATTACKING,
	STATE_WON
};

struct GameState
{
	int score = 0;
	AnimalState animalState = STATE_MENU;
	float FPSMulti = 0;
	bool paused = false;
	float timer{ 0.f };
	float levelTime = 0;
};

struct Room
{
	Vector2D size;
	Vector2D pos;
	int left;
	int right;
	int top;
	int bottom;
	int roomLeft;
	int roomRight;
	int roomTop;
	int roomBottom;
	Room()
	{

	}
	Room(Vector2D newSize, Vector2D newPos)
	{
		size = newSize / 2;
		pos = newPos;
		left = pos.x - size.x;
		right = pos.x + size.x;
		bottom = pos.y + size.y;
		top = pos.y - size.y;
		roomLeft = 0;
		roomRight = 0;
		roomTop = 0;
		roomBottom = 0;
	}
};

enum PlayerCharacter
{
	TYPE_BIRD_A, TYPE_BIRD_B, TYPE_CAT_A, TYPE_CAT_B, TYPE_DOG_A, TYPE_DOG_B, TYPE_RAT_A, TYPE_RAT_B
};

struct AnimalForms
{
	bool isDogUnlocked = true;
	bool isCatUnlocked = false;
	bool isRatUnlocked = false;
	bool isBirdUnlocked = false;
};

struct PlayerProperties
{
	PlayerCharacter playerCharacter = TYPE_DOG_A;
	int maxHealth = 5;
	int health = maxHealth;
	bool doubleJump = false;
	bool hurt = false;
	short coinsCollected = 0;
	short levelCoins = 0;
	short tempCoins = 0;
	AnimalForms animal_forms;
	bool onPlatform = false;
	bool direction = false;
	int roomNum = 1;

};