//class Car
//{
//public:
//        int score = 1;
//        int xVel, yVel;
//
//        double fitness = 0;
//
//        boolean done = false;
//        boolean replay = false; // if this car is a replay of best car
//
//        vector<double> vision;   // car vision (rays)
//        vector<double> decision; // car decision (turn direction, etc)
//
//        FFNeuralNetwork _brain;
//
//        Car() : _brain(3, 4, 1, 2), vision(0, 3), decision(0, 1) {}
//
//        boolean bodyCollide(double x, double y) {}
//
//        boolean TrackCollide(double x, float y) {}
//
//        void update()
//        { // move
//        }
//
//        Car cloneForReplay()
//        { // clone a version of the Car that will be used for a replay
//        }
//
//        Car crossover(Car parent)
//        { // crossover the car with another car
//                Car child = Car;
//                child._brain = _brain.crossover(parent._brain);
//                return child;
//        }
//
//        void mutate(double mutationRate)
//        { // mutate the cars _brain
//                _brain.mutate(mutationRate);
//        }
//
//        void calculateFitness()
//        { // calculate the fitness of the car
//        }
//}