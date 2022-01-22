#include "SLR.h"

#include <utility>

void production::print(std::ostream& ostream) const
{
        ostream << from << "--> ";
        for (const std::string& c : to) {
                std::cout << c << ' ';
        }
        std::cout << std::endl;
}

void production::toDot(std::ostream& os) const
{
        os << from << " --> ";
        for (const std::string& c : to) {
                std::cout << c << ' ';
        }
}

void production::augment() { to.insert(to.begin(), "•"); }

std::string production::getStringAfterDot() const
{
        auto iterator = std::find(to.begin(), to.end(), "•");
        iterator++;
        if (iterator == to.end())
                return "";
        else
                return *iterator;
}

void production::shiftDot()
{
        auto iterator = std::find(to.begin(), to.end(), "•");
        std::iter_swap(iterator, std::next(iterator, 1));
}

bool production::operator<(const production& p)
{
        if (this->from < p.from) {
                return true;
        } else if (this->from > p.from) {
                return false;
        }
        std::vector<std::string>::const_iterator p_it = p.to.begin();
        std::vector<std::string>::const_iterator t_it = this->to.begin();
        while (p_it != p.to.end() && t_it != this->to.end()) {
                if (*t_it < *p_it)
                        return true;
                else if (*t_it > *p_it)
                        return false;
                t_it++;
                p_it++;
        }
        if (t_it == this->to.end())
                return true;
        else
                return false;
}

bool production::operator==(const production& that) const
{
        if (this->to.size() != that.to.size()) {
                return false;
        }

        std::vector<std::string>::const_iterator p_it = that.to.begin();
        std::vector<std::string>::const_iterator t_it = this->to.begin();
        while (p_it != that.to.end() && t_it != this->to.end()) {
                if (*t_it != *p_it)
                        return false;
                t_it++;
                p_it++;
        }
        return true;
}

bool production::operator!=(const production& that) const { return !(*this == that); }
std::ostream& operator<<(std::ostream& os, const production& production)
{
        production.print(os);
        return os;
}

std::ostream& operator<<(std::ostream& os, const edge& edge)
{
        os << edge.input << ": " << edge.to->getName() << std::endl;
        return os;
}

edge::edge(state* from, state* to, const std::string input) : from(from), to(to), input(input) {}
void edge::toDot(std::ostream& os) const
{
        os << this->from->getName() << "->" << this->to->getName() << "[label="
           << "\"" << input << "\"]";
}

state::state(std::string _name) : name(std::move(_name)) {}

const std::string& state::getName() const { return name; }

void state::setName(const std::string& name) { state::name = name; }

std::vector<std::string> state::getAllStringsAfterDot() const
{
        std::set<std::string> s{};
        std::string _s{};
        for (const auto& prod : *this) {
                _s = prod.getStringAfterDot();
                if (_s != "") {
                        s.insert(_s);
                }
        }
        std::vector<std::string> v(s.size());
        std::copy(s.begin(), s.end(), v.begin());
        return v;
}

state::state(const std::string& _name, const std::vector<production>& that)
{
        setName(_name);
        this->insert(this->begin(), that.begin(), that.end());
}

void state::addEdge(state* to, const std::string& input) { edges.emplace_back(edge(this, to, input)); }

bool state::operator==(const state& that) const
{

        if (!that.size() == this->size()) {
                return false;
        }
        auto this_it = this->begin();
        auto that_it = that.begin();
        while (this_it != this->end() && that_it != that.end()) {
                if ((*this_it) != (*that_it)) {
                        return false;
                }
                this_it++;
                that_it++;
        }
        return true;
}

state::state() = default;

std::ostream& operator<<(std::ostream& os, const state& state)
{
        os << "State " << state.name << ":" << std::endl;
        for (const auto& production : state) {
                os << '\t' << production;
        }
        if (state.edges.size() > 0)
                os << "Edges :" << std::endl;
        for (const auto& edge : state.edges) {
                os << '\t' << edge;
        }
        os << std::endl;
        return os;
}
void state::toDot(std::ostream& os) const
{
        bool first = true;
        os << name << "[label = \"";
        for (const auto& prod : *this) {
                if (!first) {
                        os << "\\n";
                }
                prod.toDot(os);
                first = false;
        }
        os << "\","
           << "shape = box, xlabel=" << name << "];" << std::endl;
        for (const auto& edge : edges) {
                edge.toDot(os);
                os << std::endl;
        }
}

