#include <atomic>
#include <queue>
#include <algorithm>
#include <random>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <queue>

#include "board.hpp"
#include "butils.hpp"
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

// void get_fast_features(const Board &b, vector<float> &features){
//     float num_us = 0;
//     float num_them = 0;
//     float w_p = 0;
//     float b_p = 0;
//     float w_r = 0;
//     float b_r = 0;
//     float w_b = 0;
//     float b_b = 0;
//     float w_p_promo = 0;
//     float b_p_promo = 0;
//     float w_check = 0;
//     float b_check = 0;
//     float w_r_block = 0;
//     float b_r_block = 0;
//     float w_rook_blockage = 0;
//     float b_rook_blockage = 0;
    
//     if (b.data.w_pawn_1 != 0xff){
//         // int i = 
//         // if (i > 1){
//         //     if (j > 1) w_p_promo += (j-1) + 0.25*(i-1)*(i-1);
//         //     else w_p_promo += ((1-j) + 0.25*(i-1)*(i-1));
//         // }else{
//         //     w_p_promo += (size-3-j); //0.2
//         // }
//         w_p++;
//     }else if (b.data.w_pawn_2 != 0xff){
//         w_p++;
//     }else if (b.data.b_pawn_1 != 0xff){
//         b_p++;
//     }else if (b.data.b_pawn_2 != 0xff){
//         b_p++;
//     }else if (b.data.w_rook_1 != 0xff){
//         w_r++;
//     }else if (b.data.w_rook_2 != 0xff){
//         w_r++;
//     }else if (b.data.b_rook_1 != 0xff){
//         b_r++;
//     }else if (b.data.b_rook_2 != 0xff){
//         b_r++;
//     }else if (b.data.w_bishop != 0xff){
//         w_b++;
//     }else if (b.data.b_bishop != 0xff){
//         b_b++;
//     }
    
//     if (b.data.board_type == EIGHT_FOUR){
//         // add the other pawns
//     }
    
//     if (b.data.board_type == EIGHT_TWO){
        
//     }
    
//     features = {num_us - num_them,
//                 w_p - b_p, w_r - b_r, w_b - b_b, 
//                 b_p_promo - w_p_promo, b_check - w_check,
//                 b_rook_blockage - w_rook_blockage};
// }

