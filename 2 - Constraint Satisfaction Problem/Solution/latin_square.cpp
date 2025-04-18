#include<bits/stdc++.h>
using namespace std;

long long totNode=0, backtrack=0;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
int getrand(int a, int b)
{
    int x = uniform_int_distribution<int>(a, b)(rng);
    return x;
}
struct Variable{
    int x,y;
    vector<bool> domain;
    int domCount;
    int degCount;
    int assignedVal;
    int randWeight;
    Variable(int x,int y,vector<bool>domain, int domCount, int degCount,int assignedVal=0){
        this->x=x;
        this->y=y;
        this->domain=domain;
        this->domCount=domCount;
        this->degCount=degCount;
        this->assignedVal=assignedVal;
        this->randWeight=getrand(1,1000000);
    }
};

struct VAH1{
    string name(){
        return "VAH1";
    }
    bool operator()(Variable* const& v1, Variable* const& v2){
        int n=v1->domain.size()-1;
        if(v1->domCount == v2->domCount) return (v1->x-1)*n+v1->y < (v2->x-1)*n+v2->y;
        return v1->domCount < v2->domCount;
    }
};
struct VAH2{
    string name(){
        return "VAH2";
    }
    bool operator()(Variable* const& v1, Variable* const& v2){
        int n=v1->domain.size()-1;
        if(v1->degCount == v2->degCount) return (v1->x-1)*n+v1->y < (v2->x-1)*n+v2->y;
        return v1->degCount > v2->degCount;
    }
};
struct VAH3{
    string name(){
        return "VAH3";
    }
    bool operator()(Variable* const& v1, Variable* const& v2){
        int n=v1->domain.size()-1;
        if(v1->domCount!=v2->domCount) return v1->domCount < v2->domCount;
        if(v1->degCount == v2->degCount) return (v1->x-1)*n+v1->y < (v2->x-1)*n+v2->y;
        return v1->degCount > v2->degCount;
    }
};
struct VAH4{
    string name(){
        return "VAH4";
    }
    bool operator()(Variable* const& v1, Variable* const& v2){
        int n=v1->domain.size()-1;
        if(v1->domCount*v2->degCount == v2->domCount*v1->degCount) return (v1->x-1)*n+v1->y < (v2->x-1)*n+v2->y;
        return v1->domCount*v2->degCount < v2->domCount*v1->degCount;
    }
};

struct VAH5{
    string name(){
        return "VAH5";
    }
    bool operator()(Variable* const& v1, Variable* const& v2){
        int n=v1->domain.size()-1;
        if(v1->randWeight==v2->randWeight) return (v1->x-1)*n+v1->y < (v2->x-1)*n+v2->y;
        return v1->randWeight<v2->randWeight;
    }
};


void dO(Variable* nextVar, int d, vector< vector<Variable*> >& var, vector< pair< pair<int,int>, int> >&changes){
    int n = var.size()-1;

    nextVar->assignedVal=d;
    int i = nextVar->x, j = nextVar->y;
    for(int k=1;k<=n;k++){
        if(var[i][k]->assignedVal) continue;
        if(var[i][k]->domain[d]){
            var[i][k]->domain[d]=0;
            var[i][k]->domCount--;
            changes.push_back({{i,k},d});
        }
        var[i][k]->degCount--;
    }

    for(int k=1;k<=n;k++){
        if(var[k][j]->assignedVal) continue;
        if(var[k][j]->domain[d]){
            var[k][j]->domain[d]=0;
            var[k][j]->domCount--;
            changes.push_back({{k,j},d});
        }
        var[k][j]->degCount--;
    }
}

void undo(Variable* nextVar, int d, vector< vector<Variable*> >& var, vector< pair< pair<int,int>, int> >&changes){
    int n = var.size()-1;
    for(auto it: changes){
        int i = it.first.first;
        int j = it.first.second;
        int d = it.second;
        var[i][j]->domain[d]=1;
        var[i][j]->domCount++;
    }

    int i = nextVar->x, j = nextVar->y;
    for(int k=1;k<=n;k++){
        if(var[i][k]->assignedVal) continue;
        var[i][k]->degCount++;
    }

    for(int k=1;k<=n;k++){
        if(var[k][j]->assignedVal) continue;
        var[k][j]->degCount++;
    }
}

template<typename T>
Variable *getVariable(vector< vector<Variable*> >& var){
    set<Variable*, T> unassigned;
    int n = var.size()-1;
    int sz=0;
    for(int i=1;i<=n;i++){
        for(int j=1;j<=n;j++){
            if(var[i][j]->assignedVal) continue;
            unassigned.insert(var[i][j]);
        }
    }
    if(unassigned.size()==0) return nullptr;
    return *unassigned.begin();
}

int getValueOrder(vector< vector<Variable*> >& var, Variable* &nextVar, int d){
    int ans=0;
    int i = nextVar->x, j = nextVar->y;
    int n=var.size()-1;
    for(int k=1;k<=n;k++){
        if(var[i][k]->assignedVal) continue;
        if(var[i][k]->domain[d]){
            ans++;
        }
    }

    for(int k=1;k<=n;k++){
        if(var[k][j]->assignedVal) continue;
        if(var[k][j]->domain[d]){
            ans++;
        }
    }
    return ans;
}

