#pragma once

#include "Animation.h"
#include "Assets.h"
#include <vector>
#include <memory>



class Component
{
public:
	bool has = false;
};


// Used in the level editor to separate editor values from entity values
class CEntityConfig : public Component
{
public:
	Vec2 pos = Vec2();
	float speed = 0.0f, sizeWidth = 64, sizeHeight = 64, damage = 0, health = 0, range = 0, pointX = 0, pointY = 0;
	bool vision = false, movement = false, boss = false, isRange = false, isMovingTile = false, isLightSource = false;
	std::string animation = "", ai = "", tag = "", attribute = "";
	int  roomX = 0, roomY = 0, tileX = 0, tileY = 0;
	std::vector<Vec2> patrol = {};

	CEntityConfig() {}
};

class CButtonToggle : public Component
{
	public:
		bool dummyData = false;
		bool* data = &dummyData;

		CButtonToggle() {}
		CButtonToggle(bool* d)
			: data(d) {}
		

};

class CButtonNumCycle : public Component
{
public:
	float max = 0;
	float* data = &max;
	

	CButtonNumCycle() {}
	CButtonNumCycle(float* d, float m)
		: data(d),  max(m) {}
};

class CButtonStringCycle : public Component
{
public:
	size_t index = 0;
	std::vector<std::string> options = {"NONE"};
	std::string* data = &options[0];
	

	CButtonStringCycle() {}
	CButtonStringCycle(std::string* d, std::vector<std::string> opts, size_t i)
		: data(d), options(opts), index(i) {}
};

class CButtonSpecial : public Component
{
public:

	CButtonSpecial() {}
};


class CTransform : public Component
{
public:
	Vec2 pos = { 0.0, 0.0 };
	Vec2 prevPos = { 0.0, 0.0 };
	Vec2 scale = { 1.0, 1.0 };
	Vec2 velocity = { 0.0, 0.0 };
	Vec2 facing = { 0.0, 1.0 };
	float angle = 0;

	CTransform() {}
	CTransform(const Vec2& p)
		: pos(p) {}
	CTransform(const Vec2& p, const Vec2& sp, const Vec2& sc, float a)
		: pos(p), prevPos(p), velocity(sp), scale(sc), angle(a) {}

};


class CPalletData :public Component
{
public:
	std::vector<std::string> animations;
	std::string defaultTag = "";
	int animationIndex = 0;

	CPalletData() {}
	CPalletData(const std::vector<std::string>& animes, const std::string& s, int animeI)
		: animations(animes), defaultTag(s), animationIndex(animeI) {}
};

class CLifeSpan : public Component
{
public:
	size_t lifespan = 0;
	size_t frameCreated = 0;
	CLifeSpan() {}
	CLifeSpan(size_t duration, size_t frame)
		: lifespan(duration), frameCreated(frame) {}
};

class CDamage : public Component
{
public:
	int damage = 1;
	CDamage() {}
	CDamage(int d)
		: damage(d) {}
};

class CInvincibility : public Component
{
public:
	size_t iframes = 0;
	CInvincibility() {}
	CInvincibility(size_t f)
		: iframes(f) {}
};

class CHealth : public Component
{
public:
	int max = 1;
	int current = 1;
	CHealth() {}
	CHealth(int m, int c)
		: max(m), current(c) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool attack = false;

	CInput() {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	bool blockMove = false;
	bool blockVision = false;
	CBoundingBox() {}
	CBoundingBox(const Vec2& s)
		: size(s), halfSize(s.x / 2, s.y / 2) {}
	CBoundingBox(const Vec2& s, bool m, bool v)
		: size(s), blockMove(m), blockVision(v), halfSize(s.x / 2, s.y / 2) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& animation, bool r)
		: animation(animation), repeat(r) {}
};

class CState : public Component
{
public:
	std::string state = "stand";
	CState() {}
	CState(const std::string& s) : state(s) {}
};

class CFollowPlayer : public Component
{
public:
	Vec2 home = { 0, 0 };
	float speed = 0;
	int currentPosition = 0;
	std::vector<Vec2> positions;
	bool hasSeenPlayer = false;
	bool pathCalculated = false;
	CFollowPlayer() {}
	CFollowPlayer(Vec2 p, float s)
		: home(p), speed(s) {}

};

