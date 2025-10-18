
---

# **DX-Ball Game – Function Notes**

---

## **1️⃣ `init()`**

- **Purpose:** Initializes OpenGL settings, random seed, and game objects.
- **What it does:**

  - Sets the background color.
  - Sets 2D orthographic projection.
  - Seeds the random number generator (`srand(time(NULL))`).
  - Calls `initBlocks()` to set up the block layout.
  - Calls `loadHighScore()` to read high scores from file.

- **Called:** Once in `main()` before the game loop.
- **Flow position:** Game startup / menu preparation.

---

## **2️⃣ `initBlocks()`**

- **Purpose:** Creates all the blocks (bricks) for the current level.
- **What it does:**

  - Clears existing blocks (`blocks.clear()`).
  - Computes positions for all blocks based on row/column.
  - Sets size, active status, color, and hits.
  - Randomly assigns perks to some blocks (`EXTRA_LIFE`, `FASTER_BALL`, `WIDER_PADDLE`, or `NO_PERK`).
  - Adds them to the global `blocks` vector.

- **Called:**

  - From `init()` at game start.
  - From `resetGame()` when restarting the game.

- **Flow position:** Sets up game level before gameplay.

---

## **3️⃣ `drawText(float x, float y, const char* text)`**

- **Purpose:** Draws text on the screen.
- **What it does:** Uses `glutBitmapCharacter()` to render letters at `(x, y)`.
- **Called:** From various functions:

  - `drawHUD()`
  - `drawMenu()`
  - `drawHighScore()`
  - `drawHelp()`
  - Game end messages (WON, LOST)

- **Flow position:** Rendering helper.

---

## **4️⃣ `drawPaddle()`**

- **Purpose:** Draws the player paddle.
- **What it does:**

  - Draws a rectangle at `(paddleX, paddleY)` with `paddleWidth`.
  - Sets paddle color.

- **Called:** Every frame from `display()` when `gameState` is `PLAYING` or `PAUSED`.

---

## **5️⃣ `drawBall()`**

- **Purpose:** Draws the ball.
- **What it does:**

  - Draws a circle at `(ballX, ballY)` using `GL_TRIANGLE_FAN`.
  - Sets ball color.

- **Called:** Every frame from `display()` when `gameState` is `PLAYING` or `PAUSED`.

---

## **6️⃣ `drawBlocks()`**

- **Purpose:** Draws all active blocks.
- **What it does:**

  - Loops through `blocks` vector.
  - Checks `active` status.
  - Draws rectangle using `GL_QUADS`.
  - Optionally draws block borders (outline).

- **Called:** Every frame from `display()` when `gameState` is `PLAYING` or `PAUSED`.

---

## **7️⃣ `drawPerks()`**

- **Purpose:** Draws active perk items (falling power-ups).
- **What it does:**

  - Loops through `perks` vector.
  - Checks `active` status.
  - Draws small colored squares representing the perk.

- **Called:** Every frame from `display()` when `gameState` is `PLAYING` or `PAUSED`.

---

## **8️⃣ `drawParticles()`**

- **Purpose:** Draws explosion/spark particles.
- **What it does:**

  - Loops through `particles` vector.
  - Checks `active` status.
  - Draws points at `(x, y)` with color `(r, g, b)` and fading (`life`).

- **Called:** Every frame from `display()` when `gameState` is `PLAYING` or `PAUSED`.

---

## **9️⃣ `drawHUD()`**

- **Purpose:** Draws game information (lives, score, time, ball speed).
- **What it does:**

  - Draws `Lives`, `Score`, `Time`, and `Ball Speed` using `drawText()`.

- **Called:** Every frame from `display()` when `gameState` is `PLAYING` or `PAUSED`.

---

## **🔟 `drawMenu()`**

- **Purpose:** Draws main menu.
- **What it does:**

  - Shows game title, instructions, and navigation hints (SPACE, ESC, H, F).

- **Called:** From `display()` when `gameState == MENU`.

---

## **1️⃣1️⃣ `drawHighScore()`**

- **Purpose:** Shows high score screen.
- **What it does:**

  - Displays the highest score.
  - Displays instruction to go back (`B` key).

- **Called:** From `display()` when `gameState == HIGHSCORE`.

---

## **1️⃣2️⃣ `drawHelp()`**

- **Purpose:** Shows help/instructions screen.
- **What it does:**

  - Displays gameplay goal, controls, and perks explanation.
  - Instruction to go back (`B` key).

