#include <iostream>
#include <bits/stdc++.h>
#include <fstream>
#include <cstdlib>
#include "SportsLayout.h"
using namespace std;

typedef long long ll;

int main(int argc, char** argv )
{
    srand((unsigned) time(NULL));
    // for(ll i = 0; i < 10; i++){
    //     ll x = rand();
    //     cout << x << endl;
    // }
    
    // Parse the input.
    if ( argc < 3 ){   
        cout << "Missing arguments\n";
        cout << "Correct format : \n";
        cout << "./main <input_filename> <output_filename>";
        exit ( 0 );
    }
    
    string inputfilename ( argv[1] );
    string outputfilename ( argv[2] );
    
    SportsLayout *s  = new SportsLayout( inputfilename );
    s -> compute_allocation();
    // s -> hill_climbing();
    // s -> hill_climbing_best_first();
    // s -> mycheck();s
    // s -> simulated_annealing();
    // s -> random_sampling(); 
    // s -> checker();
    s -> write_to_file(outputfilename);

    ll cost = s -> cost_fn();
    cout<< "cost:"<<cost<<endl;

    return 0;
}