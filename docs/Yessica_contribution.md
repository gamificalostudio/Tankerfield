# Yessica Servin Dominguez
## Individual Contribution

 - **Extraction of 2D sprite from 3D models**
 
 ![enter image description here](https://user-images.githubusercontent.com/25900809/54059038-fd112e80-41f7-11e9-85d6-322caaa5f016.gif)
 - **Improvemnt of the module map**: 
	 - Added quadtree to the map render, and the sorting of tiles.
	 - Load colliders from Tile.
- **Implemented the controllers system:** Vibration, connect, detect controllers input.
- **General logic of enemies:**
They got diferents state so it was easer to understand the code, and also to detect bugs and also to add more states.
Some states are:
GET_PATH: In this state entities do the pathfinding to the nearest player.
MOVE: In this state enemies follow the path created.
BURN: When oil and fire weapons are combined, enemies enter in this state and start to run on fire.

Some times enemeis enter in a unwalkable tile so I made them to move out if this happends. 
	- Make that the rocketlauncher shoot in a sertence distance.

- **Pool of objects**  We got so many enemies that we needed this to not do so many *news*.
- **Reconfigurable keys** In the controllers option you can change de button you use in game, like the shoot button, and the helpers in game change. This information is saved when you close the game so it's ready to play again. 

### Other tasks
