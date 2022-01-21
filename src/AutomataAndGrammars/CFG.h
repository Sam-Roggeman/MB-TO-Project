//
// Created by samro on 14/10/2021.
//
#include "json.hpp"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <vector>
using json = nlohmann::json;

#ifndef PROGRAMMEEROPDRACHT_CFG_H
#define PROGRAMMEEROPDRACHT_CFG_H

class CFG
{
private:
        std::vector<std::string> V;                                     // variables - non-terminals
        std::vector<std::string> T;                                     // terminals
        std::map<std::string, std::vector<std::vector<std::string>>> P; // productions

        /**
         * elimineert alle epsilon transities uit de cfg
         */
        void EliminateEpsilon();

        std::string S; // start symbol
        /**
         * Sorteert de CFG op alfanumerische volgorde
         */
        void sort();

public:
        /**
         * default constructor maakt een CFG
         * @post this->getV = {"BINDIGIT", "S"}
         * @post this->getT = {"0","1","a","b"}
         * @post this->getP =  {{"BINDIGIT",{"0","1"}},{"S",{"a S b BINDIGIT",""}}}
         * @post this->getS = "S"
         */
        CFG();

        /**
         * print de CFG af in std::cout
         */
        void print() const;

        /**
         * Importeerd een cfg die beschreven staat in json file
         * @param filepath filepath naar de json file
         */
        CFG(const std::string& filepath);

        /**
         * Constructor van een CFG waarvoor variabelen, terminalen, producties en startsymbool meegegeven wordt
         * @param V std::vector<variabele (std::string)>
         * @param T std::vector<terminaal (std::string)>
         * @param P std::map<from variabel (std:string), producties std::vector<productie std::vector <to
         * variabele/terminal>>>
         * @param S startstate std::string
         */
        CFG(std::vector<std::string> V, std::vector<std::string> T,
            std::map<std::string, std::vector<std::vector<std::string>>> P, std::string S);

        /**
         * Zet de cfg in cnf vorm en output de stappen naar std:cout
         */
        void toCNF();
        /**
         * counts het aantal producties in de cfg
         * @return int aantal_producties
         */
        unsigned int countProductions();

        /**
         *zoekt en elimineerd de unit pairs
         */
        void EliminateUnitPairs();

        /**
         * elimineerd de "useless" symbolen
         */
        void EliminateUselessSymbols();

        /**
         * verwijderd symbolen correct uit de cfg
         * @param to_remove lijst met symbolen die uit de cfg verwijderd moeten worden
         */
        void removeSymbols(std::vector<std::string>& to_remove);

        /**
         * zoekt de symbolen die genereren
         * @return vector met genererende symbolen
         */
        std::vector<std::string> findGeneratingSymbols();

        /**
         * zoekt de bereikbare symbolen
         * @return een vector met alle bereikbare symbolen
         */
        std::vector<std::string> findReachableSymbols();

        /**
         * replaces bad bodies waarbij variabelen en terminalen in een productie zitten
         */
        void replaceBadies();

        /**
         * break bodies die meer dan 2 variabelen hebben
         */
        void breakBodies();

        /**
         * checkt of de productie een unit production is
         * @param production de production in questy
         * @return true als unitproductie
         */
        bool isUnitproduction(const std::vector<std::string>& production);

        const std::map<std::string, std::vector<std::vector<std::string>>>& getP() const;

        const std::vector<std::string>& getV() const;

        const std::vector<std::string>& getT() const;

        const std::string& getS() const;
};

// enkele operators om output te vereenvoudigen
std::ostream& operator<<(std::ostream& out, const CFG& cfg);
std::ostream& operator<<(std::ostream& out, const std::vector<std::string>& v);
std::ostream& operator<<(std::ostream& out, const std::vector<std::vector<std::string>>& v);
std::string toString(std::vector<std::string> v);

#endif // PROGRAMMEEROPDRACHT_CFG_H
