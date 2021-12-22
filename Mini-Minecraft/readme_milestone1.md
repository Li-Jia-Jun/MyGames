# **<p align="center">L-system Mini-Minecraft Log</p>**

## 12/11/2021 Milestone1 ##

## **Zhuohao Lin:**
### **Procedural Terrain**
Features implemented: 

1. Grassland biome above Y = 129: Grass block at the top, Dirt blocks below the Grass block. Water will fill in the Empty blocks between Y = 129 and Y = 138.
2. Mountain biome above Y = 129: Snow block at the top, Stone blocks below the Snow block. Water will fill in the Empty blocks between Y = 129 and Y = 138.
3. Blending height between 2 biomes.
4. Stone layer from Y = 0 to Y = 128.
5. Generating biomes based on a temperature model.

Difficulties:

1. Making grassland look more random

Solution: I use Worley noise for grassland terrain, and apply Fractional Brownian Motion to warp the Worley noise.

2. Create smooth and steep slope for mountain biome

Solution: I use 1 – abs(Perlin) for mountain biome, so that there will sharp edges at the top of mountains. Then I use FBM to further modify the output of 1 – abs(Perlin) and get a desired output.

3. Blending height between 2 biomes

Solution: I introduce a temperature variable for generating biomes and blending heights. The temperature is generated using Perlin noise, so it has a continuous property. Because of its continuity, I can linearly interpolate the heights of 2 biomes based on the temperature and obtain a blended height.

## **Jiajun Li:** 

### **Efficient Terrain Rendering and Chunking**

Features implemented:  

1. Make Chunk class derive from Drawable so that each time the program will draw a chunk rather than a cube.
2. Combine vertex positions, colors and normals into one interleaved VBO data of for chunks to improve data transfer efficiency.
3. To increase rendering efficiency, perform visibility check when constructing chunk’s VBO data so that faces that cannot be seen by player will not be added to VBO.
4. Terrain updates when player changes position. The program will only draw chunks around the player.

Difficulties: 

1. To decide wthether a face can be seen by player. 

Solution: Check if the face is next to an empty block. The next block can be in the same chunk 	with face or a neighboring chunk, so both situations need to be checked.

2. To Access each data piece in an interleaved VBO data.

Solution: Make use of stride and pointer parameters of glVertexAttribPointer. For the case of 	this program, VBO data consists of positions, colors and normals, so stride is set to 3 vec4 size. 	And pointer for each attribute is set to 0, sizeof(vec4), 2\*sizeof(vec4).

3. To get each vertex position without writing lots of if statements.

Solution: use std::map to store vertex offsets of each face, indexed by direction. So we can loop 	over the map and get each vertex positions of each direction.

## **Haoquan Liang:**
### **Game Engine Tick Function and Player Physics**
Features implemented: 

1. Apply player’s transformations by analyzing the InputBundle at every clock tick.
1. Enable the player to switch between Flight Mode and Regular Mode. In Regular Mode, gravity will be applied and volume collision will be detected, whereas in Flight Mode player can move freely without being subject to the laws of physics.
1. Player movement will be subject to acceleration and friction, so the velocity will increase as a function of time and approaches a constant due to friction. 
1. Collision will be detected and prevented while maintaining a smooth movement (so the player won’t be completely stopped).
1. The player has the ability to add and remove a block at the center of the screen and within 3 units of distance. 

Difficulties:

1. Toggle flight mode ON and OFF while dealing with changes in physics. 

Solution: Use an autoRepeat() check and a keyReleaseEvent() to make sure that it can only be 	triggered once every key-press-release cycle. Check the mode before apply any other physics.

2. Collision detection in general is hard to implement.

Solution: Cast rays from every corner of the player’s bounding box and store the axes of 		collisions. Then, set the displacement on the collision axes to the distance between the 		player and the collision point with a small margin. I found that without a small margin, player 	will still collide with other grids.

3. Make sure that if the player is in Flight Mode and in the air when the Flight Mode is turned off, the player can naturally drop to the ground. 

Solution: Add a flag for “on ground” and use gridMarch() to check the player’s collision with 	ground, but only for the y-axis. 

4. Adding new blocks is a little complicated

Solution: we need to use gridMarch to determine whether we can place a new block there. The 	idea is similar to how we implement collision detection.


