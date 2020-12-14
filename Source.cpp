#include <iostream>
#include <vector>
#include <chrono>
#include <time.h>
#include <stdlib.h>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>


using namespace std;

void testOrderStatistic(int N, std::ofstream& outFile);
int getNum(std::vector<int>& v);
void generateRandom(int A[], int n);
int modifiedPartition(int* arr, int l, int r, int k, int& counter);
int findKthSmallestElement(int* A, int left, int right, int k, int& counter);
void insertionSort(int A[], int n, int& counter);
void swap(int& a, int& b, int& counter);
bool filterImage(int wSize, string fileName);


int main()
{
	
	//*****************************MAIN MENU***************************************
	cout << "Order Statistic and Median Image Filtering Final Project\n";
	cout << "By Konstantin Novichenko, December 2020\n\n\n";

	cout << "\t\t\tOPTIONS\n";
	cout << "1. Run Order Statistic Experiment on N = 5000, 8000, 10000 for randomly choosen k in range from 1 to k = 100, 300, 500, 1000, 2000, 4000\n";
	cout << "   Every setup will be tested on sorted array, reversed array, random permutations array, and 50 random arrays - all of size N\n\n";
	cout << "2. Run Median Image Filtering Experiment for a specific noise image and a specific window size\n";
	cout << "   WARNING: The range of image indices is from 1 to 20 inclusive and the range of the window sizes starts from 2 and restricted by the image size\n\n";
	cout << "3. Run Median Image Filtering Experiment for a specific window size for all 20 images\n";
	cout << "   WARNING: The range of window sizes starts from 2 and restricted by the image size\n\n";
	cout << "4. Run Median Image Filtering Experiment window sizes from 2 to 10 inclusive for all 20 images\n";
	cout << "   WARNING: This will take a long time due to the image size. Please, see the completed collection in the files\n\n";

	string userInput;

	cout << "\nEnter the option number: ";
	cin >> userInput;

	if (userInput == "1")
	{
		cout << "\n\n*******************OPTION 1******************\n\n";

		//*******************ORDER OF STATISTICS ALGORITHM************

		srand(clock()); // set seed
		ofstream outFile;
		ifstream inFile;

		// out stream file for Order Statistic experiment
		outFile.open("data.csv");
		outFile << "N,n,array_type,k,execution_time,num_of_steps\n";

		int N[3] = { 5000, 8000, 10000 };

		for (int i = 0; i < 3; i++)
		{
			testOrderStatistic(N[i], outFile);
		}

		outFile.close(); // Stop the file stream	
	}
	else if (userInput == "2")
	{
		
		cout << "\n\n*******************OPTION 2******************\n\n";

		cout << "Please Enter Image Index: ";
		string imageIndex = "";
		cin >> imageIndex;

		cout << "Please Enter Window Size: ";
		int wSize = 2;
		cin >> wSize;
		
		// combine user input into a file name string
		string fileNameString = "image_" + imageIndex + ".pgm";

		cout << "\nProcessing: " << fileNameString << "\n";
		cout << "Windows size: " << wSize << "\n";
		filterImage(wSize, fileNameString);

	}
	else if (userInput == "3")
	{
		cout << "\n\n*******************OPTION 3******************\n\n";


		cout << "Please Enter Window Size: ";
		int wSize = 2;
		cin >> wSize;

		for (int i = 1; i <= 20; i++) 
		{
			// combine index into a file name string
			string fileNameString = "image_" + to_string(i) + ".pgm";

			cout << "\nProcessing: " << fileNameString << "\n";
			cout << "Windows size: " << wSize << "\n";
			filterImage(wSize, fileNameString);
		}
	}
	else if (userInput == "4")
	{
		cout << "\n\n*******************OPTION 4******************\n\n";

		////*******************WINDOW COMPARRISON ON ALL 20 IMAGES***************
		for (int i = 1; i <= 20; i++)
		{
			// combine index into a file name string
			string fileNameString = "image_" + to_string(i) + ".pgm";

			for (int j = 2; j <= 10; j++)
			{
				cout << "\nProcessing: " << fileNameString << "\n";
				cout << "Windows size: " << j << "\n";
				filterImage(j, fileNameString);
			}
		}
	}
	else
	{
		cout << "\n\n*******************ERROR! INVALID INPUT!******************\n\n";
	}

	
	cout << "\n***************************************************************************\n";
	
	return 0;
	system("PAUSE");
}

