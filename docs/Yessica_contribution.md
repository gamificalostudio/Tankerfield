# Yessica Servin Dominguez
## Individual Contribution

 - **Sprite extraction(Original sprites and Extraction of sprite from 3D models)**
 
I searched for how extract sprites from the files of the original game and searched and extracted from 3d models of the original game.
 
 ![enter image description here](https://user-images.githubusercontent.com/25900809/54059038-fd112e80-41f7-11e9-85d6-322caaa5f016.gif)
 - **Improved the module map**: 
	 - Added quadtree to the map render, and the sorting of tiles.
	 - Load colliders from Tile.
- **Implemented the controllers system:** Vibration, connect, detect controllers input.
- **General logic of enemies:**
They got diferents state so it was easer to understand the code, and also to detect bugs and also to add more states.
Some states are:

- GET_PATH: In this state entities do the pathfinding to the nearest player.
- MOVE: In this state enemies follow the path created.
- BURN: When oil and fire weapons are combined, enemies enter in this state and start to run on fire.
- TELEPORT: When enemies are for away of player they teleport near the players.

Some times enemeis enter in a unwalkable tile so I made them to move out if this happends. 

- **Pool of objects**  We got so many enemies that we needed this to not do so many *news*.
- **Reconfigurable keys** In the controllers option you can change de button you use in game, like the shoot button, and the helpers in game change. This information is saved when you close the game so it's ready to play again. 
 ![enter image description here](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/YessicaSD_Contributions_docs/controller_settings.png?raw=true)
### Other tasks
- Make that the rocketlauncher shoot in a sertence distance.
- Problem solving
