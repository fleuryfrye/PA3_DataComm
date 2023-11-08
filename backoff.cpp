#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <math.h>
#include <fstream>

using namespace std;
#define NUM_TRIALS 10

typedef enum
{
    LINEAR,
    EXPONENTIAL,
    LOGLOG
} GrowthMethod;


void randomlySelectSlots(int numberOfDevices, vector<int>& slotVector, int vectorSize)
{
    for(int i = 0; i < numberOfDevices; i++)
    {
        int slotNumber = rand() % vectorSize;
        slotVector[slotNumber] += 1;

    }
    return;
}


int findSuccessfulTransmissions(int* numDevices, vector<int>& slotVector)
{
    for (int i = 0; i < slotVector.size(); i++)
        {
            if(slotVector[i] == 1)
            {
                (*numDevices)--;
            }

            slotVector[i] = 0;

            if(*numDevices == 0) //final device transmitted. give back the last slot used
            {
                return i;
            }

        }

    return -1; //not all devices have transmitted, so we will keep going :)
}

void resizeVector(GrowthMethod method, vector<int>& slotVector)
{
    int currentSize = slotVector.size();
    int nextSize;

    switch(method)
    {
        case(LINEAR):
            nextSize = currentSize+1;
            break;
        case(EXPONENTIAL):
            nextSize = currentSize*2;
            break;
        case(LOGLOG):
            nextSize = ((1 + (1 / log2(log2(currentSize)))) * currentSize ) / 1;
            break;
        default:
            break;
    }

    slotVector.resize(nextSize, 0);
}

int backoff(int numberOfDevices, GrowthMethod method)
{
    srand(time(0));
    vector<int> testVector(1, 0);
    if(method == LOGLOG) //requirement that loglog starts with size 4
    {
        testVector.resize(4, 0);
    }

    randomlySelectSlots(numberOfDevices, testVector, testVector.size());

    int ret = findSuccessfulTransmissions(&numberOfDevices, testVector);

    int latency = 1;

    while(ret < 0)
    {
        latency += testVector.size();
        resizeVector(method, testVector);
        randomlySelectSlots(numberOfDevices, testVector, testVector.size());

        ret = findSuccessfulTransmissions(&numberOfDevices, testVector);
    }

    latency += ret;
    return latency;
}

void calculateAverageLatency(int numDevices, GrowthMethod method, int* averageLatency, ofstream* logFile)
{
    int totalLatency = 0;
    for(int i = 0; i < NUM_TRIALS; i++)
    {
        totalLatency += backoff(numDevices, method);
    }

    *averageLatency = totalLatency / NUM_TRIALS;
    *logFile << *averageLatency << "\n";
}


int main(void)
{
    GrowthMethod method;
    ofstream linearFile, exponentialFile, loglogFile;

    linearFile.open("linearLatency.txt");
    exponentialFile.open("binaryLatency.txt");
    loglogFile.open("loglogLatency.txt");

    for(int numDevices = 100; numDevices <= 6000; numDevices += 100)
    {
        int averageLatency;
        method = LINEAR;
        calculateAverageLatency(numDevices, method, &averageLatency, &linearFile);

        method = EXPONENTIAL;
        calculateAverageLatency(numDevices, method, &averageLatency, &exponentialFile);

        method = LOGLOG;
        calculateAverageLatency(numDevices, method, &averageLatency, &loglogFile);

    }    
    return 0;
}