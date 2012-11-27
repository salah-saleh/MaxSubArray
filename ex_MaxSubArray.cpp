 /*********************************************************************
 * Copyrights (c) Marwan Abdellah. All rights reserved.
 * This code is part of my Master's Thesis Project entitled "High
 * Performance Fourier Volume Rendering on Graphics Processing Units
 * (GPUs)" and submitted to the Systems & Biomedical Engineering
 * Department, Faculty of Engineering, Cairo University.
 * Please, don't use or distribute without authors' permission.

 * File         : ex_MaxSubArray.cpp
 * Author(s)    : Ahmed Abdel Salam <ahmed.abusalim.89@gmail.com>
		  Marwan Abdellah   <abdellah.marwan@gmail.com> 
		  Salah Saleh	    <salah_saleh@live.com>	  
 * Created      : July 2011
 * Description  :
 * Note(s)      :
 *********************************************************************/


#include "cuExternsTest.h"
#include "ex_MaxSubArray.h"

using std::cout;
using std::endl;
using std::string;




void MaxSubArray::readFile(char* fileName, int* inputArray, int numRows, int numCols)
{
	cout<<"Reading file - Starting"<<endl;

	// Array indicies
	int xIdx = 0;
	int yIdx = 0;

	// Input stream
	std::ifstream inStream(fileName);

	if (inStream)
	{
		// Reading lineRow by lineRow
		std::string lineRow;

		// Initializing the Y index
		yIdx = 0;

		// Getting line by line
		while (std::getline(inStream, lineRow))
		{
			// Getting column by column
			std::stringstream split(lineRow);

			int inputVal;

			// Resetting the X index
			xIdx = 0;
			while (split >> inputVal)
			{
				// storing the input value from the file to the array
				inputArray[((yIdx * numRows) + xIdx)] = inputVal;

				// Incrementing the X idex
				++xIdx;
			}

			// Incrementing the y index
			++yIdx;
		}
	}

	// Closing the input stream
	cout<<"Closing the input stream"<<endl;
	inStream.close();

	cout<<"Reading file - Done"<<endl;
}

/*
 * This functions is divided in 2 stages. In STAGE_1 we search for the maximum
 * for each row equals to g. So if the 2D array is 1024x1024 we will have 1024
 * maximum. In STAGE_2, we perform a reduction process to get the maximum of  
 * these maximums.
 */
