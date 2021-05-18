#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;
#define numberOfCells 37

class seed{
    public:
        int m_index = -1;
        int m_from = -1;
        float m_rich = -9;
        int m_real_rich = -1;
};

class output{
    public:
        output(int index, int seed): 
            m_index(index), m_seed(seed) {}
        output(string move, int index, int seed): 
            m_move(move), m_index(index), m_seed(seed) {}
    public:
        string m_move;
        int m_index;
        int m_seed;
};

class cell {
    public:
        cell(int index, int richness, int neigh[6][3]): m_index(index), m_rich(richness) {
            for (int i = 0; i < 6; i++) {
                m_neigh[i][0] = neigh[i][0];
                m_neigh[i][1] = -1;
                m_neigh[i][2] = -1;
            }
        }
        cell(const cell &copy){
            m_index = copy.m_index;
            m_rich = copy.m_rich;
            for (int i = 0; i < 6; i++) {
                m_neigh[i][0] = copy.m_neigh[i][0];
            }
        }
        ~cell() {}
        void reset_shadow(){
            for (int i = 0 ; i < 6 ; i++){
			    m_shadow[i] = 0;
            }
        }
        void reset(){
            reset_shadow();
            m_tree = -1;
            m_can_seed = 0;
            m_my_tree[0] = -1;
            m_my_tree[1] = -1;
            m_total_shadow = 0;
            m_ttl_shdw_tree = 0;
		}

    public:
        int m_index;
        int m_rich;
        int m_neigh[6][3];
        int m_neigh_or[18] = {-1};
        int m_tree = -1;
        int m_my_tree[2] = {-1};
        int m_shadow[6] = {0};
        int m_total_shadow = 0;
        int m_ttl_shdw_tree = 0;
        int m_can_seed = 0;
};

class tree {
    public:
        tree(int index, int size, bool isMine, bool isDormant):
            m_index(index), m_size(size), m_isMine(isMine), m_isDormant(isDormant)  {}
        tree(const tree &copy){
            m_index = copy.m_index;
            m_size = copy.m_size;
            m_isMine = copy.m_isMine;
            m_isDormant = copy.m_isDormant;
        }
        ~tree() {}

    public:
        int m_index;
        int m_size;
        bool m_isMine;
        bool m_isDormant;
        seed m_seed;
        int how_many_pts[6];
        bool tmrw = 0;
        bool tmrw_if_grow = 0;
};

class player {
    public:
        player(): m_sun(0), m_score(0) {}
        player(int sun, int score):
            m_sun(sun), m_score(score) {}
        player(int sun, int score, bool oppIsWaiting):
            m_sun(sun), m_score(score), m_oppIsWaiting(oppIsWaiting) {}
        player(const player &copy){
            m_sun = copy.m_sun;
            m_score = copy.m_score;
            m_oppIsWaiting = copy.m_oppIsWaiting;
            action = copy.action;
            m_arbre = copy.m_arbre;
            int i = -1;
            while (++i < 4) {
                nb_tree[i] = copy.nb_tree[i];
                nb_rich[i] = copy.nb_rich[i];
            }
        }
        player &operator=(const player &copy){
            m_sun = copy.m_sun;
            m_score = copy.m_score;
            m_oppIsWaiting = copy.m_oppIsWaiting;
            action = copy.action;
            m_arbre = copy.m_arbre;
            int i = -1;
            while (++i < 4) {
                nb_tree[i] = copy.nb_tree[i];
                nb_rich[i] = copy.nb_rich[i];
            }
            return *this;
        }
        ~player() {}

    public:
        int m_sun;
        int m_score;
        bool m_oppIsWaiting = -1;
        int nb_tree[4] = {0}; // nombre d'arbres par taille
        int nb_rich[4] = {0}; // nombre d'arbres par richesse
        map<float, tree> m_arbre;
        vector<output> action;
};

class game {
    public:
        game() {}
        game(int day, int angle, int nutrients):
            m_day(day), m_angle(angle), m_nutrients(nutrients) {}
        ~game() {}
        game(const game &copy){
            m_day = copy.m_day;
            m_angle = copy.m_angle;
            m_nutrients = copy.m_nutrients;
        }