SLR::SLR(const CFG& G)
{
        T = G.getT();
        V = G.getV();
        production aug_prod{};

        for (auto& productionlist : G.getP()) {
                production slr_production;
                for (const std::vector<std::string>& production : productionlist.second) {
                        slr_production = {};
                        slr_production.from = productionlist.first;
                        slr_production.to = production;
                        P.emplace_back(slr_production);
                }
        }

        // Add Augment Production and insert '•' symbol at the first position for every production in G
        for (const production& slr_production : P) {
                aug_prod = slr_production;
                aug_prod.augment();
                aug_prods.emplace_back(aug_prod);
        }
        initializeStates();
        TableFilling();
}

void SLR::print(std::ostream& ostream) const
{
        std::cout << "Base productions: " << std::endl;
        for (const auto& prod : P) {
                std::cout << '\t' << prod;
        }
        std::cout << std::endl << "Augmented productions: " << std::endl;
        for (const auto& aug_prod : aug_prods) {
                std::cout << '\t' << aug_prod;
        }
        std::cout << std::endl;
        for (const auto& [from, s] : states) {
                std::cout << s;
        }
}

std::vector<production> SLR::closure(std::vector<production>& included, std::set<std::string>& found, int current_index)
{
        int size = included.size();
        if (current_index < size) {
                production current = included.at(current_index);
                std::string s_after_dot = current.getStringAfterDot();
                bool not_found = found.find(s_after_dot) == found.end();
                bool var = isVar(s_after_dot);
                if (not_found && var) {
                        found.insert(s_after_dot);
                        for (const auto& production : aug_prods) {
                                if (production.from == s_after_dot) {
                                        included.emplace_back(production);
                                }
                        }
                }
                current_index++;
                return closure(included, found, current_index);
        } else
                return included;
}

bool SLR::isVar(const std::string& basicString) { return (std::find(V.begin(), V.end(), basicString) != V.end()); }

void SLR::initializeStates()
{
        state from{};
        initializeStartState();
        std::vector<state> todo{states.at("i0")};
        int index = 0;
        state to{};
        const state* poss{};
        unsigned int state_counter = 1;

        while (index < todo.size()) {
                from = todo.at(index);
                std::vector<std::string> all_inputs = from.getAllStringsAfterDot();
                to = {};
                for (const std::string& input : all_inputs) {
                        to = createState(from, input);
                        poss = getEqualState(to);
                        if (poss != nullptr) {
                                to = *poss;
                        } else {
                                to.setName("i" + std::to_string(state_counter));
                                states[to.getName()] = to;
                                state_counter++;
                                todo.emplace_back(to);
                        }
                        states.at(from.getName()).addEdge(&states.at(to.getName()), input);
                }
                index++;
        }
}

state SLR::initializeStartState()
{

        std::vector<production> clo{};
        for (const production& production : aug_prods) {
                if (production.from == "S") {
                        clo.push_back(production);
                }
        }

        // State0
        state i{"i0", closure(clo)};
        states[i.getName()] = i;
        return i;
}

state SLR::createState(const state& from, const std::string& input)
{
        state to{};
        production to_pro{};
        for (const production& fr_pro : from) {
                if (fr_pro.getStringAfterDot() == input) {
                        to_pro = fr_pro;
                        to_pro.shiftDot();
                        to.emplace_back(to_pro);
                }
        }
        std::vector<production> a = closure(to);

        return to;
}

std::vector<production> SLR::closure(std::vector<production>& included)
{
        std::set<std::string> _temp{};
        return closure(included, _temp, 0);
}

std::ostream& operator<<(std::ostream& os, const SLR& slr)
{
        slr.print(os);
        return os;
}

void SLR::sort()
{
        for (auto& [from, s] : states) {
                std::sort(s.begin(), s.end());
        }
}

const state* SLR::getEqualState(const state& state1)
{
        for (const auto& [from, state] : states) {
                if (state1 == state) {
                        return &state;
                }
        }
        return nullptr;
}

void SLR::TableFilling()
{
        Table t = Table();

        for (const auto& variable : V) {
                if (variable != "S") {
                        std::pair<std::string, Row> pair = std::make_pair(variable, Row());
                        pair.second = {};
                        t.AddGoTo(pair);
                }
        }

        for (const auto& terminals : T) {
                std::pair<std::string, Row> pair = std::make_pair(terminals, Row());
                pair.second = {};
                t.AddAction(pair);
        }
        std::pair<std::string, Row> pair = std::make_pair("$", Row());
        pair.second = {};
        t.AddAction(pair);

        Follows();
        std::vector<std::pair<std::string, Row>> Go = {};
        for (auto& Goto : t.GetGoTo()) {
                Row list = {};

                for (const auto& state : this->states) {
                        bool input = false;

                        for (auto& edge : state.second.GetEdge()) {
                                if (edge.GetInput() == Goto.first) {
                                        input = true;
                                        list.push_back(edge.GetTo()->getName());
                                        break;
                                }
                        }
                        if (!input) {
                                list.emplace_back("");
                        }
                }

                std::pair<std::string, Row> vec = Goto;
                vec.second = list;
                Go.push_back(vec);
        }

        t.SetGoTo(Go);

        std::vector<std::pair<std::string, Row>> act = {};
        for (auto& actions : t.GetAction()) {
                Row list = {};
                for (auto& s : this->states) {
                        ActionCheck(list, actions, s.first, s.second);
                }
                std::pair<std::string, Row> vec = actions;
                vec.second = list;
                act.push_back(vec);
        }

        t.SetAction(act);

        SetTable(t);
}
void SLR::SetTable(const Table& t) { table = t; }

