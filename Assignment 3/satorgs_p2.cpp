#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>

void outputmaker(std::string graphfile, std::string satfile, std::string outfile);

int main(int argc, char** argv)
{
    outputmaker(argv[1], argv[2], argv[3]);
}

void outputmaker(std::string graphfile, std::string satfile, std::string outfile)
{
    std::cout << "START" << std::endl;
    std::ifstream sat;
    std::ofstream aout;
    aout.open(outfile);
    sat.open(satfile);
    std::string JUDGEMENT;
    sat >> JUDGEMENT;
    if (JUDGEMENT == "UNSAT")
    {
        aout << 0;
        aout.close();
        sat.close();
        std::cout << "DONE" << std::endl;
        return;    
    }
    int n,m;
    std::ifstream grph;
    grph.open(graphfile);
    grph >> n >> m;
    grph.close();
    int t;
    bool flag=false;
    aout << "#1" << std::endl;
    for(int i = 0; i < n; i++)
    {
        sat >> t;
        if (t < 0)
        {
            if (flag)
            {
                aout << " " << -t;
            }
            else
            {
                aout << -t;
                flag = true;
            }
        }
    }
    
    aout.close();
    sat.close();
    std::cout << "DONE" << std::endl;
    return;
}