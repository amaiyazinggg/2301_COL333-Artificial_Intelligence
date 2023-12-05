#pragma once

#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class inputmaker
{
private:
    std::ofstream aout;
    // bool* graph; // use as graph[i*graphsize + j]
    vector<vector<bool>> graph;
    vector<vector<int>> adj;
public:
    int graphsize;
    int n;
    int k1;
    int k2;
    int a(int i);
    int b(int i);
    int x(int i, int j);
    int r_a(int i, int j);
    int r_b(int i, int j);
    int r_a2(int i, int j);
    int r_b2(int i, int j);
    inputmaker(std::string filename, std::string outfile); // make graph
    void initialize_cnf(); // minisat initial line
    void make_subgraphs_clauses(); // Ais and Bis both not zero
    void make_edges_clauses(); // adjacency matrix flattened
    void make_fullsubgraph_clauses(); // connected subgraph
    void make_atmostk_clauses(int k, bool forwho); // true means A, false means B
    void close_out(); // close aout
    void make_clauses();
    ~inputmaker();
};

