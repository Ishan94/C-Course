//
//  Created by Ishan  on 25/06/23.

#include <iostream>
#include <vector>
#include <random>
#include <iomanip>
#include <queue>
#include <fstream>
#include <iterator>
#include <chrono>
#include <algorithm>
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
    ~graph(){}
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
    
    ~DisjointSet(){}
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
//enumerator for the game winner
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
    ~HexBoard(){
        delete g;
    }
};

class HexGame{
public:
    HexBoard board;
    player turn;
    winner w;
    //Top row (north) & bottom row (south) for Blue Player
    //Left col (west) & right col (east) for Red Player (AI)
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
    
    int SimulateMoves(){
        
        vector<int> available_cells;

        //Fill the vector with cells that can be potentially taken
        for(int i = 0; i<s.size()-1; i++){
            if(s[i]==state::empty_cell){
                available_cells.push_back(i);
            }
        }
        
        //Copy the current state of ranks, parents for Red/Blue Disjoint Set
        vector<int> red_rank, red_parent;
        vector<int> blue_rank, blue_parent;
        
        red_rank.assign(red_set->rank.begin(), red_set->rank.end());
        blue_rank.assign(blue_set->rank.begin(), blue_set->rank.end());
        
        red_parent.assign(red_set->parent.begin(), red_set->parent.end());
        blue_parent.assign(blue_set->parent.begin(), blue_set->parent.end());
        
        //Copy the available cells to explore all choices
        vector<int> temp;
        temp.assign(available_cells.begin(), available_cells.end());
        
        double win_ratio = -1;
        int best_move = -1;
        
        //Blue Set Rank and Parent vector at start of each simulation
        vector<int> blu_rcopy, blu_pcopy;
        
        for(int i=0; i<available_cells.size(); i++){
            
            int current_move = available_cells[i];
            
            //This move changes the Blue Set Rank/Parent
            makemove(static_cast<int>(current_move/board.size), static_cast<int>(current_move%board.size), player::blue_player);
            
   
            blu_rcopy.assign(blue_set->rank.begin(), blue_set->rank.end());
            blu_pcopy.assign(blue_set->parent.begin(), blue_set->parent.end());
            
            //All cells except current available cell
            temp.erase(temp.begin()+i);
            
            int red = 0, blue = 0;
            int nsim_max = 1000;
            
            for(int nsim = 0; nsim<nsim_max; nsim++){
                
                shuffle(temp.begin(), temp.end(), default_random_engine(static_cast<unsigned int>(chrono::system_clock::now().time_since_epoch().count())));
                
                //Making Random moves using Shuffled vector
                for(int k=0; k<temp.size(); k++){
                    
                    if(k%2==0)
                        makemove(static_cast<int>(temp[k]/board.size), static_cast<int>(temp[k]%board.size), player::red_player);
                    else
                        makemove(static_cast<int>(temp[k]/board.size), static_cast<int>(temp[k]%board.size), player::blue_player);
                }
                
                findwinner();
                
                if(this->w==winner::blue){
                    blue++;
                }
                else{
                    red++;
                }
                
                //Setting states back to start of (this) simulation
                red_set->rank.assign(red_rank.begin(), red_rank.end());
                red_set->parent.assign(red_parent.begin(), red_parent.end());
                
                blue_set->rank.assign(blu_rcopy.begin(), blu_rcopy.end());
                blue_set->parent.assign(blu_pcopy.begin(), blu_pcopy.end());
                
                for(auto x:temp){
                    s[x] = state::empty_cell;
                }
                
                this->w = winner::noresultyet;
                
            }//end of all simulations for current_move
            
            //Setting states of blue set back to start of the function
            blue_set->rank.assign(blue_rank.begin(), blue_rank.end());
            blue_set->parent.assign(blue_parent.begin(), blue_parent.end());
            s[current_move] = state::empty_cell;
            temp.assign(available_cells.begin(), available_cells.end());
            
            if(static_cast<double>(blue*100.0/(nsim_max*1.0)) > win_ratio*100.0){
                win_ratio = static_cast<double>(blue*1.0/(nsim_max*1.0));
                best_move = available_cells[i];
            }
        }
            
        return best_move;
    }
  
