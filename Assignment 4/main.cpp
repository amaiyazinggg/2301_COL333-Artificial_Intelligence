#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <filesystem>
#include <bits/stdc++.h>

using namespace std;
class Graph_Node{
public:
    string Node_Name;  // Variable name
    vector<int> Children; // Children of a particular node - these are index of nodes in graph.
    vector<string> Parents; // Parents of a particular node :- note these are names of parents
    int nvalues;  // Number of categories a variable represented by this node can take
    vector<string> values; // Categories of possible values
    vector<double> CPT; // conditional probability table as a 1-d array . Look for BIF format to understand its meaning
    Graph_Node(string& name,int n,vector<string>& vals)
    {
        Node_Name=name;
        nvalues=n;
        values=vals;
    }
    string get_name()
    {
        return Node_Name;
    }
    vector<int>& get_children()
    {
        return Children;
    }
    vector<string>& get_Parents()
    {
        return Parents;
    }
    vector<string>& get_values()
    {
        return values;
    }
    void set_CPT(vector<double>& new_CPT)
    {
        CPT.clear();
        CPT=new_CPT;
    }
    void set_Parents(vector<string>& Parent_Nodes)
    {
        Parents.clear();
        Parents=Parent_Nodes;
    }
    int add_child(int new_child_index )
    {
        for(int i : Children)
        {
            if(i==new_child_index)
                return 0;
        }
        Children.push_back(new_child_index);
        return 1;
    }
};

class network{
    vector <Graph_Node*> Pres_Graph;
    map<string, int> index;
    vector<pair<vector<vector<double>>, int>> data;
    double smoothing = 0.001;
public:
    void addNode(Graph_Node* node)
    {
        if(index.find(node->get_name()) != index.end()){
            cout<<"Already Existed Node\n";
            return;
        }
        Pres_Graph.push_back(node);
        index[node->get_name()] = (int) Pres_Graph.size() - 1;
    }
    // get the index of node with a given name
    int get_index(string& val_name)
    {
        return index[val_name];
    }
    //get the iterator of a node with a given name
    Graph_Node* search_node(string& val_name)
    {
        if (index.find(val_name) != index.end()) {
            return Pres_Graph[index[val_name]];
        }
        cout << "node not found\n";
        return nullptr;
    }

    void read_dataset(string dat){
        ifstream myfile(dat);
        string temp;
        string line;
        if (myfile.is_open()) {
            cout << "file opened\n";
            while (!myfile.eof()) {
                stringstream ss;
                getline(myfile, line);
                vector<vector<double>> line_data;
                ss.str(line);
                int pos = -1;
                for (int i = 0; i < Pres_Graph.size(); ++i) {
                    ss>>temp;
                    Graph_Node* node = Pres_Graph[i];
                    if(temp == "\"?\"") {
                        pos = i;
                        vector<double> node_data(node->nvalues, (double) 1 / (double)node->nvalues);
                        line_data.push_back(node_data);
                        continue;
                    }
                    vector<double> node_data;
                    for (int j = 0; j < node->nvalues; ++j) {
                        if(temp == node->values[j]){
                            node_data.push_back((double) j);
//                            break;
                        }
                    }
                    if (node_data.size() > 1){
                        cout<<"problem\n";
                        ::exit(0);
                    }
                    line_data.push_back(node_data);
                }
                data.emplace_back(line_data, pos);
            }
//            for (int i = 0; i < 2; ++i) {
//                cout<<data[i].second<<"\n";
//                for (int j = 0; j < data[i].first.size(); ++j) {
//                    for (int k = 0; k < data[i].first[j].size(); ++k) {
//                        cout<<data[i].first[j][k]<<" ";
//                    }
//                    cout<<"\n";
//                }
//                cout<<"\n";
//            }
        } else{
            cout<<"Not able to open the dataset file\n";
        }
    }


    void update_CPT() {
        double sun = 0;
        vector<double> temp;

        Graph_Node* node;
        int i,j,k,p,pos,s,fou,l;
        vector<double> sum;
        for (i = 0; i < Pres_Graph.size(); ++i) {
            node = Pres_Graph[i];
            vector<string>& Parents = node->get_Parents();
            temp=vector<double>(node->CPT.size(), 0);
            sun = 0;
            for (j = 0; j < data.size(); ++j) {
                p = (int) (node->CPT.size()/node->nvalues);
                pos = 0;
                s = -1;
                fou = -1;
                if(i == data[j].second){
                    fou = i;
                    s = p;
                }else{
                    pos += ((int) data[j].first[i][0]) * (p);
                }
                for(l = 0; l < Parents.size(); ++l) {
                    p /= Pres_Graph[index[Parents[l]]]->nvalues;
                    if (index[Parents[l]] == data[j].second){
                        fou = data[j].second;
                        s = p;
                    }
                    else{
                        pos += ((int) data[j].first[index[Parents[l]]][0]) * (p);
                    }
                }
                if (fou != -1){
                    for (k = 0; k < Pres_Graph[fou]->nvalues; ++k) {
                        temp[pos + k*s] += data[j].first[fou][k];
                        sun += data[j].first[fou][k];
                    }
                } else{
                    temp[pos] += 1.0;
                    sun += 1;
                }
            }
            sum=vector<double>(temp.size() / node->nvalues);
            for (j = 0; j < temp.size(); ++j) {
                temp[j] += smoothing*node->nvalues;
            }
            for (j = 0; j < temp.size(); ++j) {
                sum[j % (temp.size() / node->nvalues)] += temp[j];
            }
            for (j = 0; j < temp.size(); ++j) {
                temp[j] /= sum[j % (temp.size() / node->nvalues)];
            }
            node->set_CPT(temp);
        }
    }
    double prob_calc(int val, vector<vector<double>> &dat, Graph_Node* node, int parent_index, int parent_val){
        int p = node->CPT.size();
        p /= node->nvalues;
        int pos = 0;
        pos += val*p;
        vector<string> &Parents = node->get_Parents();
        for(int l = 0; l < Parents.size(); ++l) {
            p /= Pres_Graph[index[Parents[l]]]->nvalues;
            if(parent_index == index[Parents[l]]){
                pos += ((int) parent_val) * (p);
            }
            pos += ((int) dat[index[Parents[l]]][0]) * (p);
        }
        return node->CPT[pos];
    }
    void update_data(){
        int i,j,node_index;
        double sum;
        Graph_Node* node;
        vector<double> probs;
        for (i = 0; i < data.size(); ++i) {
            if(data[i].second != -1){
                node_index = data[i].second;
                node = Pres_Graph[node_index];
                probs=vector<double>(node->nvalues, 0);
                sum = 0;
                for (j = 0; j < node->nvalues; ++j) {
                    probs[j] = prob_calc(j, data[i].first, node, -1, -1);
                    for (int k = 0; k < node->Children.size(); ++k) {
                        probs[j] *= prob_calc(data[i].first[node->Children[k]][0], data[i].first, Pres_Graph[node->Children[k]], node_index, j);
                    }
                    sum += probs[j];
                }
                for (j = 0; j < node->nvalues; ++j) {
                    data[i].first[node_index][j] = 0.5*data[i].first[node_index][j] + 0.5*(probs[j] / sum);
                }
            }
        }
    }

