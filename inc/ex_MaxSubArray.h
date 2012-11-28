 /*********************************************************************
 * Copyrights (c) Marwan Abdellah. All rights reserved.
 * This code is part of my Master's Thesis Project entitled "High
 * Performance Fourier Volume Rendering on Graphics Processing Units
 * (GPUs)" and submitted to the Systems & Biomedical Engineering
 * Department, Faculty of Engineering, Cairo University.
 * Please, don't use or distribute without authors' permission.

 * File         : ex_MaxSubArray.h
 * Author(s)    : Ahmed Abdel Salam <ahmed.abusalim.89@gmail.com>
		  Marwan Abdellah   <abdellah.marwan@gmail.com> 
		  Salah Saleh	    <salah_saleh@live.com>	  
 * Created      : July 2011
 * Description  :
 * Note(s)      :
 *********************************************************************/

#ifndef _EX_MAX_SUB_ARRAY_KERNEL_H_
#define _EX_MAX_SUB_ARRAY_KERNEL_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <omp.h>
#include <vector>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include "Shared.h"
#include <stdlib.h>
#include <sys/time.h>
    
/* @ This struct will hold the maximum of all strips can be found for each row */


namespace MaxSubArray
{
	void get_walltime_(double* wcTime);
	void readFile(char* fileName, int* inputArray, int numRows, int numCols);
	void getMax_CPU_CUDA(int* inputArray, int numCores, int numRows, int numCols, int numItr);
	void getMax_CPU_Prefix(int* inputArray, int numCores, int numRows, int numCols, int numItr);
	void getMax_CPU_Kadan(int* inputArray, int numCores, int numRows, int numCols, int numItr);	
	void getMax_CUDA(int* inputArray, int numRows, int numCols, int numItr);
}

#endif // _EX_MAX_SUB_ARRAY_KERNEL_H_
