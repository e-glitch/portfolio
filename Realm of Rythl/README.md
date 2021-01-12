Welcome to the  Sky Islands of the Relm of Rythl! Floating sky islands are common here, so don't fall off! 
They are powered by mysterious purple orbs known as Gravity Wells.
Beware the evil Skull who is the Sky King, some say he has unlocked the power of the Gravity Wells. Best of Luck! 

Authors:
		Brandon Mertes 		201618386
		Erika James 		201602265
		Keegan Churchill 	201510260
		Mackenzie Barrett 	201519360
/////////////////////////////////////////////////////////////////////////////

Youtube videos:
		Trailer: 	https://youtu.be/8Ct1lsWfW3I
		Report: 	https://youtu.be/oP7iitUZ70w

Game Basics

Top Down Adventure , with platforming aspects. Navigate the map and either kill the Boss or Kill all enemies. 
The player dies if they walk off the tiles and fall, step on water and drown, or naturally if they run out of HP. 
The player has 3 lives and if they all run out they must start the level over.

THE BOSS

The Boss title is given to the highest hp enemy in a level if there is a enemy with hp greater than 14. 
The Boss can have multiple special attributes and abilities given to it to make it a more formidable foe: 
	-Arrows that turn tiles into gravity wells to suck the player in 
	-Arrows that cause tiles to fly off in random directions pushing the player with them 
	-Boss can apply poison status on his ranged and melee attacks 
	-Boss can have multiple health bars so that his health refills when the first is emptied


SPECIAL TILES

Gravity Wells: Pulls the player into there centre
Spikes: Deals 2 damage when walked over
Moving Tiles: A tile with CPatrol that adds it's movement to the player , carry the player on it
Light Sources: illumante a 3x3(64) square when the level is a dark level.


ITEMS (Arrows, Potions)

Arrows are used by our secondary weapon, the bow, which can be fired toward the mouse cursor with a left click.
If an arrow (either player or enemy) hits a wall, it can be picked up and used by the player.

The potions we have added are strength potions, speed potions, and poison flasks. 
Strength and speed potions are simple enough; for a limited time, the player does more damage or moves faster respectively. 
The poison flask makes the player's next 3 sword swings poison the enemies, applying a damage over time and a 'poisoned' shader.

STATUS EFFECTS

Most status effects are applied by the potions described above, but the player can also be poisoned by enemies.

In total, we have speed buffs, strength buffs, poisoned, and being able to poison enemies on your attacks.

SHADERS

When an entity is poisoned, they are given a green shader that varies in intensity. 
When an NPC would be killed, they are instead given a CDying component which adds a shader that increases the transparency 
and fades the sprite to white over time. After the shader reaches full transparency, the entity is destroyed.


LIGHTING SYSTEM

It's a black screen mask that has holes in the appropriate places.

/////////////////////////////////////////////////////////////////////////////

CAMPAIGN SYSTEM 

The overworld is level-based with a similar control scheme to games such as Super Mario World. 
The player starts at their home town and has access to the first level. Upon beating this level, 
they will unlock access to the next level. This is saved in the LevelsCompleted.txt file. 
If you would like to reset progress, simply replace the number in the file with 0. If you would like to skip levels, 
increase the number. For example, to unlock the third level, put 2 in the file.



LEVEL 1

Showcase level, Boss in the top left. Defeat it to win the game. This Boss' arrows move tiles. 

LEVEL 2

Demonstrates that the player can fall off the map. Deal with archers while being pursued by flaming
skulls. Kill all enemies

LEVEL 3

Very Difficult. 2 ways to reach the boss. 1 trial by combat , defeat the monsters and archer wall (by reusing there own arrows against them) .
defeat them with a enough life to walk through the spikes and still kill the boss without falling from the Sky King's cloud castle. 
Way 2. Find the somewhat secret path down the water fall. Ride the nimbus cloud to the platformer stage. Navigate the difficult platformer 
to face the boss. 

/////////////////////////////////////////////////////////////////////////////

SETTINGS AND OPTIONS


/////////////////////////////////////////////////////////////////////////////

LEVEL EDITOR

Entity CATEGORIES

Landscape : Land, Water

Decorations: roads, and Decs

Physical: Tiles, NPCS, SPECIAL, ITEMS, PLAYER. 

A grid cell on the level can only hold one of each Entity Category.

Note: When editing entity components only appropiate data will be used. 

For Example: If the user adds patrol points to an Entity That isn't a MovingTile or is a NPC with PATROL AI, those points are ignored

Left Click : 	-Increments /Cycles up component attributes upward 			   
	     	when clicking buttons

		-Selects Entity By clicking on the Pallet
		
		-Copies an Entity of same Entity Category if clicked 		  
		on a grid cell with an occupying entity of the same category

		-Places Entity if there is no conflicting entity of
 		the same category present.
	
		-Adds Patrol point to the patrol list (when clicking button beside patrol points attribute)


Right Click : 	-Decrements /Cycles down component attributes when clicking buttons

		-Completely clears a grid cell of all entities

		-Removes Patrol point to the patrol list (when clicking button beside patrol points attribute)


Scroll	:    When a entity is selected from the pallet you can scroll through all possible Animations for that entity. 

G: Toggle Grid drawing 

C: Toggle drawing Collision box and Patrol points (Extremely useful to toggle off Grid and toggle on Collisons when customizing 
   Bounding boxes and Patrol points)

the dark boolean decides of a level is dark

Note: Gravity Well tiles will always attract the player

Spikes will always deal damage. 

(Due to Game logic) The Boss component is given to the NPC with 15 or more HP . Prioritizing the NPC with highets HP. 

no NPC has 15 plus HP then that level has no boss you must clear all enemies.

Component editing has ranges has a confined range of values to prevent user from making illegal levels. 
Patrol points are confined to a Room area and are expressed by Tile Coordinates relative to the room the entity is in.
Press the plus button to add the current value of point X and point Y to the patrol component list. 

/////////////////////////////////////////////////////////////////////////////

EXTRA FEATURES: 

Player can fall off the map , player can drown.

Unique Level Types, dark levels, and defeat boss or kill all NPCS level objectives.

Retrivable arrows, extra status affects, several unique Boss attacks.

Only winnable levels can be played from the level editor or custom level screen.

Efficient level editor cannot stack redundant entities, Can copy already existing entities in the level editor. 
Impossible for user to build 'Code Breaking' Levels.

Mimic AI looks like a regualr Tile with no health or Animation until the player walks beside it. Then it ambushes. 

/////////////////////////////////////////////////////////////////////////////

Missed Features and explainations:







