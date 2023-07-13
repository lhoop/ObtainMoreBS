#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <stdio.h>
#include <iostream>

#include <ctime>
#include <chrono>

using namespace std;
#define TYPE unsigned int
typedef signed char u8;
#define BASE_CUDA_CHECK(condition) { GPUAssert((condition), __FILE__, __LINE__); }
typedef unsigned int u32;



__device__ void Trivium_device(u32* keystream, u32* iv, u32 key_index)
{

	u32 roundnum = 810;

	//the original secret variables  
	u32 key[10] = { 41,35,190,132,225,108,214,174,82,144 };

	u32 z1 = 0;
	u32 roundnum_word = 0;
	u32 roundnum_bit = 0;
	u32 t1, t2, t3, i = 0;
	u32 s0, s1, s2, s3, s4, s5, s6, s7, s8, s9 = 0;
	u32 temp1, temp2, temp3, temp4, temp5, temp6 = 0;
	u32 z = 0;

	s0 = key[0] ^ (key[1] << 8) ^ (key[2] << 16) ^ (key[3] << 24);  //32k


	s1 = key[4] ^ (key[5] << 8) ^ (key[6] << 16) ^ (key[7] << 24);  //32k


	s2 = key[8] ^ (key[9] << 8); 


	s3 = iv[0];

	s4 = iv[1];

	s5 = iv[2];


	s6 = 0;


	s7 = 0;


	s8 = 0;


	s9 = 0x00007000;  //7000  =  0111 + 12*0



	//65         92               161          176        242           287
	//s2[30]     s2[3]           s5[27]       s5[12]      s8[30]       s9[17]



	//64         91               160          175        241           286
	//s2[31]     s2[4]           s5[28]       s5[13]      s8[31]       s9[18]




	roundnum_word = roundnum / 32;
	roundnum_bit = roundnum % 32;
	for (i = 0;i < roundnum_word;i++)
	{

		temp1 = (s2 << 30) | (s1 >> 2);
		temp2 = (s2 << 3) | (s1 >> 29);
		temp3 = (s5 << 27) | (s4 >> 5);
		temp4 = (s5 << 12) | (s4 >> 20);
		temp5 = (s8 << 30) | (s7 >> 2);
		temp6 = (s9 << 17) | (s8 >> 15);

		t1 = temp1 ^ temp2;//((s2<<30)|(s1>>2))^((s2<<3)|(s1>>29))^(((s2<<5)|(s1>>27))&((s2<<4)|(s1>>28)))^(s5<<18)|(s4>>14);

		t2 = temp3 ^ temp4;//((s5<<27)|(s4>>5))^((s5<<12)|(s4>>20))^((s5<<14)|(s4>>18))&((s5<<13)|(s4>>19))^((s8<<9)|(s7>>23));

		t3 = temp5 ^ temp6;//((s8<<30)|(s7>>2))^((s9<<17)| (s8>>15))^

		//z = t1^t2^t3;

	//	t1 = t1 + s91s92 + s171
		temp1 = (s2 << 5) | (s1 >> 27);//(((s2<<5)|(s1>>27))&((s2<<4)|(s1>>28)))^(s5<<18)|(s4>>14);
		temp2 = (s2 << 4) | (s1 >> 28);
		temp3 = (s5 << 18) | (s4 >> 14);

		t1 ^= (temp1 & temp2) ^ temp3;

		//	t2 = t2 + s175s176 + s264
		temp1 = (s5 << 14) | (s4 >> 18);//(((s5<<14)|(s4>>18))&((s5<<13)|(s4>>19)))^((s8<<9)|(s7>>23));
		temp2 = (s5 << 13) | (s4 >> 19);
		temp3 = (s8 << 9) | (s7 >> 23);

		t2 ^= (temp1 & temp2) ^ temp3;

		//	t3 = t3 + s286s287 + s69
		temp1 = (s9 << 19) | (s8 >> 13);//(()&())^()
		temp2 = (s9 << 18) | (s8 >> 14);
		temp3 = (s2 << 27) | (s1 >> 5);

		t3 ^= (temp1 & temp2) ^ temp3;




		// update register 1
		s2 = (s1) & (0x1FFFFFFF);  //29λ
		s1 = s0;
		s0 = t3;

		//	update register 2
		s5 = s4 & (0x000FFFFF);
		s4 = s3;
		s3 = t1;

		//	update register 3
		s9 = s8 & (0x00007FFF);
		s8 = s7;
		s7 = s6;
		s6 = t2;
	}
	if (roundnum_bit != 0)
	{
		temp1 = (s2 << 30) | (s1 >> 2);
		temp2 = (s2 << 3) | (s1 >> 29);
		temp3 = (s5 << 27) | (s4 >> 5);
		temp4 = (s5 << 12) | (s4 >> 20);
		temp5 = (s8 << 30) | (s7 >> 2);
		temp6 = (s9 << 17) | (s8 >> 15);

		t1 = temp1 ^ temp2;
		t2 = temp3 ^ temp4;
		t3 = temp5 ^ temp6;
		z1 = temp1 ^ temp2 ^ temp3 ^ temp4 ^ temp5 ^ temp6;
		//z1=z;
		//t1 = t1 + s91s92 + s171
		temp1 = (s2 << 5) | (s1 >> 27);
		temp2 = (s2 << 4) | (s1 >> 28);
		temp3 = (s5 << 18) | (s4 >> 14);

		t1 ^= (temp1 & temp2) ^ temp3;

		//t2 = t2 + s175s176 + s264
		temp1 = (s5 << 14) | (s4 >> 18);
		temp2 = (s5 << 13) | (s4 >> 19);
		temp3 = (s8 << 9) | (s7 >> 23);

		t2 ^= (temp1 & temp2) ^ temp3;

		//t3 = t3 + s286s287 + s69
		temp1 = (s9 << 19) | (s8 >> 13);
		temp2 = (s9 << 18) | (s8 >> 14);
		temp3 = (s2 << 27) | (s1 >> 5);

		t3 ^= (temp1 & temp2) ^ temp3;

		// update register 1
		s2 = (s1) & (0x1FFFFFFF);
		s1 = s0;
		s0 = t3;

		//update register 2
		s5 = s4 & (0x000FFFFF);
		s4 = s3;
		s3 = t1;

		//update register 3
		s9 = s8 & (0x00007FFF);
		s8 = s7;
		s7 = s6;
		s6 = t2;
	}

	temp1 = (s2 << 30) | (s1 >> 2);
	temp2 = (s2 << 3) | (s1 >> 29);
	temp3 = (s5 << 27) | (s4 >> 5);
	temp4 = (s5 << 12) | (s4 >> 20);
	temp5 = (s8 << 30) | (s7 >> 2);
	temp6 = (s9 << 17) | (s8 >> 15);



	int outbit = 0;
	z = temp1 ^ temp2 ^ temp3 ^ temp4 ^ temp5 ^ temp6;

	if (roundnum_bit != 0)
		outbit = (z1 >> (31 - roundnum_bit)) & 0x1;
	else
		outbit = z >> 31;

	keystream[0] = outbit;
}