    public:
        int m_day = 0;
        int m_angle = 0;
        int m_nutrients = 0;
        int min_shadow;
};

void all_actions(player player[2], game &game, vector<cell> &plateau){
    vector<output> action;
    tree tmp_tree(-1, -1, 1, 0);

    // COMPLETE 5 DERNIERS TOURS
    for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); game.m_day > 19 && it != player[0].m_arbre.end() && player[0].m_sun >= 4; ++it){
        if (game.m_day < 23 && player[1].nb_tree[3] == 0)
            break ;
        if (game.m_nutrients <= 1 && plateau[it->second.m_index].m_rich == 1)
            break ;
        if (it->second.how_many_pts[23 - game.m_day] < game.m_nutrients * 3){
            if (it->second.m_isDormant == 0 && it->second.m_size == 3){
                if (tmp_tree.m_index == -1)
                    tmp_tree = it->second;
                else if (!tmp_tree.tmrw && it->second.tmrw)
                    tmp_tree = it->second;
                else if (tmp_tree.tmrw == it->second.tmrw &&
                    plateau[tmp_tree.m_index].m_total_shadow < plateau[it->first].m_total_shadow){
                    tmp_tree = it->second;
                }
            }
        }
        if (game.m_day == 23 && plateau[it->second.m_index].m_rich > 1){
            if (it->second.m_isDormant == 0 && it->second.m_size == 3 ){
                if (tmp_tree.m_index == -1)
                        tmp_tree = it->second;
            }
        }
    }
    if (tmp_tree.m_index != -1) {
        output output("COMPLETE", tmp_tree.m_index, -1);
        action.push_back(output);
    }
    // COMPLETE If > 4 || GROW EN LVL 3
    tmp_tree.m_index = -1;
    for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); game.m_day > 4 && game.m_day < 23 && it != player[0].m_arbre.end() && player[0].m_sun >= 7 + player[0].nb_tree[3]; ++it) {
        if (game.m_day >= 20 && player[0].nb_tree[3] > 0)
            break ;
        if (game.m_nutrients <= 1 && plateau[it->second.m_index].m_rich == 1)
            break ;
        if (it->second.m_isDormant == 0 && it->second.m_size == 2) {
            if (tmp_tree.m_index == -1)
                tmp_tree = it->second;
            else if (tmp_tree.tmrw_if_grow && !it->second.tmrw_if_grow)
                tmp_tree = it->second;
            else if (tmp_tree.tmrw_if_grow == it->second.tmrw_if_grow &&
                plateau[tmp_tree.m_index].m_ttl_shdw_tree > plateau[it->first].m_ttl_shdw_tree){
                tmp_tree = it->second;
            }
        }
    }
    if (tmp_tree.m_index != -1) {
        if ((player[0].nb_tree[3] > 4 && game.m_day <= 16) || (player[0].nb_tree[3] > 2 && game.m_day > 16 && game.m_day < 20 && player[1].nb_tree[3])) {
            tree tmp_tree2(-1, -1, 1, 0);
            for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); it != player[0].m_arbre.end(); ++it)
                if (it->second.m_isDormant == 0 && it->second.m_size == 3){
                    if (tmp_tree2.m_index == -1)
                        tmp_tree2 = it->second;
                    else if (!tmp_tree2.tmrw && it->second.tmrw)
                        tmp_tree2 = it->second;
                    else if (tmp_tree2.tmrw == it->second.tmrw &&
                        plateau[tmp_tree2.m_index].m_total_shadow < plateau[it->first].m_total_shadow){
                        tmp_tree2 = it->second;
                    }
                }
            if (tmp_tree2.m_index != -1) {
                if (!tmp_tree2.tmrw && game.m_day <= 17)
                    ;
                else
                {
                output output("COMPLETE", tmp_tree2.m_index, -1);
                action.push_back(output);
                }
            }
        }
        else if (player[0].nb_tree[3] < 6){
            output output("GROW", tmp_tree.m_index, -1);
            action.push_back(output);
        }
    }

    // GROW EN LVL 2
    tmp_tree.m_index = -1;
    for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); game.m_day < 21 && it != player[0].m_arbre.end() && player[0].m_sun >= 3 + player[0].nb_tree[2]; ++it)
        if (it->second.m_isDormant == 0 && it->second.m_size == 1){
            if (tmp_tree.m_index == -1)
                tmp_tree = it->second;
            else if (tmp_tree.tmrw_if_grow && !it->second.tmrw_if_grow)
                tmp_tree = it->second;
            else if (tmp_tree.tmrw_if_grow == it->second.tmrw_if_grow && 
                plateau[tmp_tree.m_index].m_ttl_shdw_tree > plateau[it->first].m_ttl_shdw_tree){
                tmp_tree = it->second;
            }
            else if (game.m_day < 3 && tmp_tree.m_seed.m_rich < it->second.m_seed.m_rich)
                tmp_tree = it->second;
        }
    if (tmp_tree.m_index != -1) {
        output output("GROW", tmp_tree.m_index, -1);
        action.push_back(output);
    }

    // GROW EN LVL 1
    tmp_tree.m_index = -1;
    for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); it != player[0].m_arbre.end() && player[0].m_sun >= 1 + player[0].nb_tree[1]; ++it)
    {
        if (game.m_day >= 18 && player[0].nb_tree[1])
            break ;
        if (game.m_day == 21)
            break ;
        if (it->second.m_isDormant == 0 && it->second.m_size == 0){
            if (tmp_tree.m_index == -1)
                tmp_tree = it->second;
            else if (tmp_tree.tmrw_if_grow && !it->second.tmrw_if_grow)
                tmp_tree = it->second;
            else if (tmp_tree.tmrw_if_grow == it->second.tmrw_if_grow &&
                plateau[tmp_tree.m_index].m_ttl_shdw_tree > plateau[it->first].m_ttl_shdw_tree){
                tmp_tree = it->second;
            }
        }
    }
    if (tmp_tree.m_index != -1) {
        output output("GROW", tmp_tree.m_index, -1);
        action.push_back(output);
    }
    seed tmp;

    // SEED
    tmp_tree.m_index = -1;
    for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); !player[0].nb_tree[0] && it != player[0].m_arbre.end() && player[0].m_sun >= player[0].nb_tree[0]; ++it){
        if (player[0].nb_rich[1] > 4 && it->second.m_seed.m_real_rich == 1)
            continue ;
        if (player[0].nb_rich[2] > 3 && it->second.m_seed.m_real_rich == 2)
            continue ;
        if (it->second.m_seed.m_from != -1){
            if (tmp.m_rich < it->second.m_seed.m_rich ||
                 ((int)tmp.m_rich == (int)it->second.m_seed.m_rich && tmp.m_index > it->second.m_seed.m_index)) {
                if (it->second.m_isDormant == 0 && it->second.m_size > 1){
                    tmp.m_rich = it->second.m_seed.m_rich;
                    tmp.m_from = it->second.m_seed.m_from;
                    tmp.m_index = it->second.m_seed.m_index;
                }
                else {
                    for (std::map<float,tree>::iterator ite = player[0].m_arbre.begin(); ite != player[0].m_arbre.end(); ++ite){
                        if (ite->second.m_size > 0 && ite->second.m_isDormant == 0){
                            if (ite->second.m_size == 3) {
                                for (int i = 0; i < 18 ; i++) {
                                    if (plateau[ite->second.m_index].m_neigh_or[i] == it->second.m_seed.m_index
                                        || plateau[ite->second.m_index].m_neigh[i / 3][i % 3] == it->second.m_seed.m_index){
                                        tmp.m_rich = it->second.m_seed.m_rich;
                                        tmp.m_from = ite->second.m_index;
                                        tmp.m_index = it->second.m_seed.m_index;
                                        break ;
                                    }
                                } 
                            }

                            
                        }
                    }
                }
            }
        }
    }
    int stop = 0;
    if (tmp.m_rich > -9){
        for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); stop == 0 && it != player[0].m_arbre.end() ; ++it)
            if (it->second.m_isDormant == 0 && it->second.m_size == 3) {
                for (int i = 0;stop == 0 && i < 18; i++)
                    if (plateau[it->first].m_neigh_or[i] == tmp.m_index) {
                        tmp.m_from = it->first;
                        stop = 1;
                    }
                for (int i = 0; stop == 0 && i < 6; i++)
                    for (int j = 0; stop == 0 && j < 3; j++)
                        if (plateau[it->first].m_neigh[i][j] == tmp.m_index) {
                            tmp.m_from = it->first;
                            stop = 1;
                        }
            }
        output output("SEED", tmp.m_from, tmp.m_index);
        action.push_back(output);
    }
    if (tmp.m_from == -1 && !player[0].nb_tree[0]){ 
        for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); stop == 0 && it != player[0].m_arbre.end() ; ++it) {
            if (player[0].nb_rich[1] > 4 && plateau[game.min_shadow].m_rich == 1)
                break ;
            if (player[0].nb_rich[2] > 3 && plateau[game.min_shadow].m_rich == 2)
                break ;
            if (it->second.m_isDormant == 0 && it->second.m_size > 1) {
                for (int i = 0; stop == 0 && i < 18; i++){
                    if (plateau[it->first].m_neigh_or[i] == game.min_shadow) {
                        tmp.m_from = it->first;
                        tmp.m_index = game.min_shadow;
                        stop = 1;
                    }
                }
                for (int i = 0; stop == 0 && i < 6; i++)
                    for (int j = 0; stop == 0 && j < it->second.m_size; j++){
                        if (plateau[it->first].m_neigh[i][j] == game.min_shadow) {
                            tmp.m_from = it->first;
                            tmp.m_index = game.min_shadow;
                            stop = 1;
                        }
                    }
            }
        }
        if (stop == 1){
            output output("SEED", tmp.m_from, tmp.m_index);
            action.push_back(output);
        }
    }
    output output("WAIT", -1, -1);
    action.push_back(output);
    player[0].action = action;
    
}