void MaxSubArray::getMax_CPU_Kad(int* inputArray, int numCores, int numRows, int numCols, int numItr)
{
	//INFO("Starting CPU implementation : Iterations " + ITS(numItr));

	/* CPU timing parameters */
	// timer should be placed here	
		
	// Iterate to average the results
	for (int itr = 0; itr < numItr; itr++)
	{
			
		/*
		* An array for holding the maximum values of all
		* possible combination
		*/
		Max* maxValues = (Max*)malloc(sizeof(Max)*numRows);		   
			
		/*
		* Start of parallel region inStream which we are going
		* to divide numRows on the number of threads, each thread
		* will calculate the maximum of all possible combination
		* and only store the maximum of them all inStream maxVal
		*/
		#pragma omp parallel num_threads(numCores)
		{	
			/*
		   	* Every element will hold the summation of all elements till 
			* it's position (operating on the original input array)
			*/ 
			int* prefixSum = (int*)malloc(sizeof(int)*numCols) ;			

			// This variable will hold the maximum summation found
			int candMaxSubArr = 0;

			// This variable will hold the starting column index
			int strCol_Idx = 0;

			// Will hold the minimum value found
			int maxVal = 0; 

			// @ STAGE_1 "Starting"
			#pragma omp single				
			//start = Timers::BoostTimers::getTime_MicroSecond(); //**** timer start

			/* 
			* In this part we will go through all possible combinations of the 2D array
			* and will reduce the 2D problem into 1D
			*/
			#pragma omp for schedule(dynamic)
			for(int strRow_Idx = 0; strRow_Idx < numRows; strRow_Idx++)
			{
				// Set maxVal to zero
				maxVal = 0;

				for(int iCtr = 0; iCtr < numCols; iCtr++)
					prefixSum[iCtr] = 0;
					
			   	/* Instead of having endRow_Idx start from strRow_Idx and have rows*(rows+1)/2
				* Combination, we will make endRow_Idx start from zero so that we have a unique 
				* indexing
				*/
				for(int endRow_Idx = strRow_Idx; endRow_Idx < numRows; endRow_Idx++)
				{							
					// Setting starting column index to zero
					strCol_Idx = 0;

					// Set candMaxSubArr to zero
					candMaxSubArr = 0;
							
					// Loop on all the elements within a row
					for(int endCol_Idx = 0; endCol_Idx < numCols; endCol_Idx++)
					{
						prefixSum[endCol_Idx] = prefixSum[endCol_Idx] + inputArray[endRow_Idx*numRows+endCol_Idx];

						// Calculating the candidate to be maximum value
						candMaxSubArr = candMaxSubArr + prefixSum[endCol_Idx];

						// Check if we have a new maximum
						if(candMaxSubArr > maxVal)
						{ 
							// Copying the info 
							maxVal = candMaxSubArr;
							maxValues[strRow_Idx].val = candMaxSubArr;
							maxValues[strRow_Idx].x1 = strRow_Idx;
							maxValues[strRow_Idx].y1 = strCol_Idx;
							maxValues[strRow_Idx].x2 = endRow_Idx;
							maxValues[strRow_Idx].y2 = endCol_Idx;
						}

						// Check if we have a new minimum
						if(candMaxSubArr < 0)
						{
							// Set stripPrefixSum to zero
							candMaxSubArr = 0;//stripPrefixSum = 0;
							// Set the starting column index to end column index plus one
							strCol_Idx = endCol_Idx + 1;
						}						
					}
				}
			}
		}

		// @ STAGE_1 "Done"
		//end = Timers::BoostTimers::getTime_MicroSecond(); //**** end timer

		// Calculate the duration of STAGE_1
		//cpuProfile = Timers::BoostTimers::getDuration(start, end); //**** duration calculation

		int selectedMaxVal = 0;
		int indexMaxValue=0;

		// @ STAGE_2 "Starting"
		//start = Timers::BoostTimers::getTime_MicroSecond(); //**** timer start

		// Search for the maximum inputVal inStream all maximum candidates
		for (int iCtr = 0;  iCtr < numRows; iCtr++)
		{
			if (maxValues[iCtr].val > selectedMaxVal)
			{
				// Set selected maximum value to the current found maximum
				selectedMaxVal = maxValues[iCtr].val;
				// Keeping track of the index
				indexMaxValue = iCtr;
			}
		}
		
		// @ STAGE_2 "Done"
		//end = Timers::BoostTimers::getTime_MicroSecond(); //**** timer end

		// Calculate the duration for STAGE_2
		//cpuProfile = Timers::BoostTimers::getDuration(start, end); // duration calculation

		cout<< "Kad: " << maxValues[indexMaxValue].val << " " << maxValues[indexMaxValue].x1 << " " << maxValues[indexMaxValue].y1 << " " << maxValues[indexMaxValue].x2 << " " << maxValues[indexMaxValue].y2 << endl;;
	}

	cout<<"CPU implementation - Done"<<endl;
}

/*
 * This functions is divided in 2 stages. In STAGE_1 we search for the maximum
 * for each row equals to g. So if the 2D array is 1024x1024 we will have 1024
 * maximum. In STAGE_2, we perform a reduction process to get the maximum of  
 * these maximums.
 */
