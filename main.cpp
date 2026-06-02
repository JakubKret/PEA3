#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include "AtspGraph.h"
#include "TabuSearch.h"

// rozwiazania
std::map<std::string, int> optimalValues = {
    {"br17.atsp", 39}, {"ftv33.atsp", 1286}, {"ftv38.atsp", 1530},
    {"p43.atsp", 5620}, {"ry48p.atsp", 14422}, {"ftv55.atsp", 1608},
    {"ftv70.atsp", 1950}, {"kro124p.atsp", 36230}, {"ftv170.atsp", 2755},
    {"rbg323.atsp", 1326}, {"rbg403.atsp", 2465}
};

double calculateError(long long found, int optimal) {
    return ((double)(found - optimal) / optimal) * 100.0;
}

void runAutomatedTests() {
    std::vector<std::string> allFiles = {
        "br17.atsp", "ftv33.atsp", "ftv38.atsp", "p43.atsp", "ry48p.atsp",
        "ftv55.atsp", "ftv70.atsp", "kro124p.atsp", "rbg323.atsp", "rbg403.atsp", "ftv170.atsp"
    };

    std::cout << "\n>>> URUCHAMIANIE BADAŃ AUTOMATYCZNYCH... <<<\n";
    std::ofstream out("wyniki_kompletne.csv");
    out << "TestType;Plik;Kadencja;RozmiarListy;Koszt;Blad(%);Czas(s)\n";

    for (const auto& file : allFiles) {
        AtspGraph graph;
        if (!graph.loadFromFile(file)) continue;
        int opt = optimalValues[file];

        std::cout << "Analiza: " << file << "..." << std::endl;

        // Testy parametrow (Kadencja/Rozmiar)
        for(int ten : {2, 25, 50}) {
            for(int size : {5, 20, 100}) {
                TabuSearch ts(graph);
                ts.setTimeLimit(60.0);
                ts.setTabuTenure(ten);
                ts.setMaxTabuListSize(size);
                Solution res = ts.solve(ts.generateInitialSolutionRNN());
                out << "Parametry;" << file << ";" << ten << ";" << size << ";"
                    << res.totalCost << ";" << calculateError(res.totalCost, opt) << ";60.0\n";
            }
        }
        // Test Aspiracji
        TabuSearch tsA(graph);
        tsA.setTimeLimit(60.0);
        tsA.setAspiration(true);
        Solution resOn = tsA.solve(tsA.generateInitialSolutionRNN());
        out << "AspiracjaON;" << file << ";25;100;" << resOn.totalCost << ";" << calculateError(resOn.totalCost, opt) << ";60.0\n";
    }
    out.close();
    std::cout << "Badania zakonczone. Wyniki w 'wyniki_kompletne.csv'\n";
}

int main() {
    AtspGraph graph;
    TabuSearch* ts = nullptr;
    double timeLimit = 60.0;
    int tenure = 15;
    int listSize = 100;
    bool initialGenerated = false;
    Solution initialSol;

    int choice;
    do {
        std::cout << "\n--- MENU TS ---\n1. Wczytaj plik\n2. Czas stopu (aktualnie: " << timeLimit << "s)\n3. RNN (Rozwiazanie pocz.)\n4. Parametry (Tenure=" << tenure << ", Size=" << listSize << ")\n5. Start TS\n6. AUTO-TESTY\n0. Wyjdz\nWybor: ";
        std::cin >> choice;

        switch(choice) {
            case 1: {
                std::string f; std::cout << "Nazwa: "; std::cin >> f;
                if(graph.loadFromFile(f)) {
                    if(ts) delete ts;
                    ts = new TabuSearch(graph);
                    initialGenerated = false;
                }
                break;
            }
            case 2: std::cout << "Czas: "; std::cin >> timeLimit; if(ts) ts->setTimeLimit(timeLimit); break;
            case 3: initialSol = ts->generateInitialSolutionRNN(); initialGenerated = true; std::cout << "Koszt RNN: " << initialSol.totalCost << "\n"; break;
            case 4: std::cout << "Tenure: "; std::cin >> tenure; std::cout << "ListSize: "; std::cin >> listSize;
                    ts->setTabuTenure(tenure); ts->setMaxTabuListSize(listSize); break;
            case 5: {
                if(!initialGenerated) initialSol = ts->generateInitialSolutionRNN();
                Solution best = ts->solve(initialSol);
                std::cout << "Wynik: " << best.totalCost << "\nTrasa: ";
                for(int i : best.path) std::cout << i << " ";
                std::cout << "\n";
                break;
            }
            case 6: runAutomatedTests(); break;
        }
    } while(choice != 0);

    if(ts) delete ts;
    return 0;
}