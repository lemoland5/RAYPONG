#include "raylib.h"
#include "palette.h"

#include <fstream>  // about.txt

#define MAX_FONTS               1
#define MAX_MESSAGES            30
#define MAX_IMAGES              4
#define MAX_PALETTES            4

std::string line;

enum State {
    Controls,       // display of keyboard scheme
    Title,          // a game by niep
    Menu,           // main menu
    Ingame,         // match
    PaletteTest,    // DEBUG - display test of current color palette
    ReadTest,       // DEBUG - display test of data from read files 
    Postgame,
};

enum SubState{
    None,
    Options,
    About,
    Stats,
    Singleplayer = 1,
    Multiplayer = 2,
};

enum Result{
    None_Result,
    Win = 1,
    Loss = 2
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
    int lastTouched;
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

    const int SCREEN_WIDTH = 1000;  // 1000
    const int SCREEN_HEIGHT = 600;  // 600

    const char* PROCESS_NAME = "RAYPONG";
    SetTargetFPS(60);
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, PROCESS_NAME);

    // game manager --------------------------------------------------------------------------------------------------------------------------- 
    State GameState = Menu;         // default is Controls
    SubState GameStateSub = None;   // default is None
    Result GameStateResult = Win;
    int frame_count = 0;            // resets every 10s
    bool paused = false;
    bool AlreadyClosed = false;

    int scoreMax = 10;
    int timeMax = 3;       
    int timeCurrentMinutes = 0;
    int timeCurrentSeconds = 0;





    


    // images
    Image keyboard = LoadImage("resources/img/keyboard.png");
    Texture2D keyboardText = LoadTextureFromImage(keyboard);    // keyboard scheme
    UnloadImage(keyboard);

    // palettes -----------------------------------------------------------------------------------------------------------------------------
    Palette palettes[MAX_PALETTES] = { 0 };
    palettes[0] = PALETTE_DEFAULT;
    palettes[1] = PALETTE_GAMEBOY;
    Palette CurrentPalette = palettes[0];







    

    // fonts
    Font fonts[MAX_FONTS] = { 0 };
    fonts[0] = LoadFont("resources/fonts/mecha.png");   // mecha
    
    //messages

        // read from about.txt
    std::string about_data;
    std::ifstream about_file("resources/data/about.txt");
    if(about_file.good()){
        if (!about_file.is_open())
        {
            CloseWindow();
            return 1;
        }    
        while(std::getline(about_file, line)){
            about_data += line + "\n";
        }
        about_file.close();
    }
    else{
        about_data = "ABOUT INFORMATION NOT FOUND";
    }

        // read from log.txt
    std::string log_data[5];
    int total_games = 0;
    int log_i = 0;
    int score_i = 0;
    std::ifstream log_file("resources/data/log.txt");
    if(log_file.good()){
        if (!log_file.is_open())
        {
            CloseWindow();
            return 1;
        }    
        while(std::getline(log_file,line)){   

            log_data[0] = log_data[1];
            log_data[1] = log_data[2];
            log_data[2] = log_data[3];
            log_data[3] = log_data[4];

            log_data[log_i] = "Match: ";
            if(line[0] == '1'){
                log_data[log_i] += "PLAYER 1 WON, ";
            }
            if(line[0] == '2'){
                log_data[log_i] += "PLAYER 2 WON, ";
            }
            if(line[2] == '1'){
                log_data[log_i] += "SINGLEPLAYER, ";
            }
            if(line[2] == '2'){
                log_data[log_i] += "MULTIPLAYER, ";
            }

            log_data[log_i] += "SCORE: ";

            while(line[4+score_i] != '\n'){
                log_data[log_i] += line[4+score_i];
                ++score_i;
            }
            
            log_data[log_i] += "\n \0";
            
            score_i = 0;
            if(log_i<4) ++log_i;

            ++total_games;
        }
    }
    log_file.close();
    log_i = 0;
    score_i = 0;

    const char* messages[MAX_MESSAGES] = { "[msg]" };
    messages[0] = "A GAME BY NIEP";         // bootup
    messages[1] = "PRESS ANY KEY";          // bootup
    messages[2] = "RAYPONG";                // menu
    messages[3] = "A CLASSIC REIMAGINED";   // menu
    messages[4] = "PLAY";                   // menu
    messages[5] = "OPTIONS";                // menu
    messages[6] = "ABOUT";                  // menu
    messages[7] = "EXIT";                   // menu
    messages[8] = "RESUME";                 // pause menu
    messages[9] = "RESTART";                // pause menu
    messages[10] = "RESTART ?";             // pause menu - restart confirmation
    messages[11] = "MAIN MENU";             // pause menu
    messages[12] = const_cast<char*>(about_data.c_str());   // about
    messages[13] = "STATS";                 // menu
    messages[14] = "SAVE";                  // options   
    messages[15] = "COLOR PALETTE";         // options 
    messages[16] = "SCORE: %d";             // hud
    messages[17] = "PLAYING TO %d";         // hud
    messages[18] = "FINAL SCORE";           // postgame
    messages[19] = "TIME: %d:%i";           // postgame
    messages[20] = "TIME: %d:0%i";          // postgame
    messages[21] = "STATS";                 // stats
    messages[23] = "FEATS";                 // stats
    messages[24] = "TOTAL GAMES PLAYED: %d";// stats

    about_file.close();


    Vector2 TEXT_POS;
    TEXT_POS.x = ((SCREEN_WIDTH/2.0f) - fonts[0].baseSize*6.5f);
    TEXT_POS.y = SCREEN_HEIGHT/2.2f;
    float TEXT_SPACING = 3.0f;   




    std::ofstream log_file_write;    // logging
    // RESULT, GAMEMODE, PLAYER SCORE, ENEMY SCORE












    //transition controller
    float transition_counter = 1.0f;                                            // numeric controller
    bool transition_running_title = false;                                      // bool controller

    unsigned char transition_alpha;                                             
    transition_alpha = static_cast<unsigned char>(transition_counter * 100);    // alpha  

    // controls screen transition controller
    bool transition_running_controls = false;
    float controls_alpha = 0;

    // menu controller
    int menu_selected = 0;              // indexes menu_alpha - value '0' used as buffer
    float menu_alpha[6] = { 0 };        // slot 0 used as buffer
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
    player.color = CurrentPalette.COLOR_PONG1;
    player.score = 0;

    // enemy setup
    enemy.position = (Vector2){SCREEN_WIDTH - (SCREEN_WIDTH/10), SCREEN_HEIGHT/2 - enemy.sizeY/2};
    enemy.sizeY = 75.0f;
    enemy.sizeX = 20.0f;
    enemy.speed = 12.0f;
    enemy.color = CurrentPalette.COLOR_PONG2;
    enemy.score = 0;

    //ball setup
    ball.position = (Vector2){SCREEN_WIDTH/2, SCREEN_HEIGHT/2};
    ball.size = 12.5f;
    ball.color = CurrentPalette.COLOR_BALL;
    ball.vel = (Vector2){-8, 14};

    // goal setup
    leftgoal.wall = Left;
    leftgoal.color = CurrentPalette.COLOR_GOAL1;
    leftgoal.width = 10;

    rightgoal.wall = Right;
    rightgoal.color = CurrentPalette.COLOR_GOAL2;
    rightgoal.width = 10;

















    // ----------------------------------------------------------------------------------------------------------------------
    // Startup
    // ----------------------------------------------------------------------------------------------------------------------
    while(!WindowShouldClose()){

        ++frame_count;
        if(frame_count == 600) frame_count = 0;

        if(GameState == ReadTest){
            BeginDrawing();
                ClearBackground(CurrentPalette.COLOR_BG);

                    DrawTextEx(fonts[0], 
                    const_cast<char*>(log_data[0].c_str()),   
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*25.0f, TEXT_POS.y- fonts[0].baseSize*2.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);

                    
                    DrawTextEx(fonts[0], 
                    const_cast<char*>(log_data[1].c_str()),   
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*25.0f, TEXT_POS.y- fonts[0].baseSize*5.0f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                    
                    DrawTextEx(fonts[0], 
                    const_cast<char*>(log_data[2].c_str()),   
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*25.0f, TEXT_POS.y- fonts[0].baseSize*7.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                    
                    DrawTextEx(fonts[0], 
                    const_cast<char*>(log_data[3].c_str()),   
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*25.0f, TEXT_POS.y- fonts[0].baseSize*10.0f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                    
                    DrawTextEx(fonts[0], 
                    const_cast<char*>(log_data[4].c_str()),   
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*25.0f, TEXT_POS.y- fonts[0].baseSize*12.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
            EndDrawing();
        }

        if(GameState == PaletteTest){
            BeginDrawing();

                DrawRectangle(0,0,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_BG);
                DrawRectangle(0,60,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_TEXT);
                DrawRectangle(0,120,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_TEXT_HIGHLIGHT);
                DrawRectangle(0,180,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_TEXT_DISABLED);
                DrawRectangle(0,240,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_TEXT_CONFIRM);
                DrawRectangle(0,300,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_BALL);
                DrawRectangle(0,360,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_PONG1);
                DrawRectangle(0,420,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_GOAL1);
                DrawRectangle(0,480,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_PONG2);
                DrawRectangle(0,540,SCREEN_WIDTH,SCREEN_HEIGHT/10,CurrentPalette.COLOR_GOAL2);

            EndDrawing();
        }

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

                ClearBackground(CurrentPalette.COLOR_BG);

                DrawTexture(keyboardText, 
                SCREEN_WIDTH/2 - 275.0f, 
                SCREEN_HEIGHT/2 - 84.0f, 
                (Color){CurrentPalette.COLOR_TEXT.r, CurrentPalette.COLOR_TEXT.g, CurrentPalette.COLOR_TEXT.b, static_cast<unsigned char>(controls_alpha)});


            EndDrawing();
        }

        if(GameState == Title){

            BeginDrawing();
            
                ClearBackground(CurrentPalette.COLOR_BG);


                // author
                DrawTextEx(fonts[0], 
                messages[0], 
                TEXT_POS, 
                fonts[0].baseSize*4.0f, 
                TEXT_SPACING, 
                (Color){CurrentPalette.COLOR_TEXT.r,
                CurrentPalette.COLOR_TEXT.g,
                CurrentPalette.COLOR_TEXT.b, 
                transition_alpha});

                // button not pressed
                if(frame_count%90<=45 && !transition_running_title){
                    DrawTextEx(fonts[0], 
                    messages[1],  
                    (Vector2){TEXT_POS.x, (TEXT_POS.y + fonts[0].baseSize*3.4f)}, 
                    fonts[0].baseSize*2.0f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r,
                    CurrentPalette.COLOR_TEXT.g,
                    CurrentPalette.COLOR_TEXT.b, 
                    transition_alpha});
                }

                if(frame_count%10<=5 && transition_running_title){
                    DrawTextEx(fonts[0], 
                    messages[1],  
                    (Vector2){TEXT_POS.x, (TEXT_POS.y + fonts[0].baseSize*3.4f)}, 
                    fonts[0].baseSize*2.0f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r,
                    CurrentPalette.COLOR_TEXT.g,
                    CurrentPalette.COLOR_TEXT.b,
                    transition_alpha});
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
            
                ClearBackground(CurrentPalette.COLOR_BG);

                // title
                DrawTextEx(fonts[0], 
                messages[2], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*1.1f, TEXT_POS.y- fonts[0].baseSize*11.0f},
                fonts[0].baseSize*4.0f, 
                TEXT_SPACING, 
                CurrentPalette.COLOR_TEXT);

                DrawTextEx(fonts[0], 
                messages[3], 
                (Vector2){TEXT_POS.x - fonts[0].baseSize*1.0f, TEXT_POS.y- fonts[0].baseSize*6.5f},
                fonts[0].baseSize*2.0f, 
                TEXT_SPACING, 
                CurrentPalette.COLOR_TEXT);

                // selection (4,5,6,7)
                DrawTextEx(fonts[0],                                    
                messages[4], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*3.95f, TEXT_POS.y},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){CurrentPalette.COLOR_TEXT.r, 
                CurrentPalette.COLOR_TEXT.g, 
                CurrentPalette.COLOR_TEXT.b, 
                static_cast<unsigned char>((menu_alpha[1] + 1.0f)* 100)});

                DrawTextEx(fonts[0],                                    
                messages[5], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*2.60f, TEXT_POS.y + fonts[0].baseSize*2.5f},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){CurrentPalette.COLOR_TEXT.r, 
                CurrentPalette.COLOR_TEXT.g, 
                CurrentPalette.COLOR_TEXT.b, 
                static_cast<unsigned char>((menu_alpha[2] + 1.0f)* 100)});

                DrawTextEx(fonts[0],                                    
                messages[6], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*3.5f, TEXT_POS.y + fonts[0].baseSize*5.0f},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){CurrentPalette.COLOR_TEXT.r, 
                CurrentPalette.COLOR_TEXT.g, 
                CurrentPalette.COLOR_TEXT.b, 
                static_cast<unsigned char>((menu_alpha[3] + 1.0f)* 100)});
                
                DrawTextEx(fonts[0],                                    
                messages[13], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*3.35f, TEXT_POS.y + fonts[0].baseSize*7.5f},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){CurrentPalette.COLOR_TEXT.r, 
                CurrentPalette.COLOR_TEXT.g, 
                CurrentPalette.COLOR_TEXT.b, 
                static_cast<unsigned char>((menu_alpha[4] + 1.0f)* 100)});

                DrawTextEx(fonts[0],                                    
                messages[7], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*4.35f, TEXT_POS.y + fonts[0].baseSize*10.0f},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                (Color){CurrentPalette.COLOR_TEXT.r, 
                CurrentPalette.COLOR_TEXT.g, 
                CurrentPalette.COLOR_TEXT.b, 
                static_cast<unsigned char>((menu_alpha[5] + 1.0f)* 100)});


                EndDrawing();

                // start getting input
                if(!menu_first_interacted){
                    if(IsKeyPressed(KEY_DOWN)){         // downarrow - if nothing selected
                        menu_selected = 1;
                        menu_first_interacted = true;
                    }
                    if(IsKeyPressed(KEY_UP)){           // uparrow - if nothing selected
                        menu_selected = 5;
                        menu_first_interacted = true;
                    }   
                }
                else{
                    if(IsKeyPressed(KEY_DOWN)) ++menu_selected; // downarrow
                    if(IsKeyPressed(KEY_UP)) --menu_selected;   // uparrow

                    if(menu_selected < 1) menu_selected = 5;    //safeguard - loop to end
                    if(menu_selected > 5) menu_selected = 1;    //safeguard - loop to start
                    

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 1){            // option 1 - start game
                        GameState = Ingame;
                        GameStateSub = Singleplayer;
                        menu_selected = 0;
                        menu_first_interacted = false;
                    }
                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 2){
                        GameStateSub = Options;
                        menu_selected = 0;
                        menu_first_interacted = false;
                    }                                                            

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 3){            // option 3 - open about page
                        GameStateSub = About;
                        menu_selected = 0;
                        menu_first_interacted = false;
                    }
                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 4){            // option 4 - open stats page

                        GameStateSub = Stats;
                        menu_selected = 0;
                        menu_first_interacted = false;

                    }   

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 5){            // option 5 - close window
                        GameState = Controls;
                        GameStateSub = None;
                        for(int i = 0; i< MAX_FONTS; i++) UnloadFont(fonts[i]);
                        AlreadyClosed = true;
                        CloseWindow();
                    }
                }
                // set option alpha
                for(int i = 1; i <= 5; i++){
                    menu_alpha[i] = 0.0f;
                }
                menu_alpha[menu_selected] = 1.5f;

            }

            if(GameStateSub == About){
                // todo

                BeginDrawing();
                
                    ClearBackground(CurrentPalette.COLOR_BG);

                    // title
                    DrawTextEx(fonts[0], 
                    messages[2], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*1.5f, TEXT_POS.y- fonts[0].baseSize*12.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);

                    // about description
                    DrawTextEx(fonts[0], 
                    messages[12], 
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*12.3f, TEXT_POS.y- fonts[0].baseSize*7.5f},
                    fonts[0].baseSize*1.5f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);

                    // back to menu
                    DrawTextEx(fonts[0], 
                    messages[11], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*3.35f, TEXT_POS.y + fonts[0].baseSize*13.5f},
                    fonts[0].baseSize*1.5f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r, CurrentPalette.COLOR_TEXT.g, CurrentPalette.COLOR_TEXT.b, 255});

                
                EndDrawing();

                if(IsKeyPressed(KEY_USER_PAUSE) || IsKeyPressed(KEY_USER_SELECT)){
                    GameState = Menu;
                    GameStateSub = None;
                }
            }
        
            if(GameStateSub == Options){

                BeginDrawing();
                
                    ClearBackground(CurrentPalette.COLOR_BG);

                    // OPTIONS
                    DrawTextEx(fonts[0], 
                    messages[5], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*1.1f, TEXT_POS.y- fonts[0].baseSize*11.0f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);

                    // SAVE
                    DrawTextEx(fonts[0],                                    
                    messages[14], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*3.3f, TEXT_POS.y + fonts[0].baseSize*10.0f},
                    fonts[0].baseSize*3.0f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r, 
                    CurrentPalette.COLOR_TEXT.g, 
                    CurrentPalette.COLOR_TEXT.g, 
                    static_cast<unsigned char>((menu_alpha[4] + 1.0f)* 100)});

                    // MAIN MENU
                    DrawTextEx(fonts[0],                                    
                    messages[11], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*0.3f, TEXT_POS.y + fonts[0].baseSize*12.5f},
                    fonts[0].baseSize*3.0f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r, 
                    CurrentPalette.COLOR_TEXT.g, 
                    CurrentPalette.COLOR_TEXT.g, 
                    static_cast<unsigned char>((menu_alpha[5] + 1.0f)* 100)});


                EndDrawing();

                // start getting input
                if(!menu_first_interacted){
                    if(IsKeyPressed(KEY_DOWN)){         // downarrow - if nothing selected
                        menu_selected = 1;
                        menu_first_interacted = true;
                    }
                    if(IsKeyPressed(KEY_UP)){           // uparrow - if nothing selected
                        menu_selected = 5;
                        menu_first_interacted = true;
                    }   
                }
                else{
                    if(IsKeyPressed(KEY_DOWN)) ++menu_selected; // downarrow
                    if(IsKeyPressed(KEY_UP)) --menu_selected;   // uparrow

                    if(menu_selected < 1) menu_selected = 5;    //safeguard - loop to end
                    if(menu_selected > 5) menu_selected = 1;    //safeguard - loop to start
                    

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

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 4){            // option 4 - save settings

                    }

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 5){            // option 5 - back to menu
                        GameState = Menu;
                        GameStateSub = None;
                        menu_selected = 0;
                        menu_first_interacted = false;
                    }
                }
                
                // set option alpha
                for(int i = 1; i <= 5; i++){
                    menu_alpha[i] = 0.0f;
                }
                menu_alpha[menu_selected] = 1.5f;

            }
        
            if(GameStateSub == Stats){
                
                BeginDrawing();

                    ClearBackground(CurrentPalette.COLOR_BG);

                    // title
                    DrawTextEx(fonts[0], 
                    messages[21], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*2.5f, TEXT_POS.y- fonts[0].baseSize*12.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);

                    // TOTAL GAMES PLAYED
                    DrawTextEx(fonts[0], 
                    TextFormat(messages[24], total_games), 
                    (Vector2){TEXT_POS.x - 175 + fonts[0].baseSize*2.5f, TEXT_POS.y + 75- fonts[0].baseSize*12.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);

                    for(int i = 0; i < 5; i++){
                        DrawRectangle(TEXT_POS.x + 10 - fonts[0].baseSize*11.5f,
                        TEXT_POS.y + 150 - fonts[0].baseSize*2.5f*i,
                        log_data[i].length()*0.85f - 30,
                        fonts[0].baseSize*2.0f,
                        CurrentPalette.COLOR_GOAL2);

                        DrawTextEx(fonts[0], 
                        const_cast<char*>(log_data[i].c_str()),   
                        (Vector2){TEXT_POS.x + 10 - fonts[0].baseSize*11.5f, TEXT_POS.y + 150 - fonts[0].baseSize*2.5f*i},
                        fonts[0].baseSize*2.0f, 
                        TEXT_SPACING, 
                        CurrentPalette.COLOR_TEXT_CONFIRM);
                        
                    }




                    // MAIN MENU
                    DrawTextEx(fonts[0],                                    
                    messages[11], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*1.0f, TEXT_POS.y + fonts[0].baseSize*12.5f},
                    fonts[0].baseSize*3.0f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r, 
                    CurrentPalette.COLOR_TEXT.g, 
                    CurrentPalette.COLOR_TEXT.b, 
                    static_cast<unsigned char>((menu_alpha[1] + 1.0f)* 100)});

                    // FEATS
                    DrawTextEx(fonts[0],                                    
                    messages[23], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*3.4f, TEXT_POS.y + fonts[0].baseSize*15.0f},
                    fonts[0].baseSize*3.0f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r, 
                    CurrentPalette.COLOR_TEXT.g, 
                    CurrentPalette.COLOR_TEXT.b, 
                    static_cast<unsigned char>((menu_alpha[2] + 1.0f)* 100)});

                EndDrawing();

                // start getting input
                if(!menu_first_interacted){
                    if(IsKeyPressed(KEY_DOWN)){         // downarrow - if nothing selected
                        menu_selected = 1;
                        menu_first_interacted = true;
                    }
                    if(IsKeyPressed(KEY_UP)){           // uparrow - if nothing selected
                        menu_selected = 2;
                        menu_first_interacted = true;
                    }   
                }
                else{
                    if(IsKeyPressed(KEY_DOWN)) ++menu_selected; // downarrow
                    if(IsKeyPressed(KEY_UP)) --menu_selected;   // uparrow

                    if(menu_selected < 1) menu_selected = 2;    //safeguard - loop to end
                    if(menu_selected > 2) menu_selected = 1;    //safeguard - loop to start
                    

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 1){            // option 1 - back to menu
                        GameState = Menu;
                        GameStateSub = None;
                        menu_selected = 0;
                        menu_first_interacted = false;
                    }

                    if(IsKeyPressed(KEY_USER_SELECT) && menu_selected == 2){            // option 2 - go to feats
                        GameState = Menu;
                        GameStateSub = None;
                        menu_selected = 0;
                        menu_first_interacted = false;
                    }      

                }
                
                // set option alpha
                for(int i = 1; i <= 2; i++){
                    menu_alpha[i] = 0.0f;
                }
                menu_alpha[menu_selected] = 1.5f;



            }

        }
    
        if(GameState == Ingame){


            if(!paused){
                timeCurrentSeconds++;
                if(timeCurrentSeconds/60 >= 60){
                    ++timeCurrentMinutes;
                    timeCurrentSeconds = 0;
                }

                if(IsKeyDown(KEY_S)) player.position.y = player.position.y + player.speed;
                if(IsKeyDown(KEY_W)) player.position.y = player.position.y - player.speed;

                if(GameStateSub == Multiplayer){
                    if(IsKeyDown(KEY_DOWN)) enemy.position.y = enemy.position.y + enemy.speed;
                    if(IsKeyDown(KEY_UP)) enemy.position.y = enemy.position.y - enemy.speed;
                }

                // revolutionary ai
                if(GameStateSub == Singleplayer){
                    if(ball.position.x > SCREEN_WIDTH/1.45f && ball.lastTouched != 2){
                        if(ball.position.y < enemy.position.y + enemy.sizeY/2) enemy.position.y = enemy.position.y - enemy.speed;
                        if(ball.position.y > enemy.position.y + enemy.sizeY/2) enemy.position.y = enemy.position.y + enemy.speed;
                    }
                }

                ball.position.x += ball.vel.x;
                ball.position.y += ball.vel.y;

                // win by points
                if(player.score >= scoreMax){
                    GameState = Postgame;
                    GameStateResult = Win;
                }
                if(enemy.score >= scoreMax){
                    GameState = Postgame;
                    GameStateResult = Loss;
                }
            
                    // win by time
                if(timeMax > 0){    // if parameter is even set
                    if(timeCurrentSeconds/60 + timeCurrentMinutes*60 >= timeMax){
                        // win by points
                        if(player.score > enemy.score){
                            GameState = Postgame;
                            GameStateResult = Win;
                        }
                        if(enemy.score > player.score){
                            GameState = Postgame;
                            GameStateResult= Loss;
                        }
                    }
                }
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
                ball.lastTouched = 1;
                // if(ball.position.y < player.position.y || ball.position.y > player.position.y + player.sizeY) ball.vel.y = 0 - ball.vel.y;
            }

            // ENEMY collision handler
            if(CheckCollisionCircleRec(ball.position, ball.size, (Rectangle){enemy.position.x, enemy.position.y, player.sizeX, player.sizeY})){
                ball.position.x = enemy.position.x - ball.size;
                ball.vel.x = 0 - ball.vel.x;
                ball.lastTouched = 2;
                // if(ball.position.y < enemy.position.y || ball.position.y > enemy.position.y + enemy.sizeY) ball.vel.y = 0 - ball.vel.y;
            }

            // PLAYER GOAL collision handler
            if(CheckCollisionCircleRec(ball.position, ball.size, (Rectangle){0,0,leftgoal.width,SCREEN_HEIGHT})){
                ball.position.x = SCREEN_WIDTH/2;
                ball.position.y = SCREEN_HEIGHT/2;
                ball.vel.x = 0 - ball.vel.x;
                ++enemy.score;

                ball.lastTouched = 0;
            }

            // ENEMY GOAL collision handler
            if(CheckCollisionCircleRec(ball.position, ball.size, (Rectangle){SCREEN_WIDTH-leftgoal.width,0,leftgoal.width,SCREEN_HEIGHT})){
                ball.position.x = SCREEN_WIDTH/2;
                ball.position.y = SCREEN_HEIGHT/2;
                ball.vel.x = 0 - ball.vel.x;
                ++player.score;

                ball.lastTouched = 0;
            }
            

            BeginDrawing();

                ClearBackground(CurrentPalette.COLOR_BG);

                // hud

                // player score
                DrawTextEx(fonts[0], 
                TextFormat(messages[16],player.score), 
                (Vector2){50, 10},
                fonts[0].baseSize*2.0f, 
                TEXT_SPACING, 
                CurrentPalette.COLOR_TEXT);

                // enemy score
                DrawTextEx(fonts[0], 
                TextFormat(messages[16],enemy.score), 
                (Vector2){SCREEN_WIDTH-15-fonts[0].baseSize*8.0f, 10},
                fonts[0].baseSize*2.0f, 
                TEXT_SPACING, 
                CurrentPalette.COLOR_TEXT);

                // PLAYING TO:
                DrawTextEx(fonts[0], 
                TextFormat(messages[17], scoreMax), 
                (Vector2){SCREEN_WIDTH/2 - fonts[0].baseSize*4.4f, 10},
                fonts[0].baseSize*2.0f, 
                TEXT_SPACING, 
                CurrentPalette.COLOR_TEXT);

                // 0:01
                if(timeCurrentSeconds/60<10){
                    DrawTextEx(fonts[0], 
                    TextFormat("%i:0%d",timeCurrentMinutes, timeCurrentSeconds/60), 
                    (Vector2){SCREEN_WIDTH/2 - fonts[0].baseSize*0.8f, 38.0f},
                    fonts[0].baseSize*2.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                }
                // 0:10
                else{
                    DrawTextEx(fonts[0], 
                    TextFormat("%i:%d",timeCurrentMinutes, timeCurrentSeconds/60), 
                    (Vector2){SCREEN_WIDTH/2 - fonts[0].baseSize*0.8f, 38.0f},
                    fonts[0].baseSize*2.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                }

                DrawRectangle(player.position.x, player.position.y, player.sizeX, player.sizeY, player.color);  // player
                DrawRectangle(0,0,rightgoal.width,SCREEN_HEIGHT,leftgoal.color);                                // playergoal
                DrawRectangle(enemy.position.x, enemy.position.y, enemy.sizeX, enemy.sizeY, enemy.color);       // enemy
                DrawRectangle(SCREEN_WIDTH-leftgoal.width,0,leftgoal.width,SCREEN_HEIGHT,rightgoal.color);      // enemygoal
                DrawCircle(ball.position.x, ball.position.y, ball.size, ball.color);                            // ball

                if(paused){

                    DrawTextEx(fonts[0],                                    
                    messages[8], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*2.67f, TEXT_POS.y},
                    fonts[0].baseSize*3.0f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r, CurrentPalette.COLOR_TEXT.g, CurrentPalette.COLOR_TEXT.b, static_cast<unsigned char>((menu_alpha[1] + 1.0f)* 100)});

                    if(!restart_confirm){                        
                        DrawTextEx(fonts[0],                                    
                        TextFormat(messages[10]), 
                        (Vector2){TEXT_POS.x + fonts[0].baseSize*2.4f, TEXT_POS.y + fonts[0].baseSize*2.5f},
                        fonts[0].baseSize*3.0f, 
                        TEXT_SPACING, 
                        (Color){CurrentPalette.COLOR_TEXT_CONFIRM.r, CurrentPalette.COLOR_TEXT_CONFIRM.g, CurrentPalette.COLOR_TEXT_CONFIRM.b, static_cast<unsigned char>((menu_alpha[2] + 1.0f)* 100)});
                    }
                    else{
                        DrawTextEx(fonts[0],                                    
                        TextFormat(messages[9]), 
                        (Vector2){TEXT_POS.x + fonts[0].baseSize*2.4f, TEXT_POS.y + fonts[0].baseSize*2.5f},
                        fonts[0].baseSize*3.0f, 
                        TEXT_SPACING, 
                        (Color){CurrentPalette.COLOR_TEXT.r, CurrentPalette.COLOR_TEXT.g, CurrentPalette.COLOR_TEXT.b, static_cast<unsigned char>((menu_alpha[2] + 1.0f)* 100)});
                    }
                    
                    DrawTextEx(fonts[0],                                    
                    messages[11], 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*1.25f, TEXT_POS.y + fonts[0].baseSize*5.0f},
                    fonts[0].baseSize*3.0f, 
                    TEXT_SPACING, 
                    (Color){CurrentPalette.COLOR_TEXT.r, CurrentPalette.COLOR_TEXT.g, CurrentPalette.COLOR_TEXT.b, static_cast<unsigned char>((menu_alpha[3] + 1.0f)* 100)});
                }

            EndDrawing();

            if(IsKeyPressed(KEY_USER_PAUSE)){
                paused = !paused;   
                menu_first_interacted = false;
                menu_selected = false;
                restart_confirm = true;
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
                        GameStateSub = None;
                    }
                }

                // set option alpha
                for(int i = 1; i <= 4; i++){
                    menu_alpha[i] = 0.0f;
                }
                menu_alpha[menu_selected] = 1.5f;
            }

        }

        if(GameState == Postgame){
            BeginDrawing();

                ClearBackground(CurrentPalette.COLOR_BG);

                // PLAYER ONE WINS
                if(GameStateResult == Win){
                    DrawTextEx(fonts[0], 
                    "PLAYER ONE WINS", 
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*4.0f, TEXT_POS.y - fonts[0].baseSize*11.0f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                }
                // PLAYER TWO WINS
                if(GameStateResult == Loss){
                    DrawTextEx(fonts[0], 
                    "PLAYER TWO WINS", 
                    (Vector2){TEXT_POS.x - fonts[0].baseSize*4.0f, TEXT_POS.y- fonts[0].baseSize*11.0f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                }

                // FINAL SCORE
                DrawTextEx(fonts[0], 
                messages[18], 
                (Vector2){TEXT_POS.x - fonts[0].baseSize*0.7f, TEXT_POS.y - fonts[0].baseSize*2.0f},
                fonts[0].baseSize*4.0f, 
                TEXT_SPACING, 
                CurrentPalette.COLOR_TEXT);

                // 3:2
                DrawTextEx(fonts[0], 
                TextFormat("%i - %d",player.score,enemy.score), 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*3.3f, TEXT_POS.y + fonts[0].baseSize*3.0f},
                fonts[0].baseSize*4.0f, 
                TEXT_SPACING, 
                CurrentPalette.COLOR_TEXT_HIGHLIGHT);

                // TIME: 

                if(timeCurrentSeconds/60 < 10){
                    DrawTextEx(fonts[0], 
                    TextFormat(messages[20],timeCurrentMinutes,timeCurrentSeconds/60), 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*0.35f, TEXT_POS.y + fonts[0].baseSize*7.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                }
                else{
                    DrawTextEx(fonts[0], 
                    TextFormat(messages[19],timeCurrentMinutes,timeCurrentSeconds/60), 
                    (Vector2){TEXT_POS.x + fonts[0].baseSize*0.35f, TEXT_POS.y + fonts[0].baseSize*7.5f},
                    fonts[0].baseSize*4.0f, 
                    TEXT_SPACING, 
                    CurrentPalette.COLOR_TEXT);
                }

                // MAIN MENU
                DrawTextEx(fonts[0],                                    
                messages[11], 
                (Vector2){TEXT_POS.x + fonts[0].baseSize*1.5f, TEXT_POS.y + fonts[0].baseSize*12.5f},
                fonts[0].baseSize*3.0f, 
                TEXT_SPACING, 
                CurrentPalette.COLOR_TEXT_HIGHLIGHT); 

            EndDrawing();

            // start getting input
            if(IsKeyPressed(KEY_USER_SELECT)){

                log_file_write.open("resources/data/log.txt", std::fstream::app);
                log_file_write << GameStateResult << "," << GameStateSub  << "," << player.score << "-" << enemy.score << "\n" << "\0";
                log_file_write.close();
                
                GameState = Menu;
                GameStateSub = None;
                GameStateResult = None_Result;

                player.score = 0;
                enemy.score = 0;
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