void MaxSubArray::getMax_CPU_Dir(int* inputArray, int numCores, int numRows, int numCols, int numItr)
{
	//INFO("Starting CPU implementation : Iterations " + ITS(numItr));

	/* CPU timing parameters */
	// timer should be placed here	
		
	// Iterate to average the results
	for (int itr = 0; itr < numItr; itr++)
	{
			
		/*
		* An array for holding the maximum values of all
		* possible combination
		*/
		Max* maxValues = (Max*)malloc(sizeof(Max)*numRows);
		   
	   	/*
	   	* Every element will hold the summation of all elements till 
		* it's position (operating on the original input array)
		*/ 
		int* prefixSum = (int*)malloc(sizeof(int)*numRows*numCols) ;
			
		/*
		* Start of parallel region inStream which we are going
		* to divide numRows on the number of threads, each thread
		* will calculate the maximum of all possible combination
		* and only store the maximum of them all inStream maxVal
		*/
		#pragma omp parallel num_threads(numCores)
		{				
			   	
			// This variable will hold the strip prefix sum. 
			int stripPrefixSum = 0;

			// This variable will hold the maximum summation found
			int candMaxSubArr = 0;

			// This variable will hold the starting column index
			int strCol_Idx = 0;
				
			// Will hold the maximum value found
			int minVal = 0;

			// Will hold the minimum value found
			int maxVal = 0; 

			// @ STAGE_1 "Starting"
			#pragma omp single				
			//start = Timers::BoostTimers::getTime_MicroSecond(); //**** timer start

			// Copy the first row as it is
			#pragma omp for schedule(dynamic)
			for(int iCtr = 0; iCtr < numCols; iCtr++) 
				prefixSum[iCtr] = inputArray[iCtr];

			// Vertical summation (every element holds the summation of all the elements till it's position)
			#pragma omp for schedule(dynamic)
			for(int yCtr = 0; yCtr < numCols; yCtr++)
				for (int iCtr = 1; iCtr < numRows; iCtr++)					
					prefixSum[iCtr*numRows+yCtr] = prefixSum[(iCtr-1)*numRows+yCtr] + 
												inputArray[iCtr*numRows+yCtr];

			// Horizontal summation. Now prefix sum is obtained
			#pragma omp for schedule(dynamic)				
			for (int iCtr = 0; iCtr < numRows; iCtr++)
				for(int yCtr = 1; yCtr < numCols; yCtr++)					
					prefixSum[iCtr*numRows+yCtr] = prefixSum[iCtr*numRows+(yCtr-1)] + 
												prefixSum[iCtr*numRows+yCtr];  

			/* 
			* In this part we will go through all possible combinations of the 2D array
			* and will reduce the 2D problem into 1D
			*/
			#pragma omp for schedule(dynamic)
			for(int strRow_Idx = 0; strRow_Idx < numRows; strRow_Idx++)
			{
				// Set maxVal to zero
				maxVal = 0;
					
			   	/* Instead of having endRow_Idx start from strRow_Idx and have rows*(rows+1)/2
				* Combination, we will make endRow_Idx start from zero so that we have a unique 
				* indexing
				*/
				for(int endRow_Idx = strRow_Idx; endRow_Idx < numRows; endRow_Idx++)
				{
					// Setting prefix sum to zero
					stripPrefixSum = 0;
							
					// Setting starting column index to zero
					strCol_Idx = 0;
				
					// Set minVal to zero
					minVal = 0;

					// Set maxVal to zero
					//maxVal = 0;
							
					// Loop on all the elements within a row
					for(int endCol_Idx = 0; endCol_Idx < numCols; endCol_Idx++)
					{
						// Calculating strip prefix sum for first row     
						if (strRow_Idx == 0)
							stripPrefixSum = prefixSum[endRow_Idx*numRows+endCol_Idx];
						else
							// Calculating the strip prefix sum
							stripPrefixSum = prefixSum[endRow_Idx*numRows+endCol_Idx] - prefixSum[(strRow_Idx-1)*numRows+endCol_Idx];

						// Calculating the candidate to be maximum value
						candMaxSubArr = stripPrefixSum - minVal;

						// Check if we have a new maximum
						if(candMaxSubArr > maxVal)
						{ 
							// Copying the info 
							maxVal = candMaxSubArr;
							maxValues[strRow_Idx].val = candMaxSubArr;
							maxValues[strRow_Idx].x1 = strRow_Idx;
							maxValues[strRow_Idx].y1 = strCol_Idx;
							maxValues[strRow_Idx].x2 = endRow_Idx;
							maxValues[strRow_Idx].y2 = endCol_Idx;
						}

						// Check if we have a new minimum
						if(stripPrefixSum < minVal)
						{
							// Set stripPrefixSum to zero
							minVal = stripPrefixSum;//stripPrefixSum = 0;
							// Set the starting column index to end column index plus one
							strCol_Idx = endCol_Idx + 1;
						}						
					}
				}
			}
		}

		// @ STAGE_1 "Done"
		//end = Timers::BoostTimers::getTime_MicroSecond(); //**** end timer

		// Calculate the duration of STAGE_1
		//cpuProfile = Timers::BoostTimers::getDuration(start, end); //**** duration calculation

		int selectedMaxVal = 0;
		int indexMaxValue=0;

		// @ STAGE_2 "Starting"
		//start = Timers::BoostTimers::getTime_MicroSecond(); //**** timer start

		// Search for the maximum inputVal inStream all maximum candidates
		for (int iCtr = 0;  iCtr < numRows; iCtr++)
		{
			if (maxValues[iCtr].val > selectedMaxVal)
			{
				// Set selected maximum value to the current found maximum
				selectedMaxVal = maxValues[iCtr].val;
				// Keeping track of the index
				indexMaxValue = iCtr;
			}
		}
		
		// @ STAGE_2 "Done"
		//end = Timers::BoostTimers::getTime_MicroSecond(); //**** timer end

		// Calculate the duration for STAGE_2
		//cpuProfile = Timers::BoostTimers::getDuration(start, end); // duration calculation

		cout<< "numRows: " << maxValues[indexMaxValue].val << " " << maxValues[indexMaxValue].x1 << " " << maxValues[indexMaxValue].y1 << " " << maxValues[indexMaxValue].x2 << " " << maxValues[indexMaxValue].y2 << endl;;
	}

	cout<<"CPU implementation - Done"<<endl;
}


