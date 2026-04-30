#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iomanip>
#include <algorithm>
#include <windows.h>

using namespace std;

int main() {
    SetConsoleOutputCP(65001);
    ifstream in_file("input.txt");
    ofstream out_file("output.txt");

    if (!in_file.is_open()) {
        cout << "Не удалось открыть файл!\n";
        return 1;
    }
    string firstline;
    getline(in_file, firstline);
    stringstream ss(firstline);
    int n;
    ss >> n;
    vector<string> names(n);
    for (int i = 0; i < n; i++) {
        ss >> names[i];
    }
    map<string, double> spent;
    map<string, double> norm;
    for (const string& name : names) {
        spent[name] = 0.0;
        norm[name] = 0.0;
    }
    string line;
    while (getline(in_file, line)) {
        if (line.empty()) continue;

        stringstream line_stream(line);
        string payer;
        double amount;
        char slash;

        line_stream >> payer >> amount;

        if (!(line_stream >> slash) || slash != '/') {
            double per_person = amount / n;
            for (const string& name : names) {
                norm[name] += per_person;
            }
            spent[payer] += amount;
        } else {
            string excluded_str;
            getline(line_stream, excluded_str);
            set<string> excluded;
            stringstream excluded_stream(excluded_str);
            string ex_name;
            while (getline(excluded_stream, ex_name, ',')) {
                size_t start = ex_name.find_first_not_of(" \t");
                size_t end = ex_name.find_last_not_of(" \t");
                if (start != string::npos && end != string::npos) {
                    ex_name = ex_name.substr(start, end - start + 1);
                }
                excluded.insert(ex_name);
            }

            vector<string> included;
            for (const string& name : names) {
                if (excluded.find(name) == excluded.end()) {
                    included.push_back(name);
                }
            }

            int count = included.size();
            if (count > 0) {
                double per_included = amount / count;
                for (const string& person : included) {
                    norm[person] += per_included;
                }
                spent[payer] += amount;
            }
        }
    }
    in_file.close();

    for (const string& name : names) {
        out_file << name << " "
                 << fixed << setprecision(1)
                 << spent[name] << " "
                 << norm[name] << "\n";
    }

    map<string, double> balance;
    vector<pair<string, double>> debtors;
    vector<pair<string, double>> creditors;

    for (const string& name : names) {
        balance[name] = spent[name] - norm[name];
        if (balance[name] < 0.01) {
            debtors.push_back({name, -balance[name]});
        } else if (balance[name] > 0.01) {
            creditors.push_back({name, balance[name]});
        }
    }

    sort(debtors.begin(), debtors.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
    });
    sort(creditors.begin(), creditors.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    size_t i = 0, j = 0;
    while (i < debtors.size() && j < creditors.size()) {
        auto& debtor = debtors[i];
        auto& creditor = creditors[j];

        double transfer = min(debtor.second, creditor.second);

        out_file << debtor.first << " " << fixed << setprecision(1) << transfer << " " << creditor.first << "\n";

        debtor.second -= transfer;
        creditor.second -= transfer;

        if (debtor.second < 0.01) i++;
        if (creditor.second < 0.01) j++;
    }
    out_file.close();
    return 0;
}
