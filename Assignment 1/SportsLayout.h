#ifndef SPORTSLAYOUT_H
#define	SPORTSLAYOUT_H

#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class SportsLayout{

    private:
    int z,l;
    int** T;
    int **N;
    // int time;
    float time;
    float delta;
    int *mapping;
    clock_t start;
    clock_t end;
    // int *bestmapping;
    long long current_cost;
    vector<int> current_perm;
    vector<long long> costs; 

    public:
    SportsLayout(string inputfilename);

    bool check_output_format();

    // void readOutputFile(string output_filename);
    
    long long cost_fn();

    long long cost_fn_for_perm();

    void write_to_file(string outputfilename);
    void write_to_file_intermediate(string outputfilename);

    void readInInputFile(string inputfilename);

    void compute_allocation();

    void random_sampling();

    void new_mapping(pair<int, int> swap);

    bool Neighbourhood();
    
    void hill_climbing();
    void hill_climbing_best_first();
    
    void simulated_annealing();

    long long neighbour_cost(long long cost, int i, int j, long long pre_cost_i, long long pre_cost_j);
    long long neighbour_cost_slow(long long cost, int i, int j);
  
    void checker();  
    
    void data_analysis();
    
    void mycheck();
};


#endif