/*
 * This functions is divided in 2 stages. In STAGE_1 we search for the maximum
 * for each row equals to g. So if the 2D array is 1024x1024 we will have 1024
 * maximum. In STAGE_2, we perform a reduction process to get the maximum of  
 * these maximums.
 */
void MaxSubArray::getMax_CPU(int* inputArray, int numCores, int numRows, int numCols, int numItr)
{
	//INFO("Starting CPU implementation : Iterations " + ITS(numItr));

	/* CPU timing parameters */
	// timer should be placed here	

		
	// Iterate to average the results
	for (int itr = 0; itr < numItr; itr++)
	{
			
		/*
		* An array for holding the maximum values of all
		* possible combination
		*/
		Max* maxValues = (Max*)malloc(sizeof(Max)*numRows*numRows);
		   
	   	/*
	   	* Every element will hold the summation of all elements till 
		* it's position (operating on the original input array)
		*/ 
		int* prefixSum = (int*)malloc(sizeof(int)*numRows*numCols) ;
			
		/*
		* Start of parallel region inStream which we are going
		* to divide numRows on the number of threads, each thread
		* will calculate the maximum of all possible combination
		* and only store the maximum of them all inStream maxVal
		*/
		#pragma omp parallel num_threads(numCores)
		{				
			   	
			// This variable will hold the strip prefix sum. 
			int stripPrefixSum = 0;

			// This variable will hold the maximum summation found
			int candMaxSubArr = 0;

			// This variable will hold the starting column index
			int strCol_Idx = 0;
				
			// Will hold the maximum value found
			int minVal = 0;

			// Will hold the minimum value found
			int maxVal = 0; 

			// @ STAGE_1 "Starting"
			#pragma omp single				
			//start = Timers::BoostTimers::getTime_MicroSecond(); //**** timer start

			// Copy the first row as it is
			#pragma omp for schedule(dynamic)
			for(int iCtr = 0; iCtr < numCols; iCtr++) 
				prefixSum[iCtr] = inputArray[iCtr];

			// Vertical summation (every element holds the summation of all the elements till it's position)
			#pragma omp for schedule(dynamic)
			for(int yCtr = 0; yCtr < numCols; yCtr++)
				for (int iCtr = 1; iCtr < numRows; iCtr++)					
					prefixSum[iCtr*numRows+yCtr] = prefixSum[(iCtr-1)*numRows+yCtr] + 
												inputArray[iCtr*numRows+yCtr];

			// Horizontal summation. Now prefix sum is obtained
			#pragma omp for schedule(dynamic)				
			for (int iCtr = 0; iCtr < numRows; iCtr++)
				for(int yCtr = 1; yCtr < numCols; yCtr++)					
					prefixSum[iCtr*numRows+yCtr] = prefixSum[iCtr*numRows+(yCtr-1)] + 
												prefixSum[iCtr*numRows+yCtr];  

			/* 
			* In this part we will go through all possible combinations of the 2D array
			* and will reduce the 2D problem into 1D
			*/
			#pragma omp for schedule(dynamic)
			for(int strRow_Idx = 0; strRow_Idx < numRows; strRow_Idx++)
			{
			   	/* Instead of having endRow_Idx start from strRow_Idx and have rows*(rows+1)/2
				* Combination, we will make endRow_Idx start from zero so that we have a unique 
				* indexing
				*/
				for(int endRow_Idx = 0; endRow_Idx < numRows; endRow_Idx++)
				{
					// Set the current maximum value to zero
					maxValues[strRow_Idx*numRows+endRow_Idx].val = 0;

					// Check whether we are working within the right region
					if (endRow_Idx >= strRow_Idx)
					{
						// Setting prefix sum to zero
						stripPrefixSum = 0;
							
						// Setting starting column index to zero
						strCol_Idx = 0;
				
						// Set minVal to zero
						minVal = 0;

						// Set maxVal to zero
						maxVal = 0;
							
						// Loop on all the elements within a row
						for(int endCol_Idx = 0; endCol_Idx < numCols; endCol_Idx++)
						{
							// Calculating strip prefix sum for first row     
							if (strRow_Idx == 0)
								stripPrefixSum = prefixSum[endRow_Idx*numRows+endCol_Idx];
							else
								// Calculating the strip prefix sum
								stripPrefixSum = prefixSum[endRow_Idx*numRows+endCol_Idx] - prefixSum[(strRow_Idx-1)*numRows+endCol_Idx];

							// Calculating the candidate to be maximum value
							candMaxSubArr = stripPrefixSum - minVal;

							// Check if we have a new maximum
							if(candMaxSubArr > maxVal)
							{ 
								// Copying the info 
								maxVal = candMaxSubArr;
								maxValues[strRow_Idx*numRows+endRow_Idx].val = candMaxSubArr;
								maxValues[strRow_Idx*numRows+endRow_Idx].x1 = strRow_Idx;
								maxValues[strRow_Idx*numRows+endRow_Idx].y1 = strCol_Idx;
								maxValues[strRow_Idx*numRows+endRow_Idx].x2 = endRow_Idx;
								maxValues[strRow_Idx*numRows+endRow_Idx].y2 = endCol_Idx;
							}

							// Check if we have a new minimum
							if(stripPrefixSum < minVal)
							{
								// Set stripPrefixSum to zero
								minVal = stripPrefixSum;//stripPrefixSum = 0;
								// Set the starting column index to end column index plus one
								strCol_Idx = endCol_Idx + 1;
							}
						}
					}
				}
			}
		}

		// @ STAGE_1 "Done"
		//end = Timers::BoostTimers::getTime_MicroSecond(); //**** end timer

		// Calculate the duration of STAGE_1
		//cpuProfile = Timers::BoostTimers::getDuration(start, end); //**** duration calculation

		int selectedMaxVal = 0;
		int indexMaxValue=0;

		// @ STAGE_2 "Starting"
		//start = Timers::BoostTimers::getTime_MicroSecond(); //**** timer start

		// Search for the maximum inputVal inStream all maximum candidates
		for (int iCtr = 0;  iCtr < (numRows*numRows); iCtr++)
		{
			if (maxValues[iCtr].val > selectedMaxVal)
			{
				// Set selected maximum value to the current found maximum
				selectedMaxVal = maxValues[iCtr].val;
				// Keeping track of the index
				indexMaxValue = iCtr;
			}
		}
		
		// @ STAGE_2 "Done"
		//end = Timers::BoostTimers::getTime_MicroSecond(); //**** timer end

		// Calculate the duration for STAGE_2
		//cpuProfile = Timers::BoostTimers::getDuration(start, end); // duration calculation

		cout<< "numRows*numRows: " << maxValues[indexMaxValue].val << " " << maxValues[indexMaxValue].x1 << " " << maxValues[indexMaxValue].y1 << " " << maxValues[indexMaxValue].x2 << " " << maxValues[indexMaxValue].y2 << endl;
	}

	cout<<"CPU implementation - Done"<<endl;
}

