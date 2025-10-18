# DX-Ball Game (OpenGL / C++)

A classic **DX-Ball** clone built in **C++** using **OpenGL (GLUT)**.  
Break blocks, collect perks, and achieve the highest score!

---

## **Features**

- Classic brick-breaking gameplay
- Multiple **perks**:
  - Green Box → Extra Life
  - Red Box → Faster Ball
  - Blue Box → Wider Paddle
- **Particle effects** for block destruction
- Dynamic **ball speed increase**
- **Highscore tracking** saved in `highscore.txt`
- Pause, Menu, Help, and Highscore screens
- Mouse and keyboard controls

---

## **Controls**

| Action                        | Key / Input                    |
| ----------------------------- | ------------------------------ |
| Move Paddle                   | Mouse or LEFT/RIGHT Arrow Keys |
| Launch Ball                   | Click Mouse or SPACE           |
| Pause/Resume                  | P                              |
| Exit Game                     | ESC                            |
| Go Back (from Highscore/Help) | B                              |
| Highscore Menu                | H (from main menu)             |
| Help Menu                     | F (from main menu)             |

---

## **Installation / Running**

1. Make sure you have **OpenGL and GLUT** installed.
2. Compile the project:

```bash
g++ main.cpp -o DXBall -lGL -lGLU -lglut
./DXBall
```

## **Game Architecture**

- **Main Components:**

  - `Block` → Stores block position, color, hits, and perks
  - `PerkItem` → Stores falling perks
  - `Particle` → Stores particles for visual effects

- **Key Functions:**

  - `initBlocks()` → Initialize blocks with colors and perks
  - `drawPaddle()`, `drawBall()`, `drawBlocks()`, `drawPerks()`, `drawParticles()` → Rendering
  - `update()` → Game logic: move ball, check collisions, update perks and particles
  - `checkCollisions()` → Handles ball collisions with paddle, blocks, walls
  - `spawnPerk()` → Generates a new perk when a block is destroyed
  - `spawnParticles()` → Particle effect for block destruction
  - `resetBall()` / `resetGame()` → Resets ball or whole game

- **Game States:**

  - MENU, PLAYING, PAUSED, WON, LOST, HIGHSCORE, HELP

---

## **Gameplay Flow**

1. Start at **Menu** → Press SPACE to play
2. Move the paddle → Launch ball
3. Break all blocks to **win**
4. Collect perks for advantages
5. If ball falls → lose a life
6. Game over → Press SPACE to return to Menu

---

## **Highscore**

- Stored in `highscore.txt`
- Automatically updated if the player exceeds previous high score

---

## **Credits**

- Developed by: **Md. Atikul Islam Atik**
- Built with **C++** and **OpenGL / GLUT**

---

## **License**

This project is open-source and free to use for educational purposes.
