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
string Weapon[3]={"sword","bomb","arrow"};
class Warrior{
    protected:
    int HP;
    int id;
    string color;
};
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
class Dragon:protected Warrior{
    protected:
    string name = "dragon";
    string weapon;
    double morale;
    public:
    Dragon(int resthp,int hp,int total_num){
        HP=hp,id = total_num,morale = (double)resthp/hp,weapon=Weapon[id%3];
        printf("It has a %s,and it's morale is %.2f\n",weapon.c_str(),morale);
    }
};
class Ninja:protected Warrior{
    protected:
    string name = "ninja";
    string weapon[2];
    public:
    Ninja(int hp,int total_num){
        HP=hp,id=total_num,weapon[0]=Weapon[id%3],weapon[1]=Weapon[(id+1)%3];
        printf("It has a %s and a %s\n",weapon[0].c_str(),weapon[1].c_str());
    }
};
class Iceman:protected Warrior{
    protected:
    string name = "iceman";
    string weapon;
    public:
    Iceman(int hp,int total_num){
        HP=hp,id=total_num,weapon=Weapon[id%3];
        printf("It has a %s\n",weapon.c_str());
    }
};
class Lion:protected Warrior{
    protected:
    string name = "lion";
    int loyalty;
    public:
    Lion(int resthp,int hp,int total_num){
        loyalty = resthp,HP=hp,id= total_num;
        printf("It's loyalty is %d\n",loyalty);
    }
};
class Wolf:protected Warrior{
    protected:
    string name = "wolf";
    public:
    Wolf(int hp,int total_num){
        HP=hp,id=total_num;
    }
};
void Head::build(int t){
    if(is_build == false){return ;}
    if(HP<min_HP){is_build = false;print(t);}
    else{
        while(HP<order[will_build]->HP){will_build = (will_build + 1)%5;}
        ++total_num;
        HP -= order[will_build]->HP;
        num_warrior[will_build]++;
        print(t);
        switch(order[will_build]->name[0]){
            case 'd':
            Dragon(HP,order[will_build]->HP,total_num);
            break;
            case 'n':
            Ninja(order[will_build]->HP,total_num);
            break;
            case 'i':
            Iceman(order[will_build]->HP,total_num);
            break;
            case 'l':
            Lion(HP,order[will_build]->HP,total_num);
            break;
            case 'w':
            Wolf(order[will_build]->HP,total_num);
            break;
        }
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