__global__ void global_multi_thread(u32* part_term_dev_divide, u32 loop) {
	//u32 loadkey[10] = { 41,35,190,132,225,108,214,174,82,144 };

	u32 tid_16, i;
	u32 keystream[1];
	u32 offset_28;
	u32 streambit = 0;
	u32 tmp;

	//0-2^16
	//0, 2, 4, 6, 8, 10, 11, 12, 14, 15, 17, 19, 20, 21, 22, 23
	tid_16 = loop;
	tmp = (threadIdx.x + blockIdx.x * blockDim.x);




	for (i = 0; i < 32; i++) {
		offset_28 = ((tmp << 5) | i);


		u32 cube[3] = { 0 };


		cube[0] = (tid_16 & 0x1) | (((tid_16 >> 1) & 0x1) << 2) | (((tid_16 >> 2) & 0x1) << 4) | (((tid_16 >> 3) & 0x1) << 6) | (((tid_16 >> 4) & 0x1) << 8) | (((tid_16 >> 5) & 0x1) << 10) | (((tid_16 >> 6) & 0x1) << 11) | (((tid_16 >> 7) & 0x1) << 12) | (((tid_16 >> 8) & 0x1) << 14) | (((tid_16 >> 9) & 0x1) << 15) | (((tid_16 >> 10) & 0x1) << 17) | (((tid_16 >> 11) & 0x1) << 19) | (((tid_16 >> 12) & 0x1) << 20) | (((tid_16 >> 13) & 0x1) << 21) | (((tid_16 >> 14) & 0x1) << 22) | (((tid_16 >> 15) & 0x1) << 23) | ((offset_28 & 0x1) << 25) | (((offset_28 >> 1) & 0x1) << 27) | (((offset_28 >> 2) & 0x1) << 29) | (((offset_28 >> 3) & 0x1) << 30);

		cube[1] = (((offset_28 >> 4) & 0x1)) | (((offset_28 >> 5) & 0x1) << 2) | (((offset_28 >> 6) & 0x1) << 4) | (((offset_28 >> 7) & 0x1) << 5) | (((offset_28 >> 8) & 0x1) << 7) | (((offset_28 >> 9) & 0x1) << 9) | (((offset_28 >> 10) & 0x1) << 11) | (((offset_28 >> 11) & 0x1) << 13) | (((offset_28 >> 12) & 0x1) << 15) | (((offset_28 >> 13) & 0x1) << 16) | (((offset_28 >> 14) & 0x1) << 18) | (((offset_28 >> 15) & 0x1) << 20) | (((offset_28 >> 16) & 0x1) << 21) | (((offset_28 >> 17) & 0x1) << 22) | (((offset_28 >> 18) & 0x1) << 23) | (((offset_28 >> 19) & 0x1) << 25) | (((offset_28 >> 20) & 0x1) << 28) | (((offset_28 >> 21) & 0x1) << 30);

		cube[2] = (((offset_28 >> 22) & 0x1)) | (((offset_28 >> 23) & 0x1) << 3) | (((offset_28 >> 24) & 0x1) << 6) | (((offset_28 >> 25) & 0x1) << 8) | (((offset_28 >> 26) & 0x1) << 11) | (((offset_28 >> 27) & 0x1) << 15);

		Trivium_device(keystream, cube, i);
		streambit |= ((keystream[0] & 0x01) << (i));



	}

	/*printf("%u", streambit);*/

	part_term_dev_divide[threadIdx.x + blockIdx.x * blockDim.x] = streambit;



	__syncthreads();
}


