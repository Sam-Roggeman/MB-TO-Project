//
// Created by samro on 14/10/2021.
//

#include "CFG.h"

#include <utility>

CFG::CFG()
{
        this->V = {"BINDIGIT", "S"};
        this->T = {"0", "1", "a", "b"};
        this->P = {{"BINDIGIT", {"0", "1"}}, {"S", {"a S b BINDIGIT", ""}}};
        this->S = "S";
}

void CFG::print() const
{
        bool first_loop = true;
        // V
        std::cout << "V = {";
        // loop over de symbolen
        for (const auto& symbol : V) {
                // eerste loop geen komma
                if (!first_loop) {
                        first_loop = false;
                        std::cout << ", ";
                }
                first_loop = false;
                std::cout << symbol;
        }
        std::cout << "}" << std::endl;
        // T
        std::cout << "T = {";
        first_loop = true;
        // loop over de symbolen
        for (const auto& symbol : T) {
                // eerste loop geen komma
                if (!first_loop) {
                        std::cout << ", ";
                }
                first_loop = false;
                std::cout << symbol;
        }
        std::cout << "}" << std::endl;
        // P
        std::cout << "P = {";
        first_loop = true;
        // loop over alle symbol -> vector pairs
        for (const auto& pair : P) {
                first_loop = true;
                // loop over de pairs waarnaar het symbool kan gaan
                for (const auto& to : pair.second) {
                        std::cout << std::endl << "  " << pair.first << " -> `";
                        first_loop = true;

                        for (const auto& c : to) {
                                if (first_loop)
                                        first_loop = false;
                                else
                                        std::cout << ' ';
                                std::cout << c;
                        }
                        std::cout << "`";
                }
        }
        std::cout << std::endl << "}" << std::endl;
        // S
        std::cout << "S = " << S;
}

CFG::CFG(const std::string& filepath)
{
        // inlezen uit file
        std::ifstream input(filepath);
        json j;
        input >> j;
        // Add de variabelen
        V = j.find("Variables")->get<std::vector<std::string>>();
        // Add de terminalen
        T = j.find("Terminals")->get<std::vector<std::string>>();
        std::string key;
        std::vector<std::string> val;

        // Loop over alle producties
        for (json& element : j.find("Productions")->get<std::vector<json>>()) {
                // extract de from
                key = element.find("head")->get<std::string>();
                // extract de to
                val = element.find("body")->get<std::vector<std::string>>();
                // als de key (from) niet in de map zit
                if (P.find(key) == P.end()) {
                        // initalizeer een lege map voor deze key
                        P[key] = {};
                }
                // steek de value in de lijst van key[key]
                P[key].emplace_back(val);
        }
        // S = het startsymbool
        S = j.find("Start")->get<std::string>();
        // sorteer de CFG op alfabet
        sort();
}

void CFG::sort()
{
        // sort de variabelen
        std::sort(V.begin(), V.end());
        // sort de terminalen
        std::sort(T.begin(), T.end());
        for (auto& pair : this->P) {
                // sort de vector van mogelijke producties vanuit pair.first
                std::sort(pair.second.begin(), pair.second.end());
        }
}

CFG::CFG(std::vector<std::string> V, std::vector<std::string> T,
         std::map<std::string, std::vector<std::vector<std::string>>> P, std::string S)
    : V(std::move(V)), T(std::move(T)), S(std::move(S)), P(std::move(P))
{
        this->sort();
}

void CFG::toCNF()
{
        std::cout << "Original CFG:\n\n" << *this << "\n\n-------------------------------------";
        this->EliminateEpsilon();
        std::cout << *this << "\n\n";
        this->EliminateUnitPairs();
        std::cout << *this << "\n\n";
        this->EliminateUselessSymbols();
        std::cout << *this << "\n\n";
        this->replaceBadies();
        std::cout << *this << "\n\n";
        this->breakBodies();
        std::cout << ">>> Result CFG:\n\n" << *this << "\n";
}

