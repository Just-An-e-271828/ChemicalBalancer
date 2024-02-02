#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <unordered_set>
#include <cctype>
#include <algorithm>
#include <numeric>


using namespace std;
//將化學式分為不同化合物
vector<string> splitEquation(string Eq) {
    vector<string> compoundList;
    string currentCompound;

    for (int i = 0; i < Eq.size(); ++i) {
        if (Eq.substr(i, 3) == " + " && !currentCompound.empty()) {
            compoundList.push_back(currentCompound);
            currentCompound.clear();
            i += 2;
        } else {
            currentCompound += Eq[i];
        }
    }

    if (!currentCompound.empty()) {
        compoundList.push_back(currentCompound);
    }
    return compoundList;
}
//將分好的化合物分為不同元素，並讀取在該化合物中的數量
map<string, int> splitCompound(string compound) {
    map<string, int> elementMap;
    string element;
    int count = 0;
    string charge = "";

    for (size_t i = compound.size(); i > 0; --i) {
        if (compound[i] == '^') {
            charge = compound.substr(i+1, compound.size() - i);
            compound = compound.substr(0, i);
            char pn = charge.back();
            charge.resize(charge.size() - 1);
            if (pn == '+') {
                if (charge == "") {
                    elementMap["charge"] = 1;
                } else {
                    elementMap["charge"] = stoi(charge);
                }
            } else if (pn == '-') {
                if (charge == "") {
                    elementMap["charge"] = -1;
                } else {
                    elementMap["charge"] = -stoi(charge);
                }
            }
            break;
        }
    }    

    if (elementMap.find("charge") == elementMap.end()) {
        elementMap["charge"] = 0;
    }

    for (size_t i = 0; i < compound.size(); ++i) {
        if (isupper(compound[i])) {
            if (!element.empty()) {
                if (count == 0) {
                    count = 1;
                }
                elementMap[element] += count;
                
                count = 0;
                element.clear();
            }
            element.push_back(compound[i]);
        }

        else if (islower(compound[i])) {
            element.push_back(compound[i]);
        }

        else if (isdigit(compound[i])) {
            count = count * 10 + (compound[i] - '0');
        }

    }

    if (!element.empty()) {
        if (count == 0) {
            count = 1;
        }
        elementMap[element] += count;
    }

    return elementMap;
}
//計算向量變數中非零的數值有多少
int countNonZero(const vector<int> vec) {
    int count = 0;

    for (size_t i = 0; i < vec.size(); ++i) {
        if (vec[i] != 0) {
            count++;
        }
    }

    return count;
}
//計算不同化合物的比例(兩兩一組計算)
vector<vector<int>> compRatio (vector<vector<int>> eqTable, int x = 0) {
    vector<vector<int>> ratioTable(eqTable.size(), vector<int>(eqTable[0].size()));

    for (size_t i = 0; i < eqTable.size(); ++i) {
        if (countNonZero(eqTable[i]) == 2) {
            vector<pair<int, int>> factor;
            for (int j = 0; j < eqTable[i].size(); ++j) {   
                if (eqTable[i][j] != 0) {
                    pair<int, int> var = {j, eqTable[i][j]};
                    factor.push_back(var);
                }
            }

            int GCD = gcd(factor[0].second, factor[1].second);
            ratioTable[x][factor[0].first] = abs(factor[1].second / GCD);
            ratioTable[x][factor[1].first] = abs(factor[0].second / GCD);
            vector<vector<int>> newEqTable = eqTable;
            vector<int> zeros(newEqTable[0].size());
            newEqTable[i] = zeros;

            for (size_t j = 0; j < newEqTable.size(); ++j) {
                newEqTable[j][factor[0].first] = (newEqTable[j][factor[0].first] * ratioTable[x][factor[0].first]) + (newEqTable[j][factor[1].first] * ratioTable[x][factor[1].first]);
                newEqTable[j][factor[1].first] = 0;
            }
            vector<vector<int>> returnRatio = compRatio(newEqTable, x + 1);

            for (int j = 0; j < ratioTable.size(); ++j) {
                for (int k = 0; k < ratioTable[0].size(); ++k) {
                    ratioTable[j][k] += returnRatio[j][k];
                }
            }

            break;
        }
    }

    return ratioTable;
}
//將兩兩一組的比例整理成所有化合物之間的比例
vector<int> solve_matrix(vector<vector<int>> matrix) {
    vector<int> solved_matrix;

    for(int j = 0; j < matrix[0].size(); ++j) {
        vector<int> multipled;
        int multipler = 1;

        for(int i = 0; i < matrix.size(); ++i) {
            if (matrix[i][j] != 0) {
                multipler *= matrix[i][j];
                multipled.push_back(i);
            }
        }

        for (const int& i : multipled){
            int repeated = matrix[i][j];
            for(int k = 0; k < matrix[0].size(); ++k) {
                matrix[i][k] *= (multipler / repeated);
            }
        }
    }

    for(int j = 0; j <= matrix.size(); ++j) {
        for (int i = 0; i < matrix.size(); ++i) {
            if (matrix[i][j] != 0) {
                solved_matrix.push_back(matrix[i][j]);
                break;
            }
        }
    }
    return solved_matrix;
}
//總流程
string balance_equation(string Equation) {
    string leftEq, rightEq;
    size_t splitPos = Equation.find(" -> ");
    string balancedEquation = "";
    
    if (splitPos != string::npos) {
        leftEq = Equation.substr(0,splitPos);
        rightEq = Equation.substr(splitPos + 4);
    }
    else {
        cout << "Invalid Equation" << endl;
        return "";
    }

    vector<string> leftComp = splitEquation(leftEq);
    vector<string> rightComp = splitEquation(rightEq);
    vector<map<string, int>> elementList;

    for (int i = 0; i < leftComp.size(); ++i) {
        map<string, int> elements = splitCompound(leftComp[i]);
        elementList.push_back(elements);
        }
    for (int i = 0; i < rightComp.size(); ++i) {
        map<string, int> elements = splitCompound(rightComp[i]);
        for (auto & pair : elements) {
            pair.second *= -1;
        }
        elementList.push_back(elements);
    }
    unordered_set<string> elements;
    for (const auto& m : elementList) {
        for (const auto& entry : m) {
            elements.insert(entry.first);
        }
    }

    vector<string> elements_list(elements.begin(), elements.end());
    sort(elements_list.begin(), elements_list.end());
    int num_comp = elementList.size();
    int num_ele = elements_list.size();
    vector<vector<int>> result(num_ele, vector<int>(num_comp, 0));

    for (int j = 0; j < num_comp; ++j) {
        for (int i = 0; i < num_ele; ++i) {
            if (elementList[j].count(elements_list[i]) > 0) {
                result[i][j] = elementList[j].find(elements_list[i])->second;
            }
        }
    }

    result = compRatio(result);
    vector<int> compCount = solve_matrix(result);

    for (int i = 0; i < leftComp.size(); ++i) {
        if (compCount[i] == 1) {
            balancedEquation.append(leftComp[i]);
        } else {
            string Count = to_string(compCount[i]);
            balancedEquation.append(Count + leftComp[i]);
        }
        if (i < leftComp.size() - 1) {
        balancedEquation.append(" + ");
        }
    }

    balancedEquation += " -> ";

    for (int i = leftComp.size(); i < rightComp.size() + leftComp.size(); ++i) {
        if (compCount[i] == 1) {
            balancedEquation.append(rightComp[i - leftComp.size()]);
        } else {
            string Count = to_string(compCount[i]);
            balancedEquation.append(Count + rightComp[i - leftComp.size()]);
        }
        if (i < leftComp.size() + rightComp.size() -1) {
        balancedEquation.append(" + ");
        }
    }
    return balancedEquation;
}    
//偵測用戶輸入
int main() {
    string Eq = "";
    cout << "Enter the equation or 'exit' to quit:" << endl;
    cout << "type 'help' for more info" << endl << endl;
    while (true) {
        getline(cin, Eq);
        if (Eq == "exit") {
            break;
        } else if (Eq == "help") {
            cout << "Enter a Equation like these: 'H2 + O2 -> H2O' or 'H^+ + SO4^2- -> H2SO4'" << endl << "1.Keep a space between Compounds, plus signs, and the arrow" << endl << "2.use '^' to indicate the charge of the compound. There should be no space between '^' and the plus sign of the charge." << endl << "Enter the equation or 'exit' to quit:" << endl << endl;
        } else if (Eq != "") {
            string output = balance_equation(Eq);
            if (output != "") {
                cout << "Balanced equation: " << output << endl;
            }
        }
        Eq = "";
    }
    return 0;
}