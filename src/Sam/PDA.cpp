//
// Created by Sam Roggeman on 29/10/2021.
//

#include "PDA.h"

PDA::PDA(const json &path) {
    // inlezen uit file
    std::ifstream input(path);
    json j;
    input >> j;
    // Add de States
    States = j.find("States")->get<std::vector<std::string>>();
    // Add het Alphabet
    Alphabet = j.find("Alphabet")->get<std::vector<std::string>>();
    // Add het StackAlphabet
    StackAlphabet = j.find("StackAlphabet")->get<std::vector<std::string>>();
    //Transitions variabelen
    std::string from;
    std::string input_s;
    std::string stacktop;
    std::string to;
    std::vector<std::string> replacement;
    //Loop over alle Transitions
    for (json & element : j.find("Transitions")->get<std::vector<json>>()) {
        //extract de from
        from = element.find("from")->get<std::string>();
        //extract de input_s
        input_s = element.find("input")->get<std::string>();
        //extract de stacktop
        stacktop = element.find("stacktop")->get<std::string>();
        //extract de to
        to = element.find("to")->get<std::string>();
        //extract de replacements
        replacement = element.find("replacement")->get<std::vector<std::string>>();
        Transitions.emplace_back(std::make_tuple(from,input_s,stacktop,to,replacement));
    }
    StartState = j.find("StartState")->get<std::string>();
    StartStack = j.find("StartStack")->get<std::string>();
    cur_state = StartState;
    Stack.push(StartStack);
}

CFG PDA::toCFG() const {
    std::vector<std::string> V {};
    std::vector<std::string> T {};
    std::map<std::string, std::vector<std::vector<std::string>>> P;
    std::string S {};
    std::vector<std::string> val {};
    // Startvariabele
    V.emplace_back("S");
    // voor elke combinatie van state, stacksymbool en state2 van de PDA komt er een Variabele in de cfg
    for (auto &state1:this->States){
        for (auto &stack:this->StackAlphabet){
            for (auto &state2:this->States) {
                V.emplace_back("["+state1+","+ stack+","+state2+"]");
            }
        }
    }
    //terminalen worden gecopieerd vanuit het alphabet van de pda
    T = this->Alphabet;
    //transities vanuit S
    P["S"] = {};
    for (auto &state:this->States){
        val = {"["+this->StartState+","+this->StartStack+","+state+"]"};
        P["S"].emplace_back(val);
    }
    int stac_rep_count {};
    std::string to;

    for (auto& transition : this->Transitions){
        stac_rep_count = std::get<4>(transition).size();
        val = {};
        to = "";
        unsigned int n = 0;
        if (stac_rep_count != 0){
            //combinatie van alle states
            for (std::vector<std::string>& to_states : combinationsOfStates(stac_rep_count)){
                val = {};
                n = 0;
                //start [q,
                to = std::get<1>(transition)+" ["+std::get<3>(transition) + ",";
                for (auto to_state_it = to_states.begin()+1;to_state_it != to_states.end();to_state_it++) {
                    //voeg het stacksymbool toe en de volgende state: X, p][p,
                    to += std::get<4>(transition)[n] + ","+(*to_state_it)+"] ["+(*to_state_it)+ ",";
                    n++;
                }
                //voeg de laatste state en stacksymbool toe
                to += std::get<4>(transition)[n]+ ","+to_states[0] + "]";
                val = {to};
                P["["+std::get<0>(transition) + ","+ std::get<2>(transition)+ "," + to_states[0] +"]"].emplace_back(val);
            }
        }
        // nul stacksymbolen bij de transitie is een speciaal geval
        else {
            val = {std::get<1>(transition)};
            P["["+std::get<0>(transition) + ","+ std::get<2>(transition) + ","+ std::get<3>(transition) +"]"].emplace_back(val);
        }


    }
    S = "S";
    return {V,T,P,S};
}

std::vector<std::vector<std::string>>
PDA::combinationsOfStates(int n_stacksymbols, std::vector<std::vector<std::string>> list_to_iterate ) const {
    std::vector<std::vector<std::string>> list {};
    //als er geen stacksymbolen meer moeten worden langsgegaan
    if (n_stacksymbols == 0){
        return list_to_iterate;
    }
    //loop over elke state en voeg dit toe aan de lijst met combinatie van alle pda states
    for (auto &state:this->States){
        if (list_to_iterate.empty()){
            list.emplace_back(std::vector<std::string> {state});
        }
        else {
            for (auto current : list_to_iterate){
                current.emplace_back(state);
                list.emplace_back(current);
            }
        }
    }
    //recursief totdat er geen stacksymbols meer over zijn, vanuit alle states die we al hebben
    list = combinationsOfStates(n_stacksymbols-1, list);
    return list;

}
