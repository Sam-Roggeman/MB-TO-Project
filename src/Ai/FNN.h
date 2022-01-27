#ifndef FFN_H_NEURAL_NETWORK
#define FFN_H_NEURAL_NETWORK

#include <cmath>
#include <vector>

using namespace std;

class FFNeuralNetwork
{
private:
        vector<vector<float>> _biases;
        vector<vector<vector<float>>> _weights;
        int _num_layers;
        vector<int> _neuronsPerLayer;





        //int inputNodes, hiddenNodes, outputNodes, _hiddenLayers;
        //vector<vector<vector<float>>> weights;
        //vector<float> forward(vector<float> inputs);
public:
        FFNeuralNetwork(const vector<int>&);
        void mutateOneWeightGene(FFNeuralNetwork& parent);
        void mutateOneBiasesGene(FFNeuralNetwork& parent);
        void uniformCrossOverWeights(FFNeuralNetwork& parent1, FFNeuralNetwork& parent2, FFNeuralNetwork& child2);
        void uniformCrossOverBiases(FFNeuralNetwork& parent1, FFNeuralNetwork& parent2, FFNeuralNetwork& child2);
        vector<float> feedforward(vector<float> inputs);
        vector<float> operator()(vector<float> inputs) {
                return feedforward(inputs);
        }









        // FFNeuralNetwork(int input, int hidden, int output, int hiddenLayers);
        // FFNeuralNetwork(const FFNeuralNetwork& a) = default;
        // FFNeuralNetwork crossover(FFNeuralNetwork& partner);
        // void mutate(float mr);
        // vector<float> operator()(vector<float> inputs) {
        //         return forward(inputs);
        // }
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
