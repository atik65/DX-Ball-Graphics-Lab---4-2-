#include <GL/glut.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>

// Game constants
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define PADDLE_WIDTH 100
#define PADDLE_HEIGHT 15
#define BALL_RADIUS 8
#define BLOCK_WIDTH 70
#define BLOCK_HEIGHT 25
#define BLOCKS_PER_ROW 11
#define BLOCK_ROWS 8
#define INITIAL_LIVES 3
#define BASE_BALL_SPEED 5.0f

enum GameState {
    MENU,
    PLAYING,
    PAUSED,
    WON,
    LOST,
    HIGHSCORE,
    HELP
};


// Perk types
enum PerkType {
    EXTRA_LIFE,
    FASTER_BALL,
    WIDER_PADDLE,
    NO_PERK
};

// Block structure
typedef struct {
    float x, y;
    float width, height;
    bool active;
    int hits;
    float r, g, b;
    PerkType perk;
} Block;

// Perk item structure
typedef struct {
    float x, y;
    float vx, vy;
    PerkType type;
    bool active;
    float r, g, b;
} PerkItem;


// Particle structure
typedef struct {
    float x, y;      // position
    float vx, vy;    // velocity
    float r, g, b;   // color
    float life;      // remaining life (1.0 → 0.0)
    bool active;     // is particle alive
} Particle;


// Game state variables
GameState gameState = MENU;
float paddleX = WINDOW_WIDTH / 2 - PADDLE_WIDTH / 2;
float paddleY = 50;
float paddleWidth = PADDLE_WIDTH;
float ballX = WINDOW_WIDTH / 2;
float ballY = paddleY + PADDLE_HEIGHT + BALL_RADIUS;
float ballVX = BASE_BALL_SPEED;
float ballVY = BASE_BALL_SPEED;
float ballSpeed = BASE_BALL_SPEED;
int lives = INITIAL_LIVES;
int score = 0;
int gameTime = 0;
int startTime = 0;
bool ballLaunched = false;
int mouseX = WINDOW_WIDTH / 2;
int highScore = 0;



// Game objects
std::vector<Block> blocks;
std::vector<PerkItem> perks;
std::vector<Particle> particles;

// Function declarations
void init();
void display();
void update(int value);
void keyboard(unsigned char key, int x, int y);
void mouse(int x, int y);
void mouseClick(int button, int state, int x, int y);
void initBlocks();
void drawPaddle();
void drawBall();
void drawBlocks();
void spawnPerk(float x, float y);
void drawPerks();
void drawText(float x, float y, const char* text);
void drawMenu();
void drawHUD();
void checkCollisions();
void updatePerks();
void resetBall();
void resetGame();


void updateBackgroundColor();
void loadHighScore();
void saveHighScore();
void drawHighScore();
void drawHelp();
void spawnParticles(float x, float y, float r, float g, float b);
void drawParticles();
void updateParticles();


// Initialize blocks
void initBlocks() {
    blocks.clear();
    float startX = (WINDOW_WIDTH - (BLOCKS_PER_ROW * BLOCK_WIDTH)) / 2;
    float startY = WINDOW_HEIGHT - 100;
    
    for (int row = 0; row < BLOCK_ROWS; row++) {
        for (int col = 0; col < BLOCKS_PER_ROW; col++) {
            Block block;
            block.x = startX + col * BLOCK_WIDTH + 5;
            block.y = startY - row * BLOCK_HEIGHT - 5;
            block.width = BLOCK_WIDTH - 10;
            block.height = BLOCK_HEIGHT - 5;
            block.active = true;
            block.hits = 1;
            
        // Assign colors based on row (soft futuristic palette)
            switch (row) {
                case 0: // Crimson Red
                    block.r = 0.91f; block.g = 0.30f; block.b = 0.23f;
                    break;
                case 1: // Sunset Orange
                    block.r = 0.90f; block.g = 0.49f; block.b = 0.13f;
                    break;
                case 2: // Golden Yellow
                    block.r = 0.95f; block.g = 0.77f; block.b = 0.06f;
                    break;
                case 3: // Emerald Green
                    block.r = 0.18f; block.g = 0.80f; block.b = 0.44f;
                    break;
                case 4: // Sky Blue
                    block.r = 0.20f; block.g = 0.60f; block.b = 0.86f;
                    break;
                
            }


            
            // Random perks (20% chance)
            int perkChance = rand() % 100;
            if (perkChance < 5) block.perk = EXTRA_LIFE;
            else if (perkChance < 10) block.perk = FASTER_BALL;
            else if (perkChance < 15) block.perk = WIDER_PADDLE;
            else block.perk = NO_PERK;
            
            blocks.push_back(block);
        }
    }
}