void CFG::EliminateEpsilon()
{
        // vector die de de nullables van de huidige iteratie bijhoudt
        std::vector<std::string> nullables_iteration{};
        // vector die alle nullables bijhoudt
        std::vector<std::string> all_nullables{};
        // bool die bijhoudt of er bij de laatste iteratie een nullable is gevonden
        bool found_nullable = true;
        // startcount van producties
        int p_count_s = this->countProductions();
        // eindcount van producties
        int p_count_e{};
        // vector met indices van producties die gedelete zullenm worden
        std::vector<int> to_del{};
        std::set<std::string> encountered{};
        while (found_nullable) {
                nullables_iteration = {};
                found_nullable = false;
                // pair<variabele,production_list>
                for (auto& production_list : this->P) {
                        to_del = {};
                        // loop over de indexen van alle producties van deze variabele
                        for (int index = 0; index < production_list.second.size(); index++) {
                                // als de variabele nullable is
                                if (production_list.second[index].empty()) {
                                        // als de nullable nog niet eerder gevonden was
                                        if (std::find(all_nullables.begin(), all_nullables.end(),
                                                      production_list.first) == all_nullables.end()) {
                                                found_nullable = true;
                                                nullables_iteration.emplace_back(production_list.first);
                                                all_nullables.emplace_back(production_list.first);
                                        }
                                        to_del.emplace_back(index);
                                }
                        }
                        // inverteerd (zodat ze van groot naar klein zijn)
                        std::reverse(to_del.begin(), to_del.end());
                        // delete de producties van achter naar voor
                        for (int index : to_del) {
                                production_list.second.erase(production_list.second.begin() + index);
                        }
                }
                std::string key{};
                // Vervang alle nullables in de producties
                // pair<variabele,production_list>
                for (auto& production_list : this->P) {
                        encountered = {};
                        for (int index = 0; index < production_list.second.size(); index++) {
                                // loop over alle symbolen van alle producties
                                for (auto symbol_ind = 0; symbol_ind != production_list.second[index].size();
                                     symbol_ind++) {
                                        // loop over alle nullable symbolen van deze iteratie
                                        for (const std::string& nullable : nullables_iteration) {
                                                // als de nullable in de productie voorkomt
                                                if (production_list.second[index][symbol_ind] == nullable) {
                                                        // maak een nieuwe productie gebaseerd op de vorige
                                                        auto new_prod = production_list.second[index];
                                                        // delete de nullable
                                                        new_prod.erase((std::next(new_prod.begin(), symbol_ind)));
                                                        // maak een nieuwe string van de vector
                                                        key = toString(new_prod);
                                                        // als dit nog niet encountered is
                                                        if (encountered.find(key) == encountered.end()) {
                                                                encountered.insert(key);
                                                                production_list.second.emplace_back(new_prod);
                                                        }
                                                }
                                        }
                                }
                        }
                }
        }
        p_count_e = this->countProductions();
        std::sort(all_nullables.begin(), all_nullables.end());
        this->sort();
        std::cout << "\n\n >> Eliminating epsilon productions\n  Nullables are " << all_nullables << "\n  Created "
                  << p_count_e << " productions, original had " << p_count_s << "\n\n";
}

unsigned int CFG::countProductions()
{
        unsigned int count{};
        for (const auto& production_list : this->P) {
                count += production_list.second.size();
        }
        return count;
}

