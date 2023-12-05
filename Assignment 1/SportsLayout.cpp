#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
// #include <cstdlib>
using namespace std;
typedef long long ll;

#include "SportsLayout.h"

SportsLayout::SportsLayout(string inputfilename){ 
    readInInputFile(inputfilename);
    mapping= new int[z];
    delta = min((float)3, time*(float)6);
    // bestmapping = new int[z];
    // current_perm = new int[l];
    // current_cost = new long long;
}

bool SportsLayout::check_output_format(){
    vector<bool> visited(l,false);
    
    for(int i=0;i<z;i++){
        if((mapping[i]>=1 && mapping[i]<=l)){
            if(!visited[mapping[i]-1]) visited[mapping[i]-1]=true;
            else{
                cout<<"Repeated locations, check format\n";
                return false;
            }
        }else{
            cout<<"Invalid location, check format\n";
            return false;
        }
    }
    return true;
}

// void SportsLayout::readOutputFile(string output_filename)
// {
//         fstream ipfile;
//         ipfile.open(output_filename, ios::in);
//         if (!ipfile) {
//             cout << "No such file\n";
//             exit( 0 );
//         }
//         else {
            
//             vector<int> ip;

//             while (1) {
//                 int t;
//                 ipfile >> t;
//                 ip.push_back(t);
//                 if (ipfile.eof())
//                     break;
                
//             }
        
//         if(ip.size()!=z)
//         {
//             cout<<"number of values not equal to number of zones, check output format\n";
//             exit(0);
//         }
//         for(int i=0;i<z;i++)
//         mapping[i]=ip[i];
//     ipfile.close();

//     if(!check_output_format())
//         exit(0);
//     cout<<"Read output file, format OK"<<endl;

//         }
    
// }


long long SportsLayout::cost_fn(){
    long long cost=0;
    
    for(int i = 0; i < z; i++){
       for(int j = 0; j < z; j++) {
            cost += (long long)N[i][j]*(long long)T[mapping[i]-1][mapping[j]-1];
       }
    }
    return cost;
}

long long SportsLayout::cost_fn_for_perm(){
    long long cost=0;
    
    for(int i = 0; i < z; i++){
       for(int j = 0; j < z; j++) {
            cost += (long long)N[i][j]*(long long)T[current_perm[i]-1][current_perm[j]-1];
       }
    }
    return cost;
}

void SportsLayout::readInInputFile(string inputfilename){
    fstream ipfile;
    ipfile.open(inputfilename, ios::in);
    
    if (!ipfile) {
        cout << "No such file\n";
        exit( 0 );
    }else{
        ipfile >> time;
        ipfile >> z;
        ipfile >> l;

        if(z>l)
        {
            cout<<"Number of zones more than locations, check format of input file\n";
            exit(0);
        }

        int **tempT;
        int **tempN;

        tempT = new int*[l];
        for (int i = 0; i < l; ++i) tempT[i] = new int[l];
        
        tempN = new int*[z];
        for (int i = 0; i < z; ++i) tempN[i] = new int[z];

        for(int i = 0; i < z; i++){
            for(int j = 0; j < z; j++) ipfile >> tempN[i][j];
        }

        for(int i = 0; i < l; i++){
            for(int j = 0; j < l; j++) ipfile >> tempT[i][j];
        }

        ipfile.close();

        T= tempT;
        N= tempN;
    }
}

void SportsLayout::write_to_file(string outputfilename){
     // Open the file for writing
    ofstream outputFile(outputfilename);

    // Check if the file is opened successfully
    if (!outputFile.is_open()) {
        cerr << "Failed to open the file for writing." << std::endl;
        exit(0);
    }

    for(int i = 0; i < z; i++) outputFile << mapping[i] <<" ";

    // Close the file
    outputFile.close();

    cout << "Allocation written to the file successfully." << endl;
}

void SportsLayout::write_to_file_intermediate(string outputfilename){
     // Open the file for writing
    ofstream outputFile(outputfilename);

    // Check if the file is opened successfully
    if (!outputFile.is_open()) {
        cerr << "Failed to open the file for writing." << std::endl;
        exit(0);
    }

    for(int i = 0; i < z; i++) outputFile << current_perm[i] <<" ";

    // Close the file
    outputFile.close();

    // cout << "Allocation written to the file successfully." << endl;
}

