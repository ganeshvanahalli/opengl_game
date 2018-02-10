#include "main.h"
#include "timer.h"
#include "ball.h"
#include "ground.h"
#include "pine.h"
#include "rectangle.h"


using namespace std;

GLMatrices Matrices;
GLuint     programID;
GLFWwindow *window;

int score = 0;
int lives = 5;

/**************************
* Customizable functions *
**************************/

double xpos, ypos;
Ball player, pond, flying_balls[20], tramp;
Pine pinos[3];
Ground grnd;
Rectangle mag[2];
Ground grass;
Rectangle rect[3];

float screen_zoom = 0.5, screen_center_x = 0, screen_center_y = 0;

Timer t60(1.0 / 60);

/* Render the scene with openGL */
/* Edit this function according to your assignment */



void draw() {
    // clear the color and depth in the frame buffer
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram (programID);

    // Eye - Location of camera. Don't change unless you are sure!!
    // glm::vec3 eye ( 5*cos(camera_rotation_angle*M_PI/180.0f), 0, 5*sin(camera_rotation_angle*M_PI/180.0f) );
    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    // glm::vec3 target (0, 0, 0);
    // Up - Up vector defines tilt of camera.  Don't change unless you are sure!!
    // glm::vec3 up (0, 1, 0);

    // Compute Camera matrix (view)
    // Matrices.view = glm::lookAt( eye, target, up ); // Rotating Camera for 3D
    // Don't change unless you are sure!!
    Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    int i;

    // Scene render
    grass.draw(VP);
    grnd.draw(VP);
    pond.draw(VP);

    //PINES

    for(i=0;i<3;i++)
        pinos[i].draw(VP);

    //FLYING BALLs
    for(i=0;i<20;i++){
        flying_balls[i].draw(VP);
    }

    //TRAMPOLINE   
    for(i=0;i<2;i++)
        rect[i].draw(VP);
    tramp.draw(VP);


    //MAGNET
        rect[2].draw(VP);
        mag[0].draw(VP);
        mag[1].draw(VP);

    //PLAYER
    player.draw(VP);

}

int jump =0;
int in_pond = 0;
int in_water = 0;
float u,a,y_init,dis;
char buffer [100];
int cx;


void respawn(int i){
    flying_balls[i].position.x = 68 + 3*i;
    flying_balls[i].position.y = (((float) rand() / (RAND_MAX)) + 1)*3 + 0.5 ;
    flying_balls[i].speed = ((float) rand() / (RAND_MAX) + 1)*0.05 + 0.01 ;
}