void get_features(const Board &b, vector<float> &features){
    float num_us = 0;
    float num_them = 0;
    float w_p = 0;
    float b_p = 0;
    float w_n = 0;
    float b_n = 0;
    float w_r = 0;
    float b_r = 0;
    float w_b = 0;
    float b_b = 0;
    float w_p_promo = 0;
    float b_p_promo = 0;
    float w_check = 0;
    float b_check = 0;
    float w_rook_blockage = 0;
    float b_rook_blockage = 0;
    
    float w_r_adv = 0;
    float b_r_adv = 0;
    
    float w_2_rooks = 0;
    float b_2_rooks = 0;
    
    bool w_flag = false;
    bool b_flag = false;
    
    
    string config = board_to_str(&b.data);
    int size;
    if (b.data.board_type == SEVEN_THREE) size = 7;
    else size = 8;
    
    for (int i = 0; i < size; i++){
        for (int j = 0; j < size; j++){
            
            char piece = config[(size+1)*i+j];
            
            // CALCULATING UTILITY BASED ON THE TYPE OF PIECES
            if (piece == 'P'){
                w_p++;
                // util += 1;
                num_us++;
            }
            else if (piece == 'R'){
                if ((i == 1 && j == 0) || (i == 0 && j == 5) || (i == 6 && j == 1) || (i == 5 && j == 6)){
                    w_r_adv++;
                }
                w_r++;
                // util += 5.5; 
                num_us++;
                
                if (w_flag) w_2_rooks++;
                else w_flag = true;
            }
            else if (piece == 'B'){
                w_b++;
                // util += 3;
                num_us++;
            }
            else if (piece == 'N'){
                w_n++;
                // util += 5.5; 
                num_us++;
            }
            else if (piece == 'n'){
                b_n++;
                // util += 3;
                num_us++;
            }
            else if (piece == 'p'){
                b_p++;
                // util -= 1;
                num_them++;
            }
            else if (piece == 'r'){
                if ((i == 1 && j == 0) || (i == 0 && j == (size - 2)) || (i == (size - 1) && j == 1) || (i == (size-2) && j == (size-1))){
                    b_r_adv++;
                }
                b_r++;
                // util -= 5.5;
                num_them++;
                
                if (b_flag) b_2_rooks++;
                else b_flag = true;
            }
            else if (piece == 'b'){
                b_b++;
                // util -= 3;
                num_them++;
            }
            
            // PAWN PROMOTION
            if (b.data.board_type == EIGHT_TWO){
                if (piece == 'P'){
                    if (i > 2){
                        if (j > 2) w_p_promo += (j-2) + 0.25*(i-2)*(i-2);
                        else w_p_promo += ((2-j) + 0.25*(i-2)*(i-2));
                    }else{
                        w_p_promo += (5-j); //0.2
                    }
                }else if(piece == 'p'){
                    if (i < 5){
                        if (j < 5) b_p_promo += ((5-j) + 0.25*(5-i)*(5-i));
                        else b_p_promo +=((j-5) + 0.25*(5-i)*(5-i));
                    }else{
                        b_p_promo += (j-2); //0.2
                    }
                }
            }else{
                if (piece == 'P'){
                    if (i > 1){
                        if (j > 1) w_p_promo += (j-1) + 0.25*(i-1)*(i-1);
                        else w_p_promo += ((1-j) + 0.25*(i-1)*(i-1));
                    }else{
                        w_p_promo += (size-3-j); //0.2
                    }
                }else if(piece == 'p'){
                    int x = size - 2;
                    if (i < x){
                        if (j < x) b_p_promo += ((x-j) + 0.25*(x-i)*(x-i));
                        else b_p_promo +=((j-x) + 0.25*(x-i)*(x-i));
                    }else{
                        b_p_promo += (j-2); //0.2
                    }
                }
            }
        }
    }
    
    for (int j = 0; j < size; j++){
        char piece = config[(size+1)*j];
        if (piece == 'P' || piece == 'K' || piece == 'B' || piece == 'N') w_rook_blockage++;
    }
    
    for (int i = 0; i < size; i++){
        char piece = config[(size+1)*(size-1)+i];
        if (piece == 'P' || piece == 'K' || piece == 'B' || piece == 'N') w_rook_blockage++;
    }
    
    for (int j = 0; j < size; j++){
        char piece = config[(size+1)*j+(size-1)];
        if (piece == 'p' || piece == 'k' || piece == 'b' || piece == 'n') b_rook_blockage++;
    }
    
    for (int i = 0; i < size; i++){
        char piece = config[(size+1)*0+i];
        if (piece == 'p' || piece == 'k' || piece == 'b' || piece == 'n') b_rook_blockage++;
    }

    // // CHECKMATE and CHECK CONDITION
    // Board b_copied(b);
    // Board* b_copy = &b_copied;

    // int x = b_copy -> data.player_to_play ^ (WHITE | BLACK);
    // if (x == 1<<6) b_copy -> data.player_to_play = WHITE;
    // else b_copy -> data.player_to_play = BLACK;
    
    // if (b_copy -> data.player_to_play == WHITE){
    //     if (b_copy -> in_check()) w_check += 1.5;
    // }else{
    //     if (b_copy -> in_check()) b_check += 1.5;
    // }
    
    features = {w_n - b_n,
                w_p - b_p, w_r - b_r, w_b - b_b, 
                b_p_promo - w_p_promo, b_check - w_check,
                b_rook_blockage - w_rook_blockage,
                w_2_rooks - b_2_rooks, w_r_adv - b_r_adv,
                num_us - num_them};
} 



