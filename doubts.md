## Pose.hpp

1. The definition uses a term called as normal velocity. What is that? 

2. Before changing the floor it always checks the following: 

```c++
if(building.isMovable(sTmp)){
    s.floor(repFloor);
}
```

Would be glad to understand why that is happening. 

3. Still struggling to understand how are they choosing the mode which helps them in deciding which floor updating module to use? 
   
4. What kind of a map is predict function returning in LogLikelhood estimation. I have an overview of it but not clear in that part. 

## Updates: 12 Jan 2019 

1. What do we mean by a global heading and a local heading? 

2. OrientationAdjuster.hpp
    Line 101, 102 why cos by sin 
    How accuracy is the standard deviation (line 114)