bool check(vector< vector<Variable*> >& var){
    int n=var.size()-1;
    for(int i=1;i<=n;i++){
        int cnt[n+1]={0};
        for(int j=1;j<=n;j++){
            if(var[i][j]->assignedVal==0) return 0;
            cnt[var[i][j]->assignedVal]++;
            if(cnt[var[i][j]->assignedVal]>1) return 0;
        }
    }
    for(int i=1;i<=n;i++){
        int cnt[n+1]={0};
        for(int j=1;j<=n;j++){
            if(var[j][i]->assignedVal==0) return 0;
            cnt[var[j][i]->assignedVal]++;
            if(cnt[var[j][i]->assignedVal]>1) return 0;
        }
    }
    return 1;
}

template<typename T>
int makeAssignment(vector< vector<Variable*> >& var,int fc){

    totNode++;

    Variable* nextVar=getVariable<T>(var);
    if(nextVar==nullptr) return 1;
    if(nextVar->domCount==0){
        backtrack++;
        return 0;
    }
    int n = var.size()-1;
    vector< pair<int,int> > domain;
    for(int i=1;i<=n;i++){
        if(nextVar->domain[i]) domain.push_back({getValueOrder(var,nextVar,i),i});
    }
    sort(domain.begin(),domain.end());

    for(auto it: domain){
        int d=it.second;
        vector< pair< pair<int,int>, int> >changes;
        dO(nextVar,d,var,changes);
        if(fc==1){
            int i=nextVar->x;
            int j=nextVar->y;

            int ok=1;
            for(int k=1;k<=n;k++){
                if(!var[i][k]->assignedVal && !var[i][k]->domCount) ok=0;
                if(!var[k][j]->assignedVal && !var[k][j]->domCount) ok=0;
            }

            if(ok==0){
                undo(nextVar,d,var,changes);
                continue;
            }
        }
        int valid = makeAssignment<T>(var,fc);
        if(valid) return 1;
        undo(nextVar,d,var,changes);
    }
    nextVar->assignedVal=0;
    backtrack++;
    return 0;
}

template<typename T>
void solveCSP(vector< vector<int> >& grid,int fc=0){
    int n = grid.size()-1;
    vector< vector<Variable*> > var(n+1, vector<Variable*>(n+1));

    for(int i=1;i<=n;i++){
        for(int j=1;j<=n;j++){
            if(grid[i][j]!=0){
                vector<bool> domain(n+1,0);
                domain[grid[i][j]]=1;
                int assignedVal=grid[i][j];
                int domCount=1;
                int degCount=-1;
                Variable* v = new Variable(i,j,domain,domCount,degCount,assignedVal);
                var[i][j]=v;
                continue;
            }

            vector<bool> domain(n+1,1);
            domain[0]=0;
            int domCount=n;
            int degCount=2*n-2;
            for(int k=1;k<=n;k++){
                if(grid[i][k]){
                    degCount--;
                    if(domain[grid[i][k]]){
                        domain[grid[i][k]]=0;
                        domCount--;
                    }
                }
                if(grid[k][j]){
                    degCount--;
                    if(domain[grid[k][j]]){
                        domain[grid[k][j]]=0;
                        domCount--;
                    }
                }
            }
            Variable* v= new Variable(i,j,domain, domCount, degCount);
            var[i][j]=v;
        }
    }
    totNode=0,backtrack=0;
    clock_t start_time = clock();
    int valid = makeAssignment<T>(var,fc);
    clock_t end_time = clock();
    T t;
    if(valid){
        cout<<"-------------\n";
        cout<<"Method: ";
        if(fc) cout<<"Forward checking"<<endl;
        else cout<<"Backtracking"<<endl;
        cout<<"Heuristic: "<<t.name()<<endl;
        cout<<"Number of nodes: "<<totNode<<'\n';
        cout<<"Number of backtracks: "<<backtrack<<'\n';
        cout<<"Time: "<<float(end_time - start_time) / CLOCKS_PER_SEC<<" s"<<endl;

        assert(check(var));

        for(int i=1;i<=n;i++){
            for(int j=1;j<=n;j++){
                cout<<var[i][j]->assignedVal<<' ';
                delete var[i][j];
            }
            cout<<endl;
        }
        cout<<"-------------\n";
    }
    else{
        cout<<"grid is invalid\n";
    }
}

int main(){
//    freopen("d-10-01.txt","r",stdin);
//    freopen("output.txt","w",stdout);
    int n;
    cin>>n;
    vector< vector<int> > grid(n+1, vector<int>(n+1,0));
    for(int i=1;i<=n;i++){
        for(int j=1;j<=n;j++){
            cin>>grid[i][j];
        }
    }
    solveCSP<VAH1>(grid,0);
    solveCSP<VAH2>(grid,0);
    solveCSP<VAH3>(grid,0);
    solveCSP<VAH4>(grid,0);
    solveCSP<VAH5>(grid,0);

    solveCSP<VAH1>(grid,1);
    solveCSP<VAH2>(grid,1);
    solveCSP<VAH3>(grid,1);
    solveCSP<VAH4>(grid,1);
    solveCSP<VAH5>(grid,1);

}
