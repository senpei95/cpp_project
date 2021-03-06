// Project.cpp : Defines the entry point for the console application.
//
#include <fstream>
#include <sstream>  
#include "GetData.h"
#include "VectorOperator.h"
#include "Display.h"
#include "Stock.h"
#include <thread>
using namespace std;

void ClearUnusableCom(Group &Source)
{
	for (map<string, Stock>::iterator itr = Source.Content.begin(); itr != Source.Content.end(); )
	{
		if (itr->second.check() == 1)
		{
			Source.Content.erase(itr++);
		}
		else itr++;

	}
}

int main()
{
	cout << "Program initiated" << endl;
	string file[1] = { "Data.txt" };
	Group Beat, Meet, Miss;
	Vect BeatAAR(DayNum), BeatCAAR(DayNum), MeetAAR(DayNum), MeetCAAR(DayNum), MissAAR(DayNum), MissCAAR(DayNum);

	Index SPY("SPY", "2016-11-10", "2018-04-13");

	GetStockPool(Beat, Meet, Miss, file);
	cout << "Txt data loaded." << endl;
	curl_global_init(CURL_GLOBAL_ALL);
	
	cout << "Gathering data from the Yahoo Finance..." << endl;
	thread thread1{ GetStockPrice, ref(Beat) };
	thread thread2{ GetStockPrice, ref(Meet) };
	thread thread3{ GetStockPrice, ref(Miss) };
	thread thread4{ GetSPYPrice, ref(SPY) };
	thread1.join();
	thread2.join();
	thread3.join();
	thread4.join();
	cout << "All data we need downloaded." << endl;

	thread thread5{ ClearUnusableCom,ref(Beat) };
	thread thread6{ ClearUnusableCom,ref(Meet) };
	thread thread7{ ClearUnusableCom,ref(Miss) };
	thread5.join();
	thread6.join();
	thread7.join();
	cout << "Missing data deleted." << endl;

	SPY.CalDailyReturn();

	cout << "BootStraping..." << endl;
	int num = 30;
	for (int i = 0; i < num; i++)
	{
		
		BootStrap(Beat, SPY, BeatAAR, BeatCAAR);
		BootStrap(Meet, SPY, MeetAAR, MeetCAAR);
		BootStrap(Miss, SPY, MissAAR, MissCAAR);

	}
	BeatAAR /= num;
	MeetAAR /= num;
	MissAAR /= num;
	BeatCAAR /= num;
	MeetCAAR /= num;
	MissCAAR /= num;

	vector<vector<vector<double>>> ResultMatrix;
	ResultMatrix = SetMatrix(BeatAAR, BeatCAAR, MeetAAR, MeetCAAR, MissAAR, MissCAAR);
	Menu(Beat, Meet, Miss, ResultMatrix);
	
	return 0;
}