void best_seed(player player[2], vector<cell> &plateau){
    for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); it!= player[0].m_arbre.end(); ++it){
        if (it->second.m_size > -1){
            seed seed;
            for (int i = 0; i < 6; i++){
                if (plateau[it->second.m_index].m_neigh[i][0] != -1){
                    int j = it->second.m_index;
                    if ((j = plateau[j].m_neigh[i][0]) == -1)
                        continue ;
                    for (int m = -1; m < 2; m++){
                        int k;
                        if ((k = plateau[j].m_neigh[(i + m) % 6][0]) != -1)
                            if (plateau[k].m_rich > 0 && plateau[k].m_tree == -1){
                                plateau[k].m_can_seed = 1;
                                for (int l = 0; l < 6; l++)
                                    if (plateau[k].m_neigh[l][0] != -1 && plateau[plateau[k].m_neigh[l][0]].m_my_tree[0] > -1){
                                        plateau[k].m_can_seed = 0;
                                    }
                                float balance_seed = plateau[k].m_rich;
                                if (m == -1 || m == 1)
                                        balance_seed += 3;
                                balance_seed -= plateau[k].m_total_shadow;
                                if (seed.m_rich < balance_seed && plateau[k].m_can_seed == 1){
                                    seed.m_rich = balance_seed;
                                    seed.m_index = k;
                                    seed.m_from = it->second.m_index;
                                    seed.m_real_rich = plateau[k].m_rich;
                                }
                            }
                    }
                }
            }
            if (seed.m_index != -1){
                it->second.m_seed.m_index = seed.m_index;
                it->second.m_seed.m_from = seed.m_from;
                it->second.m_seed.m_rich = seed.m_rich;
                it->second.m_seed.m_real_rich = seed.m_real_rich;
            }
        }
    }
}

