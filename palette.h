#include "raylib.h"

#define MAX_PALETTES            5

typedef struct Palette{
    int id;                     // ID
    Color COLOR_BG;             // bg
    Color COLOR_TEXT;           // text
    Color COLOR_TEXT_HIGHLIGHT; // texthigh
    Color COLOR_TEXT_DISABLED;  // textdis
    Color COLOR_TEXT_CONFIRM;   // textcon
    Color COLOR_BALL;           // ball
    Color COLOR_PONG1;          // pong1 - default player pong
    Color COLOR_GOAL1;          // goal1 - default player goal
    Color COLOR_PONG2;          // pong2 - default enemy pong
    Color COLOR_GOAL2;          // goal2 - default enemy goal
} Palette;

// default color scheme
Palette PALETTE_DEFAULT = {
    0,
    (Color){70, 70, 70, 250},       // bg       // dark grey
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

// original gameboy
Palette PALETTE_GAMEBOY{    // I ain't good at all green, sorry
    1,
    (Color){7, 28, 7, 250},         // bg       // some green  
    (Color){50, 90, 50, 100},       // text     // some green  
    (Color){80, 90, 7, 250},        // texthigh // some green  
    (Color){7, 28, 7, 200},         // textdis  // some green  
    (Color){25, 100, 90, 100},      // textcon  // some green  
    (Color){80, 90, 7, 250},        // ball     // some green   
    (Color){80, 90, 7, 200},        // pong1    // some green  
    (Color){80, 90, 7, 100},        // goal1    // some green  
    (Color){25, 100, 90, 200},      // pong2    // some green  
    (Color){25, 100, 90, 100}       // goal2    // some green  
};

// widely successful game console virtual boy
Palette PALETTE_VIRTUALBOY{
    2,
    (Color){10, 0, 0, 250},         // bg       // black
    (Color){165, 0, 0, 100},        // text     // dark red
    (Color){170, 0, 0, 250},        // texthigh // light muddy red
    (Color){10, 0, 0, 250},         // textdis  // black
    (Color){255, 90, 90, 100},      // textcon  // pinkish red
    (Color){230, 0, 0, 250},        // ball     // vibrant red
    (Color){255, 0, 0, 250},        // pong1    // vibrant red
    (Color){255, 0, 0, 250},        // goal1    // vibrant red
    (Color){255, 90, 90, 250},      // pong2    // pinkish red
    (Color){255, 90, 90, 250}       // goal2    // pinkish red 
};

// 3 bit rgb
Palette PALETTE_3RGB{
    3,
    (Color){0, 0, 0, 250},          // bg       // black   
    (Color){0, 255, 255, 100},      // text     // cyan
    (Color){255, 255, 255, 250},    // texthigh // white
    (Color){0, 0, 255, 250},        // textdis  // blue
    (Color){255, 0, 255, 100},      // textcon  // magenta
    (Color){255, 255, 0, 250},      // ball     // yellow
    (Color){0, 255, 0, 250},        // pong1    // green
    (Color){0, 255, 0, 250},        // goal1    // green
    (Color){255, 0, 0, 250},        // pong2    // red
    (Color){255, 0, 0, 250}         // goal2    // red
};

// NYX8
Palette PALETTE_NYX8{
    4,
    (Color){8, 20, 30, 250},        // bg       // dark blue-violet-ish  
    (Color){170, 170, 170, 100},    // text     // grey
    (Color){255, 214, 190, 250},    // texthigh // light beige
    (Color){32, 57, 79, 250},       // textdis  // dark blue-grey-ish
    (Color){240, 240, 240, 100},    // textcon  // grey
    (Color){255, 214, 190, 250},    // ball     // light beige
    (Color){170, 170, 170, 250},    // pong1    // grey
    (Color){170, 170, 170, 250},    // goal1    // grey
    (Color){78, 73, 95, 250},       // pong2    // dark blue-grey-ish
    (Color){78, 73, 95, 250}        // goal2    // dark blue-grey-ish
};