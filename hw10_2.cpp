#include <iostream>
#include <cstring>
#include <string>
#include <deque>
#include <list>
#include <numeric>
#include <algorithm>
using namespace std;
int M, N, K, T,R;
bool is_end = false;
bool is_mute = false;
bool is_passivation = true;
struct Dictionary {
    string name;
    int HP;
    int power;
};
Dictionary initial_order[5] = { {"dragon",},{"ninja",},{"iceman",},{"lion",},{"wolf",} };
Dictionary* redorder[5] = { &initial_order[2],&initial_order[3],&initial_order[4],&initial_order[1],&initial_order[0] };
Dictionary* blueorder[5] = { &initial_order[3],&initial_order[0],&initial_order[1],&initial_order[2],&initial_order[4] };// order of Warriors'
string weaponname[3] = { "sword","bomb","arrow" };
class Weapon;
class Warrior;
class City;
deque<Weapon*> all_weapon;
deque<Warrior*> all_warrior;
deque<City*> all_city;
deque<City*> line;
class Weapon {
public:
    int id;
    int power;
    int times = 1e6;
    virtual void passivation(){}
};
bool cmp(const Weapon* w1, const Weapon* w2) {
    return w1->id < w2->id;
}
class Sword :public Weapon {
public:
    Sword(int user_power) {
        power = user_power * 0.2;
        id = 0;
    }
    virtual void passivation(){
        if(is_passivation)
        power = power * 0.8;
    }
};
class Bomb :public Weapon {
public:
    Bomb() {
        id = 1;
        times = 1;
    }
};
class Arrow :public Weapon {
public:
    Arrow(int R) {
        times = 3;
        power = R;
        id = 2;
    }
};
class City {
public:
    int location;
    int HP=0;
    int flag=0;
    int red_win=0;
    int blue_win=0;
    deque<Warrior*> redwarrior;
    deque<Warrior*> bluewarrior;
    City() {}
    City(int n) :location(n) {}
    City(City & c);
    void changeflag(int t){
        if(flag!=1&&red_win==2){
            flag=1;
            printf("%03d:%02d red flag raised in city %d\n",t/60,t%60,location);
        }
        if(flag!=-1&&blue_win==2){
            flag=-1;
            printf("%03d:%02d blue flag raised in city %d\n",t/60,t%60,location);
        }
    }
    int aftermath(int t);
    int fight(int t);
    int virtual_fight(int t);
    bool is_impotent(int red_use, int blue_use);
};
class Warrior {
public:
    int HP;
    int id;
    int power;
    int loyalty = 1e6;
    int get_HP = 0;
    bool is_alive = true;
    bool is_moved = false;
    bool is_arrow_killed = false;
    bool is_reached = false;
    bool will_reward = false;
    string color;
    City* locatecity = NULL;
    string name;
    deque<Weapon*> weapons;
    virtual void move(int t) {
        if (HP > 0) {
            if (this->color == "red") {
                this->locatecity->redwarrior.pop_front();
                this->locatecity = line[this->locatecity->location + 1];
                this->locatecity->redwarrior.push_back(this);
            }
            else {
                this->locatecity->bluewarrior.pop_front();
                this->locatecity = line[this->locatecity->location - 1];
                this->locatecity->bluewarrior.push_back(this);
            }
            is_moved = 1;
        }
    }
    int getsword(){
        for(int i=0;i<weapons.size();++i){
            if(weapons[i]->id==0)
            return i;
        }
        return -1;
    }
    int getbomb(){
        for(int i=0;i<weapons.size();++i){
            if(weapons[i]->id==1)
            return i;
        }
        return -1;
    }
    int getarrow(){
        for(int i=0;i<weapons.size();++i){
            if(weapons[i]->id==2)
            return i;
        }
        return -1;
    }
    void throw_away(int use_w = 0) {
        deque<Weapon*>::iterator it = weapons.begin() + use_w;
        weapons.erase(it);
    }
    int attack(Warrior* w, int use_w,int t) {
        if(is_mute==false)
        printf("%03d:%02d %s %s %d attacked %s %s %d in city %d with %d elements and force %d\n",t/60,t%60,color.c_str(),name.c_str(),id,w->color.c_str(),w->name.c_str(),w->id,locatecity->location,HP,power);
        if(use_w==-1){
            w->attacked(this->power,-1,t);
        }
        else{
            w->attacked(this->power + weapons[use_w]->power,weapons[use_w]->id,t);
            dynamic_cast<Sword*>(weapons[use_w])->passivation();
            if(weapons[use_w]->power==0){
                throw_away(use_w);
            }
        }
        return !w->is_alive;
    }
    void boom(Warrior * w,int use_w,int t){
        printf("%03d:%02d %s %s %d used a bomb and killed %s %s %d\n",t/60,t%60,color.c_str(),name.c_str(),id,w->color.c_str(),w->name.c_str(),w->id);
        this->HP=0;
        w->HP = 0;
    }
    void shot(Warrior * w,int use_w,int t){
        w->HP -= R;
        weapons[use_w]->times--;
        if(weapons[use_w]->times==0) throw_away(use_w);
        if(w->HP<=0) w->is_alive = false;
        if(w->is_alive) printf("%03d:%02d %s %s %d shot\n",t/60,t%60,color.c_str(),name.c_str(),id);
        else printf("%03d:%02d %s %s %d shot and killed %s %s %d\n",t/60,t%60,color.c_str(),name.c_str(),id,w->color.c_str(),w->name.c_str(),w->id);
    }
    int anti_attack(Warrior * w,int use_w,int t){
        if(this->name=="ninja")
        return !w->is_alive;
        if(is_mute==false)
        printf("%03d:%02d %s %s %d fought back against %s %s %d in city %d\n",t/60,t%60,color.c_str(),name.c_str(),id,w->color.c_str(),w->name.c_str(),w->id,locatecity->location);
        if(use_w == -1){
            w->attacked(0.5*power,-1,t);
        }
        else{
            w->attacked(0.5*power+weapons[use_w]->power,weapons[use_w]->id,t);
            dynamic_cast<Sword*>(weapons[use_w])->passivation();
            if(weapons[use_w]->power==0){
                throw_away(use_w);
            }
        }
        return !w->is_alive;
    }
    virtual int attacked(int deltaHP,int weapon_id, int t) {
        HP -= deltaHP;
        if (HP <= 0) {
            if(!is_mute)
            printf("%03d:%02d %s %s %d was killed in city %d\n",t/60,t%60,color.c_str(),name.c_str(),id,locatecity->location);
            is_alive = false;
        }
        return is_alive;
    }
};
class Head :public City {
public:
    string color;
    Dictionary* order[5];
    int num_warrior[5] = { 0 };
    int will_build = 0;
    int total_num = 0;
    bool is_build = true;
    Head(int n, string s, Dictionary* color_order[5]):color(s) {
        HP = n;
        for (int i = 0; i < 5; ++i) {
            order[i] = color_order[i];
        }
        location = color == "red" ? 0 : N + 1;
    }
    void build(int t);
    void print(int t,Warrior * w);
};
class Dragon :public Warrior {
public:
    double morale;
    Dragon(int resthp, int hp, int Power, string colour, int total_num) {
        HP = hp, power = Power, id = total_num, morale = (double)resthp / hp, color = colour;
        name = "dragon";
        switch (id % 3) {
            case 0: {
                Sword* s = new Sword(power);
                weapons.push_back(s);
                all_weapon.push_back(s);
                break;
            }
            case 1: {
                Bomb* b = new Bomb();
                weapons.push_back(b);
                all_weapon.push_back(b);
                break;
            }
            case 2: {
                Arrow* a = new Arrow(R);
                weapons.push_back(a);
                all_weapon.push_back(a);
                break;
            }
        }
        for(int i=0;i<weapons.size();++i){
            if(weapons[i]->id==0&&weapons[i]->power==0){
                throw_away(i);
            }
        }
    }
};
class Ninja :public Warrior {
public:
    Ninja(int hp, int Power, string colour, int total_num) {
        HP = hp, power = Power, id = total_num, color = colour;
        name = "ninja";
        switch (id % 3) {
        case 0: {
            Sword* s = new Sword(power);
            weapons.push_back(s);
            all_weapon.push_back(s);
            break;
        }
        case 1: {
            Bomb* b = new Bomb();
            weapons.push_back(b);
            all_weapon.push_back(b);
            break;
        }
        case 2: {
            Arrow* a = new Arrow(R);
            weapons.push_back(a);
            all_weapon.push_back(a);
            break;
        }
        }
        switch ((id + 1) % 3) {
        case 0: {
            Sword* s = new Sword(power);
            weapons.push_back(s);
            all_weapon.push_back(s);
            break;
        }
        case 1: {
            Bomb* b = new Bomb();
            weapons.push_back(b);
            all_weapon.push_back(b);
            break;
        }
        case 2: {
            Arrow* a = new Arrow(R);
            weapons.push_back(a);
            all_weapon.push_back(a);
            break;
        }
        }
        for(int i=0;i<weapons.size();++i){
            if(weapons[i]->id==0&&weapons[i]->power==0){
                throw_away(i);
            }
        }
        // sort(weapons.begin(), weapons.end(), cmp);
    }
};
class Iceman :public Warrior {
public:
    int step_count = 0;
    Iceman(int hp, int Power, string colour, int total_num) {
        HP = hp, power = Power, id = total_num, color = colour;
        name = "iceman";
        switch (id % 3) {
        case 0: {
            Sword* s = new Sword(power);
            weapons.push_back(s);
            all_weapon.push_back(s);
            break;
        }
        case 1: {
            Bomb* b = new Bomb();
            weapons.push_back(b);
            all_weapon.push_back(b);
            break;
        }
        case 2: {
            Arrow* a = new Arrow(R);
            weapons.push_back(a);
            all_weapon.push_back(a);
            break;
        }
        }
        for(int i=0;i<weapons.size();++i){
            if(weapons[i]->id==0&&weapons[i]->power==0){
                throw_away(i);
            }
        }
    }
    virtual void move(int t) {
        if (this->color == "red") {
            this->locatecity->redwarrior.pop_front();
            this->locatecity = line[this->locatecity->location + 1];
            this->locatecity->redwarrior.push_back(this);
        }
        else {
            this->locatecity->bluewarrior.pop_front();
            this->locatecity = line[this->locatecity->location - 1];
            this->locatecity->bluewarrior.push_back(this);
        }
        is_moved = 1;
        step_count++;
        if(step_count == 2){
            step_count = 0;
            this->HP = max(1, HP-9);
            this->power += 20;
        }
    }
};
class Lion :public Warrior {
public:
    Lion(int resthp, int hp, int Power, string colour, int total_num) {
        loyalty = resthp, HP = hp, power = Power, id = total_num, color = colour;
        name = "lion";
    }
    virtual void move(int t) {
        if (HP > 0) {
            if (this->color == "red") {
                this->locatecity->redwarrior.pop_front();
                this->locatecity = line[this->locatecity->location + 1];
                this->locatecity->redwarrior.push_back(this);
            }
            else {
                this->locatecity->bluewarrior.pop_front();
                this->locatecity = line[this->locatecity->location - 1];
                this->locatecity->bluewarrior.push_back(this);
            }
            is_moved = 1;
        }
    }
};
class Wolf :public Warrior {
public:
    Wolf(int hp, int Power, string colour, int total_num) {
        HP = hp, power = Power, id = total_num, color = colour;
        name = "wolf";
    }
    void rob(Warrior* w) {
        if (w->weapons.empty())
            return;
        int have[3] = {1,1,1};
        for(int i = 0;i<weapons.size();++i){
            have[weapons[i]->id]--;
        }
        for(int i = 0;i<w->weapons.size();++i){
            if(have[w->weapons[i]->id]){
                weapons.push_back(w->weapons[i]);
            }
        }
    }
};
void Head::build(int t) {
    if (HP < order[will_build]->HP) {
        return;
    }
    ++total_num;
    HP -= order[will_build]->HP;
    num_warrior[will_build]++;
    switch (order[will_build]->name[0]) {
        case 'd': {
            Dragon* d = new Dragon(HP, order[will_build]->HP, order[will_build]->power, this->color, total_num);
            d->locatecity = this;
            if (color == "red")
                redwarrior.push_back(d);
            else bluewarrior.push_back(d);
            all_warrior.push_back(d);
            print(t,d);
            break;
        }
        case 'n': {
            Ninja* n = new Ninja(order[will_build]->HP, order[will_build]->power, this->color, total_num);
            n->locatecity = this;
            if (color == "red")
                redwarrior.push_back(n);
            else bluewarrior.push_back(n);
            all_warrior.push_back(n);
            print(t,n);
            break;
        }
        case 'i': {
            Iceman* i = new Iceman(order[will_build]->HP, order[will_build]->power, this->color, total_num);
            i->locatecity = this;
            if (color == "red")
                redwarrior.push_back(i);
            else bluewarrior.push_back(i);
            all_warrior.push_back(i);
            print(t,i);
            break;
        }
        case 'l': {
            Lion* l = new Lion(HP, order[will_build]->HP, order[will_build]->power, this->color, total_num);
            l->locatecity = this;
            if (color == "red")
                redwarrior.push_back(l);
            else bluewarrior.push_back(l);
            all_warrior.push_back(l);
            print(t,l);
            printf("Its loyalty is %d\n", l->loyalty);
            break;
        }
        case 'w': {
            Wolf* w = new Wolf(order[will_build]->HP, order[will_build]->power, this->color, total_num);
            w->locatecity = this;
            if (color == "red")
                redwarrior.push_back(w);
            else bluewarrior.push_back(w);
            all_warrior.push_back(w);
            print(t,w);
            break;
        }
    }
    will_build = (will_build + 1) % 5;
}
void Head::print(int t, Warrior * w) {
    printf("%03d:%02d %s %s %d born\n",t / 60, t % 60, color.c_str(), order[will_build]->name.c_str(), total_num);
    if(order[will_build]->name == "dragon")
    printf("Its morale is %.2lf\n", dynamic_cast<Dragon*>(w)->morale);
}
City::City(City & c){
        location = c.location;
        HP = c.HP;
        flag = c.flag;
        red_win = c.red_win;
        blue_win = c.blue_win;
        if(!c.redwarrior.empty()){
            switch(c.redwarrior[0]->name[0]){
                case 'd':{
                Dragon * d = new Dragon(*dynamic_cast<Dragon*>(c.redwarrior[0]));
                redwarrior.push_back(d);
                all_warrior.push_back(d);
                break;
                }
                case 'n':{
                Ninja * n = new Ninja(*dynamic_cast<Ninja*>(c.redwarrior[0]));
                redwarrior.push_back(n);
                all_warrior.push_back(n);
                break;
                }
                case 'i':{
                Iceman * i = new Iceman(*dynamic_cast<Iceman*>(c.redwarrior[0]));
                redwarrior.push_back(i);
                all_warrior.push_back(i);
                break;
                }
                case 'l':{
                Lion * l = new Lion(*dynamic_cast<Lion*>(c.redwarrior[0]));
                redwarrior.push_back(l);
                all_warrior.push_back(l);
                break;
                }
                case 'w':{
                Wolf * w = new Wolf(*dynamic_cast<Wolf*>(c.redwarrior[0]));
                redwarrior.push_back(w);
                all_warrior.push_back(w);
                break;
                }
            }
        }
        if(!c.bluewarrior.empty()){
            switch(c.bluewarrior[0]->name[0]){
                case 'd':{
                Dragon * d = new Dragon(*dynamic_cast<Dragon*>(c.bluewarrior[0]));
                bluewarrior.push_back(d);
                all_warrior.push_back(d);
                break;
                }
                case 'n':{
                Ninja * n = new Ninja(*dynamic_cast<Ninja*>(c.bluewarrior[0]));
                bluewarrior.push_back(n);
                all_warrior.push_back(n);
                break;
                }
                case 'i':{
                Iceman * i = new Iceman(*dynamic_cast<Iceman*>(c.bluewarrior[0]));
                bluewarrior.push_back(i);
                all_warrior.push_back(i);
                break;
                }
                case 'l':{
                Lion * l = new Lion(*dynamic_cast<Lion*>(c.bluewarrior[0]));
                bluewarrior.push_back(l);
                all_warrior.push_back(l);
                break;
                }
                case 'w':{
                Wolf * w = new Wolf(*dynamic_cast<Wolf*>(c.bluewarrior[0]));
                bluewarrior.push_back(w);
                all_warrior.push_back(w);
                break;
                }
            }
        }
    }