void SportsLayout::data_analysis(){
    // Open the file for writing
    ofstream outputFile("greedy_data.txt");

    // Check if the file is opened successfully
    if (!outputFile.is_open()) {
        cerr << "Failed to open the file for writing." << std::endl;
        exit(0);
    }

    for (auto x : costs) outputFile << x << " ";

    // Close the file
    outputFile.close();

    cout << "Allocation written to the file successfully." << endl;
}

void SportsLayout::compute_allocation(){
    //you can write your code here 
    //comment out following dummy code
    // vector<bool> loc_occ(l, false);
    // int random;
    
    // for(int i = 0; i < z; i++){
    //     while(1){
    //         random = rand()%l + 1;
    //         if (loc_occ[random]) continue;
    //         else{
    //             loc_occ[random] = true;
    //             break;
    //         }
    //     }
    //     // cout << random << endl;
        
    //     mapping[i] = random;
    // }
    
    if (l*z >= 135000) hill_climbing_best_first();
    else hill_climbing();
    
}

void SportsLayout::new_mapping(pair<int, int> swap){
    int temp;
    temp = current_perm[swap.first];
    current_perm[swap.first] = current_perm[swap.second];
    current_perm[swap.second] = temp;
}


long long SportsLayout::neighbour_cost(long long cost, int i, int j, long long pre_cost_i, long long pre_cost_j){
    // assuming i is always less than z
    if (j < z){
        
        for (int x = 0; x < z; x++){
            int y = x;
            if (x == i || x == j){
                y  =  i + j - x;
            }
            
            cost += (long long)(N[i][x] + N[x][i])
                * (long long)T[current_perm[j] - 1][current_perm[y] - 1]
                // + (long long)N[x][i]*(long long)T[current_perm[y]-1][current_perm[j]-1]
                + (long long)(N[j][x] + N[x][j])
                * (long long)T[current_perm[i] - 1][current_perm[y] - 1];
                // + (long long)N[x][j]*(long long)T[current_perm[y]-1][current_perm[i]-1];
        }
        
        cost -= pre_cost_i + pre_cost_j;
        
    }else{
        
        for (int x = 0; x < z; x++){
            int y = x;
            if (x == i){
                y  =  i + j - x;
            }
            cost += (long long)(N[i][x]+N[x][i])*(long long)T[current_perm[j]-1][current_perm[y]-1];
                // + (long long)N[x][i]*(long long)T[current_perm[y]-1][current_perm[j]-1];
        }
        
        cost -= pre_cost_i;
    }
    
    return cost;
}

long long SportsLayout::neighbour_cost_slow(long long cost, int i, int j){
    // assuming i is always less than z
    if (j < z){
        // vector<int> new_mapping(z);
        
        // for(int i = 0; i < z; i++){
        //     // old_mapping[i] = current_perm[i];
        //     new_mapping[i] = current_perm[i];
        // }
            
        // // only difference is in new_mapping[i] and new_mapping[j]
            
        // new_mapping[i] = current_perm[j];
        // new_mapping[j] = current_perm[i];
        
        int y;
        for (int x = 0; x < z; x++)
        {
            y = x;
            if (x == i || x == j) y  =  i + j - x;
            
            cost += (long long)(N[i][x] + N[x][i]) *
                    (long long)(T[current_perm[j] - 1][current_perm[y] - 1] - T[current_perm[i] - 1][current_perm[x] - 1])
                // + (long long)N[x][i]*(long long)T[new_mapping[x]-1][new_mapping[i]-1]
                
                // + (long long)N[x][j]*(long long)T[new_mapping[x]-1][new_mapping[j]-1]
                // - (long long)(N[i][x]+N[x][i])*(long long)T[current_perm[i]-1][current_perm[x]-1]
                // - (long long)N[x][i]*(long long)T[current_perm[x]-1][current_perm[i]-1]
                // - (long long)(N[j][x]+N[x][j])*(long long)T[current_perm[j]-1][current_perm[x]-1];
                // - (long long)N[x][j]*(long long)T[current_perm[x]-1][current_perm[j]-1];
                  + (long long)(N[j][x] + N[x][j]) * 
                  (long long)(T[current_perm[i] - 1][current_perm[y] - 1] - T[current_perm[j] - 1][current_perm[x] - 1]);
        }
        
        
    }else{
        // vector<ll> new_mapping(z);
        // for(int i = 0; i < z; i++){
        //     new_mapping[i] = current_perm[i];
        // }
        
        // new_mapping[i] = current_perm[j];
        int y;
        for (int x = 0; x < z; x++)
        {
            y = x;
            if (x == i) y = j;
            
            cost += (long long)(N[i][x]+N[x][i])*
                    (long long)(T[current_perm[j]-1][current_perm[y]-1]-T[current_perm[i]-1][current_perm[x]-1]);
                // + (long long)N[x][i]*(long long)T[new_mapping[x]-1][new_mapping[i]-1];
                // - (long long)(N[i][x]+N[x][i])*(long long)T[current_perm[i]-1][current_perm[x]-1];
                // - (long long)N[x][i]*(long long)T[current_perm[x]-1][current_perm[i]-1];
        }
        
    }
    
    return cost;
}

