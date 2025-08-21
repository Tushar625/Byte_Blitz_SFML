#pragma once

#include"../../system/input.h"


namespace bb
{


/*
	---------------------------------------
	Helper function to update a MENU object
	---------------------------------------

	updates a MENU<BUTTON_TYPE> object and returns the selected button
*/

template<typename MENU_TYPE>

int menu_update(MENU_TYPE& menu) noexcept
{
	auto mpos = bb::INPUT.pointer();

	return menu.Update(
		mpos.x,
		mpos.y,
		bb::INPUT.isPressedM(sf::Mouse::Left),
		bb::INPUT.isReleasedM(sf::Mouse::Left),
		bb::INPUT.isPressed(sf::Keyboard::Scan::Up),
		bb::INPUT.isPressed(sf::Keyboard::Scan::Down),
		bb::INPUT.isPressed(sf::Keyboard::Scan::Enter)
	);
}


}