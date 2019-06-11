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

Another of the main contributions I have made to the project has been the implementation of the collision module. This module is intended to be similar to the operation of colliders and  Unity [2D physics](https://unity3d.com/es/learn/tutorials/s/physics).  During the development it has had many modifications in order to be as optimal as possible. The main features of this module are:

* Adapted to an isometric world
* Colliders Update , Add & Destroy
* OnTriggerEnter, OnTrigger & OnTriggerExit functions
* 2D Physics with Dynamic & Static Rigid Bodies
* Collisions QuadTree : Added after Vertical, this improve our debug mode ms
* Debug mode (QuadTree & Colliders Draw)

<iframe width="560" height="315" src="https://www.youtube.com/embed/fWRvSUPmsJo" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## Trailer & Gameplay Recording & Edition 


<iframe width="560" height="315" src="https://www.youtube.com/embed/QC2n7g_kISs" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
<iframe width="560" height="315" src="https://www.youtube.com/embed/TFSujlLxkXw" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>
<!--stackedit_data:
eyJoaXN0b3J5IjpbLTQ1MDM4NzgxMiw1NzUzNzc3ODMsLTE2Mz
E3NzkzMywyMDc0OTI0MzQ0LDE1MjYwNzA0NDFdfQ==
-->