//
//
//int SIZE = 20;
//int hiddenNodes = 16;
//int hiddenLayers = 2;
//
//int highscore = 0;
//
//double mutationRate = 0.05;
//double defaultmutation = mutationRate;
//
//bool humanPlaying = false; // false for AI, true to play yourself
//bool replayBest = true;    // shows only the best car of each generation
//bool seeVision = false;    // see the cars vision
//bool modelLoaded = false;  // loaded car with pretrained weights for brain
//
//// vector<int> evolution; for graph
//// EvolutionGraph graph;
//int main()
//{
//        Car car;
//        Car model;
//
//        Population pop;
//
//        if (humanPlaying) {
//                car = Car();
//        } else {
//                pop = new Population(2000);
//        }
//        if (humanPlaying) {
//                car.update();
//                if (car.done) {
//                        car = Car(); // or reset the same car
//                }
//        } else {
//                if (!modelLoaded) {
//                        if (pop.done()) {
//                                highscore = pop.bestCar.score;
//                                pop.calculateFitness();
//                                pop.naturalSelection();
//                        } else {
//                                pop.update();
//                                pop.show();
//                        }
//                } else {
//                        model.update();
//                        if (model.done()) {
//                                // reset car
//                        }
//                }
//        }
//}