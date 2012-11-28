/*********************************************************************
 * Copyrights (c) Marwan Abdellah. All rights reserved.
 * This code is part of my Master's Thesis Project entitled "High
 * Performance Fourier Volume Rendering on Graphics Processing Units
 * (GPUs)" and submitted to the Systems & Biomedical Engineering
 * Department, Faculty of Engineering, Cairo University.
 * Please, don't use or distribute without authors' permission.

 * File         : Run.cpp
 * Author(s)    : Marwan Abdellah <abdellah.marwan@gmail.com>
 * Created      : July 2012
 * Description  :
 * Note(s)      :
 *********************************************************************/

#define MAX_SIZE_1D 1024


#include "ex_MaxSubArray.h"
#include "Shared.h"

int main(int argc, char** argv)
{
	// Read the input image files
	char* fileName = argv[1];

	int numCores = atoi (argv[2]);

	int arrSize_1D = atoi (argv[3]);

	int numItr = atoi (argv[4]);

	// Allocating the input array
	int* inputArray = (int*) malloc(sizeof(int) * arrSize_1D * arrSize_1D);

    	MaxSubArray::readFile(fileName, inputArray, arrSize_1D, arrSize_1D);

	// Do the CPU implementation with OpenMP
    	std::cout<<"CPU implementation with OpenMP-CUDA"<< std::endl;
    	MaxSubArray::getMax_CPU_CUDA(inputArray, numCores, arrSize_1D, arrSize_1D, numItr);

	// Do the CPU implementation with OpenMP
    	std::cout<<"CPU implementation with OpenMP-Kadan"<< std::endl;
    	MaxSubArray::getMax_CPU_Kadan(inputArray, numCores, arrSize_1D, arrSize_1D, numItr);

	// Do the CPU implementation with OpenMP
    	std::cout<<"CPU implementation with OpenMP-Prefix"<< std::endl;
    	MaxSubArray::getMax_CPU_Prefix(inputArray, numCores, arrSize_1D, arrSize_1D, numItr);

	// GPU implementation CUDA
    	//std::cout<<"GPU implementation with CUDA"<< std::endl;
    	//MaxSubArray::getMax_CUDA(inputArray, arrSize_1D, arrSize_1D, numItr);

	// Freeing the memory
	free(inputArray);

	return 0;
}


