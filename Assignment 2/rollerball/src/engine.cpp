#include <algorithm>
#include <random>
#include <iostream>

#include "board.hpp"
#include "engine.hpp"

using namespace std;
typedef long long ll;

U16 minimax_decision(const Board& b);
int min_value(Board& b, int total_moves);
int max_value(Board& b, int total_moves);
float min_value_prune(Board& b, int total_moves, float alpha, float beta);
float max_value_prune(Board& b, int total_moves, float alpha, float beta);
bool flag1=false;
bool flag2=false;
bool flag3=false;
bool flag4=false;
bool flag5=false;

vector<float> child_values;
int max_depth = 0;

PlayerColor myplayer;

clock_t start = clock();
clock_t endt = clock();

bool endgame = false;

float utility(const Board &b){
    float util = 0;
    
    // string config = board_to_str(b.data.board_0);
    
    int num_us = 0;
    int num_them = 0;    
    
    // for (int i = 0; i < 7; i++){
    //     for (int j = 0; j < 7; j++){
    //         char piece = config[8*i+j];
    //         cout << piece;
    //     }
    //     cout << endl;
    // }
    
    string config = board_to_str(b.data.board_0);
    
    for (int i = 0; i < 7; i++){
        for (int j = 0; j < 7; j++){
            char piece = config[8*i+j];
            // this gives me what the piece is
            
            // CALCULATING UTILITY BASED ON THE TYPE OF PIECES
            if (piece == 'P'){
                if (i == 6 && j == 2){
                    util -= 0.2;
                }
                util += 1;
                num_us++;
            }
            else if (piece == 'R'){
                if (i == 6 && j > 4) util -= 0.1;
                // if (i == 1 && j == 0) util += 0.4;
                util += 5.5; 
                num_us++;
            }
            else if (piece == 'B'){
                util += 3;
                num_us++;
            }
            else if (piece == 'p'){
                if (i == 0 && j == 4){
                    util += 0.2;
                }
                util -= 1;
                num_them++;
            }
            else if (piece == 'r'){
                if (i == 0 && j < 2) util += 0.1;
                util -= 5.5;
                num_them++;
            }
            else if (piece == 'b'){
                util -= 3;
                num_them++;
            }
            
            // PAWN PROMOTION
            if (piece == 'P'){
                if (i > 1){
                    if (j > 1) util -= 0.045*((j-1) + 0.25*(i-1)*(i-1)) + 0.15;
                    else util -= 0.045*((1-j) + 0.25*(i-1)*(i-1)) + 0.15;
                }else{
                    util -= 0.045*((4-j)); //0.2
                }
                
                // if (j == 0) util -= 0.1;
            }else if(piece == 'p'){
                // if (i < 6) util += 0.05*((5-j) + (5-i));
                // else util += 0.05*((j-2));
                
                if (i < 5){
                    if (j < 5) util += 0.045*((5-j) + 0.25*(5-i)*(5-i)) + 0.15;
                    else util += 0.045*((j-5) + 0.25*(5-i)*(5-i)) + 0.15;
                }else{
                    util += 0.045*((j-2)); //0.2
                }
                
                // if (j == 6) util += 0.1;
            }
            
            // util += pieces_under_attack(b);
            // util += pieces_under_threat(b);
        }
    }
    
    // CHECKMATE and CHECK CONDITION
    Board *b_copy = b.copy();
    // if (b_copy -> get_legal_moves().size() == 0){
    //     if (b_copy -> data.player_to_play == WHITE) util = -1*1e6;
    //     else util = 1e6;
    //     free(b_copy);
    //     return util;
    // }
    
    // else util += 1.5;
    
    
    // util += pieces_under_attack(*b_copy);
    
    int x = b_copy -> data.player_to_play ^ (WHITE | BLACK);
    if (x == 1<<6) b_copy -> data.player_to_play = WHITE;
    else b_copy -> data.player_to_play = BLACK;
    // if (b_copy -> get_legal_moves().size() == 0){
    //     if (b_copy -> data.player_to_play == WHITE) util = -1*1e6;
    //     else util = 1e6;
    //     free(b_copy);
    //     return util;
    // }
    
    if (b_copy -> data.player_to_play == WHITE){
        if (b_copy -> in_check()) util -= 1.5;
    }else{
        if (b_copy -> in_check()) util += 1.5;
    }
    
    // if (b_copy -> in_check()) util += 1.5;
     
    // util -= pieces_under_attack(*b_copy);
    
    util += (num_us - num_them)*0.5;
    
    // if (num_us + num_them <= 6) endgame = true;
    
    // cout << "NUM" << num_us << " " << num_them << endl;
    
    free(b_copy);
    
    return util;
}

