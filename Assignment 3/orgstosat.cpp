#include "./inputmaker.hpp"

int main(int argc, char** argv)
{
    inputmaker* A = new inputmaker(argv[1], argv[2]);
    A->initialize_cnf(); //correct
    // A->make_subgraphs_clauses(); //correct
    // A->make_edges_clauses(); //
    // A->make_fullsubgraph_clauses();
    // A->make_atmostk_clauses(A->graphsize - A->k1, true);
    // A->make_atmostk_clauses(A->graphsize - A->k2, false);
    // A->close_out();
    A->make_clauses();
    
    // delete A;
}