void updateBackgroundColor() {
    float r = 25.0f / 255.0f;
    float g = 35.0f / 255.0f;
    float b = 45.0f / 255.0f;
    glClearColor(r, g, b, 1.0f);
}


// Load high score from file
void loadHighScore() {
    FILE* file = fopen("highscore.txt", "r");
    if (file) {
        fscanf(file, "%d", &highScore);
        fclose(file);
    } else {
        highScore = 0;
    }
}

// Save high score to file
void saveHighScore() {
    FILE* file = fopen("highscore.txt", "w");
    if (file) {
        fprintf(file, "%d", highScore);
        fclose(file);
    }
}



// Draw text on screen
void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *text);
        text++;
    }
}

// Draw paddle
void drawPaddle() {
    glColor3f(0.2f, 0.6f, 1.0f);
    glBegin(GL_QUADS);
    glVertex2f(paddleX, paddleY);
    glVertex2f(paddleX + paddleWidth, paddleY);
    glVertex2f(paddleX + paddleWidth, paddleY + PADDLE_HEIGHT);
    glVertex2f(paddleX, paddleY + PADDLE_HEIGHT);
    glEnd();
}

// Draw ball
void drawBall() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(ballX, ballY);
    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * 3.14159f * i / 20;
        float x = ballX + BALL_RADIUS * cos(angle);
        float y = ballY + BALL_RADIUS * sin(angle);
        glVertex2f(x, y);
    }
    glEnd();
}

// Draw blocks
void drawBlocks() {
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].active) {
            glColor3f(blocks[i].r, blocks[i].g, blocks[i].b);
            glBegin(GL_QUADS);
            glVertex2f(blocks[i].x, blocks[i].y);
            glVertex2f(blocks[i].x + blocks[i].width, blocks[i].y);
            glVertex2f(blocks[i].x + blocks[i].width, blocks[i].y + blocks[i].height);
            glVertex2f(blocks[i].x, blocks[i].y + blocks[i].height);
            glEnd();
            
            // Draw border
            glColor3f(0.0f, 0.0f, 0.0f);
            glLineWidth(2);
            glBegin(GL_LINE_LOOP);
            glVertex2f(blocks[i].x, blocks[i].y);
            glVertex2f(blocks[i].x + blocks[i].width, blocks[i].y);
            glVertex2f(blocks[i].x + blocks[i].width, blocks[i].y + blocks[i].height);
            glVertex2f(blocks[i].x, blocks[i].y + blocks[i].height);
            glEnd();
        }
    }
}



// Draw perks
void drawPerks() {
    for (size_t i = 0; i < perks.size(); i++) {
        if (perks[i].active) {
            glColor3f(perks[i].r, perks[i].g, perks[i].b);
            glBegin(GL_QUADS);
            glVertex2f(perks[i].x - 10, perks[i].y - 10);
            glVertex2f(perks[i].x + 10, perks[i].y - 10);
            glVertex2f(perks[i].x + 10, perks[i].y + 10);
            glVertex2f(perks[i].x - 10, perks[i].y + 10);
            glEnd();
        }
    }
}

// Draw HUD
void drawHUD() {
    char text[50];
    glColor3f(1.0f, 1.0f, 1.0f);
    
    sprintf(text, "Lives: %d", lives);
    drawText(10, WINDOW_HEIGHT - 30, text);
    
    sprintf(text, "Score: %d", score);
    drawText(10, WINDOW_HEIGHT - 55, text);


    
    sprintf(text, "Time: %d s", gameTime);
    drawText(WINDOW_WIDTH - 150, WINDOW_HEIGHT - 30, text);

    sprintf(text, "Ball Speed: %.1f", ballSpeed);
    drawText(WINDOW_WIDTH - 150, WINDOW_HEIGHT - 55, text);
}

// Draw menu
void drawMenu() {
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 + 100, "DX-BALL GAME");
    
    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2, "Press SPACE to Start");
    drawText(WINDOW_WIDTH / 2 - 80, WINDOW_HEIGHT / 2 - 40, "Press ESC to Exit");
    
    glColor3f(0.7f, 0.7f, 0.7f);
    drawText(WINDOW_WIDTH / 2 - 150, 150, "Controls: Mouse or Arrow Keys");
    drawText(WINDOW_WIDTH / 2 - 100, 110, "P - Pause/Resume");

    drawText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 80, "Press H for High Score");

    drawText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 120, "Press F for Help");


            //         MENU
            //        /  |  \
            //       H   F   SPACE
            //      /     \
            // HIGHSCORE  HELP
            //      \     /
            //         B
            //         ↓
            //        MENU

}

