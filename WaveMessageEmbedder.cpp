#include "WaveMessageEmbedder.h"
#include <iomanip>

using std::setw;
using std::right;
using std::cout;
using std::endl;

WaveMessageEmbedder::WaveMessageEmbedder(char * m, unsigned int mSize, BYTE * c, DWORD cSize)
{
    unsigned int cnt;
    mByteCount = mSize;
    cByteCount = cSize;
    std::string sm (m);
    current = 0;
    lsb_bits = 0;

//    std::bitset<(size_t) mSize>  temp (sm);
    cover = new unsigned short [cSize];
    //message = new std::vector<bool>;
    for( cnt = 0 ; cnt < mSize; cnt++)
        setMessageByte(m[cnt]);

    for( cnt = 0 ; cnt < cSize; cnt+=2)
        setCoverByte(c[cnt],c[cnt+1],cnt);

}

WaveMessageEmbedder::~WaveMessageEmbedder()
{
//    delete [] message;
    delete [] cover;
}

void WaveMessageEmbedder::setMessageByte(BYTE val)
{
    unsigned int i;
    std::bitset<8> bin (val);

    for(i = 0; i < 8; i++)
    {

        message.push_back(bin[8 - i - 1]);

    }
}

void WaveMessageEmbedder::setCoverByte(BYTE val, BYTE hival,unsigned int cnt)
{

    cover[cnt>>1] = (val+256*hival);
}

unsigned int WaveMessageEmbedder::getNbitsFromMessage(unsigned int n)
{
    unsigned int i,token = 0;

    for(i = 0; i < n ;i++)
    {
        cout << "message.front " << message.front() << "current: " << current << endl;
        token += pow(2,n-i-1)*(unsigned int)message.front();
        message.erase(message.begin());

    }
    return token;

}

unsigned int WaveMessageEmbedder::averageNLeftSamples(unsigned int n)
{
    unsigned int i;
    unsigned int sum = 0;
    //sampleVector.clear();
    for(  i = current; i < n *2 + current ; i+=2 )
    {
        //cout <<"in func:" << cover[i].to_ulong()  + cover[i + 1].to_ulong() * 256 << endl;
       sum += cover[i];
       //sampleVector.push_back(cover[i].to_ulong() + cover[i + 1].to_ulong() * 256 );

    }
    cout<< "current: " << current;

    return sum/n;

}

unsigned int WaveMessageEmbedder::averageNRightSamples(unsigned int n)
{
    unsigned int i;
    unsigned int sum = 0;
//    sampleVector.clear();
    for(  i = current; i < n * 4 + current ; i+=4 )
    {
     //  sum += cover[i + 2].to_ulong()  + cover[i + 3].to_ulong() * 256;
       //sampleVector.push_back(cover[i + 2].to_ulong() + cover[i + 3].to_ulong() * 256 );

    }

    return sum/n;

}

unsigned int WaveMessageEmbedder::getlsb(unsigned int b,unsigned int value)
{
    unsigned int i,mask = 0;
    for(i = 0; i < b; i++)
    {
        mask += pow(2,i);

    }

    return value & mask;
}



void WaveMessageEmbedder::embed(unsigned int b,unsigned int n)
{
    // get b bits from message
    unsigned int changeByte;
    unsigned int token = getNbitsFromMessage(b);
    unsigned int average_lsb = getlsb(b,averageNLeftSamples(n));
    unsigned int num,cnt,max;
    changeByte = 0;
    max = (unsigned int) pow(2,b) - 1;

    bool allones;
    //srand(time(NULL));


    while(average_lsb != token)
    {


        num = cover[current + changeByte * n] << (16 - b);
        num >> (16 - b);

        if (num == max)
            cover[current + changeByte * n] - max;
        else
            cover[current + changeByte * n]++;


        changeByte+=2;

        //cout<< "during averaging: average_lsb = " << average_lsb << "token = " << token <<endl ;


    }
     cout<< "\nembedded token: average_lsb = " << average_lsb << "token = " << token <<endl ;

    current += 4 * n;
    //unsigned int averageandgetlsbs(int d,unsigned int e,unsigned int f,unsigned int g);
    // increment or decrement average until lsb of a = b bit255,255,255,
    //increment current by n * 2
    // e = randomly generate
}

BYTE * WaveMessageEmbedder::getStegoData(unsigned int bitsPerSample,unsigned int noOfBytesToAverage)
{
    lsb_bits = bitsPerSample;
    while(current < cByteCount && !message.empty())
        embed(bitsPerSample,noOfBytesToAverage);
    //convert cover to BYTE
    return 0;
}

void WaveMessageEmbedder::extract(unsigned int b,unsigned int n)
{

    unsigned int average_lsb = getlsb(b,averageNLeftSamples(n));
    std::vector<unsigned int> temp;
    cout<< "\non extraction: average_lsb = " << average_lsb << "current = " << current <<endl ;
    unsigned int i;
    for(i = currentbits ; i < currentbits + b; i ++)
    {

        temp.insert(temp.begin(),average_lsb%2);
        average_lsb = average_lsb >> 1;
       // cout<<(bool)temp[i];

    }
    for( i = 0; i < temp.size(); i++)
    {
        message.push_back((bool)temp[i]);
    }

    temp.clear();
    current += 4 *n;
    currentbits += b;

}

BYTE * WaveMessageEmbedder::getExtractedData(unsigned int bitsPerSample,unsigned int noOfBytesToAverage)
{
    lsb_bits = bitsPerSample;
    message.clear();
    current = 0;
    currentbits = 0;
    while(current < cByteCount && currentbits < cByteCount * 8)
        extract(bitsPerSample,noOfBytesToAverage);
    //conver message to BYTE *
    return 0;
}

void WaveMessageEmbedder::print()
{
    unsigned int cnt;
    cout << "Message Bits"  <<endl;

    for( cnt = 0 ; cnt < message.size() ; cnt++){
        cout << (bool)message[cnt];
        if ( cnt % 8== 7)
            cout << ", " ;
        else if (cnt % 80 == 79)
            cout << endl;
    }
    //cout << message;

    cout << endl;

    cout << "Cover Bytes" << endl;
    cout << "Left low     Left High      Right Low      Right High" <<endl;
    for( cnt = 0 ; cnt < cByteCount; cnt++){
        if( cnt % 4 == 2 )
        cout << ",  ";

        cout<<setw(10)<< right << cover[cnt] << ".." << getlsb(lsb_bits,cover[cnt]) ;
        if ( cnt % 2 == 1)
            cout << endl;
    }

        cout << endl;
            cout << endl;
                cout << endl;

}


int main()
{
    char message [] = "a";
    BYTE cover [64];
    srand(time(NULL));
    unsigned int i;
    for(i = 0; i < 64; i ++)
        cover[i] = rand() % 256;
    WaveMessageEmbedder w (message,1,cover,64);
    //cout << w.averageNLeftSamples(4)<< endl;
    //cout << w.getlsb(2,w.averageNLeftSamples(4)) << endl;
    w.print();
    w.getStegoData(2,4);
    w.print();
    w.getExtractedData(2,4);
    w.print();

    //delete &w;





    return 0;
}