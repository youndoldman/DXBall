#pragma once

#include <SDL2/SDL.h>
#include <string>

#include "MenuItem.h"

struct ConfigItem : public MenuItem
{
	ConfigItem ( std::string name)
		:	MenuItem( name )
	{
	}

	ConfigItem() = delete;

	void SetValueTexture( SDL_Texture* texture )
	{
		valueTexture = texture;
	}
	void SetValueRect( SDL_Rect r )
	{
		valueRect = r;
		GeneratePlussMinus();
	}
	SDL_Texture* GetValueTexture( ) const
	{
		return valueTexture;
	}
	SDL_Rect GetValueRect( ) const
	{
		return valueRect;
	}
	const SDL_Rect* GetValueRectPtr( ) const
	{
		return &valueRect;
	}
	const SDL_Rect* GetPlussRectPtr( ) const
	{
		return &plussRect;
	}
	const SDL_Rect* GetMinusRectPtr( ) const
	{
		return &minusRect;
	}
	SDL_Rect GetPlussRect( ) const
	{
		return plussRect;
	}
	SDL_Rect GetMinusRect( ) const
	{
		return minusRect;
	}
	void GeneratePlussMinus()
	{
		SDL_Rect square = valueRect;
		square.w = 15;
		square.h = square.w;

		square.x += ( valueRect.w /2 ) - ( square.w / 2 );
		square.y -= square.h - 5;

		plussRect = square;

		minusRect = square;
		minusRect.y = valueRect.y + valueRect.h - 5;
	}
	void SetValue( uint32_t value_ )
	{
		if ( value == value_ )
			return;

		value = value_;
		hasChanged = true;
	}
	uint32_t GetValue()
	{
		hasChanged = false;
		return value;
	}
	bool HasChanged()
	{
		return hasChanged;
	}

	private:
	bool hasChanged;
	uint32_t value;
		SDL_Texture* valueTexture;
		SDL_Rect valueRect;
		SDL_Rect plussRect;
		SDL_Rect minusRect;
};

