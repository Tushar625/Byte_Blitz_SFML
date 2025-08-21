#pragma once


#include <SFML/Graphics.hpp>

#include <vector>


namespace bb
{


/*
	this function slices a spritesheet texture into a vector of same sized sprites

	parameters:-

	texture: the spritesheet texture

	spriteWidth, spriteHeight: dimentions of the sprites

	textureArea: area of the sprite sheet from where we extract the sprites, by
	default it's an invalid data, which represents the entire texture or spritesheet

	note:-

	the spritesheet is divided into a grid of sprites based on the sprite dimentions
	the sprites of this grid are sequentially stored in a vector of sprites and returned
*/

std::vector<sf::Sprite> generateSpriteVector(const sf::Texture& texture, unsigned int spriteWidth, unsigned int spriteHeight, sf::IntRect textureArea = sf::IntRect(-1, -1, 0, 0))
{

	if(textureArea.top < 0 || textureArea.left < 0 || textureArea.width <= 0 || textureArea.height <= 0)
	{
		// textureArea contains invalid data so we need to initialize textureArea
		// texture area represents the entire texture

		textureArea = sf::IntRect(0, 0, texture.getSize().x, texture.getSize().y);
	}


	// number of sprite cells horizontally

	unsigned int gridWidth = textureArea.width / spriteWidth;

	// number of sprite cells vertically

	unsigned int gridHeight = textureArea.height / spriteHeight;

	//std::cout << "\nGrid size: " << gridWidth << ", " << gridHeight;


	/*
		here we store the "gridWidth x gridHeight" grid of sprites sequentially
		in a linear 1D vector
	*/

	std::vector<sf::Sprite> sprites(gridWidth * gridHeight, sf::Sprite(texture));


	// sampling the sprites

	unsigned int spriteCount = 0;

	unsigned int textureY = textureArea.top;

	for (unsigned int gridY = 0; gridY < gridHeight; gridY++)
	{
		unsigned int textureX = textureArea.left;

		for (unsigned int gridX = 0; gridX < gridWidth; gridX++)
		{
			sprites[spriteCount++].setTextureRect(sf::IntRect(textureX, textureY, spriteWidth, spriteHeight));

			//std::cout << "(" << textureX << ", " << textureY << ")";
			
			textureX += spriteWidth;
		}

		//std::cout << "\n";

		textureY += spriteHeight;
	}
    
	return sprites;
}


}	// namespace bb
