//
// Created by Sam Roggeman on 19/12/2021.
//
#include "SLR.h"

#include <utility>

void production::print(std::ostream &ostream) const {
    ostream << from << "--> ";
    for (const std::string& c : to){
        std::cout << c <<' ';
    }
    std::cout << std::endl;
}

void production::augment(){
    to.insert(to.begin(),"•");
}

std::string production::getStringAfterDot() const {
    auto iterator = std::find(to.begin(),to.end(),"•");
    iterator++;
    if (iterator == to.end()) return "";
    else return *iterator;

}

void production::shiftDot() {
    auto iterator = std::find(to.begin(),to.end(),"•");
    std::iter_swap(iterator,std::next(iterator,1));
}

bool production::operator<(const production & p) {
    if (this->from < p.from){
        return true;
    }
    else if (this->from > p.from){
        return false;
    }
    std::vector<std::string>::const_iterator p_it = p.to.begin();
    std::vector<std::string>::const_iterator t_it = this->to.begin();
    while(p_it != p.to.end() && t_it != this->to.end()) {
        if (*t_it < *p_it) return true;
        else if (*t_it > *p_it) return false;
        t_it++;
        p_it++;
    }
    if (t_it == this->to.end()) return true;
    else return false;
}

bool production::operator==(const production &that) const {
    if (this->to.size() != that.to.size()){
        return false;
    }

    std::vector<std::string>::const_iterator p_it = that.to.begin();
    std::vector<std::string>::const_iterator t_it = this->to.begin();
    while(p_it != that.to.end() && t_it != this->to.end()) {
        if (*t_it != *p_it) return false;
        t_it++;
        p_it++;
    }
    return true;
}

bool production::operator!=(const production &that) const {
    return !(*this==that);
}

std::ostream &operator<<(std::ostream &os, const production &production) {
    production.print(os);
    return os;
}



std::ostream &operator<<(std::ostream &os, const edge &edge) {
    os<< edge.input << ": " << edge.to->getName() <<std::endl;
    return os;
}

edge::edge(state *from, state *to, const std::string input) : from(from), to(to), input(input) {}



state::state(std::string _name) : name(std::move(_name)){

}

const std::string &state::getName() const {
    return name;
}

void state::setName(const std::string &name) {
    state::name = name;
}

std::vector<std::string> state::getAllStringsAfterDot() const {
    std::set<std::string> s{};
    std::string _s {};
    for (const auto& prod:*this){
        _s = prod.getStringAfterDot();
        if (_s != "") {
            s.insert(_s);
        }
    }
    std::vector<std::string> v(s.size());
    std::copy(s.begin(),s.end(),v.begin());
    return v;
}

state::state(const std::string& _name, const std::vector<production> &that) {
    setName(_name);
    this->insert(this->begin(),that.begin(),that.end());
}

void state::addEdge(state *to, const std::string &input) {
    edges.emplace_back(edge(this,to,input));
}

bool state::operator==(const state& that) const {

    if (!that.size() == this->size()){
        return false;
    }
    auto this_it = this->begin();
    auto that_it = that.begin();
    while (this_it != this->end() && that_it != that.end()){
        if ((*this_it) != (*that_it)){
            return false;
        }
        this_it++;
        that_it++;
    }
    return true;
}

state::state() =default;

std::ostream &operator<<(std::ostream &os, const state &state) {
    os << "State " <<  state.name << ":" << std::endl;
    for (const auto& production:state){
        os<<'\t' << production;
    }
    if (state.edges.size() > 0)
        os << "Edges :" << std::endl;
    for (const auto& edge:state.edges){
        os <<'\t' <<edge;
    }
    os <<  std::endl;
    return os;
}



SLR::SLR(const CFG &G) {
    T = G.getT();
    V = G.getV();
    production aug_prod {};

    for (auto & productionlist:G.getP()){
        production slr_production;
        for (const std::vector<std::string>& production:productionlist.second){
            slr_production = {};
            slr_production.from = productionlist.first;
            slr_production.to = production;
            P.emplace_back(slr_production);
        }
    }

    //Add Augment Production and insert '•' symbol at the first position for every production in G
    for (const production& slr_production:P){
        aug_prod = slr_production;
        aug_prod.augment();
        aug_prods.emplace_back(aug_prod);
    }
    initializeStates();

}

void SLR::print(std::ostream &ostream) const {
    std::cout << "Base productions: " <<std::endl;
    for (const auto & prod:P){
        std::cout << '\t'<<prod;
    }
    std::cout << std::endl <<"Augmented productions: " <<std::endl;
    for (const auto & aug_prod:aug_prods){
        std::cout << '\t'<<aug_prod;
    }
    std::cout << std::endl;
    for (const auto & [from,s]:states){
        std::cout <<s;
    }
}

std::vector<production> SLR::closure(std::vector<production>& included , std::set<std::string>& found, int current_index) {
    int size = included.size();
    if (current_index<size){
        production current = included.at(current_index);
        std::string s_after_dot = current.getStringAfterDot();
        bool not_found = found.find(s_after_dot) == found.end();
        bool var = isVar(s_after_dot);
        if (not_found&& var){
            found.insert(s_after_dot);
            for (const auto& production:aug_prods){
                if (production.from == s_after_dot)
                    {
                        included.emplace_back(production);
                    }
                }
        }
        current_index++;
        return closure(included,found,current_index);
    }
    else return included;

}

bool SLR::isVar(const std::string& basicString) {
    return (std::find(V.begin(), V.end(), basicString) != V.end());
}

void SLR::initializeStates() {
    state from {};
    initializeStartState();
    std::vector<state> todo {states.at("i0")};
    int index = 0;
    state to {};
    const state *poss {};
    unsigned int state_counter = 1;

    while (index < todo.size()){
        from = todo.at(index);
        std::vector<std::string> all_inputs = from.getAllStringsAfterDot();
        to = {};
        for (const std::string& input:all_inputs){
            to = createState(from,input);
            poss = getEqualState(to);
            if (poss != nullptr){
                to = from;
            }
            else {
                to.setName("i"+std::to_string(state_counter));
                states[to.getName()] = to;
                state_counter++;
                todo.emplace_back(to);
            }
            states.at(from.getName()).addEdge(&states.at(to.getName()),input);
        }
        index++;
    }
}

state SLR::initializeStartState() {
    production s_production {};
    for (const production & production: aug_prods){
        if (production.from == "S"){
            s_production = production;
            break;
        }
    }
    std::vector<production> clo = {s_production};
    //State0
    state i{"i0",closure(clo)};
    states[i.getName()] = i;
    return i;
}

state SLR::createState(const state& from, const std::string& input) {
    state to {};
    production to_pro {};
    for (const production& fr_pro :from){
        if (fr_pro.getStringAfterDot() == input){
            to_pro = fr_pro;
            to_pro.shiftDot();
            to.emplace_back(to_pro);
        }
    }
    std::vector<production> a = closure(to);

    return to;
}

std::vector<production> SLR::closure(std::vector<production> &included) {
    std::set<std::string> _temp {};
    return closure(included,_temp,0);
}

std::ostream &operator<<(std::ostream &os, const SLR &slr) {
    slr.print(os);
    return os;
}

void SLR::sort() {
    for (auto& [from,s]:states) {
        std::sort(s.begin(), s.end());
    }
}

const state * SLR::getEqualState(const state &state1) {
    for (const auto& [from,state]:states){
        if (state1 == state){
            return &state;
        }
    }
    return nullptr;
}