void SLR::Follows()
{

        std::vector<std::string> Slist = {};
        for (auto& produc : P) {
                if (std::find(produc.to.begin(), produc.to.end(), "S") != produc.to.end() && produc.to.size() == 1 &&
                    std::find(Slist.begin(), Slist.end(), "$") == Slist.end()) {
                        Slist.emplace_back("$");
                }

                else if (std::find(produc.to.begin(), produc.to.end(), "S") != produc.to.end() &&
                         produc.to.size() > 1 && produc.to[produc.to.size() - 1] != "S") {
                        auto S = std::find(produc.to.begin(), produc.to.end(), "S");
                        S += 1;
                        if (std::find(Slist.begin(), Slist.end(), *S) == Slist.end()) {
                                Slist.push_back(*S);
                        }
                }
        }

        if (Slist.empty()) {
                Slist.emplace_back("$");
        }

        follow["S"] = Slist;

        for (auto& variables : V) {
                std::vector<std::string> list = {};
                for (auto& F : follow) {
                        if (variables != "S") {

                                ProductionLoop(variables, F, list);
                        }
                }
                if (variables != "S") {
                        auto pair = std::make_pair(variables, list);
                        follow[variables] = list;
                }
        }

        for (auto& var : follow) {
                std::vector<std::string> list = var.second;
                for (auto& F : follow) {
                        if (var.first != "S") {
                                ProductionLoop(var.first, F, list);
                        }
                }
                if (var.first != "S") {
                        var.second = list;
                }
        }
}
void SLR::ProductionLoop(std::basic_string<char> variable,
                         std::pair<const std::basic_string<char>, std::vector<std::basic_string<char>>> check,
                         std::vector<std::string>& list)
{
        for (auto& produc : P) {

                if (produc.from == check.first && produc.to.size() == 1 && produc.to[0] == variable) {
                        for (auto& string : check.second) {
                                if (std::find(list.begin(), list.end(), string) == list.end()) {
                                        list.push_back(string);
                                }
                        }
                }

                else if (produc.to.size() > 1 &&
                         std::find(produc.to.begin(), produc.to.end(), variable) != produc.to.end()) {
                        auto it = std::find(produc.to.begin(), produc.to.end(), variable);

                        if ((it + 1) != produc.to.end() && std::find(V.begin(), V.end(), *(it + 1)) == V.end() &&
                            std::find(list.begin(), list.end(), *(it + 1)) == list.end()) {
                                it += 1;
                                list.push_back(*it);
                        }

                        else if (std::find(list.begin(), list.end(), "$") == list.end()) {
                                list.emplace_back("$");
                        }
                }

                if (produc.from == check.first && produc.to.size() == 1 && produc.to[0] == variable) {
                        for (auto l : check.second) {
                                if (std::find(list.begin(), list.end(), l) == list.end()) {
                                        list.push_back(l);
                                }
                        }
                }
        }
}
void SLR::ActionCheck(Row& row, std::pair<std::string, Row>& actions, std::basic_string<char> statename, state& s)
{
        bool input = false;
        for (auto ed : s.GetEdge()) {
                if (ed.GetInput() == actions.first) {
                        std::string r = "S";
                        r += ed.GetTo()->getName().back();
                        row.push_back(r);
                        input = true;
                        break;
                }
        }

        if (!input) {
                std::pair<std::string, std::vector<std::string>> prod;
                bool dot = false;
                for (int i = 0; i < s.size(); i++) {
                        if (s[i].to.back() == "•" && std::find(follow[s[i].from].begin(), follow[s[i].from].end(),
                                                               actions.first) != follow[s[i].from].end()) {
                                auto a = s[i].to;
                                a.pop_back();
                                prod = std::make_pair(s[i].from, a);
                                dot = true;
                                break;
                        }
                }

                if(dot && s[0].from == "S" && s[0].to.back() =="•"){
                        row.push_back("A");
                }

                if (!dot) {
                        row.push_back("");
                }

                else {
                        for (int i = 0; i < P.size(); i++) {
                                if (P[i].from == prod.first && P[i].to == prod.second &&
                                    std::find(follow[P[i].from].begin(), follow[P[i].from].end(), actions.first) !=
                                        follow[P[i].from].end()) {
                                        std::string r = "R";
                                        r += std::to_string(i);
                                        row.push_back(r);
                                        break;
                                }
                        }
                }
        }
}
void SLR::PrintTable()
{
        std::cout << "  |";
        for (auto& action : table.GetAction()) {
                std::cout << action.first;
                if (action.first == "id") {
                        std::cout << "| ";
                }

                else {
                        std::cout << " | ";
                }
        }

        for (auto& Goto : table.GetGoTo()) {
                std::cout << Goto.first;
                if (Goto != table.GetGoTo().back()) {
                        std::cout << " | ";
                }
        }

        std::cout << std::endl;
        for (int i = 0; i < states.size(); i++) {
                std::cout << i << " |";
                for (auto& action : table.GetAction()) {
                        std::cout << action.second[i];
                        if (action.second[i][0] == 'R' || action.second[i][0] == 'S') {
                                std::cout << "| ";
                        }

                        else if (action.second[i] == "") {
                                std::cout << "  | ";
                        }
                }
                for (auto& Goto : table.GetGoTo()) {
                        std::cout << Goto.second[i];
                        if (Goto.second[i][0] == 'i') {
                                std::cout << "| ";
                        }

                        else if (Goto.second[i] == "") {
                                std::cout << "  | ";
                        }

                        else {
                                std::cout << " | ";
                        }
                }
                std::cout << std::endl;
        }
}
void SLR::toDot(std::ostream& os)
{
        os << "digraph SLR{" << std::endl;
        os << "forcelabels=true;" << std::endl;
        for (const auto& state : this->states) {
                state.second.toDot(os);
        }

        os << "}" << std::endl;
}