    void write_network(string bif){
        ifstream myfile(bif);
        ofstream yourfile("./solved_alarm.bif");
        string output = "";
        string line;
        string temp;
        string name;

        if(myfile.is_open()){
//            cout<<"in file";
            int k = 0;
            while (! myfile.eof() ) {
                getline (myfile,line);
                output += line + "\n";
                stringstream ss;
                ss.str(line);
                ss>>temp;
//                cout<<temp<<" ";
                if (temp == "probability") {
//                    cout<<"hello\n";
                    output += "\ttable ";
                    for (int j = 0; j < Pres_Graph[k]->CPT.size(); ++j) {
                        double x = 0;
                        if (not::isnan(Pres_Graph[k]->CPT[j])) {
                            x = Pres_Graph[k]->CPT[j];
                        }
                        std::ostringstream oss;
                        oss << std::fixed << std::setprecision(4) << x;
                        output += oss.str() + " ";
                    }
                    output += ";\n";
                    getline(myfile, line);
                    k += 1;

                }
            }
        } else{
            cout<<"no file opened\n";
        }
        output.pop_back();
        yourfile<<output;
    }


    void inititalize_CPT(){
        for (int i = 0; i < Pres_Graph.size(); ++i) {
            Graph_Node* node = Pres_Graph[i];
            vector<double> vector1(node->CPT.size(), (double) 1/ (double) node->nvalues);
            node->set_CPT(vector1);
        }
    }
};



network read_network(string bif)
{
    network Alarm;
    string line;
    int find=0;
    std::filesystem::path currentDir = std::filesystem::current_path();
    std::cout << "Current Directory: " << currentDir << std::endl;
    ifstream myfile(bif);
    string temp;
    string name;
    vector<string> values;

    if (myfile.is_open())
    {
        while (! myfile.eof() )
        {
            stringstream ss;
            getline (myfile,line);
            ss.str(line);
            ss>>temp;
            if(temp=="variable")
            {
                ss>>name;
                getline (myfile,line);
                stringstream ss2;
                ss2.str(line);
                for(int i=0;i<4;i++){
                    ss2>>temp;
                }
                values.clear();
                while(temp!="};"){
                    values.push_back(temp);
                    ss2>>temp;
                }
                auto new_node = new Graph_Node(name, (int) values.size(), values);
                Alarm.addNode(new_node);
            }
            else if(temp=="probability")
            {

                ss>>temp;
                ss>>temp;

                Graph_Node* vectorIt;
                Graph_Node* vectorIt1;
                vectorIt=Alarm.search_node(temp);
                int index=Alarm.get_index(temp);
                ss>>temp;
                values.clear();
                while(temp.compare(")")!=0)
                {
                    vectorIt1=Alarm.search_node(temp);
                    vectorIt1->add_child(index);
                    values.push_back(temp);

                    ss>>temp;

                }
                vectorIt->set_Parents(values);
                getline (myfile,line);
                stringstream ss2;

                ss2.str(line);
                ss2>> temp;

                ss2>> temp;

                vector<double> curr_CPT;
                string::size_type sz;
                while(temp!=";")
                {

                    curr_CPT.push_back(atof(temp.c_str()));

                    ss2>>temp;



                }

                vectorIt->set_CPT(curr_CPT);


            }
        }
        if(find==1) {
            myfile.close();
        }
    } else{
        cout<<"Not able to open the file\n";
    }

    return Alarm;
}


int main(int argc, char* argv[])
{
    clock_t a = ::clock(),c;
    network Alarm;
    Alarm = read_network(argv[1]);
    Alarm.read_dataset(argv[2]);
    int i=0;
    float time=10,current_time;
    while(true){
        c=::clock();
        current_time=(c- a) / CLOCKS_PER_SEC;
        if(current_time+time+8>=120)
            break;
        Alarm.update_CPT();
        Alarm.update_data();
        time=((::clock()-c) / CLOCKS_PER_SEC+time*i)/(i+1);
        i++;
//        cout<<i<<"\n";
    }
    Alarm.write_network(argv[1]);
}