//
// Created by Sam Roggeman on 19/12/2021.
//

#ifndef PROGRAMMEEROPDRACHT_SLR_H
#define PROGRAMMEEROPDRACHT_SLR_H

#include "CFG.h"
#include <algorithm>
#include <iterator> // std::next
#include <set>
#include <vector>

class state;

struct production
{
        std::string from;
        std::vector<std::string> to;

        void print(std::ostream& ostream) const;

        void augment();
        std::string getStringAfterDot() const;

        bool operator<(const production&);
        void shiftDot();
        bool operator==(const production& that) const;
        bool operator!=(const production& that) const;
        friend std::ostream& operator<<(std::ostream& os, const production& production);
        void toDot(std::ostream& os) const;
};

class edge
{
        std::string input;
        state* from;
        state* to;

public:
        edge(state* from, state* to, const std::string input);
        std::ostream friend& operator<<(std::ostream& os, const edge& edge);
        std::string GetInput() { return input; }
        state* GetTo() { return to; }
        void toDot(std::ostream& ostream) const;
};

class state : public std::vector<production>
{
        std::string name;
        std::vector<edge> edges;

public:
        explicit state(std::string _name);
        state(const std::string& _name, const std::vector<production>& that);
        void addEdge(state* to, const std::string& input);
        const std::string& getName() const;

        void setName(const std::string& name);
        bool operator==(const state& that) const;
        friend std::ostream& operator<<(std::ostream& os, const state& state);
        std::vector<std::string> getAllStringsAfterDot() const;
        std::vector<edge> GetEdge() const { return edges; }
        void toDot(std::ostream& os) const;

        state();
};
class Row : public std::vector<std::string>
{
public:
        Row();
};
class Table
{
        std::vector<std::pair<std::string, Row>> GoTo;
        std::vector<std::pair<std::string, Row>> Action;

public:
        Table()
        {
                GoTo = {};
                Action = {};
        }

        void SetGoTo(const std::vector<std::pair<std::string, Row>>& go_to);
        void SetAction(const std::vector<std::pair<std::string, Row>>& action);

        std::vector<std::pair<std::string, Row>> GetAction();
        std::vector<std::pair<std::string, Row>> GetGoTo();

        void AddAction(const std::pair<std::string, Row>& pair);
        void AddGoTo(const std::pair<std::string, Row>& pair);
};
class SLR
{
        std::vector<std::string> V;          // variables - non-terminals
        std::vector<std::string> T;          // terminals
        std::vector<production> P;           // base_productions
        std::vector<production> aug_prods;   // Augmented_productions
        std::map<std::string, state> states; // states
        std::map<std::string, std::vector<std::string>> follow = {};
        Table table = Table();

public:
        explicit SLR(const CFG& cfg);

        void print(std::ostream& ostream) const;

        void TableFilling();

        void PrintTable();

        void toDot(std::ostream& os);

private:
        std::vector<production> closure(std::vector<production>& included, std::set<std::string>& found,
                                        int current_index);
        std::vector<production> closure(std::vector<production>& included);

        bool isVar(const std::string& basicString);

        void initializeStates();

        state initializeStartState();

        void SetTable(const Table& t);

        void Follows();

        void ProductionLoop(std::basic_string<char> variable,
                            std::pair<const std::basic_string<char>, std::vector<std::basic_string<char>>> check,
                            std::vector<std::string>& list);

        void ActionCheck(Row& row, std::pair<std::string, Row>& actions, std::basic_string<char> statename, state& s);

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
        const state* getEqualState(const state& state1);
};

#endif // PROGRAMMEEROPDRACHT_SLR_H
