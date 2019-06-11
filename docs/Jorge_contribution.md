# Jorge Gemas Herencia

## Role - Managment

Inside the group, I have taken the role as managment. As responsible for this role, my responsibilities have been the creation of an Excel in which to keep a record of the tasks performed by all members and the time both estimated and invested with their deviation.

You can access the Excel used during the process by clicking [here](https://docs.google.com/spreadsheets/d/113tm7NAg_KK83d1rDMhxiy8_wCKlocxPwiix-iR6Y-M/edit?usp=sharing)

![Excel](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jorge_Contributions_doc/Excel_image.png?raw=true)

In addition, I was also in charge of the internal organization of the group thanks to the [ClickUp](https://clickup.com/) tool, where I distributed to each member what tasks they had to perform and agreed on when they would be finished.

![ClickUp](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jorge_Contributions_doc/ClickUp_image.png?raw=true)

Another aspect of which I have done as managment, is the planning and development of the internal and constant meetings of the group, with the help of the leader, [Jaume Montagut](https://github.com/JaumeMontagut) who has helped me in the different fields mentioned above.

## Wiki contribution

In relation to the project wiki, I have mainly been in charge of the section of the [production plan](https://github.com/gamificalostudio/Tankerfield/wiki/Production-Plan), where it is explained how the project will be carried out with a first projection of the approximate future between more things related to the role of managment mainly.

I have also contributed help in other fields of the wiki, where it is most remarkable in the [analysis of the game section](https://github.com/gamificalostudio/Tankerfield/wiki/General-analysis-of-the-original-game).
 
## Game Implementations

During the course of the project, an important part of the time has been directed to tasks related to the internal management, both to the development and updating of the Excel, and to the supervision of tasks and their distribution and monitoring of the project. 

I have also implemented various aspects of the game and contributed to the solution of bugs or balance / testing, contributing ideas on improvements in the game.

These are the main implementations:

### First version of the basic shot of the player
One of my first important tasks within the project was the first version of the basic shot, which was later updated by the other members of the group.

### Splitscreen
One of the longest and most difficult tasks implemented by me. Because the game is a local multiplayer game, it was necessary to implement the splitscreen. 

To know more information about how to implement a splitscreen system like Tankerfield, I have done a research on splitscreen that you can visit in the following repository: [Split-screen](https://github.com/jorgegh2/Split-screen).

![](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jorge_Contributions_doc/Split_screen.jpg?raw=true)

### Creation and managment of pick ups and reward boxes
All the creation and management of pick ups, therefore, the items and weapons that appear in the game and the reward boxes, responsible for spawning pick ups in a certain way.

All this includes the interaction of the player and the pick ups and reward boxes.

![enter image description here](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jorge_Contributions_doc/Reward_bobex_and_pick_ups.gif?raw=true)

The particle after take the weapon was did by [Gerard Marcos](https://github.com/vsRushy).

### Improvement of the waves system
The system of waves implemented by the member of the group [Gerard Marcos](https://github.com/vsRushy), I have been commissioned to improve it, putting different states of the wave and the form of creation of enemies and their formulas, both the basic and special enemies and their progression.

### Improvement of laser weapon
Another implemented implementation is the improvement of the mechanics of the laser weapon, allowing the ability to cross enemy units, hitting them only once.

Later it was modified to fire lasers continuously.

### Interaction betwee c++ and tiled
I have added the capacity to read the position of buildings, the different spawners of both reward boxes, and reward zones and enemy spawners, from the "Tiled" program to the game code and correctly place everything in its place and in the correct way.

### Abstraction of the different classes of enemies to a general class father
Due to the continuous use of the same code copied in different enemies, it was decided to create a parent class with its structure and its state machine that would share all the enemies and work correctly for everyone. In this way, it would simplify and help the creation of new enemies.

### Feedback of the charged shot
Due to the observation that the players during the playtestings were not aware that the shot could be charged, I made adjustments to how the player receives the information of what is happening while the shot is being charged. 

This includes particles, sounds, screen shaking and command shake...

![](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jorge_Contributions_doc/Feedback_charged_shot.gif?raw=true)

### Item “Healt área”
In the project, except for the help in the other items, I took care of one in particular from start to the end called "Health area". But because the lack of time and the little impact of the item due to other items such as the "health bag" and weapons that fulfill the main function of the item, which is to cure, was discarded to focus efforts on other tasks relevant to the project.

### Weapon "Electro shot"
One of the most successful implementations by me was the development from start to the end of the "Electro shot" weapon.

The weapon consists of a shot in a cone-shaped area that fires rays at enemies within range. The innovation of this weapon, is that each shot, stun the enemies for a short period of time.

To simulate the cone reach, the use of several square colliders placed in a straight line towards the direction in which the player points is implemented, the collider being bigger depending on if it is further away from the player to simulate the reach of a cone.

The weapon, like all "charged" weapons, has two types of shot: basic and charged. The basic continues as explained above, and the charged differs in a greater range and in a greater damage plus an amplified time of stun to the enemies.

![](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jorge_Contributions_doc/Electro_shot_basic.gif?raw=true)

![](https://github.com/gamificalostudio/Tankerfield/blob/master/docs/Jorge_Contributions_doc/Electro_shot_charged.gif?raw=true)

For the visual part of the weapon, it is based on a single animation of a vertical beam of a few frames. The weapon is responsible for creating a lightning animation for each enemy hit, and the animation is rotated in the direction of the target enemy and is continuously scaled according to the position of the enemy, even if it moves to convey the feeling that the enemy tank is firing the lightning at each enemy.
