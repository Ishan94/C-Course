
//  Created by Ishan  on 19/06/23.

#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <queue>
#include <fstream>
#include <iterator>
using namespace std;

//Graph class models a undirected graph with all edgeweights set to unity

class graph{
  
public:
    int nodes;
    vector<vector<int>> adjlist;
    
    //constructor takes in the number of nodes in a graph
    graph(int n): nodes(n) {
        adjlist.resize(nodes);
    }
    
    //set the adjacencylist as per user input; user to make sure the input represents a undirected graph
    void set_adjmatrix(const vector<vector<int>>& m){
        adjlist = m;
    }
    
    //add edge from node x to node y (zero indexed)
    void addedge(int x, int y) {
        //If no edge exists
        if(!checkedge(x, y)){
            adjlist[x].push_back(y);
            adjlist[y].push_back(x);
            sort(adjlist[x].begin(), adjlist[x].end());
            sort(adjlist[y].begin(), adjlist[y].end());
        }
        
        else{
            return;
            
        }
    }
    
    //check if edge exists from node x to node y (zero indexed)
    bool checkedge(int x, int y) const{
        
        //cout<<"Ishan "<<endl;
        for(auto a : adjlist[x]){
            if(a == y){
                return true;
            }
        }
        
        return false;
    }
    
    void printadj() const{
        
        for(int i = 0; i<nodes; i++){
            for(int j=0; j<adjlist[i].size(); j++){
                cout<<adjlist[i][j]<<"\t";
            }
            cout<<endl;
        }
    }
    
};

class DisjointSet{
    
public:
    vector<int> rank, parent;
    
    DisjointSet(int n){
        
        //using one based indexing, initialise the ranks for n nodes
        rank.resize(n+1,0);
        
        //Initialise the parent
        parent.resize(n+1);
        
        //Initially parent of a node is the node itself
        for(int i=0; i<=n; i++){
            parent[i] = i;
        }
        
    }
    
    //Find the ultimate parent of a node; takes a node as input and returns the boss
    int findUpar(int node){
        
        //check if parent of the node is the node itself
        if(parent[node]==node){
            return node;
        }
        
        else{
            //perform path compression along the way
            return parent[node] = findUpar(parent[node]);
        }
        
    }
    
    //Perform union by rank
    void unionByRank(int u, int v){
        
        int ulp_u = findUpar(u);
        int ulp_v = findUpar(v);
        
        //both u and v already belong on the same set
        if(ulp_u == ulp_v){
            return;
        }
        
        else{
            
            if(rank[ulp_u]>rank[ulp_v]){
                //connect v to u's ultimate parent
                parent[ulp_v] = ulp_u;
            }
            
            else if(rank[ulp_u]<rank[ulp_v]){
                //connect u to v's ultimate parent
                parent[ulp_u] = ulp_v;
            }
            
            else{
                //anyone can be connected
                parent[ulp_v] = ulp_u;
                //increase rank of ulp_u by 1
                rank[ulp_u]++;
            }
        }
        
    }
    
    //Check if path exists from node node u to node v (nodes u and v belong in the same set)
    
    bool pathexists(int u, int v) {
        
        
        if(findUpar(u)==findUpar(v)){
            return true;
        }
        
        return false;
    }
    
};

//enumerator for the state of a cell

enum class state{
    
    red_cell,
    blue_cell,
    empty_cell
};

//enumerator for the player type

enum class player{
    
    red_player,
    blue_player
};


enum class winner{
    red,
    blue,
    noresultyet
};

//Class to model a Hex Board --> Hex board is modelled as a graph with n*n nodes and connectivity as outlined in the lectures

//Red and Blue moves are stored in two disjoint sets

class HexBoard{
    
public:
    
    int size;
    graph* g;
    
    HexBoard(int n): size(n), g(new graph(n*n)){
        createhexboard(n);
    }
    
