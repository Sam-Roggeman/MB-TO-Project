#ifndef FFN_H_NEURAL_NETWORK
#define FFN_H_NEURAL_NETWORK

class FFNeuralNetwork {
public:
  int inputNodes, hiddenNodes, outputNodes, hiddenLayers;
  vector<vector<vector<double>>> weights;
  
  FFNeuralNetwork(int input, int hidden, int output, int hiddenLayers) {

    inputNodes = input;
    hiddenNodes = hidden;
    outputNodes = output;
    hiddenLayers = hiddenLayers;
    
    if (hiddenLayers) {
      weights.push_back(vector<vector<double>>());
      for (int i = 0; i < hiddenNodes; i++) {
        weights[0].push_back(vector<double>());
        for (int k = 0; k < inputNodes+1; k++) {
          weights[0][i].push_back(random(0,1)); //!!!!!!!!!
        }
      }
    }

    for (int i = 1; i < hiddenLayers; i++) {
      weights.push_back(vector<vector<double>>());
      for (int k = 0; k < hiddenNodes; k++) {
        weights[k].push_back(vector<double>());
        for (int c = 0; c < hiddenNodes+1; c++) {
          weights[i][k].push_back(random(0,1)); //!!!!!!!!!
        }
      }
    }

    weights.push_back(vector<vector<double>>());
    for (int i = 0; i < outputNodes; i++) {
      weights[weights.size()-1].push_back(vector<double>());
      for (int k = 0; k < hiddenNodes+1; k++) {
        weights[weights.size()-1][i].push_back(random(0,1)); //!!!!!!!!!!
      }
    }
  }
  
  void mutate(float mr) {
    for(vector<vector<double>>& w : weights) {
      for (int i = 0; i < w.size(); i++) {
        for (int k = 0; k < w[i].size(); k++) {
          double rand_number = random; //!!!!!!!!!!!!!!!
          if (rand_number<mr) {
            w[i][k] += random; //!!!!!!!!!
            if(w[i][j] > 1) {
              w[i][j] = 1;
            }
            if(w[i][j] <-1) {
              w[i][j] = -1;
            }
          }
        }
      }
    }
  }

  vector<double> forward(vector<double> inputs) {

    inputs.push_back(1) //for bias: 1xbias

    for (int i = 0; i < weights.size(); i++) {
      vector<double> outputs(weights[i].size(),0);
      for (int k = 0; k < weights[i].size(); k++) {
        for (int c = 0; c < weights[i][k].size(); c++) {
          outputs[k] += inputs[c]*weights[i][k][c];
        }

        //relu activation
        outputs[k] = max(0, outputs[k]);
      }
      inputs = outputs;
    } //relu on output layer too?
     
     return outputs;
  }
  
  FFNeuralNetwork crossover(const FFNeuralNetwork& partner) {
    FFNeuralNetwork child = NeuralNet(inputNodes,hiddenNodes,outputNodes,hiddenLayers);
    for(int i = 0; i < weights.size(); i++) {
      int randomColumn = floor(random(weights[0].size())); //!!!!!!!!
      int randomRow = floor(random(weights.size())); //!!!!!!!!!
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