int City::aftermath(int t){
    if (redwarrior[0]->HP > 0 && bluewarrior[0]->HP <= 0) {
        bluewarrior.pop_back();
        red_win+=1;
        blue_win=0;
        return 1;
    }
    else if (bluewarrior[0]->HP > 0 && redwarrior[0]->HP <= 0) {
        redwarrior.pop_back();
        blue_win+=1;
        red_win=0;
        return -1;
    }
    else if (bluewarrior[0]->HP > 0 && redwarrior[0]->HP > 0) {
        red_win=0;
        blue_win=0;
        return 0;
    }
    else {
        redwarrior.pop_back();
        bluewarrior.pop_back();
        return 1e6;
    }
}
int City::fight(int t) {
    if(redwarrior[0]->HP<=0||bluewarrior[0]->HP<=0)
    return aftermath(t);
    int check;
    if ((location % 2 == 1&&flag==0)||flag==1) {
        if(redwarrior[0]->attack(bluewarrior[0],redwarrior[0]->getsword(),t)) return aftermath(t);//red initiative

        if(bluewarrior[0]->anti_attack(redwarrior[0],bluewarrior[0]->getsword(),t)) return aftermath(t);//blue anti

    }
    else {
        if(bluewarrior[0]->attack(redwarrior[0],bluewarrior[0]->getsword(),t)) return aftermath(t);//blue initiative

        if(redwarrior[0]->anti_attack(bluewarrior[0],redwarrior[0]->getsword(),t)) return aftermath(t);//red anti        
    }
    return aftermath(t);
}
int City::virtual_fight(int t){
    City virtual_city = *this;
    return virtual_city.fight(t);
}
int main() {
    int n;
    cin >> n;
    for (int i = 1; i <= n; ++i) {
        is_end = false;
        line.clear();
        printf("Case %d:\n",i);
        cin >> M >> N >> R >> K >> T;
        cin >> initial_order[0].HP >> initial_order[1].HP >> initial_order[2].HP >> initial_order[3].HP >> initial_order[4].HP;
        cin >> initial_order[0].power >> initial_order[1].power >> initial_order[2].power >> initial_order[3].power >> initial_order[4].power;
        Head redhead(M, "red", redorder);
        line.push_back(&redhead);
        int j = 1;
        for (int j = 1; j <= N; ++j) {
            City* c = new City(j);
            all_city.push_back(c);
            line.push_back(c);
        }
        Head bluehead(M, "blue", blueorder);
        line.push_back(&bluehead);
        //build citys
        int t = 0;
        while (t <= T) {
            if (redhead.is_build || bluehead.is_build) {
                redhead.build(t);
                bluehead.build(t);
            }
            t += 5;//lion run!
            if (t > T) break;
            for (int j = 0; j <= N + 1; ++j) {
                if (!line[j]->redwarrior.empty() && line[j]->redwarrior[0]->name == "lion" && line[j]->redwarrior[0]->loyalty <= 0) {
                    printf("%03d:%02d red %s %d ran away\n", t / 60, t % 60, line[j]->redwarrior[0]->name.c_str(), line[j]->redwarrior[0]->id);
                    line[j]->redwarrior.pop_back();
                }
                if (!line[j]->bluewarrior.empty() && line[j]->bluewarrior[0]->name == "lion" && line[j]->bluewarrior[0]->loyalty <= 0) {
                    printf("%03d:%02d blue %s %d ran away\n", t / 60, t % 60, line[j]->bluewarrior[0]->name.c_str(), line[j]->bluewarrior[0]->id);
                    line[j]->bluewarrior.pop_back();
                }
            }
            t += 5;//warriors move!
            if (t > T) break;
            for (int j = 0; j < line.size(); ++j) {
                if (!line[j]->redwarrior.empty())
                    line[j]->redwarrior[0]->is_moved = false;
                if (!line[j]->bluewarrior.empty())
                    line[j]->bluewarrior[0]->is_moved = false;
            }
            for (int j = 0; j < line.size(); ++j) {
                if (!line[j]->redwarrior.empty() && !line[j]->redwarrior[0]->is_moved &&j<N+1)
                    line[j]->redwarrior[0]->move(t);
                if (!line[j]->bluewarrior.empty() && !line[j]->bluewarrior[0]->is_moved&&j>0)
                    line[j]->bluewarrior[0]->move(t);
            }
            if (line[0]->bluewarrior.size()==2) {
                is_end = true;
                printf("%03d:%02d %s %s %d reached red headquarter with %d elements and force %d\n", t / 60, t % 60, line[0]->bluewarrior[1]->color.c_str(), line[0]->bluewarrior[1]->name.c_str(), line[0]->bluewarrior[1]->id, line[0]->bluewarrior[1]->HP, line[0]->bluewarrior[1]->power);
                printf("%03d:%02d %s headquarter was taken\n", t / 60, t % 60, redhead.color.c_str());
            }
            if(line[0]->bluewarrior.size()==1){
                if(!line[0]->bluewarrior[0]->is_reached)
                printf("%03d:%02d %s %s %d reached red headquarter with %d elements and force %d\n", t / 60, t % 60, line[0]->bluewarrior[0]->color.c_str(), line[0]->bluewarrior[0]->name.c_str(), line[0]->bluewarrior[0]->id, line[0]->bluewarrior[0]->HP, line[0]->bluewarrior[0]->power);
                line[0]->bluewarrior[0]->is_reached = true;
            }
            for (int j = 1; j <= N; ++j) {
                if (!line[j]->redwarrior.empty()) {
                    printf("%03d:%02d %s %s %d marched to city %d with %d elements and force %d\n", t / 60, t % 60, line[j]->redwarrior[0]->color.c_str(), line[j]->redwarrior[0]->name.c_str(), line[j]->redwarrior[0]->id, line[j]->redwarrior[0]->locatecity->location, line[j]->redwarrior[0]->HP, line[j]->redwarrior[0]->power);
                }
                if (!line[j]->bluewarrior.empty()) {
                    printf("%03d:%02d %s %s %d marched to city %d with %d elements and force %d\n", t / 60, t % 60, line[j]->bluewarrior[0]->color.c_str(), line[j]->bluewarrior[0]->name.c_str(), line[j]->bluewarrior[0]->id, line[j]->bluewarrior[0]->locatecity->location, line[j]->bluewarrior[0]->HP, line[j]->bluewarrior[0]->power);
                }
            }
            if (line[N + 1]->redwarrior.size()==1) {
                if(!line[N+1]->redwarrior[0]->is_reached)
                printf("%03d:%02d %s %s %d reached blue headquarter with %d elements and force %d\n", t / 60, t % 60, line[N + 1]->redwarrior[0]->color.c_str(), line[N + 1]->redwarrior[0]->name.c_str(), line[N + 1]->redwarrior[0]->id, line[N + 1]->redwarrior[0]->HP, line[N + 1]->redwarrior[0]->power);
                line[N+1]->redwarrior[0]->is_reached = true;
            }
            if (line[N + 1]->redwarrior.size()==2) {
                is_end = true;
                printf("%03d:%02d %s %s %d reached blue headquarter with %d elements and force %d\n", t / 60, t % 60, line[N + 1]->redwarrior[1]->color.c_str(), line[N + 1]->redwarrior[1]->name.c_str(), line[N + 1]->redwarrior[1]->id, line[N + 1]->redwarrior[1]->HP, line[N + 1]->redwarrior[1]->power);
                printf("%03d:%02d %s headquarter was taken\n", t / 60, t % 60, bluehead.color.c_str());
            }
            if (is_end) {
                break;
            }
            t += 10;//Produce HP!
            if (t > T) break;
            for(int j = 1;j<=N;++j){
                line[j]->HP+=10;
            }
            t += 10;//getHP!
            if (t > T) break;
            for(int j = 1;j<= N;++j){
                if(!line[j]->redwarrior.empty()&&line[j]->bluewarrior.empty()){
                    printf("%03d:%02d %s %s %d earned %d elements for his headquarter\n",t/60,t%60,line[j]->redwarrior[0]->color.c_str(),line[j]->redwarrior[0]->name.c_str(),line[j]->redwarrior[0]->id,line[j]->HP);
                    redhead.HP += line[j]->HP;
                    line[j]->HP = 0;
                }
                else if(line[j]->redwarrior.empty()&&!line[j]->bluewarrior.empty()){
                    printf("%03d:%02d %s %s %d earned %d elements for his headquarter\n",t/60,t%60,line[j]->bluewarrior[0]->color.c_str(),line[j]->bluewarrior[0]->name.c_str(),line[j]->bluewarrior[0]->id,line[j]->HP);
                    bluehead.HP += line[j]->HP;
                    line[j]->HP = 0;
                }
            }
            t += 5;//Arrow!
            if (t > T) break;
            for (int j = 1; j <= N; ++j) {
                if (!line[j]->redwarrior.empty()&&!line[j+1]->bluewarrior.empty()&&j!=N) {
                    int use_w = line[j]->redwarrior[0]->getarrow();
                    if(use_w!=-1)
                    line[j]->redwarrior[0]->shot(line[j+1]->bluewarrior[0],use_w,t);
                }
                if(!line[j]->bluewarrior.empty()&&!line[j-1]->redwarrior.empty()&&j!=1){
                    int use_w = line[j]->bluewarrior[0]->getarrow();
                    if(use_w!=-1)
                    line[j]->bluewarrior[0]->shot(line[j-1]->redwarrior[0],use_w,t);
                }
            }
            t += 3;//Bomb!
            if(t > T) break;
            for(int j = 1; j <= N;++j){
                if(!line[j]->redwarrior.empty()&&!line[j]->bluewarrior.empty()&&line[j]->redwarrior[0]->HP>0&&line[j]->bluewarrior[0]->HP>0){
                    is_mute = true;
                    is_passivation = false;
                    int virtual_res = line[j]->virtual_fight(t);
                    is_mute = false;
                    is_passivation = true;
                    int red_use = line[j]->redwarrior[0]->getbomb();
                    int blue_use = line[j]->bluewarrior[0]->getbomb();
                    if(red_use!=-1&&virtual_res ==-1){
                        line[j]->redwarrior[0]->boom(line[j]->bluewarrior[0],red_use,t);
                        line[j]->aftermath(t);
                    }
                    else if(blue_use!=-1&&virtual_res==1){
                        line[j]->bluewarrior[0]->boom(line[j]->redwarrior[0],blue_use,t);
                        line[j]->aftermath(t);
                    }
                }
            }
            t += 2;//Fight!
            if (t > T) break;
            for (int j = 0; j <= N+1; ++j) {
                if (!line[j]->redwarrior.empty() && !line[j]->bluewarrior.empty()) {
                    Warrior * red = line[j]->redwarrior[0];
                    Warrior * blue = line[j]->bluewarrior[0];
                    int red_HP = red->HP;
                    int blue_HP = blue->HP;
                    int x = line[j]->fight(t);
                    if(x == 0){
                        if(red->name == "lion"){
                            line[j]->redwarrior[0]->loyalty -= K;
                        }
                        if(blue->name == "lion"){
                            line[j]->bluewarrior[0]->loyalty -= K;
                        }
                        if(red->name=="dragon"){
                            dynamic_cast<Dragon*>(red)->morale-=0.2;
                            if(dynamic_cast<Dragon*>(red)->morale>=0.8&&((line[j]->location%2==1&&line[j]->flag==0)||line[j]->flag==1))
                            printf("%03d:%02d red dragon %d yelled in city %d\n", t / 60, t % 60, line[j]->redwarrior[0]->id, line[j]->location);
                        }
                        if(blue->name=="dragon"){
                            dynamic_cast<Dragon*>(blue)->morale-=0.2;
                            if(dynamic_cast<Dragon*>(blue)->morale>=0.8&&((line[j]->location%2==0&&line[j]->flag==0)||line[j]->flag==-1))
                            printf("%03d:%02d blue dragon %d yelled in city %d\n", t / 60, t % 60, blue->id, line[j]->location);
                        }
                    }
                    if(x == 1){
                        red->will_reward =true;
                        if(blue->name=="lion"&&blue_HP>0){
                            line[j]->redwarrior[0]->HP += blue_HP;
                        }
                        if(red->name=="wolf"){
                            dynamic_cast<Wolf*>(red)->rob(blue);
                        }
                        if(red->name=="dragon"){
                            dynamic_cast<Dragon*>(red)->morale+=0.2;
                            if(dynamic_cast<Dragon*>(red)->morale>=0.8&&((line[j]->location%2==1&&line[j]->flag==0)||line[j]->flag==1))
                            printf("%03d:%02d red dragon %d yelled in city %d\n", t / 60, t % 60, line[j]->redwarrior[0]->id, line[j]->location);
                        }
                        red->get_HP+=line[j]->HP;
                        line[j]->HP = 0;                
                        printf("%03d:%02d %s %s %d earned %d elements for his headquarter\n",t/60,t%60,line[j]->redwarrior[0]->color.c_str(),line[j]->redwarrior[0]->name.c_str(),line[j]->redwarrior[0]->id,red->get_HP);
                        }
                    if(x==-1){
                        blue->will_reward =true;
                        if(red->name=="lion"&&red_HP>0){
                            line[j]->bluewarrior[0]->HP += red_HP;
                        }
                        if(blue->name=="wolf"){
                            dynamic_cast<Wolf*>(blue)->rob(red);
                        }
                        if(blue->name=="dragon"){
                            dynamic_cast<Dragon*>(blue)->morale-=0.2;
                            if(dynamic_cast<Dragon*>(blue)->morale>=0.8&&((line[j]->location%2==0&&line[j]->flag==0)||line[j]->flag==-1))
                            printf("%03d:%02d blue dragon %d yelled in city %d\n", t / 60, t % 60, blue->id, line[j]->location);
                        }
                        blue->get_HP+=line[j]->HP;
                        line[j]->HP = 0;
                        printf("%03d:%02d %s %s %d earned %d elements for his headquarter\n",t/60,t%60,line[j]->bluewarrior[0]->color.c_str(),line[j]->bluewarrior[0]->name.c_str(),line[j]->bluewarrior[0]->id,blue->get_HP);
                    }
                }
                line[j]->changeflag(t);
                if(line[j]->redwarrior.empty()&&!line[j]->bluewarrior.empty()){
                    if(line[j]->bluewarrior[0]->HP<=0){
                        line[j]->bluewarrior.pop_back();
                    }
                }
                if(line[j]->bluewarrior.empty()&&!line[j]->redwarrior.empty()){
                    if(line[j]->redwarrior[0]->HP<=0){
                        line[j]->redwarrior.pop_back();
                    }
                }
            }
            
            for(int j=N+1;j>=0;--j){
                if(redhead.HP<8) break;
                if(!line[j]->redwarrior.empty()&&line[j]->redwarrior[0]->will_reward){
                    redhead.HP-=8;
                    line[j]->redwarrior[0]->HP+=8;
                    line[j]->redwarrior[0]->will_reward=false;
                }
            }
            for(int j=0;j<=N+1;++j){
                if(bluehead.HP<8) break;
                if(!line[j]->bluewarrior.empty()&&line[j]->bluewarrior[0]->will_reward){
                    bluehead.HP-=8;
                    line[j]->bluewarrior[0]->HP+=8;
                    line[j]->bluewarrior[0]->will_reward=false;
                }
            }//reward!
            for(int j = 1;j<= N;++j){
                if(!line[j]->redwarrior.empty()&&line[j]->bluewarrior.empty()){
                    redhead.HP+=line[j]->redwarrior[0]->get_HP;
                    line[j]->redwarrior[0]->get_HP=0;
                }
                else if(!line[j]->bluewarrior.empty()&&line[j]->redwarrior.empty()){
                    bluehead.HP+=line[j]->bluewarrior[0]->get_HP;
                    line[j]->bluewarrior[0]->get_HP=0;
                }
            }//getHP!
            t += 10;
            if (t > T) break;
            printf("%03d:%02d %d elements in red headquarter\n", t / 60, t % 60, redhead.HP);
            printf("%03d:%02d %d elements in blue headquarter\n", t / 60, t % 60, bluehead.HP);
            t += 5;
            if (t > T) break;
            for (int j = 0; j <= N+1; ++j) {
                if(!line[j]->redwarrior.empty()){
                    printf("%03d:%02d %s %s %d has ",t/60,t%60,line[j]->redwarrior[0]->color.c_str(),line[j]->redwarrior[0]->name.c_str(),line[j]->redwarrior[0]->id);
                    int count[3] = {0};
                    int power[3] = {0};
                    for(int i=0;i<line[j]->redwarrior[0]->weapons.size();++i){
                        count[line[j]->redwarrior[0]->weapons[i]->id] = 1;
                        if(line[j]->redwarrior[0]->weapons[i]->id==0)
                        power[line[j]->redwarrior[0]->weapons[i]->id] = line[j]->redwarrior[0]->weapons[i]->power;
                        if(line[j]->redwarrior[0]->weapons[i]->id==2)
                        power[line[j]->redwarrior[0]->weapons[i]->id] = line[j]->redwarrior[0]->weapons[i]->times;
                    }
                    if(count[0]==0&&count[1]==0&&count[2]==0)
                        printf("no weapon");
                    if(count[2]==1){
                        printf("arrow");
                        if(R)
                        printf("(%d)",power[2]);
                    }
                    if(count[1]==1){
                        if(count[2]==1)
                        printf(",bomb");
                        else
                        printf("bomb");
                    }
                    if(count[0]==1){
                        if(count[1]==1||count[2]==1)
                        printf(",sword(%d)",power[0]);
                        else
                        printf("sword(%d)",power[0]);
                    }
                    printf("\n");
               }
            }
            for (int j = 0; j <= N+1; ++j) {
                if(!line[j]->bluewarrior.empty()){
                    printf("%03d:%02d %s %s %d has ",t/60,t%60,line[j]->bluewarrior[0]->color.c_str(),line[j]->bluewarrior[0]->name.c_str(),line[j]->bluewarrior[0]->id);
                    int count[3] = {0};
                    int power[3] = {0};
                    for(int i=0;i<line[j]->bluewarrior[0]->weapons.size();++i){
                        count[line[j]->bluewarrior[0]->weapons[i]->id] = 1;
                        if(line[j]->bluewarrior[0]->weapons[i]->id==0)
                        power[line[j]->bluewarrior[0]->weapons[i]->id] = line[j]->bluewarrior[0]->weapons[i]->power;
                        if(line[j]->bluewarrior[0]->weapons[i]->id==2)
                        power[line[j]->bluewarrior[0]->weapons[i]->id] = line[j]->bluewarrior[0]->weapons[i]->times;
                    }
                    if(count[0]==0&&count[1]==0&&count[2]==0)
                        printf("no weapon");
                    if(count[2]==1){
                        printf("arrow");
                        if(R)
                        printf("(%d)",power[2]);
                    }
                    if(count[1]==1){
                        if(count[2]==1)
                        printf(",bomb");
                        else
                        printf("bomb");
                    }
                    if(count[0]==1){
                        if(count[1]==1||count[2]==1)
                        printf(",sword(%d)",power[0]);
                        else
                        printf("sword(%d)",power[0]);
                    }
                    printf("\n");
               }
            }
            t += 5;
            if (t > T) break;
        }
        for(int i = 0;i<all_city.size();++i){
            delete all_city[i];
        }
        all_city.clear();
        for(int i = 0;i<all_weapon.size();++i){
            delete all_weapon[i];
        }
        all_weapon.clear();
        for(int i = 0;i<all_warrior.size();++i){
            delete all_warrior[i];
        }
        all_warrior.clear();
    }
    return 0;
}