bool SportsLayout::Neighbourhood(){
    bool found = false;
    long long cost = current_cost;
    long long new_cost;
    // long long best_cost = current_cost;
    long long temp;
    pair<int, int> swap = {0, 0};
    
    end = clock();
    
    vector<ll> pre_computed_costs(l, 0);
    
    for (int i = 0; i < z; i ++){
        for (int x = i+1; x < z; x++){
            temp = (long long)(N[i][x]+N[x][i])*(long long)T[current_perm[i]-1][current_perm[x]-1];
            pre_computed_costs[i] += temp;
            pre_computed_costs[x] += temp;
        }
    }
    
    for (int i = 0; i < z; i++){
        end = clock();
        if ((float)(end - start)/(float)CLOCKS_PER_SEC >= time*60 - delta) break;
        
        for (int j = i+1; j < l; j++){
            // if (i == j){
            //     continue;
            // }
            // else{
            new_cost = neighbour_cost(cost, i, j, pre_computed_costs[i], pre_computed_costs[j]);
            if (new_cost < current_cost){
                found = true;
                // break;
                current_cost = new_cost;
                swap = {i, j};
            }
            // }
        }
    }
    new_mapping(swap);
    return found;
}

void SportsLayout::hill_climbing(){

    for (int i = 0; i < l; i++) current_perm.push_back(i+1);
        
    ll min_cost = 9*1e18;
    ll temp_cost = 0;
    bool found;
    // clock_t start = clock();
    // clock_t end = clock();
    start = clock();
    end = clock();
    int new_sample_count = 0;
    
    ll count = 0;
    ll sec = 0;
    
    // cout << (float)(end - start)/(float)CLOCKS_PER_SEC << endl;
    // cout << time*60;
    
    while ((float)(end - start)/(float)CLOCKS_PER_SEC < time*60 - delta){
        // cout << endl << "NEW RESTART" << endl;
        new_sample_count++;
        unsigned seed = clock();
        shuffle(current_perm.begin(), current_perm.end(), default_random_engine(seed));
        
        current_cost = cost_fn_for_perm();
        
        found = Neighbourhood();
        while (found){
            found = Neighbourhood();
            end = clock();
            
            // write_to_file_intermediate(outputfilename);
            // cout << current_cost << endl;
            if ((float)(end - start)/(float)CLOCKS_PER_SEC >= time*60 - delta){
                break;
            }
        }
        
        if (current_cost < min_cost){
            min_cost = current_cost;
            for(int i = 0; i < z; i++){
                mapping[i] = current_perm[i];
            }
            cout << min_cost << " " << cost_fn() << endl; 
        }
        
        end = clock();
        count++;
        // cout << min_cost << endl;
        
        costs.push_back(current_cost);
        
        if ((end - start)/CLOCKS_PER_SEC > sec){
            cout << sec << " " << current_cost << endl;
            cout<<new_sample_count<<"\n";
            sec++;
        }
        
        // break;
    }
    
    data_analysis();
    
    cout << "---------------------------------------------" << endl;
    cout << "Total Restarts: " << new_sample_count << endl;
    cout << "Best Cost: " << min_cost << endl;
    cout << "---------------------------------------------" << endl;
    return;
}

