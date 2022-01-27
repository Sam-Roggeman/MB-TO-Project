#include "FNN.h"
#include <iostream>

#include "../game_logic/utils/Random.h"

FFNeuralNetwork::FFNeuralNetwork(int input, int hidden, int output, int hiddenLayers) {
//        weights.emplace_back(vector<vector<float>>());
//        weights[0].push_back(vector<float>{1.9759754808760226,-0.3943706780838112,-12.849600990679392,0.029854578540802867});
//        weights[0].push_back(vector<float>{0.20977238080697014,-2.9177743524380686,2.94919034248246,0.006715082140747675});
//        weights[0].push_back(vector<float>{-0.2523586111918478,-0.43541974875967027,-0.017721600384833225,0.3905988660607571});
//        weights[0].push_back(vector<float>{-25.66389069447544,-1.741153671847535,0.35631865930291473,1.9701649767058669});
//
//        weights.emplace_back(vector<vector<float>>());
//        weights[1].push_back(vector<float>{-1.4854171305984258,-0.6101188251449138,0.003296534145573682,0.3604639319359206,68.40069005204634});
//        weights[1].push_back(vector<float>{0.7863354580297504,-4.982926729278056,-16.830942281576718,0.7383157827174109,-1.9461218681447088});
//        weights[1].push_back(vector<float>{0.09109572457900096,-0.9501210009560389,-0.28881417120972874,0.8932266518534402,-7.1689614916770115});
//        weights[1].push_back(vector<float>{1.5694585460537576,-0.330314349816421,0.07730608729924907,0.07854531909518031,-0.657327199352653});
//
//        weights.emplace_back(vector<vector<float>>());
//        weights[2].push_back(vector<float>{0.07041649692263191,5.634486592049883,-0.025719215623043468,-17.044099190414634,1.1814644869103548});
	inputNodes = input;
	hiddenNodes = hidden;
	outputNodes = output;
	_hiddenLayers = hiddenLayers;
	if (_hiddenLayers) {
		weights.emplace_back(vector<vector<float>>());
		for (int i = 0; i < hiddenNodes; i++) {
			weights[0].push_back(vector<float>());
			for (int k = 0; k < inputNodes+1; k++) {
				weights[0][i].push_back(Core::Random::uniformReal(0,1)); //!!!!!!!!!
			}
		}
	}
	
	for (int i = 1; i < _hiddenLayers; i++) {
		weights.emplace_back(vector<vector<float>>());
		for (int k = 0; k < hiddenNodes; k++) {
			weights[i].push_back(vector<float>());
			for (int c = 0; c < hiddenNodes+1; c++) {
				weights[i][k].push_back(Core::Random::uniformReal(0,1)); //!!!!!!!!!
			}
		}
	}

	weights.emplace_back(vector<vector<float>>());
	for (int i = 0; i < outputNodes; i++) {
		weights[weights.size()-1].push_back(vector<float>());
		for (int k = 0; k < hiddenNodes+1; k++) {
			weights[weights.size()-1][i].push_back(Core::Random::uniformReal(0,1)); //!!!!!!!!!!
		}
	}

//        for (int i = 0; i < weights[0].size(); i++) {
//                std::cout << "|";
//                for (int k = 0; k < weights[0][i].size(); k++) {
//                        std::cout << weights[0][i][k] << "|";
//                }
//                std::cout << "\n";
//        }
}

void FFNeuralNetwork::mutate(float mr) {
//        for (vector<vector<float>>& w : weights) {
//                for (int i = 0; i < w.size(); i++) {
//                        if (Core::Random::uniformReal(0,0.9999999999) < mr) {
//                                w[i][w[0].size()-1] += w[i][w[0].size()-1]*()
//                        }
//                        w[i][w[0].size()-1]
//                }
 //       }
	for(vector<vector<float>>& w : weights) {
 		for (int i = 0; i < w.size(); i++) {
 			for (int k = 0; k < w[0].size(); k++) {
 				float rand_number = Core::Random::uniformReal(0,1);//(-1,1); //!!!!!!!!!!!!!!!
 				if (rand_number<mr) {
 					w[i][k] += Core::Random::Normal(0,1);///5.0f;///5.0f; //!!!!!!!!!
 					// if(w[i][k] > 1) {
 					// 	w[i][k] = 1;
 					// }
 					// if(w[i][k] <-1) {
 					// 	w[i][k] = -1;
 					// }
 				}
 			}
 		}
 	}
}

 vector<float> FFNeuralNetwork::forward(vector<float> inputs)
 {
         inputs.push_back(1); // for bias: 1xbias
         vector<float> forwarded = inputs;
//for (int i = 0; i < inputs.size(); i++) {
//			 std::cout << inputs[i] << " ";
//		 }
//		 std::cout << "\n";
         for (int i = 0; i < weights.size(); i++) {
                 vector<float> outputs(weights[i].size(), 0);
                 for (int k = 0; k < weights[i].size(); k++) {
                         for (int c = 0; c < weights[i][k].size(); c++) {
                                 outputs[k] += forwarded[c] * weights[i][k][c];
                         }
                         // relu activation
                         //outputs[k] = max(0.0f, outputs[k]);

                         //sigmoid
                         outputs[k] = 1.0f / (1.0f + exp(-outputs[k]));
                 }
                 forwarded = outputs;
         } // relu on output layer too?
//         std::cout << "prev: ";
//         for (int i = 0; i < forwarded.size(); i++) {
//                 std::cout << forwarded[i] << " ";
//         }
//         std::cout << "\nafter: ";
        // for (int i = 0; i < forwarded.size(); i++) {
        //         forwarded[i] = 1.0f / (1.0f + exp(-forwarded[i]));
        // }
//         for (int i = 0; i < forwarded.size(); i++) {
//                 std::cout << forwarded[i] << " ";
//         }
//         std::cout << "\n\n";
//         std::cout << forwarded[0] << "\n";
//         std::cout << forwarded[1] << "\n";
//         std::cout << forwarded[2] << "\n";
//         std::cout << forwarded[3] << "\n\n\n";

         return forwarded;
 }

 FFNeuralNetwork FFNeuralNetwork::crossover(FFNeuralNetwork& partner)
 {
         FFNeuralNetwork child = FFNeuralNetwork(inputNodes, hiddenNodes, outputNodes, _hiddenLayers);
         for (int i = 0; i < weights.size(); i++) {
                 int randomColumn = floor(Core::Random::uniformReal(0.0f,(float) weights[i][0].size())); //!!!!!!!!
                 int randomRow = floor(Core::Random::uniformReal(0.0f, (float) weights[i].size()));       //!!!!!!!!
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
