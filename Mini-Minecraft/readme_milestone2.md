# **<p align="center">L-system Mini-Minecraft Log</p>**

## 11/22/2021 Milestone2 ##

## **Zhuohao Lin:**
### **Cave Sytems**

Features implemented:

1. Underground Caves
2. Postprocess rendering pipeline, postprocess effects in water and lava
3. Water and lava do not block player&#39;s movement, but will slow down the player.

Difficulties:

1. Reducing the player&#39;s velocity in water and lava.

Solution: There is no way to reduce the player&#39;s velocity directly in our implementation, so I have to reduce the player&#39;s acceleration to reach the goal.

2. There are too many steps to introduce a postprocess pipeline before I can check whether it works.

Solution: I reference the implementation of postprocess pipeline in hw4, split the work in several big steps, and make sure all small steps are correct by double checking the code.

## **Jiajun Li:**
### **Texturing and texture animation**

Features implemented:

1. Apply correct texture to blocks and add simple texture animations to water and lava.
2. Draw opaque and transparent blocks separately and perform additional visibility check to correctly cull faces within transparent blocks.
3. Add distance fog to make the scene look more realistic.
4. Add 2D shader and draw simple UI crosshair at the center of the screen.

Difficulties:

1. To correctly cull faces after adding multithreading.

Solution: The old method in milestone1 is to update chunks VBO after linking neighbors. But there is a problem after adding multithreading because we can not guarantee all blocktypes are set when we link chunks. To solve the problem, we can inform all neighbors to recreate VBO when we finish setting all blocktypes of a chunk.

2. To create a 2D shader.

Solution: 2D shader can be created by a flat shader whose projection matrix is set to identity matrix so that vertices can be drawn to screen directly. We also benefit from the convenience of this method because we can set the position directly in screen space.

## **Haoquan Liang:**
### **Multithreaded Terrain Generation**

Features implemented:

1. Added multithreading functionality with QThread to prevent the gameplay from slowing down when new terrain is generated to expand the world.

Difficulties:

1. To implement multithreaded terrain generation, we first need to know how terrain generation works. As someone that was only responsible for player physics last milestone, it is very challenging for me.

Solution: Read and understand my teammates&#39; code and ask them a lot of questions.

2. Lock or not to lock, that is a question.

Solution: Lock as many places as possible at first, and then narrow the range to see if it causes problems.

3. After merging, a lot of problems appeared. Crashing happens very often.

Solution: Use debugger to see where the crashing happened, and trace down the call stack. Usually, it&#39;s caused by a race condition.