- **Called:** From `display()` when `gameState == HELP`.

---

## **1️⃣3️⃣ `spawnPerk(float x, float y)`**

- **Purpose:** Creates a falling perk item.
- **What it does:**

  - Creates a `PerkItem`.
  - Sets position `(x, y)`.
  - Sets downward velocity (`vy = -2`).
  - Randomly assigns perk type (`EXTRA_LIFE`, `FASTER_BALL`, `WIDER_PADDLE`) and color.
  - Adds it to `perks` vector.

- **Called:** Inside `checkCollisions()` when a block with a perk is destroyed.

---

## **1️⃣4️⃣ `spawnParticles(float x, float y, float r, float g, float b)`**

- **Purpose:** Creates a visual explosion when a block is destroyed.
- **What it does:**

  - Generates ~20 particles at `(x, y)` with random velocities.
  - Sets color `(r, g, b)` based on the block.
  - Adds them to `particles` vector.

- **Called:** Inside `checkCollisions()` when a block is destroyed.

---

## **1️⃣5️⃣ `updateParticles()`**

- **Purpose:** Updates particle positions and life for animation.
- **What it does:**

  - Moves particles based on `vx, vy`.
  - Applies gravity (`vy -= 0.05`).
  - Fades particles (`life -= 0.02`).
  - Deactivates particle when `life <= 0`.

- **Called:** Every frame from `update()` when `gameState == PLAYING`.

---

## **1️⃣6️⃣ `updatePerks()`**

- **Purpose:** Updates falling perks.
- **What it does:**

  - Moves each active perk down (`y += vy`).
  - Checks for collision with paddle:

    - Applies effect (`EXTRA_LIFE`, `FASTER_BALL`, `WIDER_PADDLE`).
    - Deactivates perk.

  - Deactivates perk if off-screen.

- **Called:** Every frame from `update()` when `gameState == PLAYING`.

---

## **1️⃣7️⃣ `checkCollisions()`**

- **Purpose:** Handles all collision logic.
- **What it does:**

  - Ball ↔ walls → reverse velocity.
  - Ball ↔ paddle → bounce and adjust angle.
  - Ball ↔ blocks → destroy block, spawn particles/perks, increase score, adjust ball speed.
  - Ball fell below screen → reduce lives, reset ball, or end game.
  - Win condition → all blocks destroyed → `gameState = WON`.

- **Called:** Every frame from `update()` when ball is launched.

---

## **1️⃣8️⃣ `resetBall()`**

- **Purpose:** Puts the ball back on paddle.
- **What it does:**

  - Resets position to above paddle.
  - Resets velocity to `BASE_BALL_SPEED`.
  - Marks `ballLaunched = false`.

- **Called:**

  - When player loses a life but has remaining lives.
  - From `resetGame()`.

---

## **1️⃣9️⃣ `resetGame()`**

- **Purpose:** Resets the whole game state.
- **What it does:**

  - Sets lives, score, gameTime, ballSpeed, paddleWidth.
  - Clears perks.
  - Calls `initBlocks()` to rebuild the blocks.
  - Calls `resetBall()`.

- **Called:**

  - When game starts from menu (`SPACE` key).
  - Optionally when restarting after WON/LOST.

---

## **2️⃣0️⃣ `update(int value)`**

- **Purpose:** Main game logic updater (runs every frame).
- **What it does:**

  - Moves ball if launched.
  - Normalizes ball speed.
  - Calls `checkCollisions()`.
  - Calls `updatePerks()` and `updateParticles()`.
  - Updates `gameTime`.
  - Requests next frame via `glutTimerFunc(16, update, 0)` → ~60 FPS.

- **Called:** First time from `main()` via:

  ```cpp
  glutTimerFunc(0, update, 0);
  ```

  Then recursively schedules itself every 16 ms.

---

## **2️⃣1️⃣ `keyboard(unsigned char key, int x, int y)`**

- **Purpose:** Handles normal key input.
- **What it does:**

  - ESC → exit.
  - SPACE → start game / launch ball / back to menu.
  - P → pause/resume.
  - H → high score menu.
  - F → help menu.
  - B → back to main menu.

- **Called:** Automatically by GLUT when a key is pressed.

---

## **2️⃣2️⃣ `specialKeyboard(int key, int x, int y)`**

- **Purpose:** Handles special keys (arrow keys).
- **What it does:**

  - LEFT → move paddle left.
  - RIGHT → move paddle right.

- **Called:** Automatically by GLUT when a special key is pressed.
