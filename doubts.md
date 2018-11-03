## Pose.hpp

1. The definition uses a term called as normal velocity. What is that? 

2. A bug in the observation model floor update! I tried thinking about it based on some permuations of that variable however am not able to make sense of it :/ 
```c++
void floorUpdateUsingObservationModel(States& states, const Beacons& beacons){
            if(beacons.size()==0){
                return;
            }
            if(!mDataStore){
                return;
            }
            
            const BLEBeacons& bleBeacons = mDataStore->getBLEBeacons();
            const Building& building = mDataStore->getBuilding();
            
            // Add floors
            std::map<int, int> obsFloors = countFloors(beacons, bleBeacons);

            State meanState = State::weightedMean(states);
            std::vector<int> floors;
            States statesTmp;
            for(auto iter = obsFloors.begin(); iter!=obsFloors.end(); iter++){
                int floor = (*iter).first;
                State sTmp(meanState);
                sTmp.floor(floor);
                floors.push_back(floor);
                statesTmp.push_back(sTmp);
            }
            assert(statesTmp.size() == floors.size());
            auto logLLs = mObsModel->computeLogLikelihood(statesTmp, beacons);
            
            //if(true){
            //    double avgLogLL = std::accumulate(logLLs.begin(), logLLs.end(), 0.0)/(logLLs.size());
            //    std::cout << "#beacons=" << beacons.size() << ",avgLogLL = " << avgLogLL << std::endl;
            //}
            
            std::vector<double> weights = ArrayUtils::computeWeightsFromLogLikelihood(logLLs);
            
            // generate floors using weights and random numbers
            // BUG HERE The two for loops are having the same i! 
            std::vector<int> floorsGenerated;
            for(int i = 0; i<states.size(); i++){
                double d = randomGenerator->nextDouble();
                double sumw = 0;
                int floorGen = std::numeric_limits<int>::max();
                for(int i=0; i<weights.size(); i++){
                    sumw+=weights.at(i);
                    if(d<=sumw){
                        floorGen = floors.at(i);
                        break;
                    }
                    if(i==weights.size()-1){
                        floorGen = floors.at(i);
                    }
                }
                assert(floorGen != std::numeric_limits<int>::max());
                floorsGenerated.push_back(floorGen);
            }
            
            // update floors
            std::vector<int> floorsWritten(states.size());
            for(int i=0; i<states.size(); i++){
                auto&s = states.at(i);
                int floor = std::round(s.floor());
                int floorGen = floorsGenerated.at(i);
                floorsWritten.at(i) = floor;
                if(floor!=floorGen){
                    State sTmp(s);
                    sTmp.floor(floorGen);
                    if(building.isMovable(sTmp)){
                        s.floor(floorGen);
                        floorsWritten.at(i) = floorGen;
                    }
                }
            }
            
            size_t fsize = 0;
            if(mVerbose){
                std::stringstream ss;
                ss << "(floor,weights,countGenerated,countWritten)=";
                for(int i=0; i<weights.size(); i++){
                    int floor = floors.at(i);
                    auto countGen = std::count(floorsGenerated.begin(), floorsGenerated.end(), floor);
                    auto countWri = std::count(floorsWritten.begin(), floorsWritten.end(), floor);
                    ss << "("<<floor<<","<<weights.at(i)<<"," << countGen << "," <<countWri <<")," ;
                }
                std::cout << ss.str() << std::endl;
                
                if(floors.size()>=2){
                    fsize = floors.size();
                }
            }
        }
    };
   
   ```
   
   3. Still struggling to understand how are they choosing the mode which helps them in deciding which floor updating module to use? 
   