bool SLR::ValidCheck( std::string& in,  std::string& head) {
        std::string input = in;
        std::string s = "i0";

        state * staat = &states[s];

        if(head == "kleur"){
                for(auto& st:staat->GetEdge()){
                        if(st.GetInput() == "l"){
                                staat = st.GetTo();
                                break;
                        }
                }
        }

        else{
                for(auto& st:staat->GetEdge()){
                        if(st.GetInput() == "n"){
                                staat = st.GetTo();
                                break;
                        }
                }
        }

        bool end = false;

        for(auto& letter:in){
                std::string l;
                l += letter;
                bool valid = false;
                for(auto & st:staat->GetEdge()){
                        if(st.GetInput() == l){
                                staat = st.GetTo();
                                valid = true;
                                break;
                        }
                }
                if(!valid){
                        std::cerr << "Input not valid" << std::endl;
                        return false;
                }

                if (l == "<"){
                        end = true;
                }
        }

        if(end && in.back() != '<'){
                std::cerr << "The input is not valid" << std::endl;
                return false;
        }

        else{
                return true;
        }
}

void SLR::ParseXML(std::vector<std::pair<std::string,std::string>>& values, const std::string& filename) {

        std::ifstream infile(filename);
        std::string line;

        while(std::getline(infile,line)){

                if (line == "<Car>" || line == "</Car>"){
                        continue;
                }

                else{
                        std::string value;
                        std::string head;

                        bool yes = false;
                        bool heads = false;

                        for(int i = 0; i < line.size(); i++){
                                if(line[i] == '<' && !yes){
                                        heads = true;
                                }
                                else if(line[i] == '<' && yes){
                                        value += line[i];
                                        break;
                                }
                                else if(line[i] == '>'){
                                        yes = true;
                                        heads = false;
                                }
                                else if(heads){
                                        head += line[i];
                                }
                                else if(yes && line[i] != ' '){
                                        value +=  line[i];
                                }
                        }
                        auto pair = std::make_pair(head,value);
                        values.push_back(pair);
                }
        }

        for(auto& pair:values){
                if(pair.first == "kleur" && pair.second.size() != 7){
                        std::cerr << "More then 6 numbers/letters for color" << std::endl;
                }
        }

        for(auto& pair:values){
                if(!ValidCheck(pair.second,pair.first)){
                        values = {};
                        return;
                }
                pair.second.pop_back();
        }
}

Row::Row() {}

void Table::SetGoTo(const std::vector<std::pair<std::string, Row>>& go_to) { GoTo = go_to; }

void Table::SetAction(const std::vector<std::pair<std::string, Row>>& action) { Action = action; }

std::vector<std::pair<std::string, Row>> Table::GetAction() { return Action; }

std::vector<std::pair<std::string, Row>> Table::GetGoTo() { return GoTo; }

void Table::AddAction(const std::pair<std::string, Row>& pair) { Action.push_back(pair); }

void Table::AddGoTo(const std::pair<std::string, Row>& pair) { GoTo.push_back(pair); }
