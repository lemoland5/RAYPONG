#include "raylib.h"

typedef struct Palette{
    Color COLOR_BG;
    Color COLOR_TEXT;
    Color COLOR_TEXT_HIGHLIGHT;
    Color COLOR_TEXT_DISABLED;
    Color COLOR_TEXT_CONFIRM;
    Color COLOR_BALL;
    Color COLOR_PONG1;
    Color COLOR_GOAL1;
    Color COLOR_PONG2;
    Color COLOR_GOAL2;
} Palette;

Palette PALETTE_DEFAULT = {
    (Color){70, 70, 70, 100},       // bg       // dark grey
    (Color){200, 200, 200, 100},    // text     // dark grey - slightly brighter
    (Color){200, 200, 200, 250},    // texthigh // white
    (Color){200, 100, 100, 100},    // textdis  // dark red
    (Color){200, 255, 200, 100},    // textcon  // pale green
    (Color){200, 200, 200, 255},    // ball     // white
    (Color){100, 200, 150, 255},    // pong1    // pale green
    (Color){75,200,75,255},         // goal1    // lime green
    (Color){100, 150, 200, 255},    // pong2    // pale blue
    (Color){75,75,200,255},         // goal2    // dark blue
};

Palette PALETTE_GAMEBOY{
    (Color){7, 28, 7 , 100},        // bg       
    (Color){50, 90, 50, 100},       // text    
    (Color){80, 90, 7, 250},        // texthigh
    (Color){7, 28, 7 , 200},        // textdis  
    (Color){25, 100, 90, 100},      // textcon  
    (Color){80, 90, 7, 250},        // ball     
    (Color){80, 90, 7, 200},        // pong1   
    (Color){80, 90, 7, 100},        // goal1    
    (Color){25, 100, 90, 200},      // pong2    
    (Color){25, 100, 90, 100},      // goal2    
};