# Jaume Montagut Guix

![jaume.jpg](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/team_images/jaume.jpg?raw=true)

## Lead

My name is Jaume Montagut Guix and I'm the leader of Gamificalo Studio.

As a lead I focused myself on organizing the team so that the development progressed on the same direction and everything was delivered on time. I also focused on creating a non-tense enviroment where everyone could give their opinon and felt comfortable.

I did so by doing the following things:
- Being approachable for all team members.
- Talking to specific people when a problem ocurred.
- Changing our workflows when something wasn't working.
- Encouraging communication and help between team members.
- Comunicating with the manager to make sure tasks were properly distributed.
- Comunicating with other teams to:
	- Improve our processes based on the methods that were working (such as QA).
	- Distribute the work for extracting the sprites from the original game.
	- Helped organizing playtestings to find bugs and ensure the quality of all the projects.

![](https://raw.githubusercontent.com/gamificalostudio/Tankerfield/master/docs/Jaume_Contributions_docs/playtesting.png)

I also took as a responsibility to help anyone who was struggling with their tasks and the responsibilites of their role when they couldn't work on their area.

## Design

I worked closely with Victor, the designer to make sure the game was fun.

I especially insisted on making the game focus on cooperation and feeling.

### Prototype:

<iframe width="560" height="315" src="https://www.youtube.com/embed/Odw3HCuumKc" frameborder="0" allow="accelerometer; autoplay; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

When we firest presented the game to the publisher on our Concept Discovery, we developed a prototype with Yessica and Gerard.

My tasks on that project were:

- Defining and organizing the tasks that we'd present on our prototype.
- Creating an isometric enviroment in Unity.
- Importing the 3D model from the original game.
- Implement the movement of the tank.
- Implement the aiming and shooting of the tank. 
- Adding enemies with physics.
- Adding an explosion particle on the cannon of the tank.
- Adding a particle trail to the bullet.
- Adding skidmarks on the tank's tires.
- Adding kickback when the tank shoots.
- Added music and multple SFX.
- Fixing bugs.

The prototype helped the team understand what we wanted to focus on, and start with the right direction before even begging to code for the final game.

### Mechanics redesign

Before the Vertical Slice, we noticed that there were some problems with our initial designed and I redesigned some of the mechanics to focus on the things that made the game fun and solved those problems.

Some of the decisions included removing ammo and the button for reloading and spawning more powerful weapons each level, so players have to constantly be switching weapons and that makes them change try different weapons and combinations.

### Balance game progression

![balance.PNG](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jaume_Contributions_docs/balance.PNG?raw=true)

Our game becomes more difficult each round. We decided that the only two things which would have progression in the game would be the enemy life, the weapon damage and the number of enemies that spawn each round. This faciltated the balance of the game because we didn't have to control so many variables.

The intention with the design has been that you never die because there is an enemy with so much health you cannot kill, but because there are so many enemies you cannot kill them all. We want to make the player feel overwhelmed by the enemies, not underpowered and that he/she makes very little damage.

### Map design

I designed and iterated the map of the game, creating zones of tension (like the bridges) where you are very likely to get surrounded by enemies and more open zones. And palced the reward boxes to make the players explore those more dangerous areas.

![maps.png](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jaume_Contributions_docs/maps.png?raw=true)

The main challenge with designing the map has been to design a space which is open (it's isn't an interior) and makes you feel claustrophobic and like there are enemies around you all the time.

![real-map.PNG](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jaume_Contributions_docs/real-map.PNG?raw=true)

### Weapons

When we redesigned the mechanics of the game before the Vertical Slice assignment, we took a close look at the weapons we had designed. Together with Victor, we discarted some of the ones which didn't bring much to the experience we were trying to create and created some new ones that had a lot more focus on cooperation.

We also had to design an alternative shot (sustained or charged) for each one.

### Enemies

I participated on the brainstorming and selection process for the enemies we wanted to have in the final game.
I designed a few enemies before the meeting and then discussed them with the team to select the ones that caused a bigger impact on gameplay and were realistic to develop with the resources we had.

We ended up deciding on having the following enemies:
- The brute, which has a lot of life and damage but is very slow and can be easily baited with more than one player.
- The rocket launcher: which does damage from a distance, and is difficult to reach alone if there are other melee enemies around you.
- The autodestruct enemy which autodestructs itself almost killing you and therefore has to be focused and taken down fast.

### Items

While Victor was doing his research, I reworked the items we had to make them had a very clear purpose.
For example, I changed an item that gave you a speed boost to make it create a portal which teleported you to a random place in the map. This fufulled much better the purpose of escaping from enemies when you're in a tough spot.

## Code

I made sure to have a minimum understanding on how all the systems of the game worked, so that I could give advice on how to do a task so that it could merge nicely with the other systems that other members were implementing and would give us more flexibility on the code if we needed to change something in the future.

### Tank

#### Movement

I implemented the tank movement which was iterated multiple times.

At first the movement responded directly to the player's input in a 1:1 reaction.
But later we added some acceleration to make it feel more heavy, like if you were really controlling a war machine.

#### Shooting

I made the tank aim and shoot with controller input  and corrected the multiple bugs it generated.
I also created the bullet which the tank shoots.

#### Color change

After the Vertical Slice assignment I created a test repository to see if it was possible to change the tanks colors without causing a huge loss on performace. I found out it could be done quite easily and Alejandro created an amazing selection color scren.

#### Reviving

I helped Aitor implementing the mechanic that other tanks could revive each other when they were dead.

### Entity module

I made changes to the entity module (called M_ObjManager on our project) to reduce the things we had to do for each object.
- The object manager automatically gets the next frame of the current animation that the object is playing. This also proved useful when we implemented mulitple cameras and the speed didn't increase because of painting everything multiple times.
- Helped adding the sprite sorting and camera culling directly on the object module.

### Camera

#### Movement

I made the camera follow the player and slightly where it's aiming and used lerp to ease its movement.

### Shake

I implemented the camera shake with the help of [Jose Antonio Prieto](https://github.com/peterMcP) and the GDC video he found [https://www.youtube.com/watch?v=tu-Qe66AvtY](https://www.youtube.com/watch?v=tu-Qe66AvtY).

### Weapon structure

I created the structure that all weapon use.

It allows us to alter properties of the functions that are called when the weapon is shot.

I also added support for two types of weapon, charged, where releasing the button shoots a basic bullet and holding the button shoots a powerful shot and sustained, where releasing the button quickly shoots a basic bullet and holding the button creates a continuos stream of damage.

### Animation class

I reestructured the Animation class we've been using since the 'Project I' and 'Development' subjects to better suit the needs of the game.

The animation class now can have multiple directions for each animation and plays on of those depending on the angle the object has.

It supports any number of directions, which is useful because the tank base and turrent have 128 frames while the enemies only have 8.

It forced people to write the frames on an xml, so the number of hardcoded rects ended being 0.

### Flamethrower weapon & Particle system

I adapted Victor's Maso particle system [https://github.com/nintervik/2D-Particle-System](https://github.com/nintervik/2D-Particle-System) to our game by making it work in isometric and integrating it with the object manager module to sort the particles and work with the camera culling.
The particles can be completely configured using xml and have a variety of properties which have been very useful to get the feeling we wanted.

We used the particle system to improve the flamethrower animation, which didn't look like a flame.

## Management

During the development of the game I've taken part in the management process in the following ways:

- Calling out and restructuring tasks or removing them when we were falling behind.
- Communicating with the manager to make sure that we're on track.
- Taking an active role in meetings and asking for the progress of each member.

I also took the role of management while Jorge was doing his research and provided with a list of tasks that we wanted to do during the Alpha stage, selected the ones that had a bigger impact on the game and organized them.


## UI

### In-game UI design

Together with Alejandro and Victor, I took part on the brainstorming process to decide the design and placement of the UI.

Our objective with the UI has been to not distract the player but to provide useful information when it's needed.

The center is occupied by the most "central" element in the game, the one which defines the objective of the game, the round number,

The item (which is by default used with the left shoulder button) is placed on the left side of the screen.
The weapon (which is by default used with the right trigger) is placed on the right side of the screen,
Causing a natural mapping between the placement of the buttons on the controller you're holding and what you see on the screen.

They are in different on the top of their screen for the top row player and on the bottom of the screen for the bottom row players so that the UI doesn't get in the way of the game, but is there when you need it.

### New round UI animation

On our playtestings we found that people didn't notice the small red animation we had when we changed round.

I designed and developed an animation to focus the attention of the player to the center with 3 phases.

1. A lot of particles (simbolyiszing the enemies which have been killed) are randomly placed on the screen and start moving toward the center turning more opaque, making the player look in that direction.
2. The particles reach the center and the rhombus starts changing color to green and the new round number is revealed in the process.
3. Two orbs are emmited from the rhombus and move on the margin of the screens until they reach the players and heal all of them, as a reward for completing that round.

## QA

We noticed that most of the bugs we had were only being fixed if they made the game almost unplayable, so we redesigned the QA workflow so that issues were an active part on the management process and were assigned and prioritized properly, just as new tasks were.

## Contact me

[https://www.linkedin.com/in/jaume-montagut-guix-7389a4166/](https://www.linkedin.com/in/jaume-montagut-guix-7389a4166/)

[https://twitter.com/_wadoren](https://twitter.com/_wadoren)

jaume.montagut.i.guix@gmail.com

<-- Go back to web