void SportsLayout::hill_climbing_best_first(){
    for (int i = 0; i < l; i++) current_perm.push_back(i+1);
        
    ll min_cost = 9*1e18;
    ll temp_cost = 0;
    bool found;
    start = clock();
    end = clock();
    int new_sample_count = 0;
    
    ll random_successor_cost;
    
    ll count = 0;
    ll sec = 0;
    
    while ((float)(end - start)/(float)CLOCKS_PER_SEC < time*60 - delta){
        cout << endl << "NEW RESTART" << endl;
        new_sample_count++;
        unsigned seed = clock();
        shuffle(current_perm.begin(), current_perm.end(), default_random_engine(seed));
        
        current_cost = cost_fn_for_perm();
        
        random_device rand_dev;
        mt19937 generator(rand_dev());
        uniform_int_distribution<int> distr_1(0, z-1); // check if the last element is included
        uniform_int_distribution<int> distr_2(0, l-1);
        
        ll nbr_checked = 0;
        while (true){
            // cout << "Neighbour check " << nbr_checked << endl; 
            ll i = distr_1(generator);
            ll j = distr_2(generator);
            while (i == j){
                j = distr_2(generator);
            }
        
            random_successor_cost = neighbour_cost_slow(current_cost, i, j);
            
            if (random_successor_cost < current_cost){
                current_cost = random_successor_cost;
                cout << current_cost << endl;
                new_mapping({i, j});
                nbr_checked = 0;
            }
            
            nbr_checked++;
            // if (nbr_checked >= (2*l*z - z*z - z)/2) break;
            if (nbr_checked >= 10*l*l){
                cout << "LIMIT EXCEEDED" << endl;
                break;
            }
            
            end = clock();
            if ((end - start)/CLOCKS_PER_SEC >= time*60 - delta){
                cout << "TIME EXCEEDED" << endl;
                break;
            }
        }
        
        if (current_cost < min_cost){
            min_cost = current_cost;
            for(int i = 0; i < z; i++){
                mapping[i] = current_perm[i];
            }
            // cout << min_cost << " " << cost_fn() << endl; 
        }
        
        end = clock();
        count++;
        // cout << min_cost << endl;
        
        // costs.push_back(current_cost);
        
        if ((float)(end - start)/(float)CLOCKS_PER_SEC > sec){
            cout << sec << " " << current_cost << endl;
            cout<<new_sample_count<<"\n";
            sec++;
        }
        
        // break;
    }
    
    // data_analysis();
    
    cout << "---------------------------------------------" << endl;
    cout << "Total Restarts: " << new_sample_count << endl;
    cout << "Best Cost: " << min_cost << endl;
    cout << "---------------------------------------------" << endl;
    return;
}


void SportsLayout::checker(){
    for (int i = 0; i < l; i++) current_perm.push_back(i+1);
    unsigned seed = clock();
    shuffle(current_perm.begin(), current_perm.end(), default_random_engine(seed));
    
    ll cost = cost_fn_for_perm();
    
    for (int i = 0; i < z; i++){
        for (int j = 0; j < z; j++){
            if (i == j){
                continue;
            }else{
                ll new_cost = neighbour_cost(cost, i, j, 0, 0); // FIX THIS
                
                ll x = current_perm[i];
                current_perm[i] = current_perm[j];
                current_perm[j] = x;
                
                cout << new_cost << " " << cost_fn_for_perm() << endl; 
                
                x = current_perm[i];
                current_perm[i] = current_perm[j];
                current_perm[j] = x;
            }
        }
    }
}

