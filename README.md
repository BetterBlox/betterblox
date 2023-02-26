# betterblox

# Dependencies
- GLAD - links the OpenGL functions
- GLFW - Creates the window
- GLM - does the matrix algebra
- irrKlang - for the sounds


Betterblox is a Minecraft mockup implemented in c++. It currently supports placing blocks in the scene and auto spawning the world based on a perlin noise algorithm. 

## Block storage. 
The blocks are added to the scene by inserting them into an unordered map with the key being a unique position (not allowing duplicates) and the value being the block type. 
The block types are stored in an enum and corrispond to the textures. 

## World generation
This is weak at the moment and uses someone elses implementation of perlin noise. The noise is only used for the height of the ground but in the future it should also decide where different blocks and objects should be stored. 

## Optimization
The world generation needs to remove blocks that are outside of a specified range. Also it would be nice to find a way to merge adjecent blocks into one block as this would majorly save on performance. 

## Inventory
A little bit of the inventory system has been added. This includes a simple class that is not being used. The inventory should be rendered to the screen and display the amount. Also, it should restrict the user from being able to place more blocks that the user has. 

## Text 
I would like the text to be rendered from scratch using binary representations for each char. It could look something like this: 
```
##### 11111
#   # 10001
#   # 10001
##### 11111
```
This can be rendered as a 2D accelerated graphic. 

## Biomes
We need to implement a class that stores blocks and objects that belong in a specific biome. This will determine the ratio of blocks that need to go in a particular biome. For example you do not want as many dirt blocks on the surface as you want grass blocks but it still looks nice with a couple. 
