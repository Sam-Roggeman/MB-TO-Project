//
// Created by Sam Roggeman on 29/10/2021.
//

#include <stack>
#include "CFG.h"
#include<tuple> // for tuple

#ifndef PROGRAMMEEROPDRACHT_PDA_H
#define PROGRAMMEEROPDRACHT_PDA_H


class PDA {
    std::vector<std::string> States {};
    std::vector<std::string> Alphabet {};
    std::vector<std::string> StackAlphabet {};
    //from,input,stacktop,to,replacement
    std::vector<std::tuple<std::string,std::string,std::string,std::string,std::vector<std::string>>> Transitions {};
    std::string StartState {};
    std::string StartStack {};
    std::stack<std::string> Stack {};
    std::string cur_state {};
public:
    std::vector<std::vector<std::string>> combinationsOfStates(int n_stacksymbols, std::vector<std::vector<std::string>> list_to_iterate= std::vector<std::vector<std::string>> {}) const;
    explicit PDA(const json &path);
    CFG toCFG() const;
};


#endif //PROGRAMMEEROPDRACHT_PDA_H
