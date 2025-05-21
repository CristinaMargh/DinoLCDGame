====== Dino LCD Game =======

====== Introduction ===== 
**Dino LCD Game** is an interactive game inspired by the famous Dino game in Chrome, implemented on a microcontroller and displayed on an LCD screen. The player controls a dinosaur that must avoid obstacles , using a joystick. The game also includes a continuous soundtrack, a start menu and a scoring system stored in EEPROM. The goal of the project is to apply the knowledge acquired in the labs in an educational way, resulting in a portable and fully functional game. In this way, the perception of the dinosaur, that can be a little annoying because it appears when I am offline, can be slightly changed.
</note>
====== General description =====
====== Block scheme ======
This block diagram highlights the essential components of the **Dino LCD Game** ​​project and how they work together to provide a complete gaming experience:

* **Arduino Uno Microcontroller** – is the central control unit that reads the inputs from the joystick and generates output for sound and display.

* **Joystick** – allows the user to control the dinosaur (e.g. jumping). It is connected to analog and digital pins and provides the main input.

* **Buzzer** – controls the background sound or provides audio feedback on certain events (jump, collision).

* **16x2 LCD Display** – graphically displays the game: the dinosaur, obstacles and score.

All these modules are connected to each other via Arduino, which manages the game logic, sound playback, graphical interface on the LCD, and user interaction via the joystick.

** Pin configurations **
   LCD - Arduino
   * GND-GND
   * VCC - 5V
   * SDA - A4
   * SCL - A5
   Joystick - Arduino
   * D2 - D1
   * A1 - A1
   * GND - GND
   * 5V - 5V
   * A3 - A0
===== Software Design =====
* Development environment: Arduino IDE 2.3.6

* Libraries : Wire.h(Used for I2C communication between Arduino and peripherals like the LCD display), LiquidCrystal_I2C(Extends LiquidCrystal to support I2C LCD screens, simplifying control and data handling)

Game flow: 
Start Game:
Display "Swipe left to Start Game" on the LCD screen.
Wait for the player to swipe the second joystick to the right to start the game.

Gameplay Loop:
Begin continuous game execution. Monitor joystick inputs for jump actions (left joystick) and start/restart actions (right joystick).

Game Mechanics:
Jump Action:
When the left joystick is pushed up, the dinosaur character performs a jump.
During a jump, obstacles are cleared based on the dinosaur's movement.

Scrolling World:
Every cycle, the game world scrolls to simulate movement.
Randomly generate obstacles (birds or cactus) on the top and bottom rows of the world.

Collision Detection:
Detect collisions between the dinosaur and obstacles.
Trigger game over conditions if collisions occur, accompanied by a buzzer sound and score display.

Game Over:
Upon collision detection:
Display "GAME OVER!" on the LCD screen along with the final score.
Wait for the player to press the right joystick to restart the game.
Reset the game world and score for a new game session.

Additional Features
Score Tracking:
Continuously update and display the player's score on the LCD screen.

Sound Feedback:
Use the buzzer to provide auditory feedback for jump actions and game over conditions.


===== Bibliography and materials =====
[[https://www.cirkitstudio.com/|Hardware design]]