void CFG::EliminateUnitPairs()
{
        std::map<std::string, std::vector<std::string>> unit_productions{};
        int unit_prod_count{};
        bool first_loop = true;
        unsigned int p_count_s = this->countProductions();
        unsigned int p_count_e{};
        bool finished = false;
        std::vector<std::string> to_add{};
        std::vector<std::pair<std::string, std::string>> unit_pairs{};
        // add unit pairs of type A,A
        for (auto& var : this->V) {
                unit_pairs.emplace_back(var, var);
        }
        // zolang we niet klaar zijn
        while (!finished) {
                finished = true;
                for (auto& [from, productions] : this->P) {
                        // loop over alle producties vanuit from symbol
                        for (auto& production : productions) {
                                if (this->isUnitproduction(production)) {
                                        // als er nog geen map element aangemaakt is
                                        if (unit_productions.find(from) == unit_productions.end()) {
                                                unit_productions[from] = {};
                                        }
                                        // als het unit pair nog niet eerder gevonden was
                                        if (std::find(unit_pairs.begin(), unit_pairs.end(),
                                                      std::make_pair(from, production[0])) == unit_pairs.end()) {
                                                unit_productions[from].emplace_back(production[0]);
                                                unit_pairs.emplace_back(from, production[0]);
                                                // enkel de oorspronkelijke unit-productions tellen bij de count
                                                if (first_loop) {
                                                        unit_prod_count++;
                                                }
                                        }
                                }
                        }
                }
                // loop over alle unit productions
                for (auto& [from, productions] : unit_productions) {
                        // loop over de indices van de producties
                        for (auto& production : productions) {
                                auto to_it = unit_productions.find(production);
                                // als de productie geen unit- productie is
                                if (to_it != unit_productions.end()) {
                                        for (auto& symbol : to_it->second) {
                                                // check of het symbol vanuit from een unit pair is
                                                if (std::find(unit_pairs.begin(), unit_pairs.end(),
                                                              std::make_pair(from, symbol)) == unit_pairs.end()) {
                                                        // voeg het toe en clear the while loop
                                                        unit_pairs.emplace_back(std::make_pair(from, symbol));
                                                        unit_productions.at(from).emplace_back(symbol);
                                                        finished = false;
                                                }
                                        }
                                }
                        }
                }
                first_loop = false;
        }
        // loop over de productiemap
        for (auto& [from, productions] : this->P) {
                // itereer over de producties vanuit from
                for (auto production = productions.begin(); production != productions.end();) {
                        // als het een unit productie is, verwijder deze
                        if (this->isUnitproduction(*production)) {
                                production = productions.erase(production);
                        } else {
                                production++;
                        }
                }
        }
        // loop over alle unit pairs
        for (auto& [from, to] : unit_pairs) {
                // loop over alle producties van to
                for (auto& element : this->P[to]) {
                        // als de nieuwe productie nog niet in de lijst zit (voorkomen van duplicates)
                        if (std::find(this->P[from].begin(), this->P[from].end(), element) == this->P[from].end()) {
                                this->P[from].emplace_back(element);
                        }
                }
        }
        first_loop = true;
        std::cout << " >> Eliminating unit pairs\n  Found " << unit_prod_count << " unit productions\n  Unit pairs: {";
        std::sort(unit_pairs.begin(), unit_pairs.end());
        for (const auto& unit_pair : unit_pairs) {
                if (!first_loop) {
                        std::cout << ", ";
                }
                std::cout << "(" << unit_pair.first << ", " << unit_pair.second << ")";
                first_loop = false;
        }

        p_count_e = this->countProductions();
        std::cout << "}\n  Created " << p_count_e << " new productions, original had " << p_count_s << "\n\n";
        this->sort();
}

void CFG::EliminateUselessSymbols()
{
        std::vector<std::string> non_generating_symbols{this->T};
        non_generating_symbols.insert(non_generating_symbols.end(), this->V.begin(), this->V.end());
        std::vector<std::string> non_reachable_symbols = non_generating_symbols;
        std::map<std::string, bool> goes_to_terminal{};
        std::tuple<int, int, int> var_ter_prod{this->V.size(), this->T.size(), this->countProductions()};

        std::cout << " >> Eliminating useless symbols\n";
        // find the generating symbols
        std::vector<std::string> generating_symbols = this->findGeneratingSymbols();
        // construct the non-generating vector
        for (const auto& sym : generating_symbols) {
                non_generating_symbols.erase(
                    std::find(non_generating_symbols.begin(), non_generating_symbols.end(), sym));
        }
        // output de generating symbols
        std::cout << "  Generating symbols: " << generating_symbols << "\n";
        // remove the non-generating productions
        this->removeSymbols(non_generating_symbols);
        // find the reachable symbols
        std::vector<std::string> reachable_symbols = this->findReachableSymbols();
        // construct the non-reachable vector
        for (const auto& sym : reachable_symbols) {
                non_reachable_symbols.erase(std::find(non_reachable_symbols.begin(), non_reachable_symbols.end(), sym));
        }
        std::cout << "  Reachable symbols: " << reachable_symbols << "\n";
        std::cout << "  Useful symbols: " << reachable_symbols << "\n";
        this->removeSymbols(non_reachable_symbols);
        // find the reachable symbols
        reachable_symbols = this->findReachableSymbols();
        // construct the non-reachable vector
        non_reachable_symbols = {this->T};

        std::cout << "  Removed " << std::get<0>(var_ter_prod) - this->V.size() << " variables, "
                  << std::get<1>(var_ter_prod) - this->T.size() << " terminals and "
                  << std::get<2>(var_ter_prod) - this->countProductions() << " productions"
                  << "\n\n";
}

