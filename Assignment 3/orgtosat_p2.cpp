#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int n;
int k;

int a(int i){
    return i;
}

int x(int i, int j){ // 1 <= j < i <= n;
    return n + ((i-1)*(i-2))/2 + j;
}

int r(int i, int j){
    return n + (n*(n-1))/2 + (i-1)*(n-k) + j;
}

int main(int argc, char** argv)
{
    k = stoi(argv[1]);
    ifstream ain;
    ofstream aout;
    ain.open(argv[2]);
    aout.open(argv[3]);

    int m;
    ain >> n >> m;

    vector<vector<bool>> graph(n, vector<bool>(n, false));
    vector<vector<int>> adj(n+1);

    int e1, e2;
    for (int i = 0; i < m; i++){
        ain >> e1 >> e2;
        // graph[(e1-1)*n + (e2-1)] = true;
        // graph[(e2-1)*n + (e1-1)] = true;
        adj[e1].push_back(e2);
        adj[e2].push_back(e1);
        
        graph[e1-1][e2-1] = true;
        graph[e2-1][e1-1] = true;
    }
    ain.close();
    int count = 0;
    for (int i = 1; i < n+1; i++){
        if (adj[i].size() < k-1){
            count++;
        }
    }

    int vars = (n + ((n)*(n-1))/2 + (n)*(n-k));
    cout << "vars: " << vars << endl;
    int clauses = ((n-1)*(n)) + 2*(n)*(n-k) + n - (n-k) + count;
    cout << "clauses: " << clauses << endl;

    aout << "p cnf " << vars << " " << clauses  << std::endl;

    count = 0;
    for (int i = 1; i < n+1; i++){
        if (adj[i].size() < k-1) aout << a(i) << " " << 0 << endl;
        count++;
    }
    // Xij
    count = 0;
    for (int i = 1; i < n+1; i++){
        for (int j = 1; j < i; j++){
            if (graph[i-1][j-1]) aout << x(i, j) << " " << 0 << endl;
            // if (graph[n*(i-1) + (j-1)]) aout << x(i, j) << " " << 0 << endl;
            else aout << "-" << x(i, j) << " " << 0 << endl;
            count++;
        }
    }
    cout << count << endl;

    // implies complete
    for (int i = 1; i < n+1; i++){
        for (int j = 1; j < i; j++){
            aout << a(i) << " " << a(j) << " " << x(i, j) << " " << 0 << endl;
            count++;
        }
    }
    cout << count << endl;

    // count registers A
    // n
    for (int i = 1; i < n+1; i++){
        aout << "-" << a(i) << " " << r(i, 1) << " " << 0 << endl;
        count += 1;
    }
    cout << count << endl;
    // (n-1)(k)
    for (int i = 2; i < n+1; i++){
        for (int j = 1; j < (n-k)+1; j++){
            aout << "-" << r(i-1, j) << " " << r(i, j) << " " << 0 << endl;
            count++;
        }
    }
    cout << count << endl;
    // (n-1)(k-1)
    for (int i = 2; i < n+1; i++){
        for (int j = 2; j < (n-k)+1; j++){
            aout << "-" << a(i) << " -" << r(i-1, j-1) << " " << r(i, j) << " " << 0 << endl;
            count++;
        }
    }
    cout << count << endl;
    
    // k-1
    for (int j = 2; j < (n-k)+1; j++){
        aout << "-" << r(1, j) << " " << 0 << endl;
        count++;
    }
    cout << count << endl;
    
    // n
    for (int i = 2; i < n+1; i++){
        aout << "-" << a(i) << " -" << r(i-1, n-k) << " " << 0 << endl;
        count+=1;
    }
    aout << r(n, n-k) << " " << 0 << endl;
    count++;
    cout << count << endl;

    aout.close();
}