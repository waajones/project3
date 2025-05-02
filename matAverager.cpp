//William Jones project3

#include <thread>
#include <chrono>
#include <fstream>
#include <iostream>
#include <omp.h>
#include <sstream>
#include <string>

using namespace std;

// a class to get more accurate time
class stopwatch{
	
private:
	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2;
	bool timing;
	
public:
	stopwatch( ): timing( false ) {
		t1 = std::chrono::high_resolution_clock::time_point::min();
		t2 = std::chrono::high_resolution_clock::time_point::min();
	}
	
	void start( ) {
		if( !timing ) {
			timing = true;
			t1 = std::chrono::high_resolution_clock::now();
		}
	}
	
	void stop( ) {
		if( timing ) {
			t2 = std::chrono::high_resolution_clock::now();
			timing = false;
		}
	}
	
	void reset( ) {
		t1 = std::chrono::high_resolution_clock::time_point::min();
		t2 = std::chrono::high_resolution_clock::time_point::min();;
		timing = false;
	}
	
	// will return the elapsed time in seconds as a double
	double getTime( ) {
		std::chrono::duration<double> elapsed = std::chrono::duration_cast<std::chrono::duration<double>>(t2-t1);
		return elapsed.count();
	}
};



// function takes an array pointer, and the number of rows and cols in the array, and 
// allocates and intializes the two dimensional array to a bunch of random numbers

void makeRandArray( unsigned int **& data, unsigned int rows, unsigned int cols, unsigned int seed )
{
	// allocate the array
	data = new unsigned int*[ rows ];
	for( unsigned int i = 0; i < rows; i++ )
	{
		data[i] = new unsigned int[ cols ];
	}
	
	// seed the number generator
	// you should change the seed to get different values
	srand(seed);
	
	// populate the array
	
	for( unsigned int i = 0; i < rows; i++ )
		for( unsigned int j = 0; j < cols; j++ )
		{
			data[i][j] = rand() % 10000 + 1; // number between 1 and 10000
		}
	
}

void getDataFromFile( unsigned int **& data, char fileName[], unsigned int &rows, unsigned int &cols )
{
	ifstream in;
	in.open( fileName );
	if( !in )
	{
		cerr << "error openeighborIng file: " << fileName << endl;
		exit( -1 );
	}
	
	in >> rows >> cols;
	data = new unsigned int*[ rows ];
	for( unsigned int i = 0; i < rows; i++ )
	{
		data[i] = new unsigned int[ cols ];
	}
	
	// now read in the data
	
	for( unsigned int i = 0; i < rows; i++ )
		for( unsigned int j = 0; j < cols; j++ )
		{
			in >> data[i][j];
		}
	
}


int main( int argc, char* argv[] ) 
{
	if( argc < 3 )
	{
		cerr<<"Usage: " << argv[0] << " [input data file] [num of threads to use] " << endl;
		
		cerr<<"or" << endl << "Usage: "<< argv[0] << " rand [num of threads to use] [num rows] [num cols] [seed value]" << endl;
                exit( 0 );
        }
	
	// read in the file
	unsigned int rows, cols, seed;
	unsigned int numThreads;
	unsigned int ** data;
	// convert numThreads to int
	{
		stringstream ss1;
		ss1 << argv[2];
		ss1 >> numThreads;
	}
	
	string fName( argv[1] );
	if( fName == "rand" )
	{
		{
			stringstream ss1;
			ss1 << argv[3];
			ss1 >> rows;
		}
		{
			stringstream ss1;
			ss1 << argv[4];
			ss1 >> cols;
		}
		{
			stringstream ss1;
			ss1 << argv[5];
			ss1 >> seed;
		}
		makeRandArray( data, rows, cols, seed );
	}
	else
	{
		getDataFromFile( data,  argv[1], rows, cols );
	}

	// tell omp how many threads to use
	omp_set_num_threads( numThreads );
	
	
	stopwatch S1;
	S1.start();

	double maxAvg = -1.0;
    int maxRow = -1, maxCol = -1;

    #pragma omp parallel
    {
        double lMaxAvg = -1.0;
        int lRow = -1, lCol = -1;

		//using IMBs manual for omp parallel processing, helped me figure out how to use directives and what directives I needed to use.
		//Used for nested loops 
		//https://www.ibm.com/docs/en/xffbg/121.141?topic=clauses-collapse. explains that using collapse allows us to basically loop 2 or more nested loops, into just one loop. For example, the (i,j) pairs.
        #pragma omp for collapse(2)
        for (unsigned int i = 0; i < rows; ++i) {
            for (unsigned int j = 0; j < cols; ++j) {
                double sum = 0.0;
                int count = 0;

                for (int neighborI = max(0, (int)i - 1); neighborI <= (int)i + 1 && neighborI < (int)rows; ++neighborI) {
                    for (int neighborJ = max(0, (int)j - 1); neighborJ <= (int)j + 1 && neighborJ < (int)cols; ++neighborJ) {
                        sum += data[neighborI][neighborJ];
                        ++count;
                    }
                }

                double avg = sum / count;

                if (avg > lMaxAvg) {
                    lMaxAvg = avg;
                    lRow = i;
                    lCol = j;
                }
            }
        }

		//prevents multiple threads entering a single cell at one time.
		//https://www.ibm.com/docs/en/zos/2.4.0?topic=processing-pragma-omp-critical
		//using the ibm manual from above, we can go to the critical section and see that we need to use the critical directive to prevent multiple threads from entering that block of code at once. Essentially making sure that only one thread can check and update a variable at a time. 
        #pragma omp critical
        {
            if (lMaxAvg > maxAvg) {
                maxAvg = lMaxAvg;
                maxRow = lRow;
                maxCol = lCol;
            }
        }
    }

    cout << "largest average: " << maxAvg << endl;
    cout << "found at cells: (" << maxRow << "," << maxCol << ")" << endl;
	
	
	S1.stop();
	
	// print out the max value here
	
	cerr << "elapsed time: " << S1.getTime( ) << endl;
}