void CFG::removeSymbols(std::vector<std::string>& to_remove)
{
        bool remove_flag = false;
        for (const auto& sym : to_remove) {
                // als het een variabele is, erase het
                if (std::find(this->V.begin(), this->V.end(), sym) != this->V.end()) {
                        this->V.erase(std::find(this->V.begin(), this->V.end(), sym));
                }
                // als het een terminaal is, erase het
                if (std::find(this->T.begin(), this->T.end(), sym) != this->T.end()) {
                        this->T.erase(std::find(this->T.begin(), this->T.end(), sym));
                }
                // als er producties vanuit sym vertrekken
                if (this->P.find(sym) != this->P.end()) {
                        this->P.erase(sym);
                }
        }
        // loop over alle producties en erase elke productie waar het symbool in voorkomt
        for (auto& prods_it : this->P) {
                for (auto prod_it = prods_it.second.begin(); prod_it != prods_it.second.end();) {
                        for (auto& sym_it : *prod_it) {
                                if (std::find(to_remove.begin(), to_remove.end(), sym_it) != to_remove.end()) {
                                        remove_flag = true;
                                        break;
                                }
                        }
                        if (remove_flag) {
                                remove_flag = false;
                                prod_it = prods_it.second.erase(prod_it);
                        } else {
                                prod_it++;
                        }
                }
        }
}

std::vector<std::string> CFG::findGeneratingSymbols()
{
        bool done = false;
        bool generating_flag = false;
        // alle terminals genereren
        std::vector<std::string> generating_symbols{this->T};
        while (!done) {
                done = true;
                for (auto& [from, productions] : this->P) {
                        if (std::find(generating_symbols.begin(), generating_symbols.end(), from) ==
                            generating_symbols.end()) {
                                // loop over alle producties van de cfg, indien het een genererende productie heeft is
                                // het symbool genererend
                                for (std::vector<std::string>& production : productions) {
                                        generating_flag = true;
                                        for (std::string& symbol : production) {
                                                // indien een van de symbolen van de productie geen genererend symbool
                                                // is, is de productie dit ook niet
                                                if (std::find(generating_symbols.begin(), generating_symbols.end(),
                                                              symbol) == generating_symbols.end()) {
                                                        generating_flag = false;
                                                }
                                        }
                                        if (generating_flag) {
                                                generating_symbols.emplace_back(from);
                                                done = false;
                                                break;
                                        }
                                }
                        }
                }
        }
        std::sort(generating_symbols.begin(), generating_symbols.end());
        return generating_symbols;
}

std::vector<std::string> CFG::findReachableSymbols()
{
        std::vector<std::string> reachable_symbols{this->S};
        std::stack<std::string> to_do{};
        // stack die bijhoudt welke symbolen we nog moeten behandelen (van degene die al bereikbaar zijn)
        to_do.push(this->S);
        std::string current_symbol{};

        while (!to_do.empty()) {
                current_symbol = to_do.top();
                to_do.pop();

                for (std::vector<std::string>& productions : this->P[current_symbol]) {
                        // loop over alle symbolen bereikbaar vanaf het huidige symbool
                        for (std::string& sym : productions) {
                                // als het symbool nog niet bereikbaar is, is het reachbaar
                                if (std::find(reachable_symbols.begin(), reachable_symbols.end(), sym) ==
                                    reachable_symbols.end()) {
                                        // als het symbool een variabele is moeten we dit op de stack pushen
                                        if (std::find(this->V.begin(), this->V.end(), sym) != this->V.end()) {
                                                to_do.push(sym);
                                        }
                                        reachable_symbols.emplace_back(sym);
                                }
                        }
                }
        }
        std::sort(reachable_symbols.begin(), reachable_symbols.end());
        return reachable_symbols;
}

