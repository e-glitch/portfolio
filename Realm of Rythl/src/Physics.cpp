#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// same overlap function as assignment 3
	if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>())
	{
		return Vec2(0, 0);
	}
	Vec2 aPos = a->getComponent<CTransform>().pos;
	Vec2 bPos = b->getComponent<CTransform>().pos;

	Vec2 aHalfSize = a->getComponent<CBoundingBox>().halfSize;
	Vec2 bHalfSize = b->getComponent<CBoundingBox>().halfSize;
	float axCoords[2] = { aPos.x - aHalfSize.x, aPos.x + aHalfSize.x };
	float ayCoords[2] = { aPos.y + aHalfSize.y, aPos.y - aHalfSize.y };
	float bxCoords[2] = { bPos.x - bHalfSize.x, bPos.x + bHalfSize.x };
	float byCoords[2] = { bPos.y + bHalfSize.y, bPos.y - bHalfSize.y };
	// reminder: SFML treats the top left as 0,0. Therefore, a value with a higher y value will be below one with a low value.
	float left = std::max(axCoords[0], bxCoords[0]);
	float right = std::min(axCoords[1], bxCoords[1]);
	float bottom = std::min(ayCoords[0], byCoords[0]);
	float top = std::max(ayCoords[1], byCoords[1]);
	// if overlap, find area of intersection. Adapted from https://stackoverflow.com/questions/4549544/total-area-of-intersecting-rectangles
	if (left < right && bottom > top)
	{
		return Vec2(right - left, bottom - top);
	}
	// no overlap. Return 0,0
	return Vec2(0, 0);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// same as above, but with previous position.
	if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>())
	{
		return Vec2(0, 0);
	}
	Vec2 aPos = a->getComponent<CTransform>().prevPos;
	Vec2 bPos = a->getComponent<CTransform>().prevPos;

	Vec2 aHalfSize = a->getComponent<CBoundingBox>().halfSize;
	Vec2 bHalfSize = b->getComponent<CBoundingBox>().halfSize;
	float axCoords[2] = { aPos.x - aHalfSize.x, aPos.x + aHalfSize.x };
	float ayCoords[2] = { aPos.y - aHalfSize.y, aPos.y + aHalfSize.y };
	float bxCoords[2] = { bPos.x - bHalfSize.x, aPos.x + bHalfSize.x };
	float byCoords[2] = { bPos.y - bHalfSize.y, aPos.y + bHalfSize.x };
	// reminder: SFML treats the top left as 0,0. Therefore, a value with a higher y value will be below one with a low value.
	float minLeft = std::max(axCoords[0], bxCoords[0]);
	float minRight = std::min(axCoords[1], bxCoords[1]);
	float minDown = std::min(ayCoords[0], byCoords[0]);
	float minUp = std::max(ayCoords[1], byCoords[1]);
	// if overlap, find area of intersection. Adapted from https://stackoverflow.com/questions/4549544/total-area-of-intersecting-rectangles
	if (minRight < minLeft && minDown < minUp)
	{
		return Vec2(minRight - minLeft, minUp - minDown);
	}
	// no overlap. Return 0,0
	return Vec2(0, 0);
}

bool Physics::IsInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
	auto left = e->getComponent<CTransform>().pos.x - e->getComponent<CBoundingBox>().halfSize.x;
	auto right = e->getComponent<CTransform>().pos.x + e->getComponent<CBoundingBox>().halfSize.x;
	auto top = e->getComponent<CTransform>().pos.y - e->getComponent<CBoundingBox>().halfSize.y;
	auto bottom = e->getComponent<CTransform>().pos.y + e->getComponent<CBoundingBox>().halfSize.y;
	if (pos.x > left && pos.x < right && pos.y > top && pos.y < bottom)
	{
		return true;
	}

	return false;
}

bool Physics::IsInRange(const Vec2& pos, std::shared_ptr<Entity> e)
{
	auto left = e->getComponent<CTransform>().pos.x - e->getComponent<CRange>().halfSize.x;
	auto right = e->getComponent<CTransform>().pos.x + e->getComponent<CRange>().halfSize.x;
	auto top = e->getComponent<CTransform>().pos.y - e->getComponent<CRange>().halfSize.y;
	auto bottom = e->getComponent<CTransform>().pos.y + e->getComponent<CRange>().halfSize.y;
	if (pos.x > left && pos.x < right && pos.y > top && pos.y < bottom)
	{
		return true;
	}

	return false;
}

bool Physics::IsInAggroRange(const Vec2& pos, std::shared_ptr<Entity> e)
{
	auto left = e->getComponent<CTransform>().pos.x - e->getComponent<CMimic>().halfSize.x;
	auto right = e->getComponent<CTransform>().pos.x + e->getComponent<CMimic>().halfSize.x;
	auto top = e->getComponent<CTransform>().pos.y - e->getComponent<CMimic>().halfSize.y;
	auto bottom = e->getComponent<CTransform>().pos.y + e->getComponent<CMimic>().halfSize.y;
	if (pos.x > left && pos.x < right && pos.y > top && pos.y < bottom)
	{
		return true;
	}

	return false;
}

Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
	// adapted from lecture
	Vec2 r = b - a;
	Vec2 s = d - c;
	Vec2 cMinusa = c - a;
	float rXs = r.x * s.y - r.y * s.x;
	float t = (cMinusa.x * s.y - cMinusa.y * s.x) / rXs;
	float u = (cMinusa.x * r.y - cMinusa.y * r.x) / rXs;
	// intersection. Return true, calculate intersection
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
	{
		return { true, Vec2(a.x + t * r.x, a.y + t * r.y) };
	}
	return { false, Vec2(0,0) };
}

bool Physics::EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
	// STUDENT TODO:
	// Implement this function
	//if the tile doesn't block vision, then return false.
	if (!e->hasComponent<CBoundingBox>() || !e->getComponent<CBoundingBox>().blockVision)
	{
		return false;
	}
	else
	{
		Vec2 pos = e->getComponent<CTransform>().pos;
		Vec2 halfSize = e->getComponent<CBoundingBox>().halfSize;
		// find all corners of bounding box, then make lines
		Vec2 topLeft = Vec2(pos.x - halfSize.x, pos.y - halfSize.y);
		Vec2 topRight = Vec2(pos.x + halfSize.x, pos.y - halfSize.y);
		Vec2 bottomLeft = Vec2(pos.x - halfSize.x, pos.y - halfSize.y);
		Vec2 bottomRight = Vec2(pos.x + halfSize.x, pos.y + halfSize.y);
		if (LineIntersect(a, b, topLeft, topRight).result || LineIntersect(a, b, topRight, bottomRight).result
			|| LineIntersect(a, b, bottomRight, bottomLeft).result || LineIntersect(a, b, topLeft, bottomLeft).result)
		{
			return true;
		}

	}

	return false;
}