    void createhexboard(int n){
        
        for(int i=0; i<n*n; i++){
            
            int row = i/n;
            int col = i%n;
            
            //Top Left
            if(row==0 && col==0){
                g->addedge(i, i+1);
                g->addedge(i, i+n);
            }
            
            //Bottom right
            else if(row==n-1 && col==n-1){
                g->addedge(i, i-1);
                g->addedge(i, i-n);
            }
            
            //Top right
            else if(row==0 && col==n-1){
                g->addedge(i, i-1);
                g->addedge(i, i+n);
                g->addedge(i, i+n-1);
            }
            
            //Bottom Left
            else if(row==n-1 && col==0){
                g->addedge(i, i+1);
                g->addedge(i, i-n);
                g->addedge(i, i-n+1);
            }
            
            //Top row (excluding corners)
            else if(row==0 && col!=0 && col!=n-1){
                g->addedge(i, i-1);
                g->addedge(i, i+1);
                g->addedge(i, i+n-1);
                g->addedge(i, i+n);
            }
            
            //Bottom row (excluding corners)
            else if(row==n-1 && col!=0 && col!=n-1){
                g->addedge(i, i-1);
                g->addedge(i, i+1);
                g->addedge(i, i-n);
                g->addedge(i, i-n+1);
            }
            
            //left column (excluding corners)
            else if(col==0 && row!=0 && row!=n-1){
                g->addedge(i, i+n);
                g->addedge(i, i+1);
                g->addedge(i, i-n);
                g->addedge(i, i-n+1);
            }
            
            //Right column (excluding corners)
            else if(col==n-1 && row!=0 && row!=n-1){
                g->addedge(i, i-1);
                g->addedge(i, i+n-1);
                g->addedge(i, i-n);
                g->addedge(i, i+n);
            }
            
            //Internal Nodes
            else {
                g->addedge(i, i-1);
                g->addedge(i, i+1);
                g->addedge(i, i-n);
                g->addedge(i, i-n+1);
                g->addedge(i, i+n);
                g->addedge(i, i+n-1);
            }
        }
    }
};

class HexGame{
    
public:
    HexBoard board;
    player turn;
    winner w;
    vector<int> top_row, bottom_row, left_col, right_col;
    DisjointSet* red_set;
    DisjointSet* blue_set;
    vector<state> s;
    
    HexGame(int n):board(HexBoard(n)), turn(player::blue_player), w(winner::noresultyet), red_set(new DisjointSet(n*n)), blue_set(new DisjointSet(n*n)){
        
        s.resize(n*n+1, state::empty_cell);
        top_row.resize(board.size);
        bottom_row.resize(board.size);
        left_col.resize(board.size);
        right_col.resize(board.size);
        fill_columns();
        
    }
    
    void fill_columns(){
        
        int corner1 = 0, corner2 = board.size*(board.size-1), corner3 = -1*board.size , corner4 = -1;
        generate(top_row.begin(), top_row.end(), [corner1]() mutable {return corner1++;});
        generate(bottom_row.begin(), bottom_row.end(), [corner2]() mutable {return corner2++;});
        generate(left_col.begin(), left_col.end(), [&, corner3]() mutable {return corner3+= board.size;});
        generate(right_col.begin(), right_col.end(), [&, corner4]() mutable {return corner4+= board.size;});
        
    }
    
    bool checkmove(int r, int c, player p){
        return r<board.size && c<board.size && s[r*board.size+c]==state::empty_cell;
    }
    
    int makemove(int r, int c, player p){
        
        if(checkmove(r, c, p)){
            int n = r*board.size+c;
            //update the state of selected cell (potentially dangerous?!)
            s[n] = p==player::blue_player?state::blue_cell:state::red_cell;
            
            cout<<*this<<endl;
            
            if(p==player::blue_player){
                //Blue player made a legal move
                //check all neighbours of the selected cell
                for(auto x:board.g->adjlist[n]){
                    //if a neighbour is occupied by blue, add them to the set (create a bridge)
                    if(s[x]==state::blue_cell){
                        blue_set->unionByRank(n, x);
                    }
                }
                findwinner();
            }
            else if(p==player::red_player){
                //Red Player makes a legal move
                //check all neighnours of the selected cell
                for(auto x:board.g->adjlist[n]){
                    if(s[x]==state::red_cell){
                        red_set->unionByRank(n, x);
                    }
                }
                findwinner();
            }
            else{
                cout<<"Something went wrong!"<<endl;
                return 0;
            }
        }
        else{
            cout<<"The move is not legal, the same player should make another move "<<endl;
            return 0;
        }
        return 1;
    }
    
