#include "FNN.h"
#include <iostream>

#include "../game_logic/utils/Random.h"

FFNeuralNetwork::FFNeuralNetwork(int input, int hidden, int output, int hiddenLayers) {
	
	inputNodes = input;
	hiddenNodes = hidden;
	outputNodes = output;
	hiddenLayers = hiddenLayers;
	if (hiddenLayers) {
		weights.emplace_back(vector<vector<float>>());
		for (int i = 0; i < hiddenNodes; i++) {
			weights[0].push_back(vector<float>());
			for (int k = 0; k < inputNodes+1; k++) {
				weights[0][i].push_back(Core::Random::uniformReal(-1,1)); //!!!!!!!!!
			}
		}
	}
	
	for (int i = 1; i < hiddenLayers; i++) {
		weights.emplace_back(vector<vector<float>>());
		for (int k = 0; k < hiddenNodes; k++) {
			weights[i].push_back(vector<float>());
			for (int c = 0; c < hiddenNodes+1; c++) {
				weights[i][k].push_back(Core::Random::uniformReal(-1,1)); //!!!!!!!!!
			}
		}
	}

	weights.emplace_back(vector<vector<float>>());
	for (int i = 0; i < outputNodes; i++) {
		weights[weights.size()-1].push_back(vector<float>());
		for (int k = 0; k < hiddenNodes+1; k++) {
			weights[weights.size()-1][i].push_back(Core::Random::uniformReal(-1,1)); //!!!!!!!!!!
		}
	}
}

void FFNeuralNetwork::mutate(float mr) {
	for(vector<vector<float>>& w : weights) {
 		for (int i = 0; i < w.size(); i++) {
 			for (int k = 0; k < w[i].size(); k++) {
 				float rand_number = Core::Random::uniformReal(-1,1); //!!!!!!!!!!!!!!!
 				if (rand_number<mr) {
 					w[i][k] += Core::Random::Normal(0,1)/5.0f; //!!!!!!!!!
 					if(w[i][k] > 1) {
 						w[i][k] = 1;
 					}
 					if(w[i][k] <-1) {
 						w[i][k] = -1;
 					}
 				}
 			}
 		}
 	}
}

 vector<float> FFNeuralNetwork::forward(vector<float> inputs)
 {
         inputs.push_back(1); // for bias: 1xbias
         for (int i = 0; i < weights.size(); i++) {
                 vector<float> outputs(weights[i].size(), 0);
                 for (int k = 0; k < weights[i].size(); k++) {
                         for (int c = 0; c < weights[i][k].size(); c++) {
                                 outputs[k] += inputs[c] * weights[i][k][c];
                         }
                         // relu activation
                         outputs[k] = max(0.0f, outputs[k]);
                 }
                 inputs = outputs;
         } // relu on output layer too?

         for (int i = 0; i < inputs.size(); i++) {
                 inputs[i] = 1 / (1 + exp(-inputs[i]));
         }

         return inputs;
 }

 FFNeuralNetwork FFNeuralNetwork::crossover(const FFNeuralNetwork& partner)
 {
         FFNeuralNetwork child = FFNeuralNetwork(inputNodes, hiddenNodes, outputNodes, hiddenLayers);
         for (int i = 0; i < weights.size(); i++) {
                 int randomColumn = floor(Core::Random::uniformReal(0.0f,(float) weights[0].size())); //!!!!!!!!
                 int randomRow = floor(Core::Random::uniformReal(0.0f, (float) weights.size()));       //!!!!!!!!
                 for (int k = 0; k < weights.size(); k++) {
                         for (int c = 0; c < weights[0].size(); c++) {
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