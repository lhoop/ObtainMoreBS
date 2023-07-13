## 1 Structure of the reference codes

810_phase1:  calculate the value of 40 superpolys by 2^44 Trivium calls.  
810_phase2:  recover the correct key by 2^41 Trivium calls.


## 2 Usage of the codes 
1. Set the key on line 23 of 810_phase1.cu

2. 'nvcc 810_phase1.cu -o main'  &&  'nohup ./main &'. Then the value of 33 equations will obtain in file "nohup.out".

3. Input the values of 40 equations (line 23-24 of 810_phase2.cu) and the key streams (832-896 rounds) corresponding to the correct key to 810_phase2.cu (line 306 of 810_phase2.cu).

4. 'nvcc 810_phase2.cu -o main'  &&  'nohup ./main &'. Then the value of the correct key will obtain in file "nohup.out".


## 3 Tip
We give the results of our practical attack in "phase1_out.txt" and "phase2_out.txt", our experiment is conducted on a PC with a A100 GPU (128 * 256 threads). 
