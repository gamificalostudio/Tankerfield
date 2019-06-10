# Gerard Marcos Freixas. Role: Programmer

## Wiki contribution

As a programmer, I had to design and write the Technical Design Document. This document precisely explains all the topics about the technology used and code information in general. The TDD is divided by:

**1.** Introduction

**2.** Unified Modelling Language

**3.** Code style

**4.** Target hardware

**5.** Performance budget

**6.** Branching policy

**7.** Version list

**8.** External libraries

**9.** Build delivery method

Furthermore, I also took part in doing the Quality Assurance Plan Document, specifically:

**·** QA Introduction

**.** QA Bug properties

**.** QA Process for QC

## Game prototype

I also took part in the development of our videogame prototype. We used the Unity Game Engine to create this our prototype. I chelped with the tank shooting mechanics and its particle.

## Game implementation

In the first place, our project needed a base code structure to start developing our video game. We used my code, which had the basic functionalities of the engine, the modules (e.g. render module, window module, pathfinding module, scene module, and so on). From there, we have been implementing all the game features. Obviously, all the code that has been written followed the rules of the TDD.

More of my individual contributions to the project are explained below:

The initial pathfinding and walkability of the enemies and how they relate to the scene/map itself.

Basic enemies attack method or template.

A wave system which spawns the enemies over time. It was removed from the project, as we decided to implement enemy spawners and it was a lot more different that was my wave system.

The reward zones. It consists on a defined group of enemies that are positioned on a strategic place in the map and the player needs to kill them in order to get a prize. They could be loaded from Tiled, so it could be more comfortable and effective to create them. Again, it was eliminated from the project, because our game had to be a lot more dynamic.

Tesla trooper (enemy) attack animation.

Win condition.

Tesla trooper (enemy) idle state.

Brute (enemy) idle state.

Suicidal enemy implementation.

Rocket Launcher enemy implementation.

Item / Weapon UI particle when using a pickup.

Pause menu workflow collaboration.

Game installer.

General bugfixes, including:

Tutorial moving icon fix (it didn't disappear from the screen).

The tank could pick up items when was dead.

The healing shot caused a game crash.

The Rocket Launcher enemy could attack players who were eliminated.

The controllers submenu in the pause menu didn't work as expected, leaving a blank screen.

General changes, including:

Many tweaks on the enemies.

Tank moves slower when charging a shot.
