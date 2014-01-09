#pragma once
#include "GamePiece.h"

#include "math/Vector2f.h"

#include "enums/Player.h"

struct SDL_Texture;
struct Bullet : GamePiece
{
	Bullet( int32_t objectID );

	//std::string GetName() const;

	void SetOwner( const Player &hitBy );
	Player GetOwner() const;

	void Update( double delta );

	void SetPosition( double x, double y );

	bool HasHitTile( const Rect &rectToHit ) const;
	private:
		Player owner;
};