void rot(float i){
    float x,y,rx,ry;
    x = player.position.x - pond.position.x;
    y = player.position.y - pond.position.y;

    rx = x*cos(i*3.1416/120) - y*sin(i*3.1416/120);
    ry = x*sin(i*3.1416/120) + y*cos(i*3.1416/120); 
    player.position.x = rx + pond.position.x;
    player.position.y = ry + pond.position.y;
}
int pause_val = 1;
float val = 0.08;
float valt = 0.5;
void tick_input(GLFWwindow *window) {
    int left  = glfwGetKey(window, GLFW_KEY_A);
    int right = glfwGetKey(window, GLFW_KEY_D);
    int up = glfwGetKey(window, GLFW_KEY_SPACE);
    int i;
    int A = glfwGetKey(window, GLFW_KEY_LEFT);
    int D = glfwGetKey(window, GLFW_KEY_RIGHT);
    int S = glfwGetKey(window, GLFW_KEY_DOWN);
    int W = glfwGetKey(window, GLFW_KEY_UP);

    int pause = glfwGetKey(window, GLFW_KEY_P);
    if(pause){
        pause_val *= -1;
    }
    y_init = -1.1;
    if(lives && pause_val == 1){   
        //MOUSE ZOOMING IN AND OUT
        if(A && screen_center_x > -3){
            screen_center_x -= 0.05;
            reset_screen();
        }
        if(D && screen_center_x < 3){
            screen_center_x += 0.05;
            reset_screen();
        }
        if(W && screen_center_y < 3){
            screen_center_y += 0.05;
            reset_screen();
        }
        if(S && screen_center_x > -3){
            screen_center_y -= 0.05;
            reset_screen();
        }
        //DIFFICULTY
        // if(score+100){
        //     tramp.speed *= (score/100);
        //     pond.speed *= (score/100);
        //     for(i=0;i<3;i++){
        //         pinos[i].speed *= (score/100);
        //     }
        // }
        rect[0].position.x -= tramp.speed;
        rect[1].position.x -= tramp.speed;

        if(pond.speed < 0.4){    
            val += 0.00003;
            valt += 0.00003;
            pinos[0].speed += 0.00003;
            pinos[1].speed += 0.00003;
            pinos[2].speed += 0.00003;
            tramp.speed += 0.00003;
            pond.speed += 0.00003;
            player.speed += 0.00003;
        }
        for(i=0;i<20;i++){
            flying_balls[i].speed += 0.00009;    
        } 

        //FURTHER MAP
        if(pond.position.x < -12.5){
            pond.position.x = (((float) rand() / (RAND_MAX)) + 1)*10 + 10;
        }

        if(tramp.position.x < -12.5){
            tramp.position.x = (((float) rand() / (RAND_MAX)) + 1)*10 + 15;
            rect[0].position.x = tramp.position.x-0.7;
            rect[1].position.x = tramp.position.x+0.7;
        }

        if(abs(tramp.position.x - pond.position.x)<3){
            tramp.position.x += 3;
            rect[0].position.x = tramp.position.x-0.7;
            rect[1].position.x = tramp.position.x+0.7;
        }
        if(pinos[0].position.x<-11){
            pinos[0].position.x = (((float) rand() / (RAND_MAX)) + 1)*10 + 20;
            pinos[1].position.x = pinos[0].position.x - 0.5;
        }

        if(abs(pinos[0].position.x - pond.position.x)<3){
            pinos[0].position.x += 3;
            pinos[1].position.x = pinos[0].position.x - 0.5;
        }
        if(pinos[2].position.x<-11){
            pinos[2].position.x = (((float) rand() / (RAND_MAX)) + 1)*10 +10;
        }
        if(abs(pinos[2].position.x - pond.position.x)<3)
            pinos[2].position.x += 3;
        

        //TRAMPOLINE
        if(player.position.x < 1.7+tramp.position.x && player.position.x > tramp.position.x-1.7 && player.position.y < 0.9 && u<0){
            u = 1.2;
            a = -0.05;
            jump =1;
        }

        //MAGNET
            if(player.position.y > mag[1].position.y && player.position.y < mag[1].position.y +1.5 )
                player.position.x -= 0.1;
            if(mag[1].position.y < 5 || mag[1].position.y > 6.8 ){
                rect[2].speed = -rect[2].speed;
                mag[0].speed = -mag[0].speed;
                mag[1].speed = -mag[1].speed;
            }
        

        //FLYING BALLS  
        for(i=0;i<20;i++){
            if(detect_collision(i)){
                if(player.position.y > flying_balls[i].position.y && u < 0){
                    respawn(i);
                    score += 10;
                    jump = 1;
                    u = 0.8;
                    break;
                }
            }
        }
        for(i=0;i<20;i++){
            if(flying_balls[i].position.x < -12){
                respawn(i);
            }
        } 

        //PINES
        if((player.position.x > pinos[0].position.x -1.3 && player.position.x < pinos[0].position.x +0.8 && player.position.y == y_init) || (player.position.x > pinos[2].position.x - 0.7 && player.position.x < pinos[2].position.x + 0.8 &&  player.position.y == y_init)){
            player.position.x = 0;
            // player.position.y = y_init;
            player.position.y = y_init + 5;
            jump = 1;
            lives -= 1;
        }

        //crossed
        if(player.position.x < -10){
            player.position.x = 0;
            // player.position.y = y_init;
            player.position.y = y_init + 5;
            jump = 1;
            lives -= 1;
        }
        
        //POND
        if(player.position.x >= -2.5 + pond.position.x && player.position.x <= 2.5 + pond.position.x && player.position.y == y_init && !in_pond){
            u = 0;
            a = -0.01;
            in_pond = 1;
            jump = 0;
            if(player.position.x < -1.6 + pond.position.x)
                player.position.x = -1.6 + pond.position.x;
            if(player.position.x > 1.6 + pond.position.x)
                player.position.x = 1.6 + pond.position.x;
        }
        
        //ON GROUND
        if(jump ==1 && !in_pond){
            u += a;
            dis = u + a;
            player.position.y += dis;
            if(player.position.y <= y_init){
                jump = 0;
                u=0;
                player.position.y = y_init;
            }
        }

        if (left && !in_pond) {
            player.position.x -= val;
        }
        if (right && !in_pond) {
            player.position.x += val;
        }
        if (up && jump == 0 && !in_pond){
            u = 0.9;
            a = -0.05;
            jump =1;
        }

        //INSIDE POND
        if(in_pond){
            if(!in_water){
                u += a;
                dis = u + a;
                player.position.y += dis;
                if(abs(pow( pow((player.position.x -pond.position.x), 2)+pow((player.position.y -pond.position.y), 2),0.5)) > 1.6 && !in_water && player.position.y < -2)
                    in_water = 1;
            }

            else{

                // player.position.x -= pond.speed;
                    if(player.position.x < pond.position.x){
                        rot(valt);
                    }
                    if(player.position.x > pond.position.x){
                        rot(-valt);
                    }
                    if(jump ==1){
                        u += a;
                        dis = u + a;
                        player.position.y += dis;
                        if(player.position.y > -2){
                            u += 2*a;
                            in_pond = 0;
                            in_water = 0;
                        }
                    }
                    if(left && player.position.x > pond.position.x -1.5){
                        rot(-1);
                    }  
                    if(right && player.position.x < pond.position.x +1.5){
                        rot(1);
                    }

                    if (up && jump == 0){
                        u = 1.2;
                        a = -0.08;
                        jump =1;
                    }  
                 
            }
        }
    }

    //PRINTING IN THE TITLE
    if(lives){
        cx = snprintf ( buffer, 100, "LIFE LEFT is %d", lives);
        if (cx>=0 && cx<100)
            snprintf ( buffer+cx, 100-cx, ", and the SCORE is %d.", score );
        glfwSetWindowTitle (window, buffer);
    }
    else{
        cx = snprintf ( buffer, 100, "GAME OVER !!!");
        if (cx>=0 && cx<100)
            snprintf ( buffer+cx, 100-cx, ", and the SCORE is %d.", score );
        glfwSetWindowTitle (window, buffer);
    }
}