/*
 * This functions is divided in 2 stages. In STAGE_1 we search for the maximum
 * for each row equals to g. So if the 2D array is 1024x1024 we will have 1024
 * maximum. In STAGE_2, we perform a reduction process to get the maximum of  
 * these maximums.
 */
void MaxSubArray::getMax_CUDA(int* hostInputArray, int numRows, int numCols, int numItr)
{
	cout<<"Starting CUDA implementation"<<endl;

	for (int itr = 0; itr < numItr; itr++)
	{
		// Memory required for input & output arrays
		const int inputArraySize  = sizeof(int) * numRows * numCols;
		const int prefixSum       = sizeof(int) * numRows * numCols;
		const int outputArraySize = sizeof(Max) * numRows * numRows;

		// Input & output arrays on the device side
		int* devInputArray;
		int* devPrefixSum;
		Max* devMaxValues;

		// the variable that will hold the maximum of all possible combination on the host side
		Max* hostMaxValue = (Max*)malloc(sizeof(Max)*1);//outputArraySize);

		// Allocating the device arrays
		cutilSafeCall(cudaMalloc((void**)&devInputArray, inputArraySize));
		cutilSafeCall(cudaMalloc((void**)&devPrefixSum, prefixSum));
		cutilSafeCall(cudaMalloc((void**)&devMaxValues, outputArraySize));

		// Upload the input array to the device side
		cutilSafeCall(cudaMemcpy(devInputArray, hostInputArray, inputArraySize, cudaMemcpyHostToDevice));

		/////////////////////////////////////////////////////////////////////////////////////////////////

		// Configuring the GPU
		dim3 cuBlock_1(numRows, 1, 1);
		dim3 cuGrid_1(numRows/cuBlock_1.x, 1, 1);

		// Invokig the CUDA kernel
		cuPrefixSum(cuBlock_1, cuGrid_1, devInputArray, devPrefixSum, numRows, numCols);
			
		// Checking if kernel execution failed or not
		cutilCheckMsg("Kernel (cuPrefixSum) execution failed \n");

		/////////////////////////////////////////////////////////////////////////////////////////////////

		// Configuring the GPU
		dim3 cuBlock_2(32, 32, 1);
		dim3 cuGrid_2(numRows/cuBlock_2.x, numRows/cuBlock_2.y, 1);

		// Invokig the CUDA kernel
		cuGetMax(cuBlock_2, cuGrid_2, devMaxValues, devPrefixSum, numRows, numCols);

		// Checking if kernel execution failed or not
		cutilCheckMsg("Kernel (cuGetMax) execution failed \n");
			
		////////////////////////////////////////////////////////////////////////////////////////////////////

		// Configuring the GPU
		dim3 cuBlock_3(1024, 1, 1);
		dim3 cuGrid_3((numRows*numRows)/(2*cuBlock_3.x), 1, 1);

		// Invokig the CUDA kernel
		cuReduction(cuBlock_3, cuGrid_3, devMaxValues, numRows);

		// Checking if kernel execution failed or not
		cutilCheckMsg("Kernel (cuReduction) execution failed \n");

		///////////////////////////////////////////////////////////////////////////////////////////////////////

		// Download the maxValues array to the host side
		cutilSafeCall(cudaMemcpy(hostMaxValue, devMaxValues, sizeof(Max)*1, cudaMemcpyDeviceToHost));//outputArraySize

		// Freeing the allocated memory on the device
		cudaFree(devMaxValues);
			
//			int selectedMaxVal = 0;
//			int indexMaxVal = 0;

//			// Search for the maximum value in all maximum candidates
//			for (int i = 0; i < (numRows*numRows); i++)
//			{
//				if (hostMaxValue[i].val > selectedMaxVal)
//				{
//					// Updating the selected values
//					selectedMaxVal = hostMaxValue[i].val;
//
//					// Updating the index
//					indexMaxVal = i;
//				}
//			}
		
		cout<< hostMaxValue->val << endl;
		// Freeing the allocated memory on the host
		free(hostMaxValue);
	}

	cout<<"CUDA implementation Done"<<endl;
}
