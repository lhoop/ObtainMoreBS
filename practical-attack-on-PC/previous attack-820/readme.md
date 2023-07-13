## 1 Structure of the reference codes

820_phase1-I2.cu and 820_phase1-I3.cu:  calculate the value of 30 superpolys by 2^53 Trivium calls.  
820_phase2:  recover the correct key by 2^50 Trivium calls.


## 2 Usage of the codes 
1. Set the key on line 22 of 820_phase1-I2.cu and 820_phase1-I3.cu 

2. 'nvcc 820_phase1-I2.cu -o main'  &&  'nohup ./main &' and 'nvcc 820_phase1-I3.cu -o main2'  &&  'nohup ./main2 &'. Then the value of 30 equations will obtain in file "nohup.out".

3. Input the values of 30 equations (line 22-23 of 820_phase2.cu) and the key streams (832-896 rounds) corresponding to the correct key to 820_phase2.cu (line 303 of 820_phase2.cu).

4. 'nvcc 820_phase2.cu -o main'  &&  'nohup ./main &'. Then the value of the correct key will obtain in file "nohup.out".


## 3 Tip
In our time test, the whole process is expected to run for 19 days a PC with a A100 GPU (128 * 256 threads). 

