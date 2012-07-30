#include "WaveMessageEmbedder.h"

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
    for(  i = current; i < n *2 + current ; i+=2 )
    {
        sum += cover[i];
    }
    return sum/n;
}

unsigned int WaveMessageEmbedder::averageNRightSamples(unsigned int n)
{
    unsigned int i;
    unsigned int sum = 0;
    for(  i = current + 1; i < n * 2 + current ; i+=2 )
    {
		sum += cover[i];      
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
    

    unsigned int changeSample = 0;
	unsigned int token;
	unsigned int average_lsb = getlsb(b,averageNLeftSamples(n));
	unsigned int average_rsb = getlsb(b,averageNRightSamples(n));
    unsigned int num,max;
	bool first_time = true;
	// get b bits from message
	if(message.size() > b)
	{
		token = getNbitsFromMessage(b);
	}
	else
	{
		int i = message.size();
		token = getNbitsFromMessage(i);
		token = token << (b-i);
	}

    max = (unsigned int) pow(2,b) - 1;

    while(average_lsb != token)
    {
        while(changeSample < n)
		{		
			num = getlsb(b,cover[current + changeSample * 2]);
						
			if(first_time == true)
			{
			   // sets all samples involved to zero
			   cover[current + changeSample * 2] -= num;
			   if (changeSample == n -1)
					first_time = false;
			}
			else
			{
				if (num == max)
					cover[current + changeSample * 2] -= max;
				else
					cover[current + changeSample * 2]++;
			}
			average_lsb = getlsb(b,averageNLeftSamples(n));
			if (average_lsb == token)
				break;
			changeSample++;
		}
		changeSample = 0;       
    }
	
	// get b bits from message
	if(message.size() > b)
	{
		token = getNbitsFromMessage(b);
	}
	else
	{
		int i = message.size();
		token = getNbitsFromMessage(i);
		token = token << (b-i);
	}

    max = (unsigned int) pow(2,b) - 1;

    while(average_rsb != token)
    {
        while(changeSample < n)
		{		
			num = getlsb(b,cover[current + 1 + changeSample * 2]);
						
			if(first_time == true)
			{
			   // sets all samples involved to zero
			   cover[current + changeSample * 2] -= num;
			   if (changeSample == n -1)
					first_time = false;
			}
			else
			{
				if (num == max)
					cover[current + 1 + changeSample * 2] -= max;
				else
					cover[current + 1 + changeSample * 2]++;
			}
			average_rsb = getlsb(b,averageNRightSamples(n));
			if (average_rsb == token)
				break;
			changeSample++;
		}
		changeSample = 0;       
    }
     

    current += 2 * n;
    //unsigned int averageandgetlsbs(int d,unsigned int e,unsigned int f,unsigned int g);
    // increment or decrement average until lsb of a = b bit255,255,255,
    //increment current by n * 2
    // e = randomly generate
}

BYTE * WaveMessageEmbedder::getStegoData(unsigned int bitsPerSample,unsigned int noOfBytesToAverage)
{
    lsb_bits = bitsPerSample;
	int remainder = cByteCount % noOfBytesToAverage;
    while(current < cByteCount - remainder && !message.empty())
        embed(bitsPerSample,noOfBytesToAverage);
    //convert cover to BYTE
    return 0;
}

void WaveMessageEmbedder::extract(unsigned int b,unsigned int n)
{

    unsigned int average_lsb = getlsb(b,averageNLeftSamples(n));
	unsigned int average_rsb = getlsb(b,averageNRightSamples(n));
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
    	
	for(i = currentbits ; i < currentbits + b; i ++)
    {

        temp.insert(temp.begin(),average_rsb%2);
        average_rsb = average_rsb >> 1;
       // cout<<(bool)temp[i];

    }
    for( i = 0; i < temp.size(); i++)
    {
        message.push_back((bool)temp[i]);
    }

    temp.clear();
    current += 2 *n;
    currentbits += 2 * b;

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
    cout << "Message Bits"  <<message.size() <<endl;

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
    cout << "        Left                 Right       " <<endl;
    for( cnt = 0 ; cnt < cByteCount/2; cnt++){
        if( cnt % 2 == 1 )
			cout << "      ";
	
       
			cout<<setw(10)<< right << cover[cnt];
        if ( cnt % 2 == 1)
            cout << endl;
    }

        cout << endl;
            cout << endl;
                cout << endl;

}


int main()
{
    char message [] = "abcd";
    BYTE cover [256];
    srand(time(NULL));
    unsigned int i;
    for(i = 0; i < 256; i ++)
        cover[i] = rand() % 256;
    WaveMessageEmbedder w (message,4,cover,256);
    //cout << w.averageNLeftSamples(4)<< endl;
    //cout << w.getlsb(2,w.averageNLeftSamples(4)) << endl;
    w.print();
    w.getStegoData(7,4);
    w.print();
    w.getExtractedData(7,4);
    w.print();

    //delete &w;





    return 0;
}