## 1 Structure of the reference codes

808_phase1:  calculate the value of 37 superpolys by 2^44 Trivium calls.  
808_phase2:  recover the correct key by 2^43 Trivium calls.


## 2 Usage of the codes 
1. Set the key on line 22 of 808_phase1.cu

2. 'nvcc 808_phase1.cu -o main'  &&  'nohup ./main &'. Then the value of 37 equations will obtain in file "nohup.out".

3. Input the values of 37 equations (line 22-23 of 810_phase2.cu) and the key streams (832-896 rounds) corresponding to the correct key to 808_phase2.cu (line 306 of 808_phase2.cu).

4. 'nvcc 808_phase2.cu -o main'  &&  'nohup ./main &'. Then the value of the correct key will obtain in file "nohup.out".


## 3 Tip
We give the results of our practical attack in "phase1_out.txt" and "phase2_out.txt", our experiment is conducted on a PC with a A100 GPU (128 * 256 threads). 

Because there are several superpoly that have a lot of items, the second phase will take longer to compile and run.