float utility(const Board &b){
    // if (b.get_legal_moves().size() == 0){
    //     cout << "AT CHECKMATE" << endl;
    //     if (b.data.player_to_play == WHITE) return -20;
    //     else return 20;
    // }
    
    vector<float> features;
    get_features(b, features);
    
    vector <float> weights;
    if (b.data.board_type == EIGHT_FOUR) weights  = {0, 1.4, 5.7, 2.7, 0.05, 0. , 0.01, 1.5, 0, 0.};
    if (b.data.board_type == SEVEN_THREE) weights = {0, 1, 5.5, 3, 0.05, 1.5, 0.02, 1.5, 0.1, 0.01};
    if (b.data.board_type == EIGHT_TWO) weights   = {3.3, 1.2, 6, 2.6, 0.05, 0. , 0.1, 0, 0, 0.};
    // const char* file_path = "weights_static.txt";
    // std::ifstream file(file_path);
    // float value;
    // int count = 0;
    // while (file >> value){
    //     weights[count] = value;
    //     count++;
    // }
    
    float util = 0;
    
    for (int i = 0; i < weights.size(); i++){
        util += features[i]*weights[i];
    }
    
    return util;
}

class MCTS_Node{
    public:
        MCTS_Node* parent;
        Board state;
        U16 chosen_move;
        vector<MCTS_Node*> children;
        MCTS_Node* best_child;
        bool child_set;
        float best_child_score;
        int visits;
        float score;
        bool root;
        
    // check if this redefinition actually works
    MCTS_Node(const Board& state)
        : state(state), child_set(false), visits(0), score(0), root(true) {};
        
    MCTS_Node(MCTS_Node* parent, const Board& state, U16 chosen_move)
        : parent(parent), state(state), chosen_move(chosen_move), child_set(false), best_child_score(0.001), visits(0), score(0), root(false) {};
};

class MCTS {
    public:
        MCTS_Node* select(MCTS_Node* node);
        void expand(MCTS_Node* node);
        void simulate(MCTS_Node* node);
        void backpropagate(MCTS_Node* node, float score);
        U16 search(const Board& initialState, float time);
};

float UCB(MCTS_Node* node){
    // cout << "CALCULATING UCB" << endl;
    if (node -> visits == 0){
        if ((node -> state).data.player_to_play == WHITE) return -1e5;
        else return 1e5;
    }
    else{
        float avg_reward = node -> score / (float) node -> visits;
        float C = 1.414; 
        float exp_term = C * sqrt(log(node -> parent -> visits) / (float) node -> visits);
        if ((node -> state).data.player_to_play == WHITE) return avg_reward - exp_term;
        else return avg_reward + exp_term;
    }
}

MCTS_Node* MCTS::select(MCTS_Node* node) {
    // cout << "RUNNING SELECT" << endl;
    
    if ( (node -> children).size() == (size_t) 0){
        // cout << "FINISHED CHOOSING NODE" << endl;
        return node;
    }
    
    MCTS_Node* chosen_node = node;
    if ((node -> state).data.player_to_play == WHITE){
        float gmax = -100000;
        // if ((size_t) node -> visits < (node -> children).size()){
            for (auto child: node -> children){
                float ucb = UCB(child);
                // cout << "UCB is " << ucb << endl;
                if (ucb > gmax){
                    // cout << "WHITE NODE CHOSEN" << endl;
                    chosen_node = child;
                    gmax = ucb;
                }
            }
        // }
        // else{
        //     if (node -> child_set){
        //         chosen_node = node -> best_child;
        //     }
        //     else return node;
        // }
    }else{
        float gmin = 100000;
        // if ((size_t) node -> visits < (node -> children).size()){
            for (auto child: node -> children){
                float ucb = UCB(child);
                if (ucb < gmin){
                    // cout << "BLACK NODE CHOSEN" << endl;
                    chosen_node = child;
                    gmin = ucb;
                }
            }
        // }
        // else{
        //     if (node -> child_set){
        //         chosen_node = node -> best_child;
        //     }
        //     else return node;
        // }
    }
    
    return select(chosen_node);
}