void total_shadow(vector<cell> &plateau){
    for (int i = 0; i < numberOfCells; i++){
        plateau[i].m_total_shadow = 0;
        plateau[i].m_ttl_shdw_tree = 0;
        for (int j = 0; j < 6 ; j++) {
            if (plateau[i].m_shadow[j] && plateau[i].m_shadow[j] >= plateau[i].m_tree){
                plateau[i].m_total_shadow++;
            if (plateau[i].m_tree > -1 && plateau[i].m_tree > 3 && plateau[i].m_shadow[j] >= plateau[i].m_tree + 1)
                plateau[i].m_ttl_shdw_tree++;
            }
        }
    }
}

void weekly_sun(player player[2], vector<cell> &plateau, game game){
    for (std::map<float,tree>::iterator it = player[0].m_arbre.begin(); it!= player[0].m_arbre.end(); ++it) {
        if (it->second.m_size < 3)
            it->second.tmrw_if_grow = plateau[it->first].m_shadow[(game.m_day + 1) % 6] >= it->second.m_size + 1;
        it->second.tmrw = plateau[it->first].m_shadow[(game.m_day + 1) % 6] >= it->second.m_size;
        for (int i = 0; i < 6; i++){
            bool tmrw = plateau[it->first].m_shadow[(game.m_day + 1 + i) % 6] < it->second.m_size;
            if (i == 0)
                it->second.how_many_pts[i] = it->second.m_size * tmrw;
            else
                it->second.how_many_pts[i] = it->second.m_size * tmrw + it->second.how_many_pts[i - 1]; 
        }
    }
}

