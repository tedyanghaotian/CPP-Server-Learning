#include <thread>
#include <queue>
#include <iostream>
#include <mutex>
#include <time.h>
#include <condition_variable>

using namespace std;


class NonAttackQueen
{
private:

	int _queen_count;

	long _upperlim, _sum;

	mutex _mutex;

	condition_variable _con_var;

	queue<thread*> _threads;

	int _run_threads;

	const int MAX_THREAD_COUNT = 4;
private:

	void calculate(long row, long md, long cd)
	{
		calculatePrivate(row, md, cd);

		{
			lock_guard<mutex> guard(_mutex);
			_run_threads--;
		}

		_con_var.notify_one();
	}

	void calculatePrivate(long row, long md, long cd)
	{
		if (row != _upperlim)
		{
			long pos = _upperlim & ~(row | md | cd);
			while (pos)
			{
				long p = pos & -pos;
				pos -= p;
				calculatePrivate(row + p, (md + p) << 1, (cd + p) >> 1);
			}
		}
		else
		{
			lock_guard<mutex> guard(_mutex);
			_sum++;
		}
	}

	bool ready()
	{
		return _run_threads <= MAX_THREAD_COUNT;
	}

	void waitAndDelete()
	{
		while (!_threads.empty())
		{
			auto t = _threads.front();
			if (t->joinable())
			{
				t->join();
			}
			_threads.pop();
			delete t;
		}
	}

public:

	NonAttackQueen(int queen_count)
		: _queen_count(queen_count)
		, _run_threads(0)
		, _upperlim(1)
		, _sum(0)
	{
		_upperlim = (_upperlim << _queen_count) - 1;
	}

	long getResultSingleThread()
	{
		auto start = clock();

		calculate(0, 0, 0);

		auto finish = clock();

		cout << "time used:" << (double)(finish - start) / CLOCKS_PER_SEC << endl;

		return _sum;
	}

	long getResultMultilThread()
	{
		auto start = clock();

		for (int i = 0; i < _queen_count; i++)
		{
			long row = 1 << i, ld = row << 1, rd = row >> 1;

			std::unique_lock<mutex> lock(_mutex);

			_con_var.wait(lock, bind(&NonAttackQueen::ready, this));

			// add thread to do calculate
			auto t = new thread(&NonAttackQueen::calculate, this, row, ld, rd);

			_threads.push(t);

			_run_threads++;
		}

		waitAndDelete();

		auto finish = clock();

		cout << "time used:" << (double)(finish - start) / CLOCKS_PER_SEC << endl;

		return _sum;
	}
};

int main()
{
	NonAttackQueen nonAttackQueen(15);
    
	cout << "NQueen Result:" << nonAttackQueen.getResultMultilThread();

    return 0;
}