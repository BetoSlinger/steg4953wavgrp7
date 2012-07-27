#ifndef WAVEMESSAGEEMBEDDER_H
#define WAVEMESSAGEEMBEDDER_H
#include <stdio.h>
#include <iostream>
#include <bitset>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <time.h>


#include "windows.h"


class WaveMessageEmbedder
{
private:
    std::vector<bool> message;
    unsigned short * cover;
    unsigned int lsb_bits;
    //std::vector<BYTE> extractm;
    DWORD mByteCount;
    DWORD cByteCount;
    long long current;
    long long currentbits;
    //std::vector<int> sampleVector;
    void embed(unsigned int b,unsigned int n);

    void setMessageByte(BYTE val);
    void setCoverByte(BYTE val,BYTE hival,unsigned int cnt);
    unsigned int averageNLeftSamples(unsigned int n);
    unsigned int getlsb(unsigned int b,unsigned int value);
    unsigned int getNbitsFromMessage(unsigned int n);
   // int averageNLeftSamples(int n);
    unsigned int averageNRightSamples(unsigned int n);
    void extract(unsigned int b,unsigned int n);




public:


    //insert overloaded constructor with cover only for extraction
    WaveMessageEmbedder(char * message,unsigned int messageSize, BYTE * cover, DWORD coverSize);
    virtual ~WaveMessageEmbedder();
    BYTE * getStegoData(unsigned int bitsPerSample,unsigned int noOfBytesToAverage);
    BYTE * getExtractedData(unsigned int bitsPerSample,unsigned int noOfBytesToAverage);
    void print();


};

#endif // WAVEMESSAGEEMBEDDER_H