bool filterImage(int wSize, string fileName)
{
	// Local file streams
	ofstream outFile;
	ifstream inFile;

	// Used for reading from the file
	ifstream infile(fileName);
	stringstream ss;
	ss << infile.rdbuf(); // assign reading from the file to the string stream

	// This data is saved for later to insert into the new file as a header
	string pData = "";
	string iData = "";

	// This used to keep track of dimension of the matrix
	int numRows = 0;
	int numColumns = 0;
	
	// Remove data about the image from PGM file (first three lines - P version, size rows by columns, and max intensity of each pixel)
	ss >> pData >> numColumns >> numRows >> iData;

	//Instantiate a pointer to a 2D dynamic array of the same size as the image
	int** image;
	image = new int* [numRows]; // populate rows with pointers to 1D dynamic arrays

	// instantiate a dynamic array in each row
	for (int i = 0; i < numRows; i++)
	{
		image[i] = new int[numColumns];
	}

	// Copy data from the image to the 2D array
	for (int i = 0; i < numRows; i++)
	{
		for (int j = 0; j < numColumns; j++)
		{
			// Pass data about the pixel from the file
			ss >> image[i][j];
		}
	}

	// close the input stream for the file
	inFile.close();

	// Array that contains data about the current subwindow
	int* subArray = new int[wSize * wSize];

	cout << "Filtering. Please, wait.\nIn Progress"; // Progress message

	// Traverse through the image
	for (int i = 0; i < numRows - wSize; i++) // rows
	{

		for (int j = 0; j < numColumns - wSize; j++) //columns
		{
			
			// Traverse through the current subwindow using subArray
			for (int k = 0; k < wSize; k++)
			{
				for (int h = 0; h < wSize; h++)
				{
					subArray[(k * wSize) + h] = image[i + k][j + h]; // Get a pixel value and insert in in the subwindow array
				}
			}

			int counter = 0; // counter is used by Order Statistic experiment for insertion sort and not valid for this experiment

			// run insertion sort on the subarray 
			// NOTE: most of the values of window size that are useable and not blurring the image are relatively small, thus insertion sort is efficient to use
			insertionSort(subArray, wSize * wSize, counter);

			// get the median value from the sorted array
			int medianValue = subArray[(wSize * wSize) / 2 - 1];

			// set the median to the middle of the current subwindow
			image[i + (wSize / 2) - 1][j + (wSize / 2) - 1] = medianValue;
			
		}
		cout << "."; // output progress
	}
	cout << "\nDone.\n"; // completion message

	// clear memory
	delete[] subArray;
	subArray = nullptr;

	// generate appropriate name for the output file
	string newFileName = "processed_images/" + fileName.substr(0, fileName.size() - 4) +"_filtered_size_" + to_string(wSize) + ".pgm";
	
	// Insert data about the image for PGM file to read properly
	outFile.open(newFileName);
	outFile << pData << endl;
	outFile << numColumns << " " << numRows << endl;
	outFile << iData << endl;

	// write to the output file
	for (int i = 0; i < numRows ; i++)
	{
		for (int j = 0; j < numColumns; j++)
		{
			outFile << image[i][j] << " ";
		}
	}

	// close output stream for the file
	outFile.close();
	
	// clear memory
	delete[] image;
	image = nullptr;

	// return true on completion
	return true;
}

// Performs the insertion sort algorithm
void insertionSort(int A[], int n, int& counter) 
{	
	counter++; // count int i = 1
	for (int i = 1; i < n; i++)
	{
		int key = A[i];
		int j = i - 1;

		while (j >= 0 && key < A[j])
		{
			A[j + 1] = A[j];
			j--;
			counter += 4; // count j >= 0, key < A[j], A[j + 1] = A[j], j--;
		}

		A[j + 1] = key;
		counter += 7; // count i < n, int key = A[i], int j = i - 1, j >= 0, key < A[j], A[j + 1] = key, i++
	}
	counter++; // count last i < n
}




