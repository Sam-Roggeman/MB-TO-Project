//
// Created by Sam Roggeman on 19/12/2021.
//

#ifndef PROGRAMMEEROPDRACHT_SLR_H
#define PROGRAMMEEROPDRACHT_SLR_H


#include "CFG.h"
#include <iterator>     // std::next
#include <algorithm>
#include <vector>
#include <set>

class state;

struct production {
    std::string from;
    std::vector<std::string> to;

    void print(std::ostream &ostream) const;

    void augment() ;
    std::string getStringAfterDot() const;

    bool operator<(const production&);
    void shiftDot();
    bool operator==(const production& that) const;
    bool operator!=(const production& that) const;
    friend std::ostream& operator<<(std::ostream& os, const production& production);
};

class edge {
    std::string input;
    state* from;
    state* to;
public:
    edge(state *from, state *to, const std::string input);
    std::ostream friend &operator<<(std::ostream &os, const edge &edge) ;
};

class state : public std::vector<production>{
    std::string name;
    std::vector<edge> edges;
public:
    explicit state(std::string _name);
    state(const std::string& _name, const std::vector<production>& that);
    void addEdge(state *to, const std::string& input);
    const std::string &getName() const;

    void setName(const std::string &name);
    bool operator==(const state& that) const;
    friend std::ostream& operator<<(std::ostream& os, const state& state);
    std::vector<std::string> getAllStringsAfterDot() const;

    state();
};

class SLR {
    std::vector<std::string> V; // variables - non-terminals
    std::vector<std::string> T; // terminals
    std::vector<production> P; // base_productions
    std::vector<production> aug_prods;
    std::map<std::string,state> states; //states


public:
explicit SLR(const CFG & cfg);

    void print(std::ostream &ostream) const;

    std::vector<production> closure(std::vector<production> &included, std::set<std::string> &found, int current_index);

private:
    std::vector<production> closure(std::vector<production> &included);
    bool isVar(const std::string& basicString);

    void initializeStates();

    state initializeStartState();
    void sort();

    /**
     * creates and initializes a new state which
     * @param from
     * @param input
     * @return
     */
    state createState(const state& from, const std::string& input);

    /**
     * prints the slr
     * @param os outstream
     * @param slr the slr to output
     * @return
     */
    friend std::ostream& operator<<(std::ostream& os, const SLR& slr);

    /**
     * checks if an equal state already exists (looks at productions and not name)
     * @param state1 state to compare
     * @return a pointer to the equal state if exists, else nullptr
     */
    const state * getEqualState(const state &state1);
};


#endif //PROGRAMMEEROPDRACHT_SLR_H