    int makemove(int r, int c, player p){
        
        if(checkmove(r, c, p)){
            int n = r*board.size+c;
            s[n] = p==player::blue_player?state::blue_cell:state::red_cell;
            
            if(p==player::blue_player){
                //Blue player made a legal move
                //check all neighbours of the selected cell
                for(auto x:board.g->adjlist[n]){
                    //if a neighbour is occupied by blue, add them to the set (create a bridge)
                    if(s[x]==state::blue_cell){
                        blue_set->unionByRank(n, x);
                    }
                }
            }
            else if(p==player::red_player){
                //Red Player makes a legal move
                //check all neighnours of the selected cell
                for(auto x:board.g->adjlist[n]){
                    if(s[x]==state::red_cell){
                        red_set->unionByRank(n, x);
                    }
                }
                //findwinner();
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

        //Top and bottom row belong to the blue player
        for(auto i:top_row){
            for(auto j:bottom_row){
                if(blue_set->findUpar(i)==blue_set->findUpar(j)){
                    //cout<<"Blue Player wins!"<<endl;
                    w = winner::blue;
                    return;
                }
            }
        }
        
        //Left and Right row are for the red player
        for(auto i:left_col){
            for(auto j:right_col){
                if(red_set->findUpar(i)==red_set->findUpar(j)){
                    //cout<<"Red Player wins!"<<endl;
                    w = winner::red;
                    return;
                }
            }
        }
        
        w = winner::noresultyet;
        //cout<<"No result yet "<<endl;
        return;
    }
    
    ~HexGame(){
        delete red_set;
        delete blue_set;
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
    
    cout<<"Enter the size of the board (recommended, 5-11) "<<endl;
    int x;
    cin>>x;
    
    cout<<"Setting up the "<<x<<" X "<<x<<" Board "<<endl;
    HexGame game(x);
    
    cout<<game<<endl;
    
    cout<<"Your colour is Red (X), Computer's colour is Blue (O), you go first!! "<<endl;
    
    player p = player::red_player;
    
    while(game.w!= winner::red && game.w!=winner::blue){
        
        if(p==player::blue_player){
            
            bool valid = true;
            
            do{
                cout<<"Computer's Turn "<<endl<<endl;
                
                //time the computation
                auto started = std::chrono::high_resolution_clock::now();
                int comp = game.SimulateMoves();
                int comp_row = static_cast<int>(comp/game.board.size);
                int comp_col = static_cast<int>(comp%game.board.size);
                auto done = std::chrono::high_resolution_clock::now();
                //end of timer
                
                cout<<"Computers Move selected (in "<<std::chrono::duration_cast<std::chrono::milliseconds>(done-started).count()/1000.0<<"s), row "<<comp_row<<", column "<<comp_col<<endl;
                
                if(!game.checkmove(comp_row, comp_col, p)){
                    cout<<"Invalid blue move, please pick another move "<<endl;
                    continue;
                }
                else{
                    valid = false;
                    game.makemove(comp_row,comp_col,p);
                    game.findwinner();
                    p = player::red_player;
                    cout<<game;
                }
            }while(valid);
        }
        
        else if(p==player::red_player){
            
            bool valid = true;
            
            do{
                int r_row, r_col;
                cout<<"Your turn, enter the move "<<endl;
                cout<<"Row"<<endl;
                cin>>r_row;
                cout<<"\nColumn"<<endl;
                cin>>r_col;
                
                cout<<"Moves selected, row "<<r_row<<", column "<<r_col<<endl;
                
                if(!game.checkmove(r_row, r_col, p)){
                    cout<<"Invalid red move, please pick another move "<<endl;
                    continue;
                }
                else{
                    valid = false;
                    game.makemove(r_row,r_col,p);
                    game.findwinner();
                    p = player::blue_player;
                    cout<<game;
                }
            }while(valid);
        }
    }
    
    if(game.w == winner::blue){
        cout<<"Computer wins !!"<<endl;
    }
        
    if(game.w == winner::red){
        cout<<" Congrats, you win !!"<<endl;
    }
    
    cout<<"Game Over!!"<<endl;

    return 0;
}