void most_sunny(vector<cell> &plateau, game &game){
    int min_shadow = plateau[0].m_index;
    for (int i = 0; i < numberOfCells; i++){
        if (plateau[i].m_tree == -1){
            if (plateau[min_shadow].m_total_shadow > plateau[i].m_total_shadow
            && plateau[i].m_rich)
                game.min_shadow = plateau[i].m_index;
            }
    }
}

void set_neighbours(vector<cell> &plateau){
    for (int i = 0; i < numberOfCells; i++) {
        for (int j = 0; j < 6; j++){
            if (plateau[i].m_neigh[j][0] != -1)
                plateau[i].m_neigh[j][1] = plateau[plateau[i].m_neigh[j][0]].m_neigh[j][0];
            else 
                plateau[i].m_neigh[j][1] = -1;
            if (plateau[i].m_neigh[j][1] != -1)
                plateau[i].m_neigh[j][2] = plateau[plateau[i].m_neigh[j][1]].m_neigh[j][0];
            else 
                plateau[i].m_neigh[j][2] = -1;
            if (plateau[i].m_neigh[j][0] != -1)
                plateau[i].m_neigh_or[j] = plateau[plateau[i].m_neigh[j][0]].m_neigh[j == 5 ? 0 : j + 1][0];
            else
                plateau[i].m_neigh_or[j] = - 1;
            if (plateau[i].m_neigh[j][1] != -1)
                plateau[i].m_neigh_or[j * 2 + 6] = plateau[plateau[i].m_neigh[j][1]].m_neigh[j == 5 ? 0 : j + 1][0];
            else
                plateau[i].m_neigh_or[j * 2 + 6] = -1;
            if (plateau[i].m_neigh_or[j * 2 + 6] != -1)
                plateau[i].m_neigh_or[j * 2 + 7] = plateau[plateau[i].m_neigh_or[j * 2 + 6]].m_neigh[j >= 4 ? j - 4 : j + 2][0];
            else
                plateau[i].m_neigh_or[j * 2 + 7] = -1;
            
        }
    }
    // dirty fix
    plateau[33].m_neigh_or[7] = 36;
    plateau[17].m_neigh_or[7] = 19;
    plateau[36].m_neigh_or[9] = 21;
    plateau[7].m_neigh_or[9] = 22;
    plateau[21].m_neigh_or[11] = 24;
    plateau[9].m_neigh_or[11] = 25;
    plateau[11].m_neigh_or[13] = 28;
    plateau[24].m_neigh_or[13] = 27;
    plateau[13].m_neigh_or[15] = 31;
    plateau[27].m_neigh_or[15] = 30;
    plateau[30].m_neigh_or[17] = 33;
    plateau[15].m_neigh_or[17] = 34;
}

