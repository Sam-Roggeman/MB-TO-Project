#include "FNN.h"
#include <iostream>
#include <fstream>

#include "../game_logic/utils/Random.h"

int FFNeuralNetwork::__id = 0;



FFNeuralNetwork::FFNeuralNetwork(const char* filename) {
        freopen(filename,"r",stdin);
        cin >> _num_layers;
        for (int i = 0; i < _num_layers; i++) {
                int s;
                cin >> s;
                _neuronsPerLayer.push_back(s);
        }
        inputNodes = _neuronsPerLayer[0];
        outputNodes = _neuronsPerLayer[_num_layers-1];
        if (_num_layers == 2) {
                hiddenNodes = 0;
                _hiddenLayers = 0;
        } else {
                hiddenNodes = _neuronsPerLayer[1];
                _hiddenLayers = _num_layers - 2;
        }
        for (int i = 0; i < _num_layers-1; i++) {
                weights.emplace_back(vector<vector<float>>());
                for (int j = 0; j < _neuronsPerLayer[i+1]; j++) {
                        weights[i].push_back(vector<float>());
                        for (int k = 0; k < _neuronsPerLayer[i]+1; k++) {
                                float ggg;
                                cin >> ggg;
                                weights[i][j].push_back(ggg);
                        }
                }
        }
}

void FFNeuralNetwork::exportWeights() {
        ofstream neural("assets/neuralNetID" + std::to_string(__id));
        __id++;
        neural << _num_layers << "\n\n";
        for (int i = 0; i < _num_layers; i++) {
                neural << _neuronsPerLayer[i] << " ";
        }
        neural << "\n\n";
        for (int i = 0; i < weights.size(); i++) {
                for (int j = 0; weights[i].size(); j++) {
                        for (int k = 0; weights[i][j].size(); k++) {
                                neural << weights[i][j][k] << " ";
                        }
                }
        }
}

#include "FNN.h"
#include <iostream>

#include "../game_logic/utils/Random.h"

FFNeuralNetwork::FFNeuralNetwork(int input, int hidden, int output, int hiddenLayers) {
        inputNodes = input;
        hiddenNodes = hidden;
        outputNodes = output;
        _hiddenLayers = hiddenLayers;
        _num_layers = hiddenLayers+2;
        _neuronsPerLayer.push_back(input);
        for (int i = 0; i < hiddenLayers; i++) {
                _neuronsPerLayer.push_back(hidden);
        }
        _neuronsPerLayer.push_back(output);

        if (_hiddenLayers) {
                weights.emplace_back(vector<vector<float>>());
                for (int i = 0; i < hiddenNodes; i++) {
                        weights[0].push_back(vector<float>());
                        for (int k = 0; k < inputNodes+1; k++) {
                                weights[0][i].push_back(Core::Random::uniformReal(0,1));
                        }
                }
        }

        for (int i = 1; i < _hiddenLayers; i++) {
                weights.emplace_back(vector<vector<float>>());
                for (int k = 0; k < hiddenNodes; k++) {
                        weights[i].push_back(vector<float>());
                        for (int c = 0; c < hiddenNodes+1; c++) {
                                weights[i][k].push_back(Core::Random::uniformReal(0,1));
                        }
                }
        }

        weights.emplace_back(vector<vector<float>>());
        for (int i = 0; i < outputNodes; i++) {
                weights[weights.size()-1].push_back(vector<float>());
                for (int k = 0; k < hiddenNodes+1; k++) {
                        weights[weights.size()-1][i].push_back(Core::Random::uniformReal(0,1));
                }
        }
}

void FFNeuralNetwork::mutate(float mr) {
        for(vector<vector<float>>& w : weights) {
                for (int i = 0; i < w.size(); i++) {
                        for (int k = 0; k < w[0].size(); k++) {
                                float rand_number = Core::Random::uniformReal(0,1);
                                if (rand_number<mr) {
                                        w[i][k] += Core::Random::Normal(0,1)*30.0f;
                                }
                        }
                }
        }
}

vector<float> FFNeuralNetwork::forward(vector<float> inputs)
{
        inputs.push_back(1); // for bias: 1xbias
        vector<float> forwarded = inputs;
        for (int i = 0; i < weights.size(); i++) {
                vector<float> outputs(weights[i].size(), 0);
                for (int k = 0; k < weights[i].size(); k++) {
                        for (int c = 0; c < weights[i][k].size(); c++) {
                                outputs[k] += forwarded[c] * weights[i][k][c];
                        }

                        //sigmoid
                        outputs[k] = 1.0f / (1.0f + exp(-outputs[k]));
                }
                forwarded = outputs;
        }

        return forwarded;
}

FFNeuralNetwork FFNeuralNetwork::crossover(FFNeuralNetwork& partner)
{
        FFNeuralNetwork child = FFNeuralNetwork(inputNodes, hiddenNodes, outputNodes, _hiddenLayers);
        for (int i = 0; i < weights.size(); i++) {
                int randomColumn = floor((float) weights[i][0].size()/2.0f);
                int randomRow = floor((float) weights[i].size()/2.0f);
                //                 int randomColumn = floor(Core::Random::uniformReal(0.0f,(float) weights[i][0].size())); //!!!!!!!!
                //                 int randomRow = floor(Core::Random::uniformReal(0.0f, (float) weights[i].size()));       //!!!!!!!!
                for (int k = 0; k < weights[i].size(); k++) {
                        for (int c = 0; c < weights[i][0].size(); c++) {
                                if ((k < randomRow) || (k == randomRow && c <= randomColumn)) {
                                        child.weights[i][k][c] = weights[i][k][c];
                                } else {
                                        child.weights[i][k][c] = partner.weights[i][k][c];
                                }
                        }
                }
        }
        return child;
}