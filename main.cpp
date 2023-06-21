#include "raylib.h"

#include <fstream>  // about.txt, options.ini
#include <string>


#define MAX_FONTS       1
#define MAX_MESSAGES    20
#define MAX_IMAGES      4

enum State {
    Title,
    Menu,
    Ingame,
    Controls
};

enum SubState{
    None,
    Options,
    About
};

enum Direction{
    Up,
    Down,
    Left,
    Right
};

typedef struct Pong{
    Vector2 position;
    float sizeX;
    float sizeY;
    float speed;
    Color color;
    int score;
} Pong;

typedef struct Ball{
    Vector2 position;
    float size;
    Vector2 vel;
    Color color;
} Ball;

typedef struct Goal{
    Direction wall;
    float width;
    Color color;
} Goal;


Pong player = { 0 };
Pong enemy = { 0 };
Ball ball = { 0 };
Goal leftgoal;
Goal rightgoal;

// Main entry point
// ----------------------------------------------------------------------------------------------------------------------
int main(){


    

    // ----------------------------------------------------------------------------------------------------------------------
    // Initialisation
    // ----------------------------------------------------------------------------------------------------------------------
    const int SCREEN_WIDTH = 1000;          // 1000
    const int SCREEN_HEIGHT = 600;          // 600
    const char* PROCESS_NAME = "RAYPONG";
    SetTargetFPS(60);
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, PROCESS_NAME);

    // game manager
    State GameState = Controls;     // default is Controls
    SubState GameStateSub = None;   // default is None
    int frame_count = 0;            // resets every 10s
    bool paused = false;
    bool AlreadyClosed = false;

    // images
    Image keyboard = LoadImage("resources/img/keyboard.png");
    Texture2D keyboardText = LoadTextureFromImage(keyboard);    // keyboard scheme
    UnloadImage(keyboard);

    

    // fonts
    Font fonts[MAX_FONTS] = { 0 };
    fonts[0] = LoadFont("resources/fonts/mecha.png");   // mecha
    
    //messages

        // read from about.txt
    std::string about_data;
    const char* ABOUT_DIRNAME = "resources/data/about.txt";
    std::ifstream about_file(ABOUT_DIRNAME);
    if(about_file.good()){
        if (!about_file.is_open())
        {
            CloseWindow();
            return 1;
        }    
        std::string line;
        while(std::getline(about_file, line)){
            about_data += line + "\n";
        }
        about_file.close();
    }
    else{
        about_data = "ABOUT INFORMATION NOT FOUND";
    }

    const char* messages[MAX_MESSAGES] = { "[msg]" };
    messages[0] = "A GAME BY NIEP";         // bootup
    messages[1] = "PRESS ANY KEY";          // bootup
    messages[2] = "RAYPONG";                // menu
    messages[3] = "A CLASSIC REIMAGINED";   // menu
    messages[4] = "PLAY";                   // menu - select
    messages[5] = "OPTIONS";                // menu - select
    messages[6] = "ABOUT";                  // menu - select
    messages[7] = "EXIT";                   // menu - select
    messages[8] = "RESUME";                 // pause menu
    messages[9] = "RESTART";                // pause menu
    messages[10] = "RESTART ?";             // pause menu - restart confirmation
    messages[11] = "MAIN MENU";             // pause menu
    messages[12] = const_cast<char*>(about_data.c_str());   // about
    

    Vector2 TEXT_POS;
    TEXT_POS.x = ((SCREEN_WIDTH/2.0f) - fonts[0].baseSize*6.5f);
    TEXT_POS.y = SCREEN_HEIGHT/2.2f;

    float TEXT_SPACING = 3.0f;

    //transition controller
    float transition_counter = 1.0f;                                            // numeric controller
    bool transition_running_title = false;                                      // bool controller

    unsigned char transition_alpha;                                             
    transition_alpha = static_cast<unsigned char>(transition_counter * 100);    // alpha  

    // controls screen transition controller
    bool transition_running_controls = false;
    float controls_alpha = 0;

    // menu controller
    int menu_selected = 0;              // indexes slot 0
    float menu_alpha[5] = { 0 };        // slot 0 used for redundancy
    menu_alpha[menu_selected] = 1.0f;
    bool menu_first_interacted = false; // to check if any option has been highlighted before
    bool restart_confirm = true;        // true means needs confirmation

    // controls controller (placeholder until I implement keybinds)
    KeyboardKey KEY_USER_SELECT = KEY_ENTER;    
    KeyboardKey KEY_USER_PAUSE = KEY_TAB;




    // player setup
    player.position = (Vector2){SCREEN_WIDTH/10, SCREEN_HEIGHT/2 - player.sizeY/2};
    player.sizeY = 75.0f;
    player.sizeX = 20.0f;
    player.speed = 12.0f;
    player.color = (Color){100, 200, 150, 255}; // pale green
    player.score = 0;

    // enemy setup
    enemy.position = (Vector2){SCREEN_WIDTH - (SCREEN_WIDTH/10), SCREEN_HEIGHT/2 - enemy.sizeY/2};
    enemy.sizeY = 75.0f;
    enemy.sizeX = 20.0f;
    enemy.speed = 12.0f;
    enemy.color = (Color){100, 150, 200, 255};  // pale blue
    enemy.score = 0;

    //ball setup
    ball.position = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    ball.size = 12.5f;
    ball.color = (Color){200, 200, 200, 255};   // white
    ball.vel = (Vector2){-7, 7};

    // goal setup
    leftgoal.wall = Left;
    leftgoal.color = (Color){75,200,75,255};    // lime green
    leftgoal.width = 10;

    rightgoal.wall = Right;
    rightgoal.color = (Color){75,75,200,255};   // dark blue
    rightgoal.width = 10;

















    // ----------------------------------------------------------------------------------------------------------------------
    // Startup
    // ----------------------------------------------------------------------------------------------------------------------
    while(!WindowShouldClose()){

        ++frame_count;
        if(frame_count == 600) frame_count = 0;

        if(GameState == Controls){

            if(frame_count%60 == 0) transition_running_controls = true;
            if(transition_running_controls && frame_count <= 180) controls_alpha += 2.0f;
            if(transition_running_controls && frame_count >= 300 && frame_count <= 420 ) controls_alpha -= 2.0f;
            if(frame_count >= 420){
                GameState = Title;
                transition_running_controls = false;
                controls_alpha = 0;
            }

            BeginDrawing();

                ClearBackground((Color){70 ,70 ,70 , 100});

                DrawTexture(keyboardText, 
                SCREEN_WIDTH/2 - 275.0f, 
                SCREEN_HEIGHT/2 - 84.0f, 
                (Color){255, 255, 255, static_cast<unsigned char>(controls_alpha)});


            EndDrawing();
        }

        if(GameState == Title){

            BeginDrawing();
            
                ClearBackground((Color){70 ,70 ,70 , 100});

                // author
                DrawTextEx(fonts[0], 
                messages[0], 
                TEXT_POS, 
                fonts[0].baseSize*4.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, transition_alpha});

                // button not pressed
                if(frame_count%90<=45 && !transition_running_title){
                    DrawTextEx(fonts[0], 
                    messages[1],  
                    (Vector2){TEXT_POS.x, (TEXT_POS.y + fonts[0].baseSize*3.4f)}, 
                    fonts[0].baseSize*2.0f, 
                    TEXT_SPACING, 
                    (Color){200, 200, 200, transition_alpha});
                }

                if(frame_count%10<=5 && transition_running_title){
                    DrawTextEx(fonts[0], 
                    messages[1],  
                    (Vector2){TEXT_POS.x, (TEXT_POS.y + fonts[0].baseSize*3.4f)}, 
                    fonts[0].baseSize*2.0f, 
                    TEXT_SPACING, 
                    (Color){200, 200, 200, transition_alpha});
                }

            EndDrawing();

            // start getting input
            if(GetKeyPressed() != 0) transition_running_title = true;   // start transition

            // transition
            if(transition_running_title == true){   
                transition_counter = transition_counter - 0.02f;        // progress transition
                transition_alpha = transition_counter * 100;            // update alpha
                if(transition_alpha == 0){                              // transition end - go to menu
                    transition_running_title = false;                     
                    GameState = Menu;
                }
                // sidenote - I have no f-in idea why alpha triggers the if statement and counter doesn't
            }
        }

        if(GameState == Menu){
            if(GameStateSub == None){

                BeginDrawing();
            
                ClearBackground((Color){70, 70, 70, 100});

                // title
                DrawTextEx(fonts[0], 
                messages[2], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*1.1f, TEXT_POS.y- fonts[0].baseSize*11.0f},
                fonts[0].baseSize*4.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, 100});

                DrawTextEx(fonts[0], 
                messages[3], 
                (Vector2){TEXT_POS.x - fonts[0].baseSize*1.0f, TEXT_POS.y- fonts[0].baseSize*6.5f},
                fonts[0].baseSize*2.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, 100});

                // selection (4,5,6,7)
                DrawTextEx(fonts[0],                                    
                messages[4], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*3.95f, TEXT_POS.y},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, static_cast<unsigned char>((menu_alpha[1] + 1.0f)* 100)});

                DrawTextEx(fonts[0],                                    
                messages[5], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*2.60f, TEXT_POS.y + fonts[0].baseSize*2.5f},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, static_cast<unsigned char>((menu_alpha[2] + 1.0f)* 100)});

                DrawTextEx(fonts[0],                                    
                messages[6], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*3.5f, TEXT_POS.y + fonts[0].baseSize*5.0f},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, static_cast<unsigned char>((menu_alpha[3] + 1.0f)* 100)});
                
                DrawTextEx(fonts[0],                                    
                messages[7], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*4.35f, TEXT_POS.y + fonts[0].baseSize*7.5f},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, static_cast<unsigned char>((menu_alpha[4] + 1.0f)* 100)});


                EndDrawing();

                // start getting input
                if(!menu_first_interacted){
                    if(IsKeyPressed(KEY_DOWN)){         // downarrow - if nothing selected
                        menu_selected = 1;
                        menu_first_interacted = true;
                    }
                    if(IsKeyPressed(KEY_UP)){           // uparrow - if nothing selected
                        menu_selected = 4;
                        menu_first_interacted = true;
                    }   
                }
                else{
                    if(menu_selected < 1) menu_selected = 4;    //safeguard - loop to end
                    if(menu_selected > 4) menu_selected = 1;    //safeguard - loop to start
                    
                    if(IsKeyPressed(KEY_DOWN)) ++menu_selected; // downarrow
                    if(IsKeyPressed(KEY_UP)) --menu_selected;   // uparrow

                    

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 1){            // option 1 - start game
                        GameState = Ingame;
                        GameStateSub = None;
                        menu_selected = 0;
                        menu_first_interacted = false;
                    }
                    // I love poland                                                    // option 2 - open settings page                 

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 3){            // option 3 - open about page
                        GameStateSub = About;
                        menu_selected = 0;
                        menu_first_interacted = false;
                    }

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 4){            // option 4 - close window
                        GameState = Controls;
                        GameStateSub = None;
                        for(int i = 0; i< MAX_FONTS; i++) UnloadFont(fonts[i]);
                        AlreadyClosed = true;
                        CloseWindow();
                    }
                }
                
                // set option alpha
                for(int i = 1; i <= 4; i++){
                    menu_alpha[i] = 0.0f;
                }
                menu_alpha[menu_selected] = 1.5f;

            }

            if(GameStateSub == About){
                // todo

                BeginDrawing();
                
                    ClearBackground((Color){70, 70, 70, 100});

                    // title
                    DrawTextEx(fonts[0], 
                    messages[2], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*1.5f, TEXT_POS.y- fonts[0].baseSize*12.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    (Color){200, 200, 200, 100});

                    // about description
                    DrawTextEx(fonts[0], 
                    messages[12], 
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*12.3f, TEXT_POS.y- fonts[0].baseSize*7.5f},
                    fonts[0].baseSize*1.5f, 
                    TEXT_SPACING, 
                    (Color){200, 200, 200, 100});

                    // back to menu
                    DrawTextEx(fonts[0], 
                    messages[11], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*3.85f, TEXT_POS.y + fonts[0].baseSize*13.5f},
                    fonts[0].baseSize*1.5f, 
                    TEXT_SPACING, 
                    (Color){200, 200, 200, 255});

                
                EndDrawing();

                if(IsKeyPressed(KEY_USER_PAUSE) || IsKeyPressed(KEY_USER_SELECT)){
                    GameState = Menu;
                    GameStateSub = None;
                }
            }
        }
    
        if(GameState == Ingame){

            if(!paused){
                if(IsKeyDown(KEY_S)) player.position.y = player.position.y + player.speed;
                if(IsKeyDown(KEY_W)) player.position.y = player.position.y - player.speed;

                if(IsKeyDown(KEY_DOWN)) enemy.position.y = enemy.position.y + enemy.speed;
                if(IsKeyDown(KEY_UP)) enemy.position.y = enemy.position.y - enemy.speed;

                ball.position.x += ball.vel.x;
                ball.position.y += ball.vel.y;
            }

            // BALL wall collision handler
            if(ball.position.x + ball.size > SCREEN_WIDTH){
                ball.position.x = SCREEN_WIDTH - ball.size;
                ball.vel.x = 0.0f - ball.vel.x;
            }
            if(ball.position.y + ball.size > SCREEN_HEIGHT){
                ball.position.y = SCREEN_HEIGHT - ball.size;
                ball.vel.y = 0.0f - ball.vel.y;
            }
            if(ball.position.x - ball.size < 0){
                ball.position.x = SCREEN_WIDTH/2;
                ball.position.y = SCREEN_HEIGHT/2;
                //ball.vel.x = 0.0f - ball.vel.x;
            }
            if(ball.position.y - ball.size < 0){
                ball.position.y = ball.size;
                ball.vel.y = 0.0f - ball.vel.y;
            }

            // PONG wall collision handler
            if(player.position.y < 0) player.position.y = 0;
            if(player.position.y + player.sizeY > SCREEN_HEIGHT) player.position.y = SCREEN_HEIGHT-player.sizeY;

            if(enemy.position.y < 0) enemy.position.y = 0;
            if(enemy.position.y + enemy.sizeY > SCREEN_HEIGHT) enemy.position.y = SCREEN_HEIGHT-enemy.sizeY;

            // PLAYER collision handler
            if(CheckCollisionCircleRec(ball.position, ball.size, (Rectangle){player.position.x, player.position.y, player.sizeX, player.sizeY})){
                ball.position.x = player.position.x + player.sizeX + ball.size;
                ball.vel.x = 0 - ball.vel.x;
                // if(ball.position.y < player.position.y || ball.position.y > player.position.y + player.sizeY) ball.vel.y = 0 - ball.vel.y;
            }

            // ENEMY collision handler
            if(CheckCollisionCircleRec(ball.position, ball.size, (Rectangle){enemy.position.x, enemy.position.y, player.sizeX, player.sizeY})){
                ball.position.x = enemy.position.x - ball.size;
                ball.vel.x = 0 - ball.vel.x;
                // if(ball.position.y < enemy.position.y || ball.position.y > enemy.position.y + enemy.sizeY) ball.vel.y = 0 - ball.vel.y;
            }

            // PLAYER GOAL collision handler
            if(CheckCollisionCircleRec(ball.position, ball.size, (Rectangle){0,0,leftgoal.width,SCREEN_HEIGHT})){
                ball.position.x = SCREEN_WIDTH/2;
                ball.position.y = SCREEN_HEIGHT/2;
                ball.vel.x = 0 - ball.vel.x;
                ++enemy.score;
            }

            // ENEMY GOAL collision handler
            if(CheckCollisionCircleRec(ball.position, ball.size, (Rectangle){SCREEN_WIDTH-leftgoal.width,0,leftgoal.width,SCREEN_HEIGHT})){
                ball.position.x = SCREEN_WIDTH/2;
                ball.position.y = SCREEN_HEIGHT/2;
                ball.vel.x = 0 - ball.vel.x;
                ++player.score;
            }
            

            BeginDrawing();

                ClearBackground((Color){70, 70, 70, 100});

                // hud
                DrawTextEx(fonts[0], 
                TextFormat("SCORE: %d",player.score), 
                (Vector2){50, 10},
                fonts[0].baseSize*2.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, 100});

                DrawTextEx(fonts[0], 
                TextFormat("SCORE: %d",enemy.score), 
                (Vector2){SCREEN_WIDTH-15-fonts[0].baseSize*8.0f, 10},
                fonts[0].baseSize*2.0f, 
                TEXT_SPACING, 
                (Color){200, 200, 200, 100});

                DrawRectangle(player.position.x, player.position.y, player.sizeX, player.sizeY, player.color);
                DrawRectangle(enemy.position.x, enemy.position.y, enemy.sizeX, enemy.sizeY, enemy.color);
                DrawCircle(ball.position.x, ball.position.y, ball.size, ball.color);
                DrawRectangle(0,0,rightgoal.width,SCREEN_HEIGHT,leftgoal.color);
                DrawRectangle(SCREEN_WIDTH-leftgoal.width,0,leftgoal.width,SCREEN_HEIGHT,rightgoal.color);

                if(paused){

                    DrawTextEx(fonts[0],                                    
                    messages[8], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*2.67f, TEXT_POS.y},
                    fonts[0].baseSize*3.0f, 
                    TEXT_SPACING, 
                    (Color){200, 200, 200, static_cast<unsigned char>((menu_alpha[1] + 1.0f)* 100)});

                    if(!restart_confirm){                        
                        DrawTextEx(fonts[0],                                    
                        TextFormat(messages[10]), 
                        (Vector2){TEXT_POS.x + fonts[0].baseSize*2.4f, TEXT_POS.y + fonts[0].baseSize*2.5f},
                        fonts[0].baseSize*3.0f, 
                        TEXT_SPACING, 
                        (Color){200, 255, 200, static_cast<unsigned char>((menu_alpha[2] + 1.0f)* 100)});
                    }
                    else{
                        DrawTextEx(fonts[0],                                    
                        TextFormat(messages[9]), 
                        (Vector2){TEXT_POS.x + fonts[0].baseSize*2.4f, TEXT_POS.y + fonts[0].baseSize*2.5f},
                        fonts[0].baseSize*3.0f, 
                        TEXT_SPACING, 
                        (Color){200, 200, 200, static_cast<unsigned char>((menu_alpha[2] + 1.0f)* 100)});
                    }
                    
                    DrawTextEx(fonts[0],                                    
                    messages[11], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*1.25f, TEXT_POS.y + fonts[0].baseSize*5.0f},
                    fonts[0].baseSize*3.0f, 
                    TEXT_SPACING, 
                    (Color){200, 200, 200, static_cast<unsigned char>((menu_alpha[3] + 1.0f)* 100)});
                }

            EndDrawing();

            if(IsKeyPressed(KEY_USER_PAUSE)){
                paused = !paused;   
                menu_first_interacted = false;
                menu_selected = false;
            }


            if(paused){

                // start getting input
                if(!menu_first_interacted){
                    if(IsKeyPressed(KEY_DOWN)){         // downarrow - if nothing selected
                        menu_selected = 1;
                        menu_first_interacted = true;
                    }
                    if(IsKeyPressed(KEY_UP)){           // uparrow - if nothing selected
                        menu_selected = 3;
                        menu_first_interacted = true;
                    }
                }
                else{
                    if(menu_selected < 1) menu_selected = 3;    //safeguard - loop to end
                    if(menu_selected > 3) menu_selected = 1;    //safeguard - loop to start
                    
                    if(IsKeyPressed(KEY_DOWN)){
                        ++menu_selected;            // downarrow
                        restart_confirm = true;
                    }
                    if(IsKeyPressed(KEY_UP)){
                        --menu_selected;            // uparrow
                        restart_confirm = true;
                    }

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 1){            // option 1 - resume game
                        paused = false;
                        menu_first_interacted = false;
                        menu_selected = 0;
                    }
                          
                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 2){            // option 2 - restart match
                        
                        if(restart_confirm) restart_confirm = false;
                        else{
                            restart_confirm = true;
                            paused = false;

                            player.score = 0;
                            enemy.score = 0;

                            ball.position.x = SCREEN_WIDTH/2;
                            ball.position.y = SCREEN_HEIGHT/2;

                            player.position.y = SCREEN_HEIGHT/2 - player.sizeY/2;
                            enemy.position.y = SCREEN_HEIGHT/2 - enemy.sizeY/2;

                            menu_first_interacted = false;
                            menu_selected = 0;
                        }
                    }                      

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 3){            // option 3 - go back to menu
                        
                        /*for(int i = 0; i < MAX_FONTS; i++) UnloadFont(fonts[i]);
                        AlreadyClosed = true;
                        CloseWindow();*/

                        paused = false;
                        menu_first_interacted = false;
                        menu_selected = 0;          
                        GameState = Menu;
                    }
                }

                // set option alpha
                for(int i = 1; i <= 4; i++){
                    menu_alpha[i] = 0.0f;
                }
                menu_alpha[menu_selected] = 1.5f;
            }

        }

    }

    // ----------------------------------------------------------------------------------------------------------------------
    // De-Initialisation
    // ----------------------------------------------------------------------------------------------------------------------
    if(!AlreadyClosed){
        for(int i = 0; i < MAX_FONTS; i++) UnloadFont(fonts[0]);
        CloseWindow();
    }

    return 0;

}