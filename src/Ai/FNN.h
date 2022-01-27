#ifndef FFN_H_NEURAL_NETWORK
#define FFN_H_NEURAL_NETWORK

#include <cmath>
#include <vector>

using namespace std;

class FFNeuralNetwork
{
private:
        int inputNodes, hiddenNodes, outputNodes, _hiddenLayers;
        vector<vector<vector<float>>> weights;
        vector<float> forward(vector<float> inputs);
public:
        FFNeuralNetwork(int input, int hidden, int output, int hiddenLayers);
        FFNeuralNetwork(const FFNeuralNetwork& a) = default;
        FFNeuralNetwork crossover(FFNeuralNetwork& partner);
        void mutate(float mr);
        vector<float> operator()(vector<float> inputs) {
                return forward(inputs);
        }
};

#endif
        // FFNeuralNetwork clone() {
        //    FFNeuralNetwork clone = new FFNeuralNetwork(inputNodes,hiddenNodes,ouputNodes,hiddenLayers);
        //    for(int i = 0; i < weights.size(); i++) {
        //       clone.weights[i] = weights[i].clone();
        //    }
        //    return clone;
        // }

        // void load(vector<vector<vector<double>>> weight) {
        //     for(int i = 0; i < weights.size(); i++) {
        //        weights[i] = weight[i];
        //     }
        // }

        // vector<vector<vector<double>>> pull() {
        //    vector<vector<vector<double>>> model = weights.clone();
        //    return model;
        // }