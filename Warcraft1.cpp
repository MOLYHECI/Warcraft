#include <iostream>
#include <cstring>
#include <string>
using namespace std;
int M,min_HP;
struct Dictionary{
    string name;
    int HP;
};
Dictionary initial_order[5]={{"dragon",},{"ninja",},{"iceman",},{"lion",},{"wolf",}};
Dictionary * redorder[5]={& initial_order[2],& initial_order[3],& initial_order[4],& initial_order[1],& initial_order[0]};
Dictionary * blueorder[5]={& initial_order[3],& initial_order[0],& initial_order[1],& initial_order[2],& initial_order[4]};// order of Warriors
class Head{
    private:
    int HP;
    string color;
    Dictionary * order[5];
    int num_warrior[5]={0};
    int will_build = 0;
    int total_num = 0;
    public:
    bool is_build = true;
    Head(int n,string s,Dictionary * color_order[5]):HP(n),color(s){
        for(int i=0;i<5;++i){
            order[i] = color_order[i];
        }
    }
    void build(int t);
    void print(int t);
};
class Warrior{
    private:
    int HP;
    int id;
    string color;
    string name;
    public:
    Warrior(int HP,string name,int totalnum){};
};
void Head::build(int t){
    if(is_build == false){return ;}
    if(HP<min_HP){is_build = false;print(t);}
    else{
        while(HP<order[will_build]->HP){will_build = (will_build + 1)%5;}
        Warrior(order[will_build]->HP,order[will_build]->name,total_num++);
        HP -= order[will_build]->HP;
        num_warrior[will_build]++;
        print(t);
        will_build = (will_build + 1)% 5;
    }
}
void Head::print(int t){
    if(is_build)
    printf("%03d %s %s %d born with strength %d,%d %s in %s headquarter\n",
    t,color.c_str(),order[will_build]->name.c_str(),total_num,order[will_build]->HP,num_warrior[will_build],order[will_build]->name.c_str(),color.c_str());
    else
    printf("%03d %s headquarter stops making warriors\n",t,color.c_str());
}
int get_min_HP(){
    int m = 1e7;
    for(int i=0;i<5;++i){
        m=min(m,initial_order[i].HP);
    }
    return m;
}
int main(){
    int N;
    cin >> N;
    for(int i=1;i<=N;++i){
        cout << "Case:" << i << endl;
        cin >> M >> initial_order[0].HP >> initial_order[1].HP >> initial_order[2].HP >> initial_order[3].HP >> initial_order[4].HP;
        min_HP = get_min_HP();
        Head redhead(M,"red",redorder);
        Head bluehead(M,"blue",blueorder);
        int t = 0;
        while(redhead.is_build||bluehead.is_build){
            redhead.build(t);
            bluehead.build(t);
            ++t;
        }
    }
    return 0;
}
