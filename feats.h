#include <fstream>
#include "raylib.h"

#define MAX_FEATS   4

typedef struct Feat{
    int id;               // id - used in stats screen
    const char* name;           // display name
    const char* description;    // display desc
    const char* reward;
} Feat;

Feat feat0pongmalong = (Feat){0, "Pong Malong", "In singleplayer, score 5 points in under 60 seconds", "Gameboy color palette"};
Feat feat1socialpong = (Feat){1, "Social Pong", "In multiplayer, bounce the ball 20 times without scoring a single point", "Virtualboy color palette"};
Feat feat2flawless = (Feat){2, "Flawless", "In singleplayer, win by time in a 2 minute or longer match without losing a point", "3-bit RGB color palette"};
Feat feat3buzzerpinger = (Feat){3, "Buzzer Pinger", "In singleplayer, win by points in the last 5 seconds of a 1 minute or longer match", "NYX8 color palette"};


// returns a string of 0's and 1's from feats.txt
std::string GetFeats(){         
    std::ifstream feats_file("resources/data/feats.txt");
    std::string line;
    std::string feats_data = "";

    if(feats_file.is_open()){
        std::string line;
        while(std::getline(feats_file, line)){

            feats_data += line + "\n";
        }
        feats_file.close();
    }
    else{
        feats_data = "FERROR";
    }

    return feats_data;
}

// marks single feat as unlocked in feats.txt if not already unlocked
void UnlockFeat(int featId){

    std::string feats_unlock = GetFeats();

    if(feats_unlock != "FERROR"){           // if read was successful

        std::ofstream feats_file_write;

        if(feats_unlock[featId] != '1'){    // check if not already unlocked

            feats_unlock[featId] = '1';

            feats_file_write.open("resources/data/feats.txt",std::fstream::trunc);
            feats_file_write << feats_unlock << "\0";
            feats_file_write.close();
        }
    }
}

// checks for unlock conditions of every feat
void FeatTest(int playerScore, int enemyScore, int maxScore, int timeS, int timeM, int timeMax, int gamemode, int gameresult, int ballbounce){
    if(gamemode == 1 && timeM < 1 && playerScore >= 5) UnlockFeat(0);
    if(gamemode == 2 && playerScore <= 0 && enemyScore <= 0 && ballbounce == 20) UnlockFeat(1);
    if(gamemode == 1 && timeM >= 2 && gameresult == 1 && enemyScore <= 0) UnlockFeat(2);
    if(gamemode == 1 && (timeM*60) + (timeS/60) > timeMax - 5 && timeMax >= 60 && gameresult == 1 && playerScore == maxScore) UnlockFeat(3);
}