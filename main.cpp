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

double squaredDistance(const Instance& a, const Instance& b, const vector<int>& featureSet) {
    double dist = 0.0;

    int feature;
    for (int i = 0; i < (int)featureSet.size(); i++) {
        feature = featureSet[i];
        double diff = a.features[feature - 1] - b.features[feature - 1];
        dist += diff * diff;
    }

    return dist;
}

double leaveOneOutAccuracy(const vector<Instance>& data, const vector<int>& featureSet) {
    if (featureSet.empty()) {
        
        int count1 = 0;
        int count2 = 0;
        
        for (int i = 0; i < (int)data.size(); i++) {
            if (data[i].label == 1.0) {
                count1++;
            } else {
                count2++;
            }
        }

        return 100.0 * max(count1, count2) / data.size();
    }

    int correct = 0;

    for (int i = 0; i < (int)data.size(); i++) {
        double bestDistance = numeric_limits<double>::max();
        double nearestLabel = -1;

        for (int j = 0; j < (int)data.size(); j++) {
            if (i == j) { continue; }

            double dist = squaredDistance(data[i], data[j], featureSet);

            if (dist < bestDistance) {
                bestDistance = dist;
                nearestLabel = data[j].label;
            }
        }

        if (nearestLabel == data[i].label) {
            correct++;
        }
    }

    return 100.0 * correct / data.size();
}

string featureSetToString(const vector<int>& featureSet) {
    if (featureSet.empty()) {
        return "{}";
    }

    string s = "{";

    for (int i = 0; i < (int)featureSet.size(); i++) {
        s += to_string(featureSet[i]);

        if (i != (int)featureSet.size() - 1) {
            s += ",";
        }
    }

    s += "}";
    return s;
}

vector<int> forwardSelection(const vector<Instance>& data, int numFeatures) {
    vector<int> currentSet;
    vector<int> bestOverallSet;

    double bestOverallAccuracy = 0.0;

    cout << "Beginning search: \n";

    for (int level = 1; level <= numFeatures; level++) {
        int featureToAddAtThisLevel = -1;
        double bestSoFar = 0.0;

        for (int feature = 1; feature <= numFeatures; feature++) {
            // Skip if feature is already in the current set
            if (find(currentSet.begin(), currentSet.end(), feature) != currentSet.end()) {
                continue;
            }

            vector<int> tempSet = currentSet;
            tempSet.push_back(feature);
            sort(tempSet.begin(), tempSet.end());

            double accuracy = leaveOneOutAccuracy(data, tempSet);

            cout << "   Using feature(s) " << featureSetToString(tempSet) << " accuracy is " << fixed << setprecision(1) << accuracy << "% \n";

            if (accuracy > bestSoFar) {
                bestSoFar = accuracy;
                featureToAddAtThisLevel = feature;
            }
        }

        if (featureToAddAtThisLevel != -1) {

            currentSet.push_back(featureToAddAtThisLevel);
            sort(currentSet.begin(), currentSet.end());

            cout << "Feature set " << featureSetToString(currentSet) << " was best, accuracy is " << fixed << setprecision(1) << bestSoFar << "%\n\n";

            if (bestSoFar > bestOverallAccuracy) {
                bestOverallAccuracy = bestSoFar;
                bestOverallSet = currentSet;
            } else {
                cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)\n\n";
            }
        }
    }

    cout << "Finished search! The best feature subset is " << featureSetToString(bestOverallSet) << " which has an accuracy of " << fixed << setprecision(1) << bestOverallAccuracy << "%\n";

    return bestOverallSet;
}

vector<int> backwardElimination(const vector<Instance>& data, int numFeatures) {
    vector<int> currentSet;
    
    for (int i = 1; i <= numFeatures; i++) {
        currentSet.push_back(i);
    }

    vector<int> bestOverallSet = currentSet;
    double bestOverallAccuracy = leaveOneOutAccuracy(data, currentSet);

    cout << "Beginning search: \n";

    for (int level = numFeatures; level >= 1; level--) {
        int featureToRemoveAtThisLevel = -1;
        double bestSoFar = 0.0;
        vector<int> bestCandidateSet;

        int feature;
        for (int i = 0; i < (int)currentSet.size(); i++) {
            feature = currentSet[i];
            vector<int> tempSet;

            for (int j = 0; j < (int)currentSet.size(); j++) {
                if (currentSet[j] != feature) {
                    tempSet.push_back(currentSet[j]);
                }
            }

            double accuracy = leaveOneOutAccuracy(data, tempSet);

            cout << "   Using feature(s) " << featureSetToString(tempSet) << " accuracy is " << fixed << setprecision(1) << accuracy << "% \n";

            if (accuracy > bestSoFar) {
                bestSoFar = accuracy;
                featureToRemoveAtThisLevel = feature;
                bestCandidateSet = tempSet;
            }
        }

        if (featureToRemoveAtThisLevel != -1) {
            currentSet = bestCandidateSet;

            cout << "Feature set " << featureSetToString(currentSet) << " was best, accuracy is " << fixed << setprecision(1) << bestSoFar << "% \n\n";

            if (bestSoFar > bestOverallAccuracy) {
                bestOverallAccuracy = bestSoFar;
                bestOverallSet = currentSet;
            } else {
                cout << "(Warning, Accuracy has decreased! Continuing search in case of local maxima)\n\n";
            }
        }

        if (currentSet.empty()) break;
    }

    cout << "Finished search! The best feature subset is " << featureSetToString(bestOverallSet) << " which has an accuracy of " << fixed << setprecision(1) << bestOverallAccuracy << "% \n";

    return bestOverallSet;
}


int main() {
        cout << "Welcome to Kevin's Feature Selection Algorithm \n";

        string filename = "CS170_Large_DataSets_all/CS170_Large_DataSet__2.txt";
        cout << "Running on file: " << filename << "\n";

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

        double allAccuracy = leaveOneOutAccuracy(data, allFeatures);
        cout << "Running nearest neighbor with all " << numFeatures << " features, using \"leaving-one-out\" evaluation, I get an accuracy of " << fixed << setprecision(1) << allAccuracy << "%\n\n";

        if (choice == 1) {
            forwardSelection(data, numFeatures);
        } else if (choice == 2) {
            backwardElimination(data, numFeatures);
        } else {
            cout << "Invalid choice.\n";
        }

         return 0;
    }