// Need to clarify when do we choose a child to run the simulation
void MCTS::expand(MCTS_Node* node) {
    // cout << "RUNNING EXPAND" << endl;
    
    auto moves = (node -> state).get_legal_moves();
    
    // add all children to the leaf node
    for (auto this_move: moves){
        Board b_copy = Board(node -> state);
        b_copy.do_move_(this_move);
        MCTS_Node* new_child = new MCTS_Node(node, b_copy, this_move);
        (node -> children).push_back(new_child);
    }
}

void MCTS::simulate(MCTS_Node* node) {
    // cout << "RUNNING SIMULATE" << endl;
    // instead of rollout we backup the utility (evaluation)
    float eval;
    if ((node -> children).size() == 0){
        if ((node -> state).data.player_to_play == WHITE){
            if (!(node -> state).in_check()) eval = -100;
            else eval = -1000;
            // eval = -1000;
        }else{
            if (!(node -> state).in_check()) eval = 100;
            else eval = 1000;
            // eval = 1000;
        }
    }
    else{
        if (node -> visits == 0){
            eval = utility(node -> state);
        }else{
            int size = (node -> children).size();
            const int range_from  = 0;
            const int range_to    = size-1;
            std::random_device                  rand_dev;
            std::mt19937                        generator(rand_dev());
            std::uniform_int_distribution<int>  distr(range_from, range_to);
            int random_index = distr(generator);
            MCTS_Node* selected_child = (node -> children)[random_index];
            
            eval = utility(selected_child -> state);
            node = selected_child;
        }
        // return utility(node -> state);
    }
    
    MCTS_Node* node_par = node;
    MCTS_Node* node_child = node;
    // keep backing up the scores for every parent
    while(1){
        // cout << "IN LOOP" << endl;
        node_par -> visits += 1;
        node_par -> score += eval;
        // cout << board_to_str(&(node_par -> state).data);
        if (node_par -> root == true){
            break;
        }
        node_child = &(*node_par);
        node_par = node_par -> parent;
        
        float child_score = UCB(node_child);
        
        if ( (node_par -> child_set) == false){
            // cout << "SETTING UP PARENT" << endl;
            // If best child not yet set
            node_par -> child_set = true;
            node_par -> best_child = node_child;
            node_par -> best_child_score = child_score;
        }else{
            if ((node_par -> state).data.player_to_play == WHITE){
                // If white and this child has higher score then set this as best child
                if ((node_par -> best_child_score) < child_score){
                    // cout << "UPDATING WHITE PARENT" << endl;
                    node_par -> best_child = node_child;
                    node_par -> best_child_score = child_score;
                }
            }else{
                // If black and this child has lower score then set this as best child
                if ((node_par -> best_child_score) > child_score){
                    // cout << "UPDATING BLACK PARENT" << endl;
                    node_par -> best_child = node_child;
                    node_par -> best_child_score = child_score;
                }
            }
        }
    }
}

// void MCTS::backpropagate(MCTS_Node* node, float eval) {
//     // cout << "RUNNING BACKPROPAGATE" << endl;
    
//     MCTS_Node* node_par = node;
//     MCTS_Node* node_child = node;
//     // keep backing up the scores for every parent
//     while(1){
//         // cout << "IN LOOP" << endl;
//         node_par -> visits += 1;
//         node_par -> score += eval;
//         // cout << board_to_str(&(node_par -> state).data);
//         if (node_par -> root == true){
//             break;
//         }
//         node_child = &(*node_par);
//         node_par = node_par -> parent;
        
//         float child_score = UCB(node_child);
        
