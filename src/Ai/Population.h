// class Population {
// public:
//  vector<Car> cars;
//  Car bestCar;
//
//  int bestCarScore = 0;
//  int population_size;
//  int generation = 0;
//  //int samebest = 0;
//
//  double bestFitness = 0;
//  double fitnessSum = 0;
//
//  Population(int size) {
//    for(int i = 0; i < size; i++) {
//      cars.push_back(Car());
//    }
//    population_size = size;
//    bestCar = cars[0].clone();
//    bestCar.replay = true;
//  }
//
//  boolean done() {  //check if all the cars in the population are crashed/finished
//    for(int i = 0; i < cars.size(); i++) {
//      if(!cars[i].done())
//        return false;
//    }
//    if(!bestCar.done()) {
//       return false;
//    }
//    return true;
//  }
//
//  void update() {  //update all the cars in the generation
//    if(!bestCar.done()) {  //if the best car is not done -> update it, this car is a replay of the best from the past
//    generation
//       bestCar.update();
//    }
//    for(int i = 0; i < cars.size(); i++) {
//      if(!cars[i].done()) {
//         cars[i].update();
//      }
//    }
//  }
//
//  void setBestCar() {  //set the best car of the generation
//    double max = 0;
//    int maxIndex = 0;
//    for(int i = 0; i < cars.size(); i++) {
//      if(cars[i].fitness > max) {
//        max = cars[i].fitness;
//        maxIndex = i;
//      }
//    }
//    if(max > bestFitness) {
//      bestFitness = max;
//      bestCar = cars[maxIndex].cloneForReplay();
//      bestCarScore = cars[maxIndex].score;
//    } else {
//      bestCar = bestCar.cloneForReplay();
//    }
//  }
//
//  Car selectParent() {
//    double rand = random(fitnessSum); //!!!!!!!!
//    double summation = 0;
//    for(int i = 0; i < cars.size(); i++) {
//      summation += cars[i].fitness;
//      if(summation > rand) {
//        return cars[i];
//      }
//    }
//    return cars[0];
//  }
//
//  void naturalSelection() {
//    vector<Car> newgenCars;
//    setBestCar();
//    calculateFitnessSum();
//    newgenCars.push_back(bestCar);
//    for(int i = 1; i < cars.size(); i++) {
//      Car child = selectParent().crossover(selectParent());
//      child.mutate();
//      newgenCars.push_back(child);
//    }
//    cars = newgenCars;
//    //evolution.add(bestCarScore); for graph
//    generation+=1;
//  }
//
//  void mutate() {
//    for(int i = 1; i < cars.size(); i++) {  //start from 1 as to not override the best car placed in index 0
//      cars[i].mutate();
//    }
//  }
//
//  void calculateFitness() {
//    for(int i = 0; i < cars.size(); i++) {
//      cars[i].calculateFitness();
//    }
//  }
//
//  void calculateFitnessSum() {
//    fitnessSum = 0;
//    for(int i = 0; i < cars.size(); i++) {
//      fitnessSum += cars[i].fitness;
//    }
//  }
//}