#include <iostream>

using namespace std;

const int QueenNum = 18;

long sum = 0, upperlim = 1;

void advCalculateNQueen(long row, long md, long cd)
{
    if (row != upperlim)
    {
        long pos = upperlim & ~(row|md|cd);
        while (pos)
        {
            long p = pos & -pos;
            pos -= p;
            advCalculateNQueen(row + p, (md + p) << 1, (cd + p) >> 1);
        }
    }
    else
    {
        sum++;
    }
}


int main()
{
	upperlim = (upperlim << QueenNum) - 1;

	advCalculateNQueen(0, 0, 0);
    
	cout << "NQueen Result:" << sum;

    return 0;
}