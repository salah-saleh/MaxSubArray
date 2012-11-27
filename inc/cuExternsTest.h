 /*********************************************************************
 * Copyrights (c) Marwan Abdellah. All rights reserved.
 * This code is part of my Master's Thesis Project entitled "High
 * Performance Fourier Volume Rendering on Graphics Processing Units
 * (GPUs)" and submitted to the Systems & Biomedical Engineering
 * Department, Faculty of Engineering, Cairo University.
 * Please, don't use or distribute without authors' permission.

 * File         : cuExternsTest.h
 * Author(s)    : Marwan Abdellah   <abdellah.marwan@gmail.com> 
		  Salah Saleh	    <salah_saleh@live.com>	  
 * Created      : July 2011
 * Description  :
 * Note(s)      :
 *********************************************************************/

#ifndef CUEXTERNS_TEST_H_
#define CUEXTERNS_TEST_H_

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
#include <cutil_inline.h>


extern  
void cuPrefixSum(dim3 cuBlock, dim3 cuGrid, int* devInputArray, int* devPrefixSum, 
              int numRows, int numCols);

extern  
void cuGetMax(dim3 cuBlock, dim3 cuGrid, Max* devMaxValues, int* devPrefixSum, 
              int numRows, int numCols);

extern
void cuReduction(dim3 cuBlock, dim3 cuGrid, Max* devMaxValues, int numRows);

#endif /* CUEXTERNS_TEST_H_ */