// Returns K'th smallest element in the array
int findKthSmallestElement(int* A, int left, int right, int k, int& counter)
{
	// k is within the bounds
	if (k > 0 && k <= right - left + 1)
	{
		int n = right - left + 1; // total number of elements
 
		int i; // counter for number of medians 
		int medianOfMedians = -1; 
		int* arrayMedians; //  array of all medians

		counter += 5;
		// find the size of the array of all medians
		if (n % 5 != 0)
		{
			arrayMedians = new int[n / 5 + 1]; 
		}
		else
		{
			arrayMedians = new int[n / 5];
		}

		counter += 3;

		// find all the medians from the subarrays of size 5
		for (i = 0; i < n / 5; i++)
		{
			// run insertion sort on subarray of size 5
			insertionSort(A + left + i * 5, 5, counter);
			arrayMedians[i] = A[left + i * 5 + 2]; // get the middle element of subarray as a median
			counter += 4;
		
		}
			
		// if exists, get the the mesian for the last group that has size less than 5
		if (i * 5 < n) 
		{
			// run insertion sort on subarray of size n % 5
			insertionSort(A + left + i * 5, n % 5, counter);
			arrayMedians[i] = A[left + i * 5 + ((n%5)/2)]; // get the middle element of subarray as a median
			i++; //increment the total number of medians

			counter += 3;
		}
		counter++;

		// if the total number of medians is one, set it as a median of all medians.
		// otherwise, continue looking for it recursively
		if (i == 1)
		{
			medianOfMedians = arrayMedians[i - 1];
		}
		else
		{
			medianOfMedians = findKthSmallestElement(arrayMedians, 0, i - 1, i / 2, counter);
		}
		
		counter += 2;

		// partition the array around a median of all medians
		int pos = modifiedPartition(A, left, right, medianOfMedians, counter);

		// if position is same as Kth element - found
		counter += 2;
		if (pos - left == k - 1)
		{
			// clear memory
			delete[] arrayMedians;
			arrayMedians = nullptr;

			counter += 3;
			return A[pos];
		}
			
		counter++;
		if (pos - left > k - 1)  // if greater - continue looking recursively
		{
			// clear memory
			delete[] arrayMedians;
			arrayMedians = nullptr;

			counter += 3;
			return findKthSmallestElement(A, left, pos - 1, k, counter);
		}
			
		// clear memory
		delete[] arrayMedians;
		arrayMedians = nullptr;

		counter += 3;
		// in any other case - continue looking in the lower part recursively
		return findKthSmallestElement(A, pos + 1, right, k - pos + left - 1, counter);
	}

	counter++;
	// out of bounds, prevent overflow
	return INT_MAX;
}

void swap(int& a, int& b, int& counter)
{
	int temp = a;
	a = b;
	b = temp;
	counter += 3;
}

// Modified standard partition algorithm that partitions the array around the pivot 
int modifiedPartition(int* A, int left, int right, int p, int& counter)
{
	// Modified
	int i;
	counter += 3;

	// look for the pivot in the array within the bound from left to right
	for (i = left; i < right; i++)
	{
		if (A[i] == p)
		{
			counter += 2;
			break;
		}
		counter+=3;
			
	}
		
	counter+=2;
	swap(A[i], A[right], counter); // swap it

	// Standard
	i = left;
	counter += 3;
	for (int j = left; j <= right - 1; j++)
	{
		if (A[j] <= p)
		{
			swap(A[i], A[j], counter);
			i++;
		}
		counter += 3;
	}
	counter += 3;
	swap(A[i], A[right], counter);

	return i;
}



// Function that gets the random number from the vector
int getNum(std::vector<int>& v)
{
	// Size of the vector 
	int n = v.size();

	// Make sure the number is within 
	// the index range 
	int index = rand() % n;

	// Get random number from the vector 
	int num = v[index];

	// Remove the number from the vector 
	int t = v[index];
	v[index] = v[n - 1];
	v[n - 1] = t;
	v.pop_back();

	// Return the removed number 
	return num;
}

// Function to generate n non-repeating random numbers 
void generateRandom(int A[], int n)
{
	std::vector<int> v(n);

	// Fill the vector with the values 
	// 1, 2, 3, ..., n 
	for (int i = 0; i < n; i++)
		v[i] = i + 1;

	// While vector has elements 
	// get a random number from the vector and insert it into the array
	int index = 0;
	while (v.size()) {
		A[index] = getNum(v);
		index++;
	}
}



