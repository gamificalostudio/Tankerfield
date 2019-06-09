# Víctor Segura - Designer

## Role

 As a Designer, I collaborated with the Leader, [Jaume Montagut](https://github.com/JaumeMontagut), in order to design and decide all the aspects in the game. At the beginning, we decided with the group the main idea of the game and decided the Game Pillars we were going to use.

We created, having in consideration the ideas of the members of the group, and designed how the weapons, enemies and items in the game would work.

We had one or two point during the development where we had to redesign some important aspects of the game to make it more accessible, assure the game was going to work better or adapting it to the publisher expectations.

## Wiki contribution

* [Game Design Document](https://github.com/gamificalostudio/Tankerfield/wiki/Game-Design-Document)
	* I made the main part of the GDD, writing down the first ideas, mechanics, weapons and enemies that decided with the team.
 
# Game Implementations

## Object Manager
The first task I had in the game code was adding the Object Manager. I took the Entity Manager of my previous project [Alien Earth](https://github.com/VictorSegura99/Alien_Earth) and adapted it to work with STL containers.

## Enemies

### First Tesla Trooper Version
### Brute

## Weapons
 
### Double Missile 

This shot creates a bullet that when it collides, it creates an Object Explosiod the bullet removes itself. This Object Explosion creates a collider during one frame and then deleted, and when the animation ends, the object removes itself.

The Normal Shot creates 2 bullets and the charged Shot creates 4.

**Normal Shot:**

<iframe width="560" height="315" src="https://www.youtube.com/embed/W6fsnRjkULc" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

**Charged Shot:**

<iframe width="560" height="315" src="https://www.youtube.com/embed/Axyet1AsBhY" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

### Healing Shot
### Weapon Oil

## Items
### Happy Hour
Happy Hour is an item that reduces the fire cadency of the player at a quarter of the original.

### Instant Help
Instant Help is an item that creates a portal in front of the player casting the item and another portal in a random place of the walkability map. The portals are managed and linked through the item. 

When a player enters the portal, they get teleported to the other one.

<iframe width="1280" height="720" src="https://www.youtube.com/embed/2dE1WgqqKh4?rel=0" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

## UI
### Credits Menu
![Credits Menu](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/V%C3%ADctor_Contributions_docs/Credits%20Menu.jpg?raw=true)
The credits menu direct you directly to the Githubs and LinkedIns of all members, as well as the website of the game and the Github Repository.

### Options Menu
![Options Menu](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/V%C3%ADctor_Contributions_docs/Options%20Menu.jpg?raw=true)
For the Options Menu, I added the options you see in the image.

The Fullscreen change a bool in the Window Module that active or desactive the fullscreen mode.

Master Volume changes a float with base of 1 that is multiplied with the music volume and with every sfx volume.

The Music Volume and SFX volume are ints that modify the volume, after multiplying themselves with the master volume.

## Video Player

I added a VideoPlayer to display the Logo Intro at the beginning of the game. I used the [Personal Research](https://github.com/AxelAlavedra/Video-Player-Research) of my classmate [Axel Alavedra](https://github.com/AxelAlavedra), which was a great help and worked great.

I made a little modification because there was a bug that printed a frame green just before reproducing the video.
﻿