__global__ void  getsum(u32* part_term_host_divide, u32 loop, u32* sum1_dev, u32 dim) {
	u32 A = 0;
	u32 A1 = 0;
	u32 B = 0;
	u32 B_5 = 0;
	u32 index = threadIdx.x + blockIdx.x * blockDim.x;

	sum1_dev[index] = 0;

	for (A1 = dim * index; A1 < dim * (index + 1); A1++) {

		for (B_5 = 0; B_5 < 32; B_5++) {

			B = ((A1 << 5) | B_5);
			A = loop;

			if ((((B >> 11) & 0x1) == 0) && (((B >> 2) & 0x1) == 0) && (((A >> 8) & 0x1) == 0) && (((A >> 1) & 0x1) == 0)) {
				sum1_dev[index] ^= (((part_term_host_divide[A1]) >> (B_5)) & 0X1);
			}
			if ((((B >> 15) & 0x1) == 0) && (((B >> 2) & 0x1) == 0) && (((A >> 14) & 0x1) == 0) && (((A >> 11) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 1);
			}

			if ((((B >> 23) & 0x1) == 0) && (((B >> 16) & 0x1) == 0) && (((B >> 12) & 0x1) == 0) && (((A >> 6) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 2);
			}

			if ((((A >> 11) & 0x1) == 0) && (((A >> 6) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 3);
			}

			if ((((B >> 17) & 0x1) == 0) && (((B >> 15) & 0x1) == 0) && (((B >> 5) & 0x1) == 0) && (((A >> 6) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 4);
			}
			if ((((B >> 19) & 0x1) == 0) && (((B >> 14) & 0x1) == 0) && (((A >> 1) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 5);
			}

			if ((((B >> 3) & 0x1) == 0) && (((A >> 9) & 0x1) == 0) && (((A >> 6) & 0x1) == 0) && (((A >> 1) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 6);
			}

			if ((((B >> 19) & 0x1) == 0) && (((B >> 3) & 0x1) == 0) && (((A >> 9) & 0x1) == 0) && (((A >> 7) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 7);
			}
			if ((((B >> 21) & 0x1) == 0) && (((B >> 12) & 0x1) == 0) && (((B >> 9) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 8);
			}
			if ((((B >> 21) & 0x1) == 0) && (((B >> 19) & 0x1) == 0) && (((B >> 13) & 0x1) == 0) && (((A >> 9) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 9);
			}
			if ((((B >> 21) & 0x1) == 0) && (((B >> 17) & 0x1) == 0) && (((B >> 16) & 0x1) == 0) && (((B >> 2) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 10);
			}
			if ((((B >> 24) & 0x1) == 0) && (((B >> 23) & 0x1) == 0) && (((B >> 13) & 0x1) == 0) && (((B >> 2) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 11);
			}
			if ((((B >> 12) & 0x1) == 0) && (((B >> 10) & 0x1) == 0) && (((B >> 1) & 0x1) == 0) && (((A >> 6) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 12);
			}
			if ((((B >> 18) & 0x1) == 0) && (((B >> 12) & 0x1) == 0) && (((A >> 15) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 13);
			}
			if ((((B >> 27) & 0x1) == 0) && (((B >> 17) & 0x1) == 0) && (((B >> 7) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 14);
			}
			if ((((B >> 21) & 0x1) == 0) && (((A >> 12) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 15);
			}
			if ((((B >> 13) & 0x1) == 0) && (((B >> 21) & 0x1) == 0) && (((B >> 5) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 16);
			}
			if ((((B >> 20) & 0x1) == 0) && (((B >> 19) & 0x1) == 0) && (((A >> 8) & 0x1) == 0) && (((A >> 1) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 17);
			}
			if ((((B >> 21) & 0x1) == 0) && (((B >> 3) & 0x1) == 0) && (((A >> 6) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 18);
			}
			if ((((B >> 18) & 0x1) == 0) && (((B >> 2) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 19);
			}
			if ((((B >> 22) & 0x1) == 0) && (((B >> 20) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 20);
			}
			if ((((B >> 8) & 0x1) == 0) && (((B >> 3) & 0x1) == 0) && (((A >> 9) & 0x1) == 0) && (((A >> 7) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 21);
			}
			if ((((B >> 21) & 0x1) == 0) && (((B >> 7) & 0x1) == 0) && (((B >> 5) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 22);
			}
			if ((((B >> 17) & 0x1) == 0) && (((A >> 6) & 0x1) == 0) && (((A >> 2) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 23);
			}
			if ((((B >> 22) & 0x1) == 0) && (((B >> 18) & 0x1) == 0) && (((B >> 6) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 24);
			}
			if ((((B >> 20) & 0x1) == 0) && (((B >> 17) & 0x1) == 0) && (((A >> 11) & 0x1) == 0) && (((A >> 6) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 25);
			}
			if ((((B >> 17) & 0x1) == 0) && (((B >> 13) & 0x1) == 0) && (((A >> 10) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 26);
			}
			if ((((B >> 27) & 0x1) == 0) && (((B >> 12) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 27);
			}
			if ((((B >> 21) & 0x1) == 0) && (((B >> 0) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 28);
			}
			if ((((B >> 23) & 0x1) == 0) && (((B >> 21) & 0x1) == 0) && (((B >> 5) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 29);
			}
			if ((((B >> 26) & 0x1) == 0) && (((B >> 5) & 0x1) == 0) && (((B >> 2) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 30);
			}
			if ((((B >> 18) & 0x1) == 0) && (((B >> 15) & 0x1) == 0) && (((B >> 5) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 31);
			}




		}


	}

	__syncthreads();
}

__global__ void  getsum2(u32* part_term_host_divide, u32 loop, u32* sum1_dev, u32 dim) {
	u32 A = 0;
	u32 A1 = 0;
	u32 B = 0;
	u32 B_5 = 0;
	u32 index = threadIdx.x + blockIdx.x * blockDim.x;

	sum1_dev[index] = 0;
	for (A1 = dim * index; A1 < dim * (index + 1); A1++) {



		for (B_5 = 0; B_5 < 32; B_5++) {

			B = ((A1 << 5) | B_5);

			A = loop;

			if ((((B >> 12) & 0x1) == 0) && (((B >> 0) & 0x1) == 0) && (((A >> 15) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1));

			}

			if ((((A >> 6) & 0x1) == 0) && (((A >> 2) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 1);

			}

			if ((((B >> 21) & 0x1) == 0) && (((B >> 13) & 0x1) == 0) && (((B >> 5) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 2);

			}

			if ((((B >> 24) & 0x1) == 0) && (((B >> 23) & 0x1) == 0) && (((B >> 2) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 3);

			}

			if ((((B >> 20) & 0x1) == 0) && (((B >> 17) & 0x1) == 0) && (((B >> 15) & 0x1) == 0) && (((A >> 11) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 4);

			}

			if ((((B >> 20) & 0x1) == 0) && (((B >> 9) & 0x1) == 0) && (((A >> 11) & 0x1) == 0) && (((A >> 6) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 5);

			}

			if ((((A >> 11) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 6);

			}

			if ((((B >> 10) & 0x1) == 0) && (((A >> 11) & 0x1) == 0) && (((A >> 0) & 0x1) == 0)) {
				sum1_dev[index] ^= ((((part_term_host_divide[A1]) >> (B_5)) & 0X1) << 7);

			}



		}


	}

	__syncthreads();
}


__host__ void host_creat_memory(u32** part_term) {
	u32* part_term_dev_divide;
	u32* part_term_host_divide;
	u32 B, A;
	u32 i;
	u32 rate = 64 * 2;

	u32 bolcknum = 128 * rate, threadnum = 512;
	u32 loop, offset;
	u32 t_begin, t_end;


	loop = (0x1 << 16);


	u32 bolcknum_2 = (0x1 << 11), threadnum_2 = (0x1 << 5);
	u32 sum1_size = bolcknum_2 * threadnum_2;
	u32 dim = (bolcknum * threadnum) / sum1_size;


	u32* sum1_host;
	u32* sum2_host;
	u32* sum1_dev;
	u32* sum2_dev;

	sum1_host = (u32*)malloc(sizeof(u32) * sum1_size);
	cudaMalloc((void**)&sum1_dev, sizeof(u32*) * sum1_size);


	sum2_host = (u32*)malloc(sizeof(u32) * sum1_size);
	cudaMalloc((void**)&sum2_dev, sizeof(u32*) * sum1_size);

	part_term_host_divide = (u32*)malloc(sizeof(u32) * bolcknum * threadnum);
	cudaMalloc((void**)&part_term_dev_divide, sizeof(u32) * bolcknum * threadnum);

	









	//loop 16  
	for (B = 0;B < loop;B++) {

		//b   8
		//if (B == 16*16) {
		//	/*t_end = clock();
		//	printf("\n \n");
		//	printf("b: %u\n", t_begin);
		//	printf("e: %u\n", t_end);

		//	printf("b-e: %u\n", (t_end - t_begin)/ CLOCKS_PER_SEC);*/

		//	printf("B:%u\n", B);
		//	time_t end = time(nullptr);
		//	printf("time: %ld\n", (end - now));
		//	int enow = (end - now);
		//	float alltime = enow * 18.2/B;
		//	printf("maybetime:%f\n\n", (alltime));
		//	exit(0);
		//}

		global_multi_thread << <bolcknum, threadnum >> > (part_term_dev_divide, B);


		cudaMemcpy(part_term_host_divide, part_term_dev_divide, sizeof(u32) * bolcknum * threadnum, cudaMemcpyDeviceToHost);


		getsum << < bolcknum_2, threadnum_2 >> > (part_term_dev_divide, B, sum1_dev, dim);

		getsum2 << < bolcknum_2, threadnum_2 >> > (part_term_dev_divide, B, sum2_dev, dim);

		cudaMemcpy(sum1_host, sum1_dev, sum1_size * sizeof(u32), cudaMemcpyDeviceToHost);

		for (A = 0;A < sum1_size;A++)
		{
			part_term[0][0] ^= sum1_host[A];
		}

		cudaMemcpy(sum2_host, sum2_dev, sum1_size * sizeof(u32), cudaMemcpyDeviceToHost);



		for (A = 0;A < sum1_size;A++)
		{
			part_term[0][1] ^= sum2_host[A];

		}


	}

	//printf("allresult: %d \n", part_term[0][0]);

	cudaFree(part_term_dev_divide);
	free(part_term_host_divide);
	cudaFree(sum1_dev);
	free(sum1_host);
	cudaFree(sum2_dev);
	free(sum2_host);
}


u32 cpu_prepare() {
	u32** part_term;
	u32 part_num = 40;
	part_term = (u32**)malloc(sizeof(u32*));
	for (int i = 0;i < 1;i++)
		part_term[i] = (u32*)malloc(part_num * sizeof(u32));

	for (int a = 0;a < 1;a++) {
		for (int b = 0;b < part_num;b++)
		{
			part_term[a][b] = 0;
		}
	}
	host_creat_memory(part_term);


	u32 sum = 0;

	for (int a = 0;a < 1;a++) {
		for (int b = 0;b < 2;b++)
		{
			printf("equ-num %u:  %u \n", b, part_term[a][b]);
		}
	}


	for (int a = 0;a < 32;a++)
	{
		printf("%d", (part_term[0][0] >> a) & 0x1);
	}
	printf("\n");
	for (int a = 0;a < 8;a++)
	{
		printf("%d", (part_term[0][1] >> a) & 0x1);
	}





	return 0;
}

int main(int argc, char** argv)
{
	u32 c;
	time_t now = time(nullptr);
	cpu_prepare();

	time_t end = time(nullptr);
	printf("\ntime: %ld second\n", (end - now));

	return 0;

}

