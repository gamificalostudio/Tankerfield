# UI - Alejandro Gamarra Niño
![enter image description here](https://raw.githubusercontent.com/gamificalostudio/Tankerfield/master/docs/Aurelio_Contributions_docs/aurelio_photo.jpg)
## Role
I present myself, my name is Alejandro Aurelio Gamarra Niño and I am responsible of the UI of our game Tankerfield and with my colleagues have made the development of this project possible. As the person in charge of the User Interface section, my main tasks have been:

* The supervision of the design of the UI.
* Design of the flow of menus, HUD and the different elements at the individual level.
* Organize and have control over the documents related to the UI.
* Intregration of the UI in the game code.
* Generate art

During the development of the game the UI has been evolving iteration after iteration always looking to be as intuitive and simple as possible to be able to adapt to the pillars of our game.

# Game Implementations

## Module UI / UI Manager
I implemented the UI module that serves as a factory and manager for all instances of UI elements.  This allows the creation of different types of elements, including:
* Images 
* Buttons
* Labels
* Bars
* Input Text
* Text Panels
* Tables 
* Sliders 

Other functions of the UI manager:
* Update Normal & In-Game Elements
* Add FXS (FadeOn, FadeOut, IntermitentFX)
* Color Modifications (like arrows)
* Draw Sort by hierarchy
* Pivots


## Atlas Creation & Modification 
![enter image description here](https://github.com/gamificalostudio/Tankerfield/blob/development/Tankerfield/Game/textures/ui/atlas.png?raw=true)

## Menus Flow 
![enter image description here](https://raw.githubusercontent.com/gamificalostudio/Tankerfield/master/docs/Aurelio_Contributions_docs/UI_FLOW.jpg)

## Module Collision / Collision Manager
Another of the main contributions I have made to the project has been the implementation of the collision module. This module is intended to be similar to the operation of colliders and  [Unity 2D physics](https://unity3d.com/es/learn/tutorials/s/physics). The main features of this module are:

* Colliders Update & Destroy
* OnTriggerEnter, OnTrigger & OnTriggerExit functions
* 2D Physics with Dynamic & Static Rigid Bodies
* Collisions QuadTree : Added after Vertical, this improve our debug mode ms
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTE1NTMxODA4NjksNTc1Mzc3NzgzLC0xNj
MxNzc5MzMsMjA3NDkyNDM0NCwxNTI2MDcwNDQxXX0=
-->