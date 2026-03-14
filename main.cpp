#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

struct Instance {
    double label;
    vector<double> features;
};

vector<Instance> loadData(const string& filename) {
    ifstream fin(filename);
    if (!fin) {
        throw runtime_error("Could not open file: " + filename);
    }

    vector<Instance> data;
    string line;

    while (getline(fin, line)) {
        if (line.empty()) {
            continue;
        }

        stringstream ss(line);
        Instance inst;
        ss >> inst.label;

        double value;
        while (ss >> value) {
            inst.features.push_back(value);
        }

        if (!inst.features.empty()) {
            data.push_back(inst);
        }
    }

    if (data.empty()) {
        throw runtime_error("No data loaded from file.");
    }

    return data;
}


int main() {
        cout << "Welcome to Kevin's Feature Selection Algorithm \n";

        string filename = "Small_data/CS170_Small_DataSet__3.txt";

        vector<Instance> data = loadData(filename);

        int numFeatures = data[0].features.size();
        int numInstances = data.size();

        cout << "Type the number of the algorithm you want to run.\n";
        cout << "1) Forward Selection\n";
        cout << "2) Backward Elimination\n";

        int choice;
        cin >> choice;

        cout << "\nThis dataset has " << numFeatures << " features with " << numInstances << " instances.\n";

        vector<int> allFeatures;
        for (int i = 1; i <= numFeatures; i++) {
            allFeatures.push_back(i);
        }

         return 0;
    }