void tick_elements() {
    int i;
    if(lives && pause_val == 1){
        player.tick();
        for(i=0;i<20;i++){
            flying_balls[i].tick();
        } 
        for(i=0;i<3;i++)
            pinos[i].tick(); 
        rect[2].tick();
        mag[0].tick();
        mag[1].tick(); 
        pond.tick(); 
        tramp.tick();
    }
}


/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *window, int width, int height) {
    int i;
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    player       = Ball(-7.0, -1.1, 0.9,0,COLOR_GREY);
    grnd       = Ground(1, -3, 4,COLOR_BROWN);
    grass       = Ground(1, -1,4,COLOR_GREEN);
    pond       = Ball(-0.2, -2, 2.5 ,1,COLOR_BLUE);
    pinos[0]       = Pine(14,-2,COLOR_BLUE);
    pinos[1]       = Pine(pinos[0].position.x - 0.5,-2,COLOR_BLUE);
    pinos[2]       = Pine(6,-2,COLOR_BLUE);

    tramp       = Ball(10.2, 0, 0.6 ,1,COLOR_RED);
    rect[0]     = Rectangle(tramp.position.x-0.7, -1, 2 ,0.2, COLOR_RED);
    rect[1]     = Rectangle(tramp.position.x+0.7, -1, 2 ,0.2, COLOR_RED);

    rect[2]     = Rectangle(-9.8, 7, 1.5 ,0.2, COLOR_RED);
    mag[0]      = Rectangle(-8.8,7.75,0.4,2,COLOR_RED );
    mag[1]      = Rectangle(-8.8,6.25,0.4,2,COLOR_RED );

    rect[2].speed = 0.01;
    mag[0].speed = 0.01;
    mag[1].speed = 0.01;

    rect[0].speed = 0.0;
    rect[1].speed = 0.0;
    
    pinos[0].speed = 0.03;
    pinos[1].speed = 0.03;
    pinos[2].speed = 0.03;
    tramp.speed = 0.03;
    pond.speed = 0.03;
    player.speed = 0.03;
    for(i=0;i<20;i++){
        if(i%6 == 0){
            flying_balls[i] = Ball(8+ 3*i ,(((float) rand() / (RAND_MAX)) + 1)*3 + 0.5 ,(((float) rand() / (RAND_MAX)) + 1 ) * 0.4 + 0.15, 0,COLOR_V);
        }
        else if(i%6 == 1){
            flying_balls[i] = Ball(8+ 3*i ,(((float) rand() / (RAND_MAX)) + 1)*3 + 0.5 ,(((float) rand() / (RAND_MAX)) + 1 ) * 0.4 + 0.15, 0,COLOR_I);
        }
        else if(i%6 == 2){
            flying_balls[i] = Ball(8+ 3*i ,(((float) rand() / (RAND_MAX)) + 1)*3 + 0.5 ,(((float) rand() / (RAND_MAX)) + 1 ) * 0.4 + 0.15, 0,COLOR_B);
        }
        else if(i%6 == 3){
            flying_balls[i] = Ball(8+ 3*i ,(((float) rand() / (RAND_MAX)) + 1)*3 + 0.5 ,(((float) rand() / (RAND_MAX)) + 1 ) * 0.4 + 0.15, 0,COLOR_G);
        }
        else if(i%6 == 4){
            flying_balls[i] = Ball(8+ 3*i ,(((float) rand() / (RAND_MAX)) + 1)*3 + 0.5 ,(((float) rand() / (RAND_MAX)) + 1 ) * 0.4 + 0.15, 0,COLOR_Y);
        }
        else{
            flying_balls[i] = Ball(8+ 3*i ,(((float) rand() / (RAND_MAX)) + 1)*3 + 0.5 ,(((float) rand() / (RAND_MAX)) + 1 ) * 0.4 + 0.15, 0,COLOR_R);
        }
        flying_balls[i].speed = ((float) rand() / (RAND_MAX) + 1)*0.03 + 0.01 ;
    }

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("Sample_GL.vert", "Sample_GL.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow (window, width, height);

    // Background color of the scene
    glClearColor (COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0, 0.0f); // R, G, B, A
    glClearDepth (1.0f);

    glEnable (GL_DEPTH_TEST);
    glDepthFunc (GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}

// void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
// {
//     screen_zoom = yoffset;
//     screen_center_x = xpos;
//     screen_center_y = ypos;
// }


int main(int argc, char **argv) {
    srand(time(0));
    int width  = 1000;
    int height = 800;

    window = initGLFW(width, height);

    initGL (window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in float buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
        glfwGetCursorPos(window, &xpos, &ypos);
            
        glfwSetScrollCallback(window, scroll_callback);
    }

    quit(window);
}

bool detect_collision(int i) {
    return abs(pow( pow((player.position.x - flying_balls[i].position.x), 2)
    + pow((player.position.y - flying_balls[i].position.y), 2),0.5)) <= (player.rad + flying_balls[i].rad);
}

void reset_screen() {
    float top    = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left   = screen_center_x - 4 / screen_zoom - 2;
    float right  = screen_center_x + 4 / screen_zoom + 2;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