// Draw high score
void drawHighScore() {
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2 + 60, "HIGH SCORES");

    glColor3f(1.0f, 1.0f, 1.0f);
    char text[50];
    sprintf(text, "Highest Score: %d", highScore);
    drawText(WINDOW_WIDTH / 2 - 90, WINDOW_HEIGHT / 2, text);

    glColor3f(0.7f, 0.7f, 0.7f);
    drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 80, "Press B to go Back");
}



// Draw help menu
void drawHelp() {
    glColor3f(1.0f, 1.0f, 0.0f);
    drawText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 150, "HELP MENU");

    glColor3f(1.0f, 1.0f, 1.0f);
    drawText(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 80, "Goal: Break all blocks without losing the ball!");
    
    glColor3f(0.8f, 0.8f, 0.8f);
    drawText(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 + 30, "Controls:");
    drawText(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 + 5,  "- Move Paddle: Mouse or LEFT/RIGHT Arrow Keys");
    drawText(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 20, "- Launch Ball: Click Mouse or Press SPACE");
    drawText(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 45, "- Pause/Resume: P");
    drawText(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 70, "- Exit Game: ESC");

    glColor3f(0.7f, 0.7f, 1.0f);
    drawText(WINDOW_WIDTH / 2 - 200, WINDOW_HEIGHT / 2 - 120, "Perks:");
    drawText(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 145, "- Green Box: Extra Life");
    drawText(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 170, "- Red Box: Faster Ball");
    drawText(WINDOW_WIDTH / 2 - 180, WINDOW_HEIGHT / 2 - 195, "- Blue Box: Wider Paddle");

    glColor3f(0.7f, 0.7f, 0.7f);
    drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 250, "Press B to go Back");
}

// Draw particles
void drawParticles() {
    glPointSize(4.0f);

    glBegin(GL_POINTS);
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles[i].active) {
            glColor4f(particles[i].r, particles[i].g, particles[i].b, particles[i].life);
            glVertex2f(particles[i].x, particles[i].y);
        }
    }
    glEnd();
}





// Spawn perk
void spawnPerk(float x, float y) {
    PerkItem perk;
    perk.x = x;
    perk.y = y;
    perk.vx = 0;
    perk.vy = -2.0f;
    perk.active = true;
    
    int type = rand() % 3;
    switch(type) {
        case 0:
            perk.type = EXTRA_LIFE;
            perk.r = 0.0f; perk.g = 1.0f; perk.b = 0.0f;
            break;
        case 1:
            perk.type = FASTER_BALL;
            perk.r = 1.0f; perk.g = 0.0f; perk.b = 0.0f;
            break;
        case 2:
            perk.type = WIDER_PADDLE;
            perk.r = 0.0f; perk.g = 0.0f; perk.b = 1.0f;
            break;
    }
    
    perks.push_back(perk);
}

// Spawn particles
void spawnParticles(float x, float y, float r, float g, float b) {
    for (int i = 0; i < 20; i++) { // 20 particles per explosion
        Particle p;
        p.x = x;
        p.y = y;
        p.vx = (rand() % 100 - 50) / 25.0f;  // random -2.0f to +2.0f
        p.vy = (rand() % 100 - 50) / 25.0f;  
        p.r = r;
        p.g = g;
        p.b = b;
        p.life = 1.0f;
        p.active = true;
        particles.push_back(p);
    }
}

// Update particles
void updateParticles() {
    for (size_t i = 0; i < particles.size(); i++) {
        if (particles[i].active) {
            particles[i].x += particles[i].vx;
            particles[i].y += particles[i].vy;
            particles[i].vy -= 0.05f;   // gravity effect
            particles[i].life -= 0.02f; // fade out

            if (particles[i].life <= 0.0f) {
                particles[i].active = false;
            }
        }
    }

  

}