//         if ( (node_par -> child_set) == false){
//             // cout << "SETTING UP PARENT" << endl;
//             // If best child not yet set
//             node_par -> child_set = true;
//             node_par -> best_child = node_child;
//             node_par -> best_child_score = child_score;
//         }else{
//             if ((node_par -> state).data.player_to_play == WHITE){
//                 // If white and this child has higher score then set this as best child
//                 if ((node_par -> best_child_score) < child_score){
//                     // cout << "UPDATING WHITE PARENT" << endl;
//                     node_par -> best_child = node_child;
//                     node_par -> best_child_score = child_score;
//                 }
//             }else{
//                 // If black and this child has lower score then set this as best child
//                 if ((node_par -> best_child_score) > child_score){
//                     // cout << "UPDATING BLACK PARENT" << endl;
//                     node_par -> best_child = node_child;
//                     node_par -> best_child_score = child_score;
//                 }
//             }
//         }
//     }
// }

void free_tree(MCTS_Node* node){
    for (auto child: node -> children){
        free_tree(child);
    }
    free(node);
}

U16 MCTS::search(const Board& initialState, float time) {
    MCTS_Node initial_node = MCTS_Node(initialState);
    expand(&initial_node);
    
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point end;
    int i = 0;
    if (time > 1){
        time -= 0.5;
    }else if (time > 0.5){
        time -= 0.2;
    }
    cout << "time is " << time << endl;
    
    while(std::chrono::duration_cast<std::chrono::seconds>(end - start).count() < time){
        // cout << "RUNNING MCTS ITERATION: " << i << endl;
        
        MCTS_Node* best_node = select(&initial_node);
        if ((best_node -> children).size() == 0) expand(best_node);
        // float eval = simulate(best_node);
        // backpropagate(best_node, eval);
        simulate(best_node);
        i++;
        end = std::chrono::high_resolution_clock::now();
    }
    cout << "MCTS TOTAL ITERATIONS: " << i << endl;
    cout << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << endl;
    
    if (i == 0){
        int size = (initial_node.children).size();
        const int range_from  = 0;
        const int range_to    = size-1;
        std::random_device                  rand_dev;
        std::mt19937                        generator(rand_dev());
        std::uniform_int_distribution<int>  distr(range_from, range_to);
        int random_index = distr(generator);
        
        return initial_node.children[random_index] -> chosen_move;
    }
    
    U16 select_move = 0;
    if ((initial_node.state).data.player_to_play == WHITE){
        float gmax = -1e10;
        for (auto child : initial_node.children){
            float avg_reward = (child -> score) / (float) child -> visits;
            if (avg_reward > gmax){
                select_move = child -> chosen_move;
                gmax = avg_reward;
            }
        }
    }else{
        float gmin = 1e10;
        for (auto child : initial_node.children){
            float avg_reward = (child -> score) / (float) child -> visits;
            if (avg_reward < gmin){
                select_move = child -> chosen_move;
                gmin = avg_reward;
            }
        }
    }
    
    for (auto child: initial_node.children){
        free_tree(child);
    }
    
    return select_move;
}


void Engine::find_best_move(const Board& b) {

    // pick a random move
    // start = clock();
    auto moveset = b.get_legal_moves(); // unordered set
    
    // string myboard = board_to_str(&b.data);
        
    // myplayer = b.data.player_to_play;
    
    if (moveset.size() == 0) {
        this->best_move = 0;
    }
    else{
        MCTS mcts_instance;
        float rem_time = std::chrono::duration_cast<std::chrono::duration<float>>(time_left).count();
        cout << "time " << rem_time << endl;
        
        if (rem_time < 1){
            std::vector<U16> moves;
            for (auto m : moveset) {
                std::cout << move_to_str(m) << " ";
            }
            std::cout << std::endl;
            std::sample(
                moveset.begin(),
                moveset.end(),
                std::back_inserter(moves),
                1,
                std::mt19937{std::random_device{}()}
            );
            this->best_move = moves[0];
        }
        else{
            if (b.data.board_type == SEVEN_THREE){
                rem_time = min(rem_time/20.0, 5.0);
            }else if (b.data.board_type == EIGHT_FOUR){
                rem_time = min(rem_time/20.0, 6.0);
            }else{
                rem_time = min(rem_time/20.0, 8.0);
            }
            
            this -> best_move = mcts_instance.search(b, rem_time);
        }
    }   
}