void CFG::replaceBadies()
{
        std::map<std::string, std::string> terminal_to_var{};
        unsigned int start_var = this->V.size();
        unsigned int start_prod = this->countProductions();
        // stell de terminal_to_var map op:
        // check of er al producties zijn die dit doen
        for (auto& [from, producties] : this->P) {
                // er mag maar een productie zijn vanuit from
                if (producties.size() != 1) {
                        break;
                }
                // loop over alle producties
                for (auto& productie : producties) {
                        // er mag maar een symbool zijn en dit moet een terminaal zijn
                        if (productie.size() == 1 &&
                            std::find(this->T.begin(), this->T.end(), productie[0]) != this->T.end()) {
                                terminal_to_var[productie[0]] = from;
                        }
                }
        }
        std::string var{};
        // anders maken we eigen variabelen naar de onbrekende terminalen
        for (auto& term : this->T) {
                if (terminal_to_var.find(term) == terminal_to_var.end()) {
                        var = "_" + term;
                        terminal_to_var[term] = var;
                        this->V.emplace_back(var);
                        this->P[var] = {{term}};
                }
        }
        // replace de terminale symbols met de respectievelijke variabele
        for (auto& [from, producties] : this->P) {
                for (auto& productie : producties) {
                        for (auto& symbol : productie) {
                                if (std::find(this->T.begin(), this->T.end(), symbol) != this->T.end() &&
                                    terminal_to_var.at(symbol) != from) {
                                        symbol = terminal_to_var.at(symbol);
                                }
                        }
                }
        }
        std::vector<std::string> new_var{std::next(this->V.begin(), start_var), this->V.end()};
        this->sort();

        std::cout << " >> Replacing terminals in bad bodies\n"
                     "  Added "
                  << this->V.size() - start_var << " new variables: " << new_var << "\n  Created "
                  << this->countProductions() << " new productions, original had " << start_prod << "\n\n";
}

void CFG::breakBodies()
{
        int counter = 0;
        bool done = false;
        std::map<std::string, unsigned int> var_counter{};
        std::string new_var{};
        // houdt de counter bij voor elke variabele zodat we weten welke index de volgende zal krijgen
        for (auto& [from, to_list] : this->P) {
                var_counter[from] = 1;
        }
        while (!done) {
                done = true;
                for (auto& [from, productions] : this->P) {
                        for (auto& production : productions) {
                                // als de productie te groot is
                                if (production.size() > 2) {
                                        var_counter.at(from)++;
                                        new_var = from + "_" + std::to_string(var_counter.at(from));
                                        // voeg een nieuwe variabele toe
                                        this->V.emplace_back(new_var);
                                        // nieuwe productie vanuit de nieuwe variabele tot de laatste 2 symbolen van de
                                        // productie
                                        this->P[new_var] = {{production.at(production.size() - 2),
                                                             production.at(production.size() - 1)}};
                                        // vervang de laatste 2 symbolen van de productie door de nieuwe variabele
                                        production.at(production.size() - 2) = new_var;
                                        production.erase(production.end() - 1);
                                        counter++;
                                        done = false;
                                }
                        }
                }
        }
        std::cout << " >> Broke " << counter << " bodies, added " << counter << " new variables";
        std::cout << "\n";
        this->sort();
}

bool CFG::isUnitproduction(const std::vector<std::string>& production)
{
        // als de lengte van de productie 0 is en het een variabele is
        return (production.size() == 1 && std::find(this->V.begin(), this->V.end(), production.at(0)) != this->V.end());
}

std::ostream& operator<<(std::ostream& out, const CFG& cfg)
{
        cfg.print();
        return out;
}
std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& v)
{
        out << "{";
        bool first_loop = true;
        for (const std::string& e : v) {
                if (!first_loop) {
                        out << ", ";
                } else {
                        first_loop = false;
                }
                out << e;
        }
        out << "}";
        return out;
}

std::ostream& operator<<(std::ostream& out, const std::vector<std::vector<std::string>>& v)
{
        for (auto& c : v) {
                out << c;
        }
        return out;
}

std::string toString(std::vector<std::string> v)
{
        std::string result{};
        for (const std::string& q : v) {
                result += q;
        }
        return result;
}

const std::map<std::string, std::vector<std::vector<std::string>>>& CFG::getP() const { return P; }

const std::vector<std::string>& CFG::getV() const { return V; }

const std::vector<std::string>& CFG::getT() const { return T; }

const std::string& CFG::getS() const { return S; }