// Update perks
void updatePerks() {
    for (size_t i = 0; i < perks.size(); i++) {
        if (perks[i].active) {
            perks[i].y += perks[i].vy;
            
            // Check collision with paddle
            if (perks[i].y <= paddleY + PADDLE_HEIGHT &&
                perks[i].y >= paddleY &&
                perks[i].x >= paddleX &&
                perks[i].x <= paddleX + paddleWidth) {
                
                perks[i].active = false;
                
                switch(perks[i].type) {
                    case EXTRA_LIFE:
                        lives++;
                        break;
                    case FASTER_BALL:
                        ballSpeed += 1.0f;
                        break;
                    case WIDER_PADDLE:
                        paddleWidth = (paddleWidth < 180) ? paddleWidth + 30 : paddleWidth;
                        break;
                    default:
                        break;
                }
            }
            
            // Remove if off screen
            if (perks[i].y < 0) {
                perks[i].active = false;
            }
        }
    }
}

// Check collisions
void checkCollisions() {
    // Wall collisions
    if (ballX - BALL_RADIUS <= 0 || ballX + BALL_RADIUS >= WINDOW_WIDTH) {
        ballVX = -ballVX;
    }
    if (ballY + BALL_RADIUS >= WINDOW_HEIGHT) {
        ballVY = -ballVY;
    }
    
    // Paddle collision
    if (ballY - BALL_RADIUS <= paddleY + PADDLE_HEIGHT &&
        ballY - BALL_RADIUS >= paddleY &&
        ballX >= paddleX &&
        ballX <= paddleX + paddleWidth) {
        
        ballVY = ballSpeed;  // Set to positive ballSpeed
        float hitPos = (ballX - paddleX) / paddleWidth;
        ballVX = (hitPos - 0.5f) * ballSpeed * 1.5f;  // Angle control
        
        // Normalize to maintain constant speed
        float magnitude = sqrt(ballVX * ballVX + ballVY * ballVY);
        ballVX = (ballVX / magnitude) * ballSpeed;
        ballVY = (ballVY / magnitude) * ballSpeed;
    }
    
    // Block collisions
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].active) {
            if (ballX + BALL_RADIUS >= blocks[i].x &&
                ballX - BALL_RADIUS <= blocks[i].x + blocks[i].width &&
                ballY + BALL_RADIUS >= blocks[i].y &&
                ballY - BALL_RADIUS <= blocks[i].y + blocks[i].height) {
                
                blocks[i].active = false;

                spawnParticles(blocks[i].x + blocks[i].width / 2, 
                blocks[i].y + blocks[i].height / 2, 
                blocks[i].r, blocks[i].g, blocks[i].b);


                ballVY = -ballVY;
                score += 10;
                
                // Increase ball speed gradually every 50 points
                if (score % 50 == 0) {
                    ballSpeed += 0.5f;
                }
                
                if (blocks[i].perk != NO_PERK) {
                    spawnPerk(blocks[i].x + blocks[i].width / 2, 
                             blocks[i].y + blocks[i].height / 2);
                }
            }
        }
    }
    
    // Ball fell off screen
    if (ballY - BALL_RADIUS <= 0) {
        lives--;
        if (lives <= 0) {
            gameState = LOST;
        } else {
            resetBall();
        }
    }
    
    // Check win condition
    bool allDestroyed = true;
    for (size_t i = 0; i < blocks.size(); i++) {
        if (blocks[i].active) {
            allDestroyed = false;
            break;
        }
    }
    if (allDestroyed) {
        gameState = WON;
    }
}

// Reset ball
void resetBall() {
    ballX = WINDOW_WIDTH / 2;
    ballY = paddleY + PADDLE_HEIGHT + BALL_RADIUS;
    ballVX = BASE_BALL_SPEED;
    ballVY = BASE_BALL_SPEED;
    ballLaunched = false;
}

// Reset game
void resetGame() {
    lives = INITIAL_LIVES;
    score = 0;
    gameTime = 0;
    startTime = glutGet(GLUT_ELAPSED_TIME) / 1000;
    ballSpeed = BASE_BALL_SPEED;
    paddleWidth = PADDLE_WIDTH;
    perks.clear();
    initBlocks();
    resetBall();
}



