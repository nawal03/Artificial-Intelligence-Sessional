#include<bits/stdc++.h>
using namespace std;

int courseCnt,studentCnt;
vector<int> enrolledStu;
vector<int> day;
vector<vector<int>> adj;
vector<vector<int>> enrolledCrs;
vector< set<int> > sat;
vector<int>unassDeg;

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

int random(int l, int r) {
    return uniform_int_distribution<int>(l, r) (rng);
}

vector<int> split(string s){
    int cur=0;
    vector<int>courseList;
    for(char ch: s){
        if(ch==' '){
            courseList.push_back(cur);
            cur=0;
            continue;
        }
        cur=cur*10+(ch-'0');
    }
    if(cur) courseList.push_back(cur);
    return courseList;
}

int selectDay(int u){
    set<int> st;
    for(int v: adj[u]){
        if(day[v]) st.insert(day[v]);
    }

    for(int i=1;;i++){
        if(st.find(i)==st.end()) return i;
    }
}

void scheduleByLargestDegree(){
    vector<int> order;
    for(int i=1;i<=courseCnt;i++){
        order.push_back(i);
    }

    sort(order.begin(),order.end(),
        [](const int & u, const int & v)
        {
            return adj[u].size() > adj[v].size();
        });
    for(int i: order){
        day[i]=selectDay(i);
    }
}

void scheduleBySaturationDegree(){
    vector<int> order;
    for(int i=1;i<=courseCnt;i++){
        order.push_back(i);
    }

    sat.resize(courseCnt+1);
    unassDeg.resize(courseCnt+1);
    for(int i=1;i<=courseCnt;i++) unassDeg[i]=adj[i].size();

    for(int i=0;i<courseCnt;i++){
        sort(order.begin()+i,order.end(),
        [](const int & u, const int & v)
        {
            if(sat[u].size()!=sat[v].size())
                return sat[u].size() > sat[v].size();
            return unassDeg[u] > unassDeg[v];
        });

        int j=order[i];
        day[j]=selectDay(j);
        for(int k: adj[j]){
            sat[k].insert(day[j]);
            unassDeg[k]--;
        }
    }
}

void scheduleByLargestEnrollment(){
    vector<int> order;
    for(int i=1;i<=courseCnt;i++){
        order.push_back(i);
    }
    sort(order.begin(),order.end(),
        [](const int & u, const int & v)
        {
            return enrolledStu[u] > enrolledStu[v];
        });

    for(int i: order){
        day[i]=selectDay(i);
    }
}

void scheduleByRandomOrdering(){
    vector<int> order;
    for(int i=1;i<=courseCnt;i++){
        order.push_back(i);
    }

    unsigned seed = random(0,1000);
    shuffle(order.begin(), order.end(), default_random_engine(seed));

    for(int i: order){
        day[i]=selectDay(i);
    }
}

double linearPenalty(){
    double psum=0;
    for(int i=1;i<=studentCnt;i++){
        for(int j=0;j<enrolledCrs[i].size();j++){
            for(int k=j+1;k<enrolledCrs[i].size();k++){
                int gap=abs(day[enrolledCrs[i][j]]-day[enrolledCrs[i][k]]);
                if(gap==0) return 1e12;
                if(gap<=5) psum+=2*(5-gap);
            }
        }
    }
    return psum/studentCnt;
}

double expPenalty(){
    double psum=0;
    for(int i=1;i<=studentCnt;i++){
        for(int j=0;j<enrolledCrs[i].size();j++){
            for(int k=j+1;k<enrolledCrs[i].size();k++){
                int gap=abs(day[enrolledCrs[i][j]]-day[enrolledCrs[i][k]]);
                if(gap==0) return 1e12;
                if(gap<=5) psum+=(1<<(5-gap));
            }
        }
    }
    return psum/studentCnt;
}

void dfs(int u,int x,int y,vector<int>&chain,vector<int>&vis){
    chain.push_back(u);
    vis[u]=1;
    vector<int> child;
    for(int v: adj[u]){
        if(vis[v]) continue;
        if(day[v]==y){
            dfs(v,y,x,chain,vis);
        }
    }
}

void KempeInterchange(int u, double (*penalty)()){
    if(adj[u].size()==0) return;

    double pen = penalty();
    vector<int> chain;
    vector<int> vis(courseCnt+1,0);
    int x=day[u];
    int y=day[adj[u][random(0,adj[u].size()-1)]];
    dfs(u,x,y,chain,vis);

    for(int i:chain){
        if(day[i]==x) day[i]=y;
        else day[i]=x;
    }

    if(penalty()<pen) return;

    for(int i:chain){
        if(day[i]==x) day[i]=y;
        else day[i]=x;
    }

    return;

}

void pairSwap(int u,int v, double (*penalty)()){
    double pen = penalty();
    swap(day[u],day[v]);
    if(penalty()<pen) return;
    swap(day[u],day[v]);
    return;
}

int main(){
     string s = "car-f-92";
//    string s = "car-s-91";
//    string s = "kfu-s-93";
//    string s = "tre-s-92";
//    string s = "yor-f-83";

    cout<<s<<endl;

    ifstream crs,stu;
    ofstream sol;
    string temp;

    //course count
    crs.open(s+".crs");
    while(getline(crs,temp)){
        courseCnt++;
    }
    crs.close();

    enrolledStu.resize(courseCnt+1);
    day.resize(courseCnt+1,0);
    adj.resize(courseCnt+1);
    enrolledCrs.resize(1);

    //input enrolled student count
    crs.open(s+".crs");
    for(int i=1;i<=courseCnt;i++){
        int id;
        crs>>id;
        crs>>enrolledStu[id];
    }
    crs.close();

    //input courses of students
    stu.open(s+".stu");
    while(getline(stu,temp)){
        studentCnt++;
        vector<int> courseList = split(temp);
        enrolledCrs.push_back(courseList);
        for(int i: courseList){
            for(int j: courseList){
                if(i==j) continue;
                adj[i].push_back(j);
            }
        }
    }
    stu.close();

    //make adj[i] unique
    for(int i=1;i<=courseCnt;i++){
        sort(adj[i].begin(),adj[i].end());
        adj[i].erase(unique(adj[i].begin(),adj[i].end()),adj[i].end());
    }




    //schedule the course
//    scheduleByLargestDegree();
//    scheduleBySaturationDegree();
//    scheduleByLargestEnrollment();
    scheduleByRandomOrdering();

    int timeslots=0;
    for(int i: day) timeslots=max(timeslots,i);
    cout<<"Time Slots: "<<timeslots<<endl;

    double (*penalty)();
    penalty = &linearPenalty;

    cout<<"After Construction: "<<penalty()<<endl;
    for(int i=0;i<10000;i++){
        KempeInterchange(random(1,courseCnt),penalty);
    }
    cout<<"After Kempe Interchange: "<<penalty()<<endl;

    for(int i=0;i<10000;i++){
        pairSwap(random(1,courseCnt),random(1,courseCnt),penalty);
    }
    cout<<"After Pair Swap: "<<penalty()<<endl;

    sol.open(s+".sol");
    for(int i=1;i<=courseCnt;i++) sol<<i<<' '<<day[i]<<endl;
}