float max_value_prune(Board& b, int total_moves, float alpha, float beta){
    // cout << max_depth << endl;
    if (total_moves == max_depth) return utility(b);
    
    float child;
    float v = -1*1e6;
    
    auto moveset = b.get_legal_moves();
    
    if(moveset.size() == 0 && !(b.in_check())){
        v = 0;
    }
    
    for (auto successor: moveset){
        // cout << "Max value" << endl;
        
        Board *b_copy = b.copy();
        b_copy -> do_move(successor);
        
        child = min_value_prune(*b_copy, total_moves+1, alpha, beta);
        
        if (total_moves == 0) child_values.push_back(child);
        v = max(v, child);
        
        // cout << "max " << v << endl;
        
        alpha = max(alpha, child);
        free(b_copy);
        
        if (alpha >= beta) return child;
        
        endt = clock();
        if ((endt-start)/CLOCKS_PER_SEC >= 2) break;
    }
    
    return v;
}

float min_value_prune(Board& b, int total_moves, float alpha, float beta){
    // cout << max_depth << endl;
    if (total_moves == max_depth) return utility(b);
    
    float child;
    float v = 1*1e6;
    
    auto moveset = b.get_legal_moves();
    
    if(moveset.size() == 0 && !(b.in_check())){
        v = 0;
    }
    
    for (auto successor: moveset){
        // cout << "Min Value " << endl;
        
        Board *b_copy = b.copy();
        b_copy -> do_move(successor);
        child = max_value_prune(*b_copy, total_moves+1, alpha, beta);
        
        if (total_moves == 0) child_values.push_back(child);
        v = min(v, child);
        
        // cout << "min " << v << endl;
        
        beta = min(beta, child);
        
        free(b_copy);
        
        if (alpha >= beta) return child;
        
        endt = clock();
        if ((endt-start)/CLOCKS_PER_SEC >= 2) break;
    }
    
    return v;
}

void Engine::find_best_move(const Board& b) {

    // pick a random move
    start = clock();
    auto moveset = b.get_legal_moves(); // unordered set
    
    string myboard = board_to_str(b.data.board_0);
    // cout << myboard;
    // cout << myboard.size() << " " << endl;
    // for (int i = 0; i < 49; i++) cout << i << " " << myboard[i] << endl;
    
    // int ourcount = 0;
    // int theircount = 0;
    // for (int i = 0; i < 7; i++){
    //     for(int j = 0; j < 7; j++){
    //         if (myboard[i] == 'P' || myboard[i] == 'p' || myboard[i] == 'B' || myboard[i] == 'b' || myboard[i] == 'R' || myboard[i] == 'r'){
    //             piececount++;
    //         }
    //     }
    // }
    
    // default_random_engine generator;
 
    // int A = 1, B = 3;
 
    // Initializing of uniform_int_distribution class
    // uniform_int_distribution<int> distribution(A, B);
    
    
    myplayer = b.data.player_to_play;
    
    if (moveset.size() == 0) {
        this->best_move = 0;
    }else{
        // cout << "Copy Made" << endl;
        float v;
        if (b.data.player_to_play == WHITE){
            max_depth = 0;
            // endgame = false;
            // if (piececount <=6 ) endgame = true;
            while (max_depth < 20){
                // if (endgame && max_depth >= 2) break;
                 
                Board *initial_board_copy = b.copy();
                max_depth++;
                
                child_values = {};
                v = max_value_prune(*initial_board_copy, 0, -1*1e6, 1e6);
            
                auto moveset = b.get_legal_moves();
                
                int count = 0;
                int ind = 0;
                // int check = distribution(generator);
                for (auto successor: moveset){
                    endt = clock();
                    if ((endt-start)/CLOCKS_PER_SEC >= 2) break;
                    
                    // cout << "Size is " << child_values.size() << " " << v <<" "<<move_to_str(successor) <<  " " << child_values[count] << endl;
                    if (child_values[count] == v){
                        ind++;
                        // cout << "In here";
                        // free(initial_board_copy);
                        this -> best_move = successor;
                        if(v == 1*1e6){
                            return;
                        }
                        // if (ind == check) break;
                        break;
                    }
                    count++;
                }
                free(initial_board_copy);
                endt = clock();
                if ((endt-start)/CLOCKS_PER_SEC >= 2) break;
            }
        }else{
            max_depth = 0;
            // endgame = false;
            // if (piececount <=6 ) endgame = true;
            
            while(max_depth < 20){
                // if (endgame && max_depth >= 2) break;
                
                Board *initial_board_copy = b.copy();
                max_depth++;
                
                child_values = {};
                v = min_value_prune(*initial_board_copy, 0, -1*1e6, 1e6);
            
                auto moveset = b.get_legal_moves();
                
                int count = 0;
                int ind = 0;
                // int check = distribution(generator);
                for (auto successor: moveset){
                    endt = clock();
                    if ((endt-start)/CLOCKS_PER_SEC >= 2) break;
                    
                    // cout << "Size is " << child_values.size() << " " << v<<" "<<move_to_str(successor) <<  " " << child_values[count] << endl;
                    
                    if (child_values[count] == v){
                        ind++;
                        // cout << "In here";
                        // free(initial_board_copy);
                        this -> best_move = successor;
                        if(v == -1*1e6){
                            return;
                        }
                        // if (ind == check) break;
                        break;
                    }
                    count++;
                }
                free(initial_board_copy);
                endt = clock();
                if ((endt-start)/CLOCKS_PER_SEC >= 2) break;
            }
        }
        
        // cout << "Max val found" << endl;
    }
    
    
}
