#include<bits/stdc++.h>
#include <iostream>
using namespace std;

struct Node{
    int n;
    vector< vector<int> > grid;
    int step;
    string direcion;
    Node* par;
    Node(int n){
        this->n = n;
        grid.resize(n, vector<int>(n,0));
        step=0;
        direcion="";
        par=nullptr;
    }

    Node(int n, vector< vector<int> > grid, int step, string direcion, Node* par){
        this->n = n;
        this->grid=grid;
        this->step=step;
        this->direcion = direcion;
        this->par=par;
    }

    vector<Node*> getAdj(){
        vector<Node*> adj;
        int di[]={0,0,1,-1};
        int dj[]={1,-1,0,0};
        string dir[] = {"LEFT","RIGHT","UP","DOWN"};
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(grid[i][j]==0){
                    for(int k=0;k<4;k++){
                        int ii = i+di[k];
                        int jj = j+dj[k];
                        if(ii>=0 && ii<n && jj>=0 && jj<n){
                            swap(grid[i][j],grid[ii][jj]);
                            Node *node= new Node(n, grid, step+1, dir[k], this);
                            swap(grid[i][j],grid[ii][jj]);

                            if(par==nullptr || *node!=*par){
                                adj.push_back(node);
                            }
                            else{
                                delete node;
                            }
                        }
                    }
                    return adj;
                }
            }
        }
    }

    bool operator != (const Node &nd){
        if(nd.n != n) return true;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){
                if(grid[i][j] != nd.grid[i][j]) return true;
            }
        }
        return false;
    }

    friend ostream & operator << (ostream &out, const Node &nd);
    friend istream & operator >> (istream &in,  Node &nd);
};

ostream & operator << (ostream &out, const Node &nd){
    out<<nd.step<<" ::::::::::::::: "<<nd.direcion<<endl;
    for(int i=0;i<nd.n;i++){
        for(int j=0;j<nd.n;j++){
            out<<nd.grid[i][j]<<' ';
        }
        out<<endl;
    }
    return out;
}

istream & operator >> (istream &in, Node &nd){
    for(int i=0;i<nd.n;i++){
        for(int j=0;j<nd.n;j++){
            in >> nd.grid[i][j];
        }
    }
    return in;
}

struct HammingCompare{
    int getDistance(Node* const& nd){
        int dis=0;
        for(int i=0;i<nd->n;i++){
            for(int j=0;j<nd->n;j++){
                int actual = i*nd->n+j+1;
                if(actual!=nd->grid[i][j]) dis++;
            }
        }
        dis--; // for the blank
        return dis;
    }
    bool operator()(Node* const& n1, Node* const& n2){
        return n1->step+getDistance(n1) > n2->step+ getDistance(n2);
    }
};

struct ManHattanCompare{
    int getDistance(Node* const& nd){
        int dis=0;
        for(int i=0;i<nd->n;i++){
            for(int j=0;j<nd->n;j++){
                int x = nd->grid[i][j]-1;
                if(x<0) continue; // for the blank
                int ii = x/nd->n;
                int jj = x%nd->n;
                dis+=abs(ii-i)+abs(jj-j);
            }
        }
        return dis;
    }
    bool operator()(Node* const& n1, Node* const& n2){
        return n1->step+getDistance(n1) > n2->step+ getDistance(n2);
    }
};

bool solvable(Node *start){
    int n = start->n;
    int inv=0;
    int row0;
    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            if(start->grid[i][j]==0){
                row0 = i;
                continue;
            }
            for(int p=0;p<=i;p++){
                for(int q=0;q<(p==i? j:n);q++){
                    if(start->grid[p][q]==0) continue;
                    if(start->grid[i][j]<start->grid[p][q]) inv++;
                }
            }
        }
    }
    if(n%2){
        return inv%2==0;
    }
    else{
        return n-row0 != inv%2;
    }


}

template<typename T>
void AStarSearch(Node* start){

    if(!solvable(start)){
        cout<<"Not Solvable"<<endl;
        return;
    }
    cout<<"Solvable"<<endl;
    priority_queue<Node*, vector<Node*> ,T> pq;
    pq.push(start);
    int explored=0,expanded=1;
    T t;
    Node* fin;
    set<Node*> s;
    map< vector<vector<int> >,int> vis;
    while(!pq.empty()){
         Node* u = pq.top();
         pq.pop();
         explored++;
         vis[u->grid]=1;
         if(t.getDistance(u)==0){
            fin = u;
            break;
         }
         vector<Node*> nodes = u->getAdj();
         for(auto v: nodes){
            s.insert(v);
            if(vis[v->grid]) continue;
            expanded++;
            pq.push(v);
         }
    }

    cout<<"Number of steps to reach the end state "<<fin->step<<endl<<endl;
    stack<Node*> st;
    st.push(fin);

    while(fin->par!=nullptr){
        fin=fin->par;
        st.push(fin);
    }

    while(!st.empty()){
        Node* u = st.top();
        st.pop();
        cout<<*u;
    }
    cout<<endl;
    cout<<"Total expanded "<<expanded<<endl;
    cout<<"Total explored "<<explored<<endl;
    cout<<"----------------------"<<endl;
    cout<<endl;

    for(auto node: s) delete node;

}

int main(){
    while(true){
        cout<<"Enter the grid size: ";
        int n;
        cin>>n;
        cout<<"Enter the grid (0 for blank): "<<endl;
        Node* node = new Node(n);
        cin>>(*node);

        cout<<endl;
        cout<<"---Hamming Distance Heuristic---"<<endl;
        AStarSearch<HammingCompare>(node);

        cout<<"---Manhattan Distance Heuristic---"<<endl;
        AStarSearch<ManHattanCompare>(node);
        delete node;
    }

}

