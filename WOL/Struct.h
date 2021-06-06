#pragma once
enum SCENE { SCENE_LOGO, SCENE_STAGE, SCENE_OVER, SCENE_END };
enum DIR { DIR_DOWN, DIR_UP, DIR_LEFT, DIR_RIGHT, DIR_END };
enum TYPE { TYPE_PLAYER, TYPE_WIZARD, TYPE_BOSS, TYPE_SWORD, TYPE_SWORD_ATTACK, TYPE_ARCHER, TYPE_END };
enum STATUS { ST_IDLE, ST_SUMMON, ST_HIT, ST_MOVE, ST_MLEFT, ST_MRIGHT, ST_ATTACK, ST_DEATH, ST_END };
enum TIMER {};

struct Character 
{
	int posX;
	int posY;
	int animPosX;
	int animPosY;
	int ef_animPosX; // 이펙트용 애니메이션
	int ef_animPosY; // 이펙트용 애니메이션
	STATUS st;
	DIR dir;
};