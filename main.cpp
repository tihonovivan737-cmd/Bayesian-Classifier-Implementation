#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

double expectation(double* var, size_t size1) {
    double sum = 0;
    for (size_t i = 0; i < size1; ++i)
        sum += var[i];
    return sum / size1;
}

double variance(double* var, size_t size1) {
    double sum = 0;
    int size2;
    double mean = expectation(var, size1);
    for (size_t i = 0; i < size1; ++i)
        sum += (var[i] - mean) * (var[i] - mean);
        size2=size1- 1;
    return sum / size2;
}


double calculatePosterior(double x, double y, double mean1, double mean2, double var1, double var2) {
    double density = 1 / (2 * M_PI * sqrt(var1) * sqrt(var2)) * exp(- ((pow(x - mean1, 2) / (2 * var1)) + (pow(y - mean2, 2) / (2 * var2))));
    return density;
}

int main() {
    setlocale(LC_ALL, "Russian");

    string fileChoice;
    cout << "Выберите файл для обработки (train/test): ";
    cin >> fileChoice;

    ifstream file_data;
    if (fileChoice == "train") {
        file_data.open("iris34_train.txt");
    } else if (fileChoice == "test") {
        file_data.open("iris34_test.txt");
    } else {
        cout << "Неверный выбор файла. Программа завершается." << endl;
        return 1;
    }

    ifstream file_train("iris34_train.txt");

    const int dataSize = 105;
    const int featureCount = 2;
    const int classCount = 3;

    double** features;
    features = new double*[dataSize];
    for (int i = 0; i < dataSize; ++i)
        features[i] = new double[featureCount];

    double* num_class = new double[dataSize];

    for (int i = 0; i < dataSize; i++) {
        for (int j = 0; j < featureCount; j++)
            file_train >> features[i][j];
        file_train >> num_class[i];
    }

    double means[classCount][featureCount];
    double variances[classCount][featureCount];


    for (int c = 0; c < classCount; c++) {
        for (int f = 0; f < featureCount; f++) {
            double currentFeatureValues[dataSize];
            int count = 0;

            for (int i = 0; i < dataSize; i++) {
                if (num_class[i] == c) {
                    currentFeatureValues[count] = features[i][f];
                    count++;
                }
            }

            means[c][f] = expectation(currentFeatureValues, count);
            variances[c][f] = variance(currentFeatureValues, count);

            cout << "Класса " << c << ": " << "Признак " << f + 1 << " - Мат.Ожидание: " << means[c][f] << ",  Ср.кв. Отклонений: " << sqrt(variances[c][f]) << "\n";

        }
        cout << endl;
    }


    int correctPredictions = 0;
    int totalSamplesTest = 0;

    while (!file_data.eof()) {
        double testFeatures[featureCount];
        double testClass;

        for (int i = 0; i < featureCount; i++) {
            file_data >> testFeatures[i];
        }
        file_data >> testClass;

        double maxPosterior = 0;
        int predictedClass = 0;

        for (int c = 0; c < classCount; c++) {
            double posterior = 1.0;


        for (int f = 0; f < featureCount; f++) {
                posterior *= calculatePosterior(testFeatures[0], testFeatures[1], means[c][0], means[c][1], variances[c][0], variances[c][1]);
            }

            if (posterior > maxPosterior) {
                maxPosterior = posterior;
                predictedClass = c;
            }
        }

        cout << "Предполагаемый класс: " << predictedClass << ", Фактический класс: " << testClass << endl;

        if (predictedClass == testClass) {
            correctPredictions++;
        }
        totalSamplesTest++;
    }

    for (int i = 0; i < dataSize; ++i) {
        delete[] features[i];
    }
    delete[] features;
    delete[] num_class;


    cout << endl <<"Точность: " << (double)correctPredictions / totalSamplesTest * 100 << "%" << endl;

    return 0;
}
