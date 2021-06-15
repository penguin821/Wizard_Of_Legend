#pragma once
enum SCENE { SCENE_LOGO, SCENE_STAGE, SCENE_OVER, SCENE_END };
enum DIR { DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT, DIR_END };
enum TYPE { TYPE_PLAYER, TYPE_WIZARD, TYPE_BOSS, TYPE_SWORD, TYPE_EFFECT, TYPE_ARCHER, TYPE_END };
enum STATUS { ST_IDLE, ST_SUMMON, ST_HIT, ST_MOVE, ST_MLEFT, ST_MRIGHT, ST_ATTACK, ST_DEATH, ST_END };
enum ELEMENT { EL_ICE, EL_FIRE, EL_WIND, EL_ICE_END, EL_FIRE_END, EL_END };
enum TIMER { TM_ANIMATION, TM_MOVE, TM_ATTACK, TM_PL_ATTACK, TM_END };
enum EFFECT_SOUND { EF_ICESHOOT, EF_ICEPOP, EF_FIRESHOOT, EF_FIREPOP, EF_PLAYERWALK, EF_PLAYERHIT,
	EF_MONSTERHIT, EF_MONSTERATTACK, EF_MONSTERMOVE, EF_END };
enum CHANNEL { CH_BACK, CH_MONSTER, CH_PLAYER, CH_PARTICLE, CH_END };
enum MAP { M_MAP1, M_MAP2, M_BOSS, M_END };

struct Character
{
	int posX;
	int posY;
	int sizeX; // 캐릭터 이미지 한칸 크기
	int sizeY;
	int animPosX;
	int animPosY;
	int ef_animPosX; // 이펙트용 애니메이션
	int ef_animPosY; // 이펙트용 애니메이션
	int ef_sizeX; // 이펙트용 이미지 한칸 크기
	int ef_sizeY;
	int hp;
	int moveSpeed;
	STATUS st;
	DIR dir;
	TYPE type;
	ELEMENT el;
};

struct Effect
{
	int posX;
	int posY;
	int endPosX;
	int endPosY;
	int sizeX; // 캐릭터 이미지 한칸 크기
	int sizeY;
	int animPosX;
	int animPosY;
	float mid;
	ELEMENT el;
};

struct MapTile
{
	RECT m;
	bool isObs;
};

struct img
{
	CImage Portal;
	int posX;
	int posY;
	int endPosX;
	int endPosY;
	int sizeX; // 캐릭터 이미지 한칸 크기
	int sizeY;
	
	float mid;
	ELEMENT el;
};