    void findwinner(){
        
        //winner p = winner::noresultyet;
        
        //Top and bottom row belong to the blue player
        for(auto i:top_row){
            for(auto j:bottom_row){
                //cout<<"The value of i is "<<i<<endl;
                
                if(blue_set->findUpar(i)==blue_set->findUpar(j)){
                    cout<<"Blue player wins!"<<endl;
                    w = winner::blue;
                    return;
                }
            }
        }
        
        //Left and Right row are for the red player
        for(auto i:left_col){
            for(auto j:right_col){
                if(red_set->findUpar(i)==red_set->findUpar(j)){
                    cout<<"Red Player wins!"<<endl;
                    w = winner::red;
                    return;
                }
            }
        }
        return;
    }
    
    
    friend ostream& operator<<(ostream& out, const HexGame& h){
        
        int indent = 1;
        for (int i=0; i<h.board.size; i++) {
          
          cout << setiosflags(ios::left);

          cout << setw(indent * 2) << setfill(' ') << ' ';
          int col = 0;
          for (int j=0; j<h.board.size; j++) {
            char cell;
                
            int num = i*h.board.size+j;
            if(h.s[num]==state::empty_cell){
                cell='.';
            }
            else if (h.s[num]==state::red_cell){
                cell = 'X';
            }
            else if (h.s[num]==state::blue_cell){
                cell = 'O';
            }
            else{
                cell='N';
                cout<<"Something went wrong!"<<endl;
                return out;
            }
            string link = ++col == h.board.size ? "" : " - ";
            cout << cell << link;
          }
          
          cout << endl;
            
          if (indent < h.board.size){
            cout << setw(indent * 2) << setfill(' ') << ' ';
            cout << " \\";
            for (int k = 1; k < h.board.size; k ++){
              cout << " / \\";
            }
            cout << endl;
              indent++;
          }
        }
        
        return out;
    }

};


int main(int argc, const char * argv[]) {
    

    HexGame game(11);
    
    cout<<"Enter your player choice:\n1. Red Player\n2. Blue Player "<<endl;
    int x;
    cin>>x;
    
    player p = player::blue_player;
    
    while(game.w!= winner::red && game.w!=winner::blue){
        
        if(p==player::blue_player){
            
            bool valid = true;
            
            do{
                int b_row, b_col;
                cout<<"Blue Player's turn, enter the move "<<endl;
                cout<<"Row"<<endl;
                cin>>b_row;
                cout<<"\nColumn"<<endl;
                cin>>b_col;
                
                if(!game.checkmove(b_row, b_col, p)){
                    cout<<"Invalid blue move, please pick another move "<<endl;
                    continue;
                }
                else{
                    valid = false;
                    game.makemove(b_row,b_col,p);
                    p = player::red_player;
                }
            }while(valid);
        }
        
        else if(p==player::red_player){
            
            bool valid = true;
            
            do{
                int r_row, r_col;
                cout<<"Red Player's turn, enter the move "<<endl;
                cout<<"Row"<<endl;
                cin>>r_row;
                cout<<"\nColumn"<<endl;
                cin>>r_col;
                
                if(!game.checkmove(r_row, r_col, p)){
                    cout<<"Invalid red move, please pick another move "<<endl;
                    continue;
                }
                else{
                    valid = false;
                    game.makemove(r_row,r_col,p);
                    p = player::blue_player;
                }
            }while(valid);
        }
        
    }
    
    cout<<"Game Over!!"<<endl;
    
    return 0;
}