int main()
{
    int numberOfCell; // 37
    cin >> numberOfCell; cin.ignore();
    vector<cell> plateau;
    for (int i = 0; i < numberOfCells; i++) {
        int index; // 0 is the center cell, the next cells spiral outwards
        int richness; // 0 if the cell is unusable, 1-3 for usable cells
        int neigh[6][3]; // the index of the neighbouring cell for each direction
        cin >> index >> richness >> neigh[0][0] >> neigh[1][0] >> neigh[2][0] >> neigh[3][0] >> neigh[4][0] >> neigh[5][0]; cin.ignore();
        cell tmp(index, richness, neigh);
        plateau.push_back(tmp);
    }
    set_neighbours(plateau);
    
    while (1) {

        // The Game
        int day; // the game lasts 24 days: 0-23
        cin >> day; cin.ignore();
        int angle = day % 6;
        int nutrients; // the base score you gain from the next COMPLETE action
        cin >> nutrients; cin.ignore();
        game game(day, angle, nutrients);

        // My Score
        int sun; 
        int score;
        cin >> sun >> score; cin.ignore();

        // opponent Scor
        int oppSun;
        int oppScore;
        bool oppIsWaiting;
        cin >> oppSun >> oppScore >> oppIsWaiting; cin.ignore();
        player player[2] = {{sun, score}, {oppSun, oppScore, oppIsWaiting}};

        int numberOfTrees; //Nombre Arbre Total
        cin >> numberOfTrees; cin.ignore();
        for (int i = 0; i < numberOfCells; i++)
            plateau[i].reset();
        for (int i = 0; i < numberOfTrees; i++) {
            int cellIndex; // location of this tree
            int size; // size of this tree: 0-3
            bool isMine; // 1 if this is your tree
            bool isDormant; // 1 if this tree is dormant
            cin >> cellIndex >> size >> isMine >> isDormant; cin.ignore();

            player[(1 - isMine)].nb_tree[size]++;
            player[(1 - isMine)].nb_rich[plateau[cellIndex].m_rich]++;
            plateau[cellIndex].m_my_tree[(isMine ? 0: 1)] = size;
            plateau[cellIndex].m_tree = size;

            // add tree in map<tree>
            tree tmp(cellIndex, size, isMine, isDormant);
            player[1 - isMine].m_arbre.insert(std::pair<float,tree>(cellIndex, tmp));

            // define value of shadow for each cell
            if (size > 0){
                for (int j = 0; j < 6 ; j++) {
                    for (int k = 0 ; k < size; k++) {
                        if (plateau[cellIndex].m_neigh[j][k] != -1)
                            if (size > plateau[plateau[cellIndex].m_neigh[j][k]].m_shadow[j])
                                plateau[plateau[cellIndex].m_neigh[j][k]].m_shadow[j] = size;
                    }
                }
            }
        }

        // Shadow day by day
        total_shadow(plateau);
        
        // Best seed for each tree
        best_seed(player, plateau);

        // Never used this, probably a mistake
        int numberOfPossibleMoves;
        cin >> numberOfPossibleMoves; cin.ignore();
        for (int i = 0; i < numberOfPossibleMoves; i++) {
            string possibleMove;
            getline(cin, possibleMove);
        }
      
        // Weekly Earning Sun for each tree
        weekly_sun(player, plateau, game);

        // Most Sunny Spot
        most_sunny(plateau, game);

        // All action possible in a vector, range in this order Complete/Grow/Seed/Wait
        all_actions(player, game, plateau);

        output action(-1, -1);
        action = player[0].action.at(0);

        // if a Tree is the only one who can seed, he won't grow
        output var_output = player[0].action.at(0);
        int tmp_index = -1;
        int tmp_seed = -1;
        int nb_seed = 0;
        int at = -1;
        int at_seed = -1;
        for (std::vector<output>::iterator it = player[0].action.begin(); it!= player[0].action.end(); ++it) {
            at++;
            if (it->m_move == "GROW" && tmp_index == -1)
                tmp_index = it->m_index;
            if (it->m_move == "SEED"){
                nb_seed += 1;
                tmp_seed = it->m_index;
                at_seed = at;
            }
        }
        if (tmp_index != -1 && tmp_index == tmp_seed && nb_seed == 1 && day < 16){
            action = player[0].action.at(at_seed);
        }    
            
        cout << action.m_move << " ";
        if (action.m_move != "WAIT")
            cout << to_string(action.m_index);
        if (action.m_move == "SEED")
            cout << " " << to_string(action.m_seed);
        cout << endl;
    }
}
