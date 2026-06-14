#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include "AtspGraph.h"
#include "GeneticAlgorithm.h"

std::map<std::string, int> optimalValues = {
    {"br17.atsp", 39}, {"ftv33.atsp", 1286}, {"ftv38.atsp", 1530},
    {"p43.atsp", 5620}, {"ry48p.atsp", 14422}, {"ftv55.atsp", 1608},
    {"ftv70.atsp", 1950}, {"kro124p.atsp", 36230}, {"ftv170.atsp", 2755},
    {"rbg323.atsp", 1326}, {"rbg403.atsp", 2465}
};

double calculateError(long long found, int optimal) {
    return ((double)(found - optimal) / optimal) * 100.0;
}

// =====================================================================
// KOMBAJN BADAWCZY - GENERATOR SPRAWOZDANIA
// =====================================================================
void runAutomatedTests() {
    std::vector<std::string> allFiles = {
        "br17.atsp", "ftv33.atsp", "ftv38.atsp", "p43.atsp", "ry48p.atsp",
        "ftv55.atsp", "ftv70.atsp", "kro124p.atsp", "rbg323.atsp", "rbg403.atsp", "ftv170.atsp"
    };

    double limitOthers = 60.0; // 1 min na iterację dla testow 3.5, 4.0, 4.5, 5.0
    double limitRozmiar = 900.0; // 15 min dla testu rozmiaru (3.0)

    std::cout << "\n>>> START AUTOMATYCZNYCH BADAN <<<\n";

    // 1. Ocena 3.0 - Wpływ rozmiaru (15 min)
    std::cout << "[1/5] Test 3.0: Zaleznosc czasu/bledu od rozmiaru (LIMIT = 15min)...\n";
    std::ofstream out1("wyniki_3_0_rozmiar.csv");
    out1 << "Plik;N;Optimum;KosztZnaleziony;SredniaPop;Blad(%);CzasZnalezieniaRekordu(s)\n";
    for (const auto& file : allFiles) {
        AtspGraph g; if (!g.loadFromFile(file)) continue;
        GeneticAlgorithm ga(g);
        ga.setParameters(limitRozmiar, 100, 0.05, 0.8, 1, 1, 2); // Turniej, OX, Inwersja
        std::cout << " -> " << file << "... " << std::flush;
        GeneticResult res = ga.solve();
        int opt = optimalValues[file];
        out1 << file << ";" << g.getDimension() << ";" << opt << ";" << res.bestCost << ";"
             << res.averageLastPopulationCost << ";" << calculateError(res.bestCost, opt) << ";" << res.timeToFindBest << "\n";
        std::cout << "OK (" << calculateError(res.bestCost, opt) << "%)\n";
    }
    out1.close();

    // 2. Ocena 3.5 - Wpływ populacji
    std::cout << "\n[2/5] Test 3.5: Wplyw rozmiaru populacji...\n";
    std::ofstream out2("wyniki_3_5_populacja.csv");
    out2 << "Plik;RozmiarPopulacji;KosztZnaleziony;Blad(%);CzasZnalezieniaRekordu(s)\n";
    std::vector<int> popSizes = {10, 50, 100, 200};
    for (const auto& file : allFiles) {
        AtspGraph g; if (!g.loadFromFile(file)) continue;
        int opt = optimalValues[file];
        for (int pop : popSizes) {
            GeneticAlgorithm ga(g);
            ga.setParameters(limitOthers, pop, 0.05, 0.8, 1, 1, 2);
            GeneticResult res = ga.solve();
            out2 << file << ";" << pop << ";" << res.bestCost << ";" << calculateError(res.bestCost, opt) << ";" << res.timeToFindBest << "\n";
        }
        std::cout << " -> " << file << " ukonczono.\n";
    }
    out2.close();

    // 3. Ocena 4.0 - Wpływ metody mutacji (Swap vs Inwersja)
    std::cout << "\n[3/5] Test 4.0: Wplyw metody mutacji...\n";
    std::ofstream out3("wyniki_4_0_mutacja.csv");
    out3 << "Plik;MetodaMutacji;KosztZnaleziony;Blad(%);CzasZnalezieniaRekordu(s)\n";
    for (const auto& file : allFiles) {
        AtspGraph g; if (!g.loadFromFile(file)) continue;
        int opt = optimalValues[file];
        for (int mut = 1; mut <= 2; ++mut) {
            GeneticAlgorithm ga(g);
            ga.setParameters(limitOthers, 100, 0.05, 0.8, 1, 1, mut);
            GeneticResult res = ga.solve();
            out3 << file << ";" << (mut==1 ? "Swap" : "Inwersja") << ";" << res.bestCost << ";" << calculateError(res.bestCost, opt) << ";" << res.timeToFindBest << "\n";
        }
    }
    out3.close();

    // 4. Ocena 4.5 - Wpływ metody krzyżowania (OX vs PMX)
    std::cout << "\n[4/5] Test 4.5: Wplyw metody krzyzowania...\n";
    std::ofstream out4("wyniki_4_5_krzyzowanie.csv");
    out4 << "Plik;MetodaKrzyzowania;KosztZnaleziony;Blad(%);CzasZnalezieniaRekordu(s)\n";
    for (const auto& file : allFiles) {
        AtspGraph g; if (!g.loadFromFile(file)) continue;
        int opt = optimalValues[file];
        for (int cross = 1; cross <= 2; ++cross) {
            GeneticAlgorithm ga(g);
            ga.setParameters(limitOthers, 100, 0.05, 0.8, 1, cross, 2);
            GeneticResult res = ga.solve();
            out4 << file << ";" << (cross==1 ? "OX" : "PMX") << ";" << res.bestCost << ";" << calculateError(res.bestCost, opt) << ";" << res.timeToFindBest << "\n";
        }
    }
    out4.close();

    // 5. Ocena 5.0 - Wpływ metody selekcji (Turniej vs Ruletka)
    std::cout << "\n[5/5] Test 5.0: Wplyw metody selekcji...\n";
    std::ofstream out5("wyniki_5_0_selekcja.csv");
    out5 << "Plik;MetodaSelekcji;KosztZnaleziony;Blad(%);CzasZnalezieniaRekordu(s)\n";
    for (const auto& file : allFiles) {
        AtspGraph g; if (!g.loadFromFile(file)) continue;
        int opt = optimalValues[file];
        for (int sel = 1; sel <= 2; ++sel) {
            GeneticAlgorithm ga(g);
            ga.setParameters(limitOthers, 100, 0.05, 0.8, sel, 1, 2);
            GeneticResult res = ga.solve();
            out5 << file << ";" << (sel==1 ? "Turniej" : "Ruletka") << ";" << res.bestCost << ";" << calculateError(res.bestCost, opt) << ";" << res.timeToFindBest << "\n";
        }
    }
    out5.close();

    std::cout << "\n>>> ZAKONCZONO POMYSLNIE! Wszystkie pliki .csv wygenerowane. <<<\n";
}