class CPatrol : public Component
{
public:
	std::vector<Vec2> positions;
	size_t currentPosition = 0;
	float speed = 0;
	CPatrol() {}
	CPatrol(std::vector<Vec2>& pos, float s) : positions(pos), speed(s) {}
};

class CFollowPath : public Component
{
public:
	std::vector<Vec2> positions;
	size_t currentPosition = 0;
	CFollowPath() {}
	CFollowPath(std::vector<Vec2>& pos) : positions(pos) {}
};

class CLevelPath : public Component
{
public:
	std::string path;
	CLevelPath() {}
	CLevelPath(std::string s) : path(s) {}
};

class CRange : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	int range = 0;
	CRange() {}
	CRange(const Vec2& s, int r)
		: size(s* r), halfSize(s.x* r / 2, s.y* r / 2), range(r) {}
};

class CAttackCooldown : public Component
{
public:
	size_t cooldown = 0;
	size_t frameCreated = 0;
	CAttackCooldown() {}
	CAttackCooldown(size_t duration, size_t frame)
		: cooldown(duration), frameCreated(frame) {}
};

class CInventory : public Component
{
public:
	int arrows = 10;
	int healthPotions = 3;
	int poisonFlasks = 3;
	int strengthPotions = 3;
	int speedPotions = 3;
	CInventory() {}
	CInventory(int arrow, int hPotions) : arrows(arrow), healthPotions(hPotions) {}
	CInventory(int a, int h, int p, int st, int sp) : arrows(a), healthPotions(h), poisonFlasks(p), strengthPotions(st), speedPotions(sp) {}
};

class CPoisoned : public Component
{
public:
	int poisonDuration = 0;
	int poisonDamage = 0;
	int timeBetweenTicks = 0;
	int timeSinceLastTick = 0;
	CPoisoned() {}
	CPoisoned(int dur, int dam, int t) : poisonDuration(dur), poisonDamage(dam), timeBetweenTicks(t) {}
};

class CBuffs : public Component
{
public:

	// if entity deals poison damage on attack
	bool attacksPoison = false;
	int poisonCharges = 0;
	int poisonDamage = 0;
	int poisonRate = 60;
	int poisonDuration = 3;

	// attack buff
	bool attackBuffed = false;
	int attackBuffDuration = 0;
	int attackBuffStrength = 0;

	// speed buff
	bool speedBuffed = false;
	int speedBuffDuration = 0;
	int speedBuffStrength = 0;
	CBuffs() {}
	CBuffs(bool aP, int pC, int pDa, int pR, int pDu) : attacksPoison(aP), poisonCharges(pC), poisonDamage(pDa), poisonRate(pR), poisonDuration(pDu) {}
};

class CDying : public Component
{
public:

	float ticksUntilDeath = 20.0f;
	CDying() {}
	CDying(float t) : ticksUntilDeath(t) {}
};

class CBoss : public Component
{
public:

	bool alive = true;
	int levelBoss = 1;
	int healthBars = 0;
	bool gravityArrows = false;
	bool moveTileArrows = false;
	bool poisonArrows = false;
	CBoss() {}
	CBoss(int hpBars) : healthBars(hpBars) {}
};


class CSpecialArrow : public Component
{
public:

	bool spawnGravity = false;
	bool moveTile = false;
	bool poison = false;

	CSpecialArrow() {}
};

class CFalling : public Component
{
public:
	int timeFalling = 0;
	CFalling() {}
};

class CMimic : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	int range = 0;
	int hp = 1;
	Vec2 home = { 0, 0 };
	float speed = 0;
	CMimic() {}
	CMimic(const Vec2& s, int r, int h, Vec2 v, float sp)
		: size(s* r), halfSize(s.x* r / 2, s.y* r / 2), range(r), hp(h), home(v), speed(sp) {}
};

class CLight : public Component
{
public:

	float light = 20.0f;
	CLight() {}
	CLight(int t) : light(t) {}
};