void testOrderStatistic(int N, std::ofstream& outFile)
{
	int n[6] = { 100, 300, 500, 1000, 2000, 4000 };
	// Inititalize time points
	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
	int counter = 0;


	// Initialize dynamic arrays of size N
	int* alreadySortedArray = new int[N];
	int* reversedArray = new int[N];
	int* randomArray = new int[N];
	

	// Initialize average step and time counters (used for 50 random arrays)
	long long averageTime = 0;
	long long averageSteps = 0;

	// Get random permutations for the first array
	generateRandom(randomArray, N);

	// Loops that generates Sorted, Reversed, Random Permunation of N arrays
	for (int i = 0; i < N; i++)
	{
		// Sorted
		alreadySortedArray[i] = i + 1;	

		// Reversed
		reversedArray[i] = N - i;		
	}


	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			int randomK = rand() % n[i] + 1;


			// ***************************************Sorted Array***********************************************

			cout << std::setfill('-') << std::setw(100) << "-" << std::setfill(' ') << std::endl; // Formating lines	
			
			start = std::chrono::steady_clock::now(); // Set timer
			findKthSmallestElement(alreadySortedArray, 0, N - 1, randomK, counter);
			end = std::chrono::steady_clock::now();	// Stop timer
			

			// Output
			cout << "\nSAMPLE (N = " << N << ", n = "<< n[i] << ", k = " << randomK<<")\n\n";
			cout << std::setw(30) << "ORDER STATISTICS SMALLEST Kth ELEMENT for Sorted Array" << "\n";

			cout << "Number of operations" << "\tExecution Time (ns)\n";

			cout << std::setw(20) << counter;
			cout << " " << std::setw(22) << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "\n";

			// Output results in .csv file
			outFile << N << ","<< n[i] << ",sorted," << randomK << ","<< std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "," << counter << "\n";

			// Reset steps counter
			counter = 0;

			// ***************************************Reversed Array***********************************************

			cout << std::setfill('-') << std::setw(100) << "-" << std::setfill(' ') << std::endl; // Formating lines	

			start = std::chrono::steady_clock::now(); // Set timer
			findKthSmallestElement(reversedArray, 0, N - 1, randomK, counter);
			end = std::chrono::steady_clock::now();	// Stop timer

			// Output
			cout << "\nSAMPLE (N = " << N << ", n = " << n[i] << ", k = " << randomK << ")\n\n";
			cout << std::setw(30) << "ORDER STATISTICS SMALLEST Kth ELEMENT for Reversed Array" << "\n";

			cout << "Number of operations" << "\tExecution Time (ns)\n";

			cout << std::setw(20) << counter;
			cout << " " << std::setw(22) << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "\n";

			// Output results in .csv file
			outFile << N << "," << n[i] << ",reversed," << randomK << "," << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "," << counter << "\n";

			// Reset steps counter
			counter = 0;
			

			// ***************************************Reversed Array***********************************************

			cout << std::setfill('-') << std::setw(100) << "-" << std::setfill(' ') << std::endl; // Formating lines	

			start = std::chrono::steady_clock::now(); // Set timer
			findKthSmallestElement(randomArray, 0, N - 1, randomK, counter);
			end = std::chrono::steady_clock::now();	// Stop timer

			// Output
			cout << "\nSAMPLE (N = " << N << ", n = " << n[i] << ", k = " << randomK << ")\n\n";
			cout << std::setw(30) << "ORDER STATISTICS SMALLEST Kth ELEMENT for Random Permutations Array" << "\n";

			cout << "Number of operations" << "\tExecution Time (ns)\n";

			cout << std::setw(20) << counter;
			cout << " " << std::setw(22) << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "\n";

			// Output results in .csv file
			outFile << N << "," << n[i] << ",random_sequence," << randomK << "," << std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() << "," << counter << "\n";

			// Reset steps counter
			counter = 0;
			
			// ***************************************50 Random Instances***********************************************

			for (int k = 0; k < 50; k++) // 50 pairs of random arrays
			{
				for (int s = 0; s < N; s++) // Populate the current pair with random numbers in range from 1 to N
				{
					int randomNumberTemp = rand() % N + 1;
					randomArray[s] = randomNumberTemp;					
				}


				start = std::chrono::steady_clock::now(); // Set timer
				findKthSmallestElement(randomArray, 0, N - 1, randomK, counter);
				end = std::chrono::steady_clock::now(); // Stop timer


				// Increment the average values of time and steps for insertion sort, then reset step counter
				averageTime += (long long)(std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count());
				averageSteps += (long long)counter;
				counter = 0;
			
				
			}


			// Output
			cout << "\nSAMPLE (N = " << N << ", n = " << n[i] << ", k = " << randomK << ")\n\n";
			cout << std::setw(30) << "ORDER STATISTICS SMALLEST Kth ELEMENT for Average 50 Random Arrays" << "\n";

			cout << "Number of operations" << "\tExecution Time (ns)\n";

			cout << std::setw(20) << averageSteps / 50;
			cout << " " << std::setw(22) << averageTime / 50 << "\n";
			

			// Output results in .csv file
			outFile << N << "," << n[i] << ",random_50," << randomK  << "," << averageTime / 50 << "," << averageSteps / 50 << "\n";
			cout << std::setfill('-') << std::setw(100) << "-" << std::setfill(' ') << std::endl; // Formating lines
			averageTime = 0;
			averageSteps = 0;
		}
	}

	// Clear memory
	delete[] alreadySortedArray;
	delete[] reversedArray;
	delete[] randomArray;
	alreadySortedArray = nullptr;
	reversedArray = nullptr;
	randomArray = nullptr;
}











