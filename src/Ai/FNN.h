#ifndef FFN_H_NEURAL_NETWORK
#define FFN_H_NEURAL_NETWORK

#include <cmath>
#include <vector>

using namespace std;

class FFNeuralNetwork
{
private:
        int _num_layers;
        vector<int> _neuronsPerLayer;
        static int __id;

        int inputNodes, hiddenNodes, outputNodes, _hiddenLayers;
        vector<vector<vector<float>>> weights;
        vector<float> forward(vector<float> inputs);
public:
        FFNeuralNetwork(int input, int hidden, int output, int hiddenLayers);
        FFNeuralNetwork(const char* filename);
        void exportWeights();

         FFNeuralNetwork(const FFNeuralNetwork& a) = default;
         FFNeuralNetwork crossover(FFNeuralNetwork& partner);
         void mutate(float mr);
         vector<float> operator()(vector<float> inputs) {
                 return forward(inputs);
         }
};

#endif