// Display function
void display() {
    updateBackgroundColor();
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    if(score > highScore) {
        highScore = score;
        saveHighScore();
    }
    
    if (gameState == MENU) {
        drawMenu();
    } else if (gameState == PLAYING) {
        drawBlocks();
        drawPaddle();
        drawBall();
        drawPerks();
        drawParticles();
        drawHUD();
    } else if (gameState == PAUSED) {
        drawBlocks();
        drawPaddle();
        drawBall();
        drawPerks();
        drawHUD();
        
        glColor3f(1.0f, 1.0f, 0.0f);
        drawText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2, "PAUSED");
        drawText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 40, "Press P to Resume");
    } else if (gameState == WON) {
        glColor3f(0.0f, 1.0f, 0.0f);
        drawText(WINDOW_WIDTH / 2 - 60, WINDOW_HEIGHT / 2, "YOU WON!");
        
        char text[50];
        sprintf(text, "Score: %d  Time: %d s", score, gameTime);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 40, text);
        drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 80, "Press SPACE for Menu");
    } else if (gameState == LOST) {
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText(WINDOW_WIDTH / 2 - 70, WINDOW_HEIGHT / 2, "GAME OVER");
        
        char text[50];
        sprintf(text, "Score: %d", score);
        glColor3f(1.0f, 1.0f, 1.0f);
        drawText(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 40, text);
        drawText(WINDOW_WIDTH / 2 - 120, WINDOW_HEIGHT / 2 - 80, "Press SPACE for Menu");
    } else if (gameState == HIGHSCORE) {
        drawHighScore();
    } else if (gameState == HELP) {
        drawHelp();
    }
    
    glFlush();
    glutSwapBuffers();
}

// Update function
void update(int value) {
    if (gameState == PLAYING) {
        if (ballLaunched) {
            // Normalize velocity to maintain constant speed
            float magnitude = sqrt(ballVX * ballVX + ballVY * ballVY);
            if (magnitude > 0) {
                ballVX = (ballVX / magnitude) * ballSpeed;
                ballVY = (ballVY / magnitude) * ballSpeed;
            }
            
            ballX += ballVX;
            ballY += ballVY;
            
            checkCollisions();
            updatePerks();
            updateParticles();
        } else {
            ballX = paddleX + paddleWidth / 2;
        }
        
        // Update game time
        int currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000;
        gameTime = currentTime - startTime;
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

// Keyboard function
void keyboard(unsigned char key, int x, int y) {
    if (key == 27) { // ESC
        exit(0);
    } else if (key == ' ' || key == 32) {
        if (gameState == MENU) {
            gameState = PLAYING;
            resetGame();
        } else if (gameState == PLAYING && !ballLaunched) {
            ballLaunched = true;
        } else if (gameState == WON || gameState == LOST) {
            gameState = MENU;
        }
    } else if (key == 'p' || key == 'P') {
        if (gameState == PLAYING) {
            gameState = PAUSED;
        } else if (gameState == PAUSED) {
            gameState = PLAYING;
        }
    } else if (key == 'h' || key == 'H') {
        if (gameState == MENU) {
            gameState = HIGHSCORE;
        }
    }else if (key == 'f' || key == 'F') {
        if (gameState == MENU) {
            gameState = HELP;
        }
    }
     else if (key == 'b' || key == 'B') {
        if (gameState == HIGHSCORE || gameState == HELP) {
            gameState = MENU;
        }
    }
}


// Special keyboard for arrow keys
void specialKeyboard(int key, int x, int y) {
    if (gameState == PLAYING) {
        if (key == GLUT_KEY_LEFT) {
            paddleX -= 20;
            if (paddleX < 0) paddleX = 0;
        } else if (key == GLUT_KEY_RIGHT) {
            paddleX += 20;
            if (paddleX + paddleWidth > WINDOW_WIDTH) 
                paddleX = WINDOW_WIDTH - paddleWidth;
        }
    }
}

// Mouse motion

void mouse(int x, int y) {
    if (gameState == PLAYING) {
        mouseX = x;
        paddleX = x - paddleWidth / 2;
        
        if (paddleX < 0) paddleX = 0;
        if (paddleX + paddleWidth > WINDOW_WIDTH) 
            paddleX = WINDOW_WIDTH - paddleWidth;
    }
}

// Mouse click
void mouseClick(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (gameState == PLAYING && !ballLaunched) {
            ballLaunched = true;
        }
    }
}

// Initialize OpenGL
void init() {

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, WINDOW_WIDTH, 0.0, WINDOW_HEIGHT);
    glMatrixMode(GL_MODELVIEW);
    
    srand(time(NULL));
    initBlocks();
    loadHighScore();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("DX-Ball Game");
    
    init();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeyboard);
    glutPassiveMotionFunc(mouse);
    glutMouseFunc(mouseClick);
    glutTimerFunc(0, update, 0);
    
    glutMainLoop();
    
    return 0;
}