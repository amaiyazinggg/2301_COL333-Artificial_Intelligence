#pragma once

#include "engine_base.hpp"
// #include <unordered_map>


using namespace std;



// vector<string> last_12_boards
class Engine : public AbstractEngine {

    // add extra items here. 
    // Note that your engine will always be instantiated with the default 
    // constructor.
    
    public:
        int count20 =0;
        // vector<pair<string,int>> last_boards_1;
        unordered_map<string,int> last_boards_1;
        queue<string> last_boards;
        void find_best_move(const Board& b) override;

};