int main() {
    AtspGraph graph;
    GeneticAlgorithm* ga = nullptr;

    // Domyslne parametry algorytmu
    double timeLimit = 60.0;
    int populationSize = 100;
    double mutationRate = 0.05;   // 5%
    double crossoverRate = 0.80;  // 80%

    int selectionMethod = 1; // 1: Turniej, 2: Ruletka
    int crossoverMethod = 1; // 1: OX (Order), 2: PMX
    int mutationMethod = 2;  // 1: Swap, 2: Inwersja (Inwersja jest zazwyczaj lepsza dla TSP)

    std::string currentFile = "";

    int choice;
    do {
        std::cout << "\n=== ALGORYTM GENETYCZNY (TSP/ATSP) ===\n";
        std::cout << "1. Wczytanie danych z pliku (obecnie: " << (currentFile.empty() ? "brak" : currentFile) << ")\n";
        std::cout << "2. Kryterium stopu [czas w sek.] (obecnie: " << timeLimit << " s)\n";
        std::cout << "3. Wielkosc populacji poczatkowej (obecnie: " << populationSize << ")\n";
        std::cout << "4. Wspolczynnik mutacji (obecnie: " << mutationRate << ")\n";
        std::cout << "5. Wspolczynnik krzyzowania (obecnie: " << crossoverRate << ")\n";
        std::cout << "6. Wybor metody krzyzowania (obecnie: " << (crossoverMethod == 1 ? "OX" : "PMX") << ")\n";
        std::cout << "7. Wybor metody mutacji (obecnie: " << (mutationMethod == 1 ? "Swap" : "Inwersja") << ")\n";
        std::cout << "8. Wybor metody selekcji (obecnie: " << (selectionMethod == 1 ? "Turniej" : "Ruletka") << ")\n";
        std::cout << "9. URUCHOM ALGORYTM\n";
        std::cout << "10. AUTO-TESTY (Generowanie plikow do sprawozdania)\n";
        std::cout << "0. Wyjscie\n";
        std::cout << "Wybor: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                std::cout << "Podaj nazwe pliku (np. ftv47.atsp): ";
                std::cin >> currentFile;
                if (graph.loadFromFile(currentFile)) {
                    std::cout << "Wczytano pomyslnie. Rozmiar: " << graph.getDimension() << "\n";
                    if (ga) delete ga;
                    ga = new GeneticAlgorithm(graph);
                } else {
                    currentFile = "";
                }
                break;
            }
            case 2: std::cout << "Podaj czas [s]: "; std::cin >> timeLimit; break;
            case 3: std::cout << "Podaj wielkosc populacji: "; std::cin >> populationSize; break;
            case 4: std::cout << "Podaj wspolczynnik mutacji (0.0 - 1.0): "; std::cin >> mutationRate; break;
            case 5: std::cout << "Podaj wspolczynnik krzyzowania (0.0 - 1.0): "; std::cin >> crossoverRate; break;
            case 6: std::cout << "Metoda krzyzowania (1-OX, 2-PMX): "; std::cin >> crossoverMethod; break;
            case 7: std::cout << "Metoda mutacji (1-Swap, 2-Inwersja): "; std::cin >> mutationMethod; break;
            case 8: std::cout << "Metoda selekcji (1-Turniej, 2-Ruletka): "; std::cin >> selectionMethod; break;
            case 9: {
                if (!graph.isLoaded() || !ga) {
                    std::cout << "Brak danych! Najpierw wczytaj plik (Opcja 1).\n";
                    break;
                }
                ga->setParameters(timeLimit, populationSize, mutationRate, crossoverRate,
                                  selectionMethod, crossoverMethod, mutationMethod);

                std::cout << "\nTrwa ewolucja (" << timeLimit << " s)...\n";
                GeneticResult result = ga->solve();

                int optimum = optimalValues.count(currentFile) ? optimalValues[currentFile] : -1;

                std::cout << "\n=== WYNIKI ===\n";
                std::cout << "Najlepsze znalezione rozwiazanie: " << result.bestCost << "\n";
                std::cout << "Czas znalezienia najlepszego (s): " << result.timeToFindBest << "\n";
                std::cout << "Srednia dlugosc w ostatniej populacji: " << result.averageLastPopulationCost << "\n";

                if (optimum != -1) {
                    double errBest = calculateError(result.bestCost, optimum);
                    double errAvg = calculateError(result.averageLastPopulationCost, optimum);
                    std::cout << "Optimum dla " << currentFile << ": " << optimum << "\n";
                    std::cout << "Blad (najlepsze): " << errBest << " %\n";
                    std::cout << "Blad (srednia ost. pop.): " << errAvg << " %\n";
                }
                break;
            }
            case 10: runAutomatedTests(); break;
        }
    } while (choice != 0);

    if (ga) delete ga;
    return 0;
}