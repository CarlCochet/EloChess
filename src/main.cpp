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

    // Variables init
    // Players stores the players names, opening stores the openings names
    std::vector<std::string> players, opening;

    // opCount counts the use of an opening, elos stores the elos data, score stores the total results
    std::vector<double> opCount, elos(0), score(0);

    // wins, count the wins, loss counts the loss, draws counts the draws. White versions add the side information
    // totalMatch counts the matches per player, resultSum counts wins/loss/draws of full tournament
    std::vector<int> wins(0), loss(0), draws(0), totalMatch(0), resultSum(3), winWhite(0), lossWhite(0), drawWhite(0);

    // k is the max elo earn per match, matchCount counts matches of full tourney, white and black are indexes for array
    // matchMaxMove stores index of longest game, maxMove stores move amount in longest game
    // readChoice select the elo detection mode, strValue is for substr conversion to int
    int k(10), matchCount(0), white(-1), black(-1), matchMaxMove(0), maxMove(0), readChoice(0), strValue(0);

    // nextMatch tells wether we are reading the header or the moves, validMatch verify the match has been played
    bool nextMatch(true), validMatch(false);

    // gameStart detects the header start, game detect the moves start, lineCheck gets the 7 first symbols of a line
    // substr is used whenever a part of a line is needed
    std::string gameStart, game, lineCheck, substr;

    std::cout << "Welcome to EloChess! This program calculates elo evolution for players from a tournament file and "
                 "also gives some additional statistics.\n" << std::endl;

    while(readChoice != 1 && readChoice!= 2) {
        std::cout << "Do you want to get players elo from the tournament pgn data (might not be correct), or do you "
                     "want to read the elo.elo file (you can fix elo manually there)? (type 1 or 2)";
        std::cin >> readChoice;
    }

    std::cout << std::endl;

    // Choice 1 so we read the whole pgn first to detect max elo of each player as a starting point
    if(readChoice == 1){
        std::ifstream file("../tournament.pgn");
        if(file) {
            std::string line;

            while (getline(file, line)) {

                // We take 6 chars to check Event
                if(line.length() >= 6) {
                    gameStart = line.substr(0, 6);
                }
                // 2 chars to detect 1.
                if(line.length() >= 2){
                    game = line.substr(0, 2);
                }
                // Detects a new match is setting up
                if(gameStart == "[Event") {
                    if (!nextMatch) {
                        nextMatch = true;
                    }
                // Detects the moves start
                } else if(game == "1."){
                    nextMatch = false;
                    white = -1;
                    black = -1;
                } else {
                    // Here it means we are in the header, it's an optimization as we won't process lines when parsing
                    // the actual moves of the game (which is obviously useless)
                    if(nextMatch) {

                        // This substirng tells us where we are in the header
                        if (line.length() >= 7) {
                            lineCheck = line.substr(0, 7);
                        }

                        // For white and black we create a new space in all the related arrays. Also putting the names
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

                            // This gives us the index of the player in the array
                            ptrdiff_t pos = std::distance(players.begin(),
                                                          std::find(players.begin(), players.end(), substr));

                            // And we allocate the position to white and black, for clarity.
                            if (pos < players.size()) {
                                if (lineCheck == "[White ") {
                                    white = pos;
                                } else {
                                    black = pos;
                                }
                            }
                        }

                        // Now we get the elos and select the max (to minimize errors due to bad configuration in pgn)
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
        // Here we only read elo.elo so we can just get the values lines by lines
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

    // And now it's time for the big part!
    if(file){
        std::string line;

        // Again reading line by line the .pgn. The start is very close to the first where we get players names and elos
        // has we don't have all these information if you used elo.elo file.
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
                    // lineCheck here will once again be used to detect all important parts of the header
                    if(line.length() >= 7) {
                        lineCheck = line.substr(0, 7);
                    }


                    if(lineCheck == "[White " || lineCheck == "[Black "){
                        substr = line.substr(8, line.length()-10);

                        // This is the exact same as part 1 except that we don't get the elos as we already have them
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

                    // Here is the main elo calculation part, first we get the result of the match
                    if(lineCheck == "[Result") {

                        // Here we get the result string
                        substr = line.substr(9, line.length() - 11);

                        double w = 0;

                        // For the elo calculation we put the values for white side and then reverse the calculation
                        // for the black side as elo calculation is symmetrical
                        if (substr == "1/2-1/2") {

                            // We add for the draws statistics
                            draws[white]++;
                            draws[black]++;

                            // This is the variable that we will use for calculation
                            w = 0.5;

                            // The index 2 of result sum counts the total amount of draws
                            resultSum[2]++;

                            // And this is the draws as white for the player (no need to count black draws as we deduce
                            // it with a substraction at the end of the program)
                            drawWhite[white]++;
                        } else if (substr == "1-0") {

                            // Add the wins/loss stats
                            wins[white]++;
                            loss[black]++;
                            w = 1;

                            // Index 0 counts the total amount of white wins
                            resultSum[0]++;

                            // wins as white for the players, we deduce black wins with a substract at the end
                            winWhite[white]++;
                        } else if (substr == "0-1") {

                            // Add the wins/loss stats
                            wins[black]++;
                            loss[white]++;
                            w = 0;

                            // Index 1 counts the total amount of white loss
                            resultSum[1]++;

                            // loss as white for the players, we deduce black loss with a substract at the end
                            lossWhite[white]++;
                        } else {

                            // If the result isn't normal, then the match is invalid and should not be counted
                            // we substract 1 to matchCount as we added 1 at the start of this invalid match
                            matchCount--;
                            validMatch = false;
                        }

                        // If the match has a valid result we can calculate elos
                        if (validMatch) {

                            // This is the differential that determines the amount of elo that can be earned/lost
                            double d = elos[white] - elos[black];

                            // We use w to count the scores
                            score[white] += w;
                            score[black] += 1 - w;

                            // And we count the match for players now that we know it is a valid match
                            totalMatch[white]++;
                            totalMatch[black]++;

                            // In the FIDE rules, the differential is maxed at 400 elo.
                            if (d > 400) {
                                d = 400;
                            }

                            // This gives the win probability based on the elo differential and is used to calculate
                            // the potential elo gains
                            double pd = 1 / (1 + pow(10, -d / 400));

                            // This is the formula used to calculate the new elo of the players, pretty simple when you
                            // already have all the values set
                            elos[white] += k * (w - pd);
                            elos[black] += k * (pd - w);
                        }

                        // Just setting back validMatch at true by default...
                        validMatch = true;
                    }

                    // This is for pure statistics and get the amount of moves in a game
                    if(lineCheck == "[PlyCou"){
                        if(line.length() > 11) {
                            substr = line.substr(11, line.length() - 13);
                        }

                        // Playcount gives the number of half-moves, so we divide by 2 to get the number of moves
                        int moveNum = (atoi( substr.c_str() )) / 2;

                        // And we check wether it is the longest game or not
                        if(moveNum > maxMove){
                            maxMove = moveNum;
                            matchMaxMove = matchCount;
                        }
                    }

                    // This is also pure statistics, for the openings
                    if(lineCheck == "[Openin"){
                        if(line.length() > 10) {
                            substr = line.substr(10, line.length() - 12);
                        }

                        // We get the name of the opening, init at 1 if it was never used
                        if(!(std::find(opening.begin(), opening.end(), substr) != opening.end())){
                            opening.push_back(substr);
                            opCount.push_back(1);
                        } else {
                            // If it was already used, then we add 1, to count the times it was used in the tourney
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


    // Now everything has been calculated it is all info display

    // Displaying the openings stats
    while(!opening.empty()){
        // We get the max to sort in decreasing order
        int i = max_index(opCount);
        std::cout << std::setw(50) << opening[i] << " played " << std::setw(3) << opCount[i] << " times." << std::endl;

        // And we delete the max so that we don't get the same value in the next loop
        opening.erase(opening.begin() + i);
        opCount.erase(opCount.begin() + i);
    }

    std::cout << "\n" << "-----------------------------------------------------------------------------------------" <<
              "---------------\n" << std::endl;

    // Displaying the players stats
    while(!players.empty()){
        // We get the max score to sort in decreasing order

        int i = max_index(score);

        std::cout << std::setw(19) << players[i] << " (" << std::setw(7) << elos[i] << ") | Wins: " << std::setw(2) <<
                  wins[i] << " (" << std::setw(2) << winWhite[i] << " " << std::setw(2) << wins[i] - winWhite[i] <<
                  ") | Loss: " << std::setw(2) << loss[i] << " (" << std::setw(2) << lossWhite[i] << " " <<
                  std::setw(2) << loss[i] - lossWhite[i] << ") | Draws: " << std::setw(2) << draws[i] << " (" <<
                  std::setw(2) << drawWhite[i] << " " << std::setw(2) << draws[i] - drawWhite[i] <<
                  ") | Score: " << std::setw(4) << score[i] << "/" << std::setw(2) << totalMatch[i] << std::endl;

        // And we delete the max so that we don't get the same value in the next loop
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

    // Finally we display general info, here longest game with number of moves
    std::cout << std::endl <<"Longest game: Game " << matchMaxMove << " with " << maxMove <<
              " moves." << std::endl;

    // And here the total amount of games, white wins, black wins and draws
    std::cout << "Games: " << matchCount << " | White wins: " << resultSum[0] << " | Black wins: " << resultSum[1] <<
              " | Draws: " << resultSum[2] << std::endl << std::endl;

    system("pause");
    return 0;
}

// This function is just there to easily get the index of the max element of an array of doubles
int max_index(std::vector<double> vector) {
    double maxi = -1;
    int index = 0;
    if(!vector.empty()) {
        for (int i(0); i < vector.size(); i++) {
            if(vector[i] > maxi){
                maxi = vector[i];
                index = i;
            }
        }
    }
    return index;
}
