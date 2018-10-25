/////////////////////////////////////////////////////////////////////
//                                                                 //
//                          EloChess                               //
//                                                                 //
//              Written by Carl Cochet 22/10/2018                  //
//                                                                 //
/////////////////////////////////////////////////////////////////////


#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <iomanip>

int max_index(std::vector<double> vector);

int main() {

    std::vector<std::string> players;
    std::vector<std::string> opening;
    std::vector<double> opCount;
    std::vector<double> elos(0), score(0);
    //  = {3178, 3144, 3143, 3323, 3198, 3461, 3284, 3399, 3282, 3112, 3140, 3040, 3139, 3089,
    //                                3184, 3272, 3122, 3098, 3350, 3125, 3027, 3223, 3184, 3267, 3197, 3403, 3114, 3012,
    //                                3161, 3157, 3183, 3280, 3182}

    std::vector<int> wins(0), loss(0), draws(0), totalMatch(0), resultSum(3), winWhite(0), lossWhite(0), drawWhite(0);
    int k(10), matchCount(0), white(-1), black(-1), matchMaxMove(0), maxMove(0), readChoice(0), strValue(0);
    bool nextMatch(true), validMatch(false);
    std::string gameStart, game, lineCheck, substr;

    std::cout << "Welcome to EloChess! This program calculates elo evolution for players from a tournament file and "
                 "also gives some additional statistics.\n" << std::endl;

    while(readChoice != 1 && readChoice!= 2) {
        std::cout << "Do you want to get players elo from the tournament pgn data (might not be correct), or do you want to"
                   " read the elo.elo file (you can fix elo manually there)? (type 1 or 2)";
        std::cin >> readChoice;
    }

    std::cout << std::endl;

    if(readChoice == 1){
        std::ifstream file("../tournament.pgn");
        if(file) {
            std::string line;

            while (getline(file, line)) {
                if(line.length() >= 6) {
                    gameStart = line.substr(0, 6);
                }
                if(line.length() >= 2){
                    game = line.substr(0, 2);
                }
                if(gameStart == "[Event") {
                    if (!nextMatch) {
                        nextMatch = true;
                    }

                } else if(game == "1."){
                    nextMatch = false;
                    white = -1;
                    black = -1;
                } else {
                    if(nextMatch) {

                        if (line.length() >= 7) {
                            lineCheck = line.substr(0, 7);
                        }

                        if (lineCheck == "[White " || lineCheck == "[Black ") {
                            substr = line.substr(8, line.length() - 10);

                            if (!(std::find(players.begin(), players.end(), substr) != players.end())) {
                                players.push_back(substr);
                                elos.push_back(0);
                                wins.push_back(0);
                                loss.push_back(0);
                                draws.push_back(0);
                                score.push_back(0);
                                totalMatch.push_back(0);
                                winWhite.push_back(0);
                                lossWhite.push_back(0);
                                drawWhite.push_back(0);
                            }
                            ptrdiff_t pos = std::distance(players.begin(),
                                                          std::find(players.begin(), players.end(), substr));
                            if (pos < players.size()) {
                                if (lineCheck == "[White ") {
                                    white = pos;
                                } else {
                                    black = pos;
                                }
                            }
                        }
                        if(lineCheck == "[WhiteE"){
                            substr = line.substr(11, line.length()-13);
                            strValue = atoi(substr.c_str());
                            if(strValue > elos[white]){
                                elos[white] = strValue;
                            }
                        }
                        if(lineCheck == "[BlackE"){
                            substr = line.substr(11, line.length()-13);
                            strValue = atoi(substr.c_str());
                            if(strValue > elos[black]){
                                elos[black] = strValue;
                            }
                        }
                    }
                }
            }
        }
        file.close();
    } else {
        std::ifstream file("../elo.elo");
        if(file) {
            std::string line;
            while (getline(file, line)) {
                elos.push_back(atoi(line.c_str()));
            }
        }
        file.close();
    }



    std::ifstream file("../tournament.pgn");

    if(file){
        std::string line;

        while(getline(file, line)){
            if(line.length() >= 6) {
                gameStart = line.substr(0, 6);
            }
            if(line.length() >= 2){
                game = line.substr(0, 2);
            }
            if(gameStart == "[Event") {
                if (!nextMatch) {
                    matchCount++;
                    nextMatch = true;
                }

            } else if(game == "1."){
                nextMatch = false;
                white = -1;
                black = -1;
            } else {

                if(nextMatch){

                    if(line.length() >= 7) {
                        lineCheck = line.substr(0, 7);
                    }

                    if(lineCheck == "[White " || lineCheck == "[Black "){
                        substr = line.substr(8, line.length()-10);

                        if(!(std::find(players.begin(), players.end(), substr) != players.end())){
                            players.push_back(substr);

                            wins.push_back(0);
                            loss.push_back(0);
                            draws.push_back(0);
                            score.push_back(0);
                            totalMatch.push_back(0);
                            winWhite.push_back(0);
                            lossWhite.push_back(0);
                            drawWhite.push_back(0);
                        }
                        ptrdiff_t pos = std::distance(players.begin(),
                                std::find(players.begin(), players.end(), substr));
                        if(pos < players.size()) {
                            if(lineCheck == "[White "){
                                white = pos;
                            } else {
                                black = pos;
                            }
                        }
                    }
                    if(lineCheck == "[Result") {
                        substr = line.substr(9, line.length() - 11);

                        double w = 0;

                        if (substr == "1/2-1/2") {
                            draws[white]++;
                            draws[black]++;
                            w = 0.5;
                            resultSum[2]++;
                            drawWhite[white]++;
                        } else if (substr == "1-0") {
                            wins[white]++;
                            loss[black]++;
                            w = 1;
                            resultSum[0]++;
                            winWhite[white]++;
                        } else if (substr == "0-1") {
                            wins[black]++;
                            loss[white]++;
                            w = 0;
                            resultSum[1]++;
                            lossWhite[white]++;
                        } else {
                            matchCount--;
                            validMatch = false;
                        }

                        if (validMatch) {
                            double d = elos[white] - elos[black];

                            score[white] += w;
                            score[black] += 1 - w;
                            totalMatch[white]++;
                            totalMatch[black]++;

                            if (d > 400) {
                                d = 400;
                            }

                            double pd = 1 / (1 + pow(10, -d / 400));

                            elos[white] += k * (w - pd);
                            elos[black] += k * (pd - w);
                        }

                        validMatch = true;
                    }

                    if(lineCheck == "[PlyCou"){
                        if(line.length() > 11) {
                            substr = line.substr(11, line.length() - 13);
                        }

                        int moveNum = (atoi( substr.c_str() )) / 2;
                        if(moveNum > maxMove){
                            maxMove = moveNum;
                            matchMaxMove = matchCount;
                        }
                    }

                    if(lineCheck == "[Openin"){
                        if(line.length() > 10) {
                            substr = line.substr(10, line.length() - 12);
                        }

                        if(!(std::find(opening.begin(), opening.end(), substr) != opening.end())){
                            opening.push_back(substr);
                            opCount.push_back(1);
                        } else {
                            ptrdiff_t opIndex = std::distance(opening.begin(),
                                                              std::find(opening.begin(), opening.end(), substr));
                            opCount[opIndex]++;
                        }
                    }
                }
            }
        }
    } else {
        std::cout << "Error: File not found." << std::endl;
    }


    while(opening.size() > 0){
        int i = max_index(opCount);
        std::cout << std::setw(50) << opening[i] << " played " << std::setw(3) << opCount[i] << " times." << std::endl;
        opening.erase(opening.begin() + i);
        opCount.erase(opCount.begin() + i);
    }

    std::cout << "\n" << "-----------------------------------------------------------------------------------------" <<
              "---------------\n" << std::endl;

    while(players.size() > 0){
        int i = max_index(score);

        std::cout << std::setw(19) << players[i] << " (" << std::setw(7) << elos[i] << ") | Wins: " << std::setw(2) <<
                  wins[i] << " (" << std::setw(2) << winWhite[i] << " " << std::setw(2) << wins[i] - winWhite[i] <<
                  ") | Loss: " << std::setw(2) << loss[i] << " (" << std::setw(2) << lossWhite[i] << " " <<
                  std::setw(2) << loss[i] - lossWhite[i] << ") | Draws: " << std::setw(2) << draws[i] << " (" <<
                  std::setw(2) << drawWhite[i] << " " << std::setw(2) << draws[i] - drawWhite[i] <<
                  ") | Score: " << std::setw(4) << score[i] << "/" << std::setw(2) << totalMatch[i] << std::endl;

        players.erase(players.begin() + i);
        elos.erase(elos.begin() + i);
        wins.erase(wins.begin() + i);
        loss.erase(loss.begin() + i);
        draws.erase(draws.begin() + i);
        score.erase(score.begin() + i);
        totalMatch.erase(totalMatch.begin() + i);
        winWhite.erase(winWhite.begin() + i);
        lossWhite.erase(lossWhite.begin() + i);
        drawWhite.erase(drawWhite.begin() + i);
    }

    std::cout << std::endl <<"Longest game: Game " << matchMaxMove << " with " << maxMove <<
              " moves." << std::endl;

    std::cout << "Games: " << matchCount << " | White wins: " << resultSum[0] << " | Black wins: " << resultSum[1] <<
              " | Draws: " << resultSum[2] << std::endl << std::endl;

    //printf("Press enter to exit.\n");
    system("pause");
    return 0;
}

int max_index(std::vector<double> vector) {
    double maxi = -1;
    int index = 0;
    if(vector.size() > 0) {
        for (int i(0); i < vector.size(); i++) {
            if(vector[i] > maxi){
                maxi = vector[i];
                index = i;
            }
        }
    }
    return index;
}