void SportsLayout::simulated_annealing(){
    for (int i = 0; i < l; i++) current_perm.push_back(i+1);
        
    ll min_cost = 9*1e18;
    ll temp_cost = 0;
    bool found;
    clock_t start = clock();
    clock_t end = clock();
    int new_sample_count = 0;
    double temp;
    
    ll random_successor_cost, objective_difference;
    
    
    double bad_move_probability;
    ll random_prob;
    
    ll count = 0;
    ll sec = 0;
    
    // RANDOM RESTART
    while ((end - start)/CLOCKS_PER_SEC <= time){
        cout << endl << "NEW RESTART" << endl;
        
        new_sample_count++;
        
        // Initialising a random permutation
        unsigned seed = clock();
        shuffle(current_perm.begin(), current_perm.end(), default_random_engine(seed));
        
        // for(int i = 0; i < z; i++){
        //     mapping[i] = current_perm[i];
        // }
        
        current_cost = cost_fn_for_perm();
        
        temp = 1e18; //initialize temp with 10 lets say
        count = 0;
        while (true){
            temp = (double)temp/(double)(count+1);
            // temp = (double)temp/(double) ;
            // temp = 1e18 * exp(-1*(count+1));
            
            count++;
            cout << temp << endl;
            if (temp < 1e-10) break; // maybe set it less than epsilon instead of exactly 0
            
            // find out a random successor with a uniform distribution
            random_device rand_dev;
            mt19937 generator(rand_dev());
            uniform_int_distribution<int> distr_1(0, z-1); // check if the last element is included
            uniform_int_distribution<int> distr_2(0, l-1);
            
            ll i = distr_1(generator);
            ll j = distr_2(generator);
            while (i == j){
                j = distr_2(generator);
            }
            
            // cout << "Random indices: " << i << " " << j << endl;
            
            // calculate the cost of the random successor
            random_successor_cost = neighbour_cost(current_cost, i, j, 0, 0); // FIX THIS
            
            // calculate the difference in cost with the current cost
            objective_difference = random_successor_cost - current_cost;
            
            if (objective_difference < 0){ // if the change in cost is positive, make that change
                new_mapping({i, j}); // now make the current this swap
                current_cost = random_successor_cost;
            }else{
                // if not then calculate the probability of taking that move
                bad_move_probability = exp(-1*(double)objective_difference/(double)temp);
                bad_move_probability *= 10000;
                // cout << bad_move_probability << endl;
                
                uniform_int_distribution<int> distr_3(0, 10000);
                random_prob = distr_3(generator);
                
                // take the move with that probability
                if (random_prob <= bad_move_probability){
                    // cout << "Bad move taken" << endl;
                    new_mapping({i, j});
                    current_cost = random_successor_cost;
                }
            }
            
            // only update if the cost is lowered
            // but do we need to check every iteration of simulated annealing
            // or only the final result obtained at the end of the simulation
            if (current_cost < min_cost){
                min_cost = cost_fn_for_perm();
                for(int i = 0; i < z; i++){
                    mapping[i] = current_perm[i];
                }
            }
            
            end = clock();
            if ((end - start)/CLOCKS_PER_SEC > time){
                break;
            }
        }
        
        // here to obviously check karke mapping update karo
        
        
        end = clock();
        count++;
        // cout << "hello" << endl;
        cout << min_cost << endl;
        if ((end - start)/CLOCKS_PER_SEC > sec){
            cout << sec << " " << current_cost << endl;
            cout<< "Total restarts: " << new_sample_count <<"\n";
            sec++;
        }
    }
    return;
}

// void SportsLayout::random_sampling(){ // uncomment best mapping to run
//     vector<int> perm(l);
//     for (int i = 0; i < l; i++) perm[i] = i+1;
        
//     ll min_cost = 9*1e18;
//     ll temp_cost = 0;
    
//     clock_t start = clock();
//     clock_t end = clock();
    
//     ll count = 0;
//     ll sec = 0;
    
//     while ((end - start)/CLOCKS_PER_SEC <= time*60){
//         unsigned seed = clock();
//         shuffle(perm.begin(), perm.end(), default_random_engine(seed));
        
//         for(int i = 0; i < z; i++){
//             mapping[i] = perm[i];
//         }
        
//         temp_cost = cost_fn();
//         if (temp_cost < min_cost){
//             min_cost = temp_cost;
//             for (int j = 0; j < z; j++) bestmapping[j] = mapping[j];
//         }
        
//         end = clock();
//         count++;
        
//         if ((end - start)/CLOCKS_PER_SEC > sec){
//             cout << sec << " " << temp_cost << endl;
//             sec++;
//         }
//     }
//     mapping = bestmapping;
// }
