#include <iostream>
#include <cstring>
#include <string>
#include <deque>
#include <list>
#include <algorithm>
using namespace std;
int M, N, K, T;
bool is_end = false;
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
deque<City*> line;
class Weapon {
public:
    int id;
    int power;
    int times = 1e6;
    virtual void power_remake(int user_power) {}
};
bool cmp(const Weapon* w1, const Weapon* w2) {
    if (w1->id == w2->id && w1->id == 2)
        return w1->times < w2->times;
    return w1->id < w2->id;
}
bool robcmp(const Weapon* w1, const Weapon* w2) {
    if (w1->id == w2->id && w1->id == 2)
        return w1->times > w2->times;
    return w1->id < w2->id;
}
class Sword :public Weapon {
public:
    Sword(int user_power) {
        power = user_power * 0.2;
        id = 0;
    }
    virtual void power_remake(int user_power) { power = user_power * 0.2; }
};
class Bomb :public Weapon {
public:
    Bomb(int user_power) {
        power = user_power * 0.4;
        id = 1;
        times = 1;
    }
    virtual void power_remake(int user_power) { power = user_power * 0.4; }
};
class Arrow :public Weapon {
public:
    Arrow(int user_power) {
        times = 2;
        power = user_power * 0.3;
        id = 2;
    }
    virtual void power_remake(int user_power) { power = user_power * 0.3; }
};
class City {
public:
    int location;
    deque<Warrior*> redwarrior;
    deque<Warrior*> bluewarrior;
    City() {}
    City(int n) :location(n) {}
    int fight(int t);
    bool is_impotent(int red_use, int blue_use);
};
class Warrior {
public:
    int HP;
    int id;
    int power;
    int loyalty = 1e6;
    bool is_alive = true;
    bool is_moved = false;
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
    void throw_away(int use_w) {
        deque<Weapon*>::iterator it = weapons.begin() + use_w;
        weapons.erase(it);
    }
    virtual void rob(Warrior* w, int t) {
        cout << "?" << endl;
    }
    virtual int attack(Warrior* w, int use_w) {
        w->attacked(weapons[use_w]->power);
        if (weapons[use_w]->id == 1 && name != "ninja") {
            attacked(0.5 * weapons[use_w]->power);
        }
        if (weapons[use_w]->id != 0) {
            weapons[use_w]->times--;
            if (weapons[use_w]->times <= 0) {
                throw_away(use_w);
                return 0;
            }
        }
        return 1;
    }
    virtual void attacked(int deltaHP) {
        HP -= deltaHP;
        if (HP <= 0) is_alive = false;
    }
};
class Head :public City {
public:
    int HP;
    string color;
    Dictionary* order[5];
    int num_warrior[5] = { 0 };
    int will_build = 0;
    int total_num = 0;
    bool is_build = true;
    Head(int n, string s, Dictionary* color_order[5]) :HP(n), color(s) {
        for (int i = 0; i < 5; ++i) {
            order[i] = color_order[i];
        }
        location = color == "red" ? 0 : N + 1;
    }
    void build(int t);
    void print(int t);
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
            break;
        }
        case 1: {
            Bomb* b = new Bomb(power);
            weapons.push_back(b);
            break;
        }
        case 2: {
            Arrow* a = new Arrow(power);
            weapons.push_back(a);
            break;
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
            break;
        }
        case 1: {
            Bomb* b = new Bomb(power);
            weapons.push_back(b);
            break;
        }
        case 2: {
            Arrow* a = new Arrow(power);
            weapons.push_back(a);
            break;
        }
        }
        switch ((id + 1) % 3) {
        case 0: {
            Sword* s = new Sword(power);
            weapons.push_back(s);
            break;
        }
        case 1: {
            Bomb* b = new Bomb(power);
            weapons.push_back(b);
            break;
        }
        case 2: {
            Arrow* a = new Arrow(power);
            weapons.push_back(a);
            break;
        }
        }
        sort(weapons.begin(), weapons.end(), cmp);
    }
    virtual int attack(Warrior* w, int use_w) {
        w->attacked(weapons[use_w]->power);
        if (weapons[use_w]->id != 0) {
            weapons[use_w]->times--;
            if (weapons[use_w]->times <= 0) {
                throw_away(use_w);
                return 0;
            }
        }
        return 1;
    }
};
class Iceman :public Warrior {
public:
    Iceman(int hp, int Power, string colour, int total_num) {
        HP = hp, power = Power, id = total_num, color = colour;
        name = "iceman";
        switch (id % 3) {
        case 0: {
            Sword* s = new Sword(power);
            weapons.push_back(s);
            break;
        }
        case 1: {
            Bomb* b = new Bomb(power);
            weapons.push_back(b);
            break;
        }
        case 2: {
            Arrow* a = new Arrow(power);
            weapons.push_back(a);
            break;
        }
        }
    }
    virtual void move(int t) {
        if (HP > 0) {
            HP -= int(HP * 0.1);

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
class Lion :public Warrior {
public:
    Lion(int resthp, int hp, int Power, string colour, int total_num) {
        loyalty = resthp, HP = hp, power = Power, id = total_num, color = colour;
        name = "lion";
        printf("Its loyalty is %d\n", loyalty);
        switch (id % 3) {
        case 0: {
            Sword* s = new Sword(power);
            weapons.push_back(s);
            break;
        }
        case 1: {
            Bomb* b = new Bomb(power);
            weapons.push_back(b);
            break;
        }
        case 2: {
            Arrow* a = new Arrow(power);
            weapons.push_back(a);
            break;
        }
        }
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
        loyalty -= K;
    }
};
class Wolf :public Warrior {
public:
    Wolf(int hp, int Power, string colour, int total_num) {
        HP = hp, power = Power, id = total_num, color = colour;
        name = "wolf";
    }
    virtual void rob(Warrior* w, int t) {
        if (w->name == "wolf" || w->weapons.empty())
            return;
        int num = 0;
        int will_robbed = w->weapons[0]->id;
        sort(w->weapons.begin(), w->weapons.end(), robcmp);
        while (weapons.size() < 10 && !w->weapons.empty() && w->weapons[0]->id == will_robbed) {
            w->weapons[0]->power_remake(power);
            weapons.push_back(w->weapons[0]);
            num++;
            w->weapons.pop_front();
        }
        sort(weapons.begin(), weapons.end(), cmp);
        sort(w->weapons.begin(), w->weapons.end(), cmp);
        printf("%03d:%02d %s wolf %d took %d %s from %s %s %d in city %d\n", t / 60, t % 60, color.c_str(), id, num, weaponname[will_robbed].c_str(), w->color.c_str(), w->name.c_str(), w->id, w->locatecity->location);
    }
};
void Head::build(int t) {
    if (is_build == false) { return; }
    else {
        if (HP < order[will_build]->HP) {
            is_build = false;
            return;
        }
        ++total_num;
        HP -= order[will_build]->HP;
        num_warrior[will_build]++;
        print(t);
        switch (order[will_build]->name[0]) {
        case 'd': {
            Dragon* d = new Dragon(HP, order[will_build]->HP, order[will_build]->power, this->color, total_num);
            d->locatecity = this;
            if (color == "red")
                redwarrior.push_back(d);
            else bluewarrior.push_back(d);
            break;
        }
        case 'n': {
            Ninja* n = new Ninja(order[will_build]->HP, order[will_build]->power, this->color, total_num);
            n->locatecity = this;
            if (color == "red")
                redwarrior.push_back(n);
            else bluewarrior.push_back(n);
            break;
        }
        case 'i': {
            Iceman* i = new Iceman(order[will_build]->HP, order[will_build]->power, this->color, total_num);
            i->locatecity = this;
            if (color == "red")
                redwarrior.push_back(i);
            else bluewarrior.push_back(i);
            break;
        }
        case 'l': {
            Lion* l = new Lion(HP, order[will_build]->HP, order[will_build]->power, this->color, total_num);
            l->locatecity = this;
            if (color == "red")
                redwarrior.push_back(l);
            else bluewarrior.push_back(l);
            break;
        }
        case 'w': {
            Wolf* w = new Wolf(order[will_build]->HP, order[will_build]->power, this->color, total_num);
            w->locatecity = this;
            if (color == "red")
                redwarrior.push_back(w);
            else bluewarrior.push_back(w);
            break;
        }
        }
        will_build = (will_build + 1) % 5;
    }
}
void Head::print(int t) {
    printf("%03d:%02d %s %s %d born\n",
        t / 60, t % 60, color.c_str(), order[will_build]->name.c_str(), total_num);
}
bool City::is_impotent(int red_use, int blue_use) {
    for (int i = 0; i < redwarrior[0]->weapons.size(); ++i) {
        if (redwarrior[0]->weapons[i]->id != 0)
            return false;
    }
    for (int i = 0; i < bluewarrior[0]->weapons.size(); ++i) {
        if (bluewarrior[0]->weapons[i]->id != 0)
            return false;
    }
    if ((redwarrior[0]->weapons.empty() || (!redwarrior[0]->weapons.empty() && redwarrior[0]->weapons[0]->id == 0 && redwarrior[0]->weapons[0]->power == 0)) && (bluewarrior[0]->weapons.empty() || (!bluewarrior[0]->weapons.empty() && bluewarrior[0]->weapons[0]->id == 0 && bluewarrior[0]->weapons[0]->power == 0)))
        return true;
    if (redwarrior[0]->HP <= 0 && bluewarrior[0]->HP <= 0)
        return true;
    return false;
}
int City::fight(int t) {
    if (location % 2 == 1) {
        int red_use = 0;
        int blue_use = 0;
        while (redwarrior[0]->HP > 0 && bluewarrior[0]->HP && (!redwarrior[0]->weapons.empty() || !bluewarrior[0]->weapons.empty())) {
            if (is_impotent(red_use, blue_use))
                break;
            if (!redwarrior[0]->weapons.empty() && redwarrior[0]->HP > 0) {
                int t = redwarrior[0]->attack(bluewarrior[0], red_use);
                if (t == 1) red_use = (red_use + 1) % redwarrior[0]->weapons.size();
                else if (redwarrior[0]->weapons.size() > 0) red_use = red_use % redwarrior[0]->weapons.size();
                if (redwarrior[0]->HP <= 0 || bluewarrior[0]->HP <= 0) {
                    redwarrior[0]->is_alive = redwarrior[0]->HP > 0;
                    bluewarrior[0]->is_alive = bluewarrior[0]->HP > 0;
                    break;
                }
            }
            if (!bluewarrior[0]->weapons.empty() && bluewarrior[0]->HP > 0) {
                int t = bluewarrior[0]->attack(redwarrior[0], blue_use);
                if (t == 1) blue_use = (blue_use + 1) % bluewarrior[0]->weapons.size();
                else if (bluewarrior[0]->weapons.size() > 0)  blue_use = blue_use % bluewarrior[0]->weapons.size();
                if (redwarrior[0]->HP <= 0 || bluewarrior[0]->HP <= 0) {
                    redwarrior[0]->is_alive = redwarrior[0]->HP > 0;
                    bluewarrior[0]->is_alive = bluewarrior[0]->HP > 0;
                    break;
                }
            }
        }
    }
    else {
        int red_use = 0;
        int blue_use = 0;
        while (redwarrior[0]->HP > 0 && bluewarrior[0]->HP && (!redwarrior[0]->weapons.empty() || !bluewarrior[0]->weapons.empty())) {
            if (is_impotent(red_use, blue_use))
                break;
            if (!bluewarrior[0]->weapons.empty()) {
                int t = bluewarrior[0]->attack(redwarrior[0], blue_use);
                if (t == 1) blue_use = (blue_use + 1) % bluewarrior[0]->weapons.size();
                else if (bluewarrior[0]->weapons.size() > 0)  blue_use = blue_use % bluewarrior[0]->weapons.size();
                if (redwarrior[0]->HP <= 0 || bluewarrior[0]->HP <= 0) {
                    redwarrior[0]->is_alive = redwarrior[0]->HP > 0;
                    bluewarrior[0]->is_alive = bluewarrior[0]->HP > 0;
                    break;
                }
            }
            if (!redwarrior[0]->weapons.empty()) {
                int t = redwarrior[0]->attack(bluewarrior[0], red_use);
                if (t == 1) red_use = (red_use + 1) % redwarrior[0]->weapons.size();
                else if (redwarrior[0]->weapons.size() > 0)  red_use = red_use % redwarrior[0]->weapons.size();
                if (redwarrior[0]->HP <= 0 || bluewarrior[0]->HP <= 0) {
                    redwarrior[0]->is_alive = redwarrior[0]->HP > 0;
                    bluewarrior[0]->is_alive = bluewarrior[0]->HP > 0;
                    break;
                }
            }
        }
    }
    if (redwarrior[0]->HP > 0 && bluewarrior[0]->HP <= 0) {
        sort(bluewarrior[0]->weapons.begin(), bluewarrior[0]->weapons.end(), robcmp);
        while (!bluewarrior[0]->weapons.empty() && redwarrior[0]->weapons.size() < 10) {
            bluewarrior[0]->weapons[0]->power_remake(redwarrior[0]->power);
            redwarrior[0]->weapons.push_back(bluewarrior[0]->weapons[0]);
            bluewarrior[0]->weapons.pop_front();
        }
        while (!bluewarrior[0]->weapons.empty()) {
            bluewarrior[0]->weapons.pop_front();
        }
        sort(redwarrior[0]->weapons.begin(), redwarrior[0]->weapons.end(), cmp);
        printf("%03d:%02d red %s %d killed blue %s %d in city %d remaining %d elements\n", t / 60, t % 60, redwarrior[0]->name.c_str(), redwarrior[0]->id, bluewarrior[0]->name.c_str(), bluewarrior[0]->id, location, redwarrior[0]->HP);
        bluewarrior.pop_back();
        return 1;
    }
    else if (bluewarrior[0]->HP > 0 && redwarrior[0]->HP <= 0) {
        sort(redwarrior[0]->weapons.begin(), redwarrior[0]->weapons.end(), robcmp);
        while (!redwarrior[0]->weapons.empty() && bluewarrior[0]->weapons.size() < 10) {
            redwarrior[0]->weapons[0]->power_remake(bluewarrior[0]->power);
            bluewarrior[0]->weapons.push_back(redwarrior[0]->weapons[0]);
            redwarrior[0]->weapons.pop_front();
        }
        while (!redwarrior[0]->weapons.empty()) {
            redwarrior[0]->weapons.pop_front();
        }
        sort(bluewarrior[0]->weapons.begin(), bluewarrior[0]->weapons.end(), cmp);
        printf("%03d:%02d blue %s %d killed red %s %d in city %d remaining %d elements\n", t / 60, t % 60, bluewarrior[0]->name.c_str(), bluewarrior[0]->id, redwarrior[0]->name.c_str(), redwarrior[0]->id, location, bluewarrior[0]->HP);
        redwarrior.pop_back();
        return -1;
    }
    else if (bluewarrior[0]->HP > 0 && redwarrior[0]->HP > 0) {
        printf("%03d:%02d both red %s %d and blue %s %d were alive in city %d\n", t / 60, t % 60, redwarrior[0]->name.c_str(), redwarrior[0]->id, bluewarrior[0]->name.c_str(), bluewarrior[0]->id, location);
        return 0;
    }
    else {
        printf("%03d:%02d both red %s %d and blue %s %d died in city %d\n", t / 60, t % 60, redwarrior[0]->name.c_str(), redwarrior[0]->id, bluewarrior[0]->name.c_str(), bluewarrior[0]->id, location);
        redwarrior.pop_back();
        bluewarrior.pop_back();
        return 1e6;
    }
}
int main() {
    int n;
    cin >> n;
    for (int i = 1; i <= n; ++i) {
        is_end = false;
        line.clear();
        cout << "Case" << ' ' << i << ':' << endl;
        cin >> M >> N >> K >> T;
        cin >> initial_order[0].HP >> initial_order[1].HP >> initial_order[2].HP >> initial_order[3].HP >> initial_order[4].HP;
        cin >> initial_order[0].power >> initial_order[1].power >> initial_order[2].power >> initial_order[3].power >> initial_order[4].power;
        Head redhead(M, "red", redorder);
        line.push_back(&redhead);
        int j = 1;
        for (int j = 1; j <= N; ++j) {
            City* c = new City(j);
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
                if (!line[j]->redwarrior.empty() && !line[j]->redwarrior[0]->is_moved)
                    line[j]->redwarrior[0]->move(t);
                if (!line[j]->bluewarrior.empty() && !line[j]->bluewarrior[0]->is_moved)
                    line[j]->bluewarrior[0]->move(t);
            }
            if (!line[0]->bluewarrior.empty()) {
                is_end = true;
                printf("%03d:%02d %s %s %d reached red headquarter with %d elements and force %d\n", t / 60, t % 60, line[0]->bluewarrior[0]->color.c_str(), line[0]->bluewarrior[0]->name.c_str(), line[0]->bluewarrior[0]->id, line[0]->bluewarrior[0]->HP, line[0]->bluewarrior[0]->power);
                printf("%03d:%02d %s headquarter was taken\n", t / 60, t % 60, redhead.color.c_str());
            }
            for (int j = 1; j <= N; ++j) {
                if (!line[j]->redwarrior.empty()) {
                    printf("%03d:%02d %s %s %d marched to city %d with %d elements and force %d\n", t / 60, t % 60, line[j]->redwarrior[0]->color.c_str(), line[j]->redwarrior[0]->name.c_str(), line[j]->redwarrior[0]->id, line[j]->redwarrior[0]->locatecity->location, line[j]->redwarrior[0]->HP, line[j]->redwarrior[0]->power);
                }
                if (!line[j]->bluewarrior.empty()) {
                    printf("%03d:%02d %s %s %d marched to city %d with %d elements and force %d\n", t / 60, t % 60, line[j]->bluewarrior[0]->color.c_str(), line[j]->bluewarrior[0]->name.c_str(), line[j]->bluewarrior[0]->id, line[j]->bluewarrior[0]->locatecity->location, line[j]->bluewarrior[0]->HP, line[j]->bluewarrior[0]->power);
                }
            }
            if (!line[N + 1]->redwarrior.empty()) {
                is_end = true;
                printf("%03d:%02d %s %s %d reached blue headquarter with %d elements and force %d\n", t / 60, t % 60, line[N + 1]->redwarrior[0]->color.c_str(), line[N + 1]->redwarrior[0]->name.c_str(), line[N + 1]->redwarrior[0]->id, line[N + 1]->redwarrior[0]->HP, line[N + 1]->redwarrior[0]->power);
                printf("%03d:%02d %s headquarter was taken\n", t / 60, t % 60, bluehead.color.c_str());
            }
            if (is_end) {
                break;
            }
            t += 25;//Rob!
            if (t > T) break;
            for (int j = 1; j <= N; ++j) {
                if (!line[j]->redwarrior.empty() && line[j]->redwarrior[0]->name == "wolf" && !line[j]->bluewarrior.empty()) {
                    line[j]->redwarrior[0]->rob(line[j]->bluewarrior[0], t);
                }
                if (!line[j]->bluewarrior.empty() && line[j]->bluewarrior[0]->name == "wolf" && !line[j]->redwarrior.empty()) {
                    line[j]->bluewarrior[0]->rob(line[j]->redwarrior[0], t);
                }
            }
            t += 5;//Fight!
            if (t > T) break;
            for (int j = 1; j <= N; ++j) {
                if (!line[j]->redwarrior.empty() && !line[j]->bluewarrior.empty()) {
                    int x = line[j]->fight(t);
                    if (x >= 0 && x <= 1 && line[j]->redwarrior[0]->name == "dragon") {
                        printf("%03d:%02d red dragon %d yelled in city %d\n", t / 60, t % 60, line[j]->redwarrior[0]->id, line[j]->location);
                    }
                    if (x <= 0 && x >= -1 && line[j]->bluewarrior[0]->name == "dragon") {
                        printf("%03d:%02d blue dragon %d yelled in city %d\n", t / 60, t % 60, line[j]->bluewarrior[0]->id, line[j]->location);
                    }
                }
            }
            t += 10;
            if (t > T) break;
            printf("%03d:%02d %d elements in red headquarter\n", t / 60, t % 60, redhead.HP);
            printf("%03d:%02d %d elements in blue headquarter\n", t / 60, t % 60, bluehead.HP);
            t += 5;
            if (t > T) break;
            for (int j = 1; j <= N; ++j) {
                if (!line[j]->redwarrior.empty()) {
                    int a[3] = { 0 };
                    for (int k = 0; k < line[j]->redwarrior[0]->weapons.size(); ++k) {
                        a[line[j]->redwarrior[0]->weapons[k]->id]++;
                    }
                    printf("%03d:%02d red %s %d has %d sword %d bomb %d arrow and %d elements\n", t / 60, t % 60, line[j]->redwarrior[0]->name.c_str(), line[j]->redwarrior[0]->id, a[0], a[1], a[2], line[j]->redwarrior[0]->HP);
                }
                if (!line[j]->bluewarrior.empty()) {
                    int b[3] = { 0 };
                    deque<Weapon*>::iterator it2 = line[j]->bluewarrior[0]->weapons.begin();
                    for (; it2 < line[j]->bluewarrior[0]->weapons.end(); it2++) {
                        b[(*it2)->id]++;
                    }
                    printf("%03d:%02d blue %s %d has %d sword %d bomb %d arrow and %d elements\n", t / 60, t % 60, line[j]->bluewarrior[0]->name.c_str(), line[j]->bluewarrior[0]->id, b[0], b[1], b[2], line[j]->bluewarrior[0]->HP);
                }
            }
            t += 5;
            if (t > T) break;
        }
    }
    return 0;
}