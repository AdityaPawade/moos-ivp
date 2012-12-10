/*
 * DBTestClient.cpp
 *
 *  Created on: Sep 19, 2011
 *      Author: pnewman
 */
#ifdef _WIN32
#define NOMINMAX
#endif
#include <iostream>
#include "MOOS/libMOOS/App/MOOSApp.h"
#include "MOOS/libMOOS/Utils/ConsoleColours.h"
#include <queue>

#ifndef _WIN32
#include <termios.h>
#include <sys/ioctl.h>
#include <stdio.h>

unsigned int GetScreenWidth()
{
   struct winsize ws;
   ioctl(0, TIOCGWINSZ, &ws);
   return ws.ws_col;
}
#else
{
	unsigned int GetScreenWidth()
	{
		return 80;
	}
}
#endif


void PrintHelp()
{

    MOOSTrace("\n\nPub/Sub detail settings:\n");
    MOOSTrace("  -s=<string>            : list of subscriptions in form var_name@period eg -s=x,y,z\n");
    MOOSTrace("  -w=<string>            : list of wildcard subscriptions in form var_pattern:app_pattern@frequency_hz eg -w='x*:*@0.1,*:GPS@0.0' \n");
    MOOSTrace("  -p=<string>            : list of publications in form var_name[:optional_binary_size]@frequency_hz eg -p=x@0.5,y:2048@2.0\n");
    MOOSTrace("  --latency              : show latency (time between posting and receiving)\n");
    MOOSTrace("  --bandwidth            : print bandwidth\n");
    MOOSTrace("  --verbose              : verbose output\n");

    MOOSTrace("\n\nspying helpful settings:\n");
    MOOSTrace("  --spy          		: spy on all variables at 1Hz\n");
    MOOSTrace("  --spy_all          	: spy on all variables\n");
    MOOSTrace("  --spy_proc=<string>    : spy on all variables from a named processes\n");


    MOOSTrace("\n\nNetwork failure simulation:\n");
    MOOSTrace("  --simulate_network_failure             : enable simulation of network/app failure\n");
    MOOSTrace("  --network_failure_prob=<numeric>       : probability of each DB interaction having network failure [0.1]\n");
    MOOSTrace("  --network_failure_time=<numeric>       : duration of network failure [3s]\n");
    MOOSTrace("  --application_failure_prob=<numeric>   : probability of application failing during DB-communication [0]\n");

    MOOSTrace("\n\nExample Usage:\n");
    MOOSTrace("  ./moos_test --moos_name=C1 --moos_apptick=30  -s=x -p=y@0.5,z@2.0\n");
    MOOSTrace("  ./moos_test C2 --simulate_network_failure --network_failure_prob=0.05 --network_failure_time=10.0 --application_failure_prob=0.05 -s=z \n");
    MOOSTrace("  ./moos_test --moos_name=C1 --moos_apptick=10  -s=x -p=y:4567@0.5,z@2.0\n");
    MOOSTrace("  ./moos_test --spy_proc=camera_A,camera_B\n");


}



class DBTestClient : public CMOOSApp
{
public:
    DBTestClient(){};

    bool OnProcessCommandLine()
    {

        _bShowLatency =  m_CommandLineParser.GetFlag("-l","--latency");
        _bVerbose = m_CommandLineParser.GetFlag("-v","--verbose");
        _bShowBandwidth =   m_CommandLineParser.GetFlag("-b","--bandwidth");

        std::string temp;

        if(m_CommandLineParser.GetVariable("-s",temp))
        {
        	_vSubscribe = MOOS::StringListToVector(temp);
        }

        std::vector<std::string> vPublish;
        if(m_CommandLineParser.GetVariable("-p",temp))
        {
        	vPublish = MOOS::StringListToVector(temp);
        }

        if(m_CommandLineParser.GetVariable("-w",temp))
        {
        	_vWildSubscriptions = MOOS::StringListToVector(temp);
        }
        if(m_CommandLineParser.GetFlag("--spy"))
        {
        	_vWildSubscriptions.push_back("*:*@1.0");
        	_bVerbose = true;
        }
        if(m_CommandLineParser.GetFlag("--spy_all"))
        {
        	_vWildSubscriptions.push_back("*:*@0.0");
        	_bVerbose = true;
        }
        if(m_CommandLineParser.GetVariable("--spy_proc",temp))
        {
        	std::vector<std::string> v = MOOS::StringListToVector(temp);
        	std::vector<std::string>::iterator q;
        	for(q=v.begin();q!=v.end();q++)
        	{
        		_vWildSubscriptions.push_back("*:"+*q+"@0.0");
        	}
        	_bVerbose = true;
        }



        _NetworkStallProb=0.0;
        m_CommandLineParser.GetVariable("--network_failure_prob",_NetworkStallProb);

        _NetworkStallTime = 0.0;
        m_CommandLineParser.GetVariable("--network_failure_time",_NetworkStallTime);

        _ApplicationExitProb = 0.0;
        m_CommandLineParser.GetVariable("--application_failure_prob",_ApplicationExitProb);

        _SimulateNetworkFailure= m_CommandLineParser.GetFlag("-N","--simulate_network_failure");


        //finalluy we will amke ouselves responsive!
        SetAppFreq(10,400);
        SetIterateMode(COMMS_DRIVEN_ITERATE_AND_MAIL);


        std::vector<std::string>::iterator q;
        unsigned int MaxArraySize=0;

        for(q = vPublish.begin();q!=vPublish.end();q++)
        {
        	unsigned int nArraySize=0;
            std::string sEntry  = *q;
            std::string sVar = MOOSChomp(sEntry,"@");

            std::string sName = MOOSChomp(sVar,":");
            if(!sVar.empty())
            {
            	//we are being told to send an array
            	std::stringstream ss(sVar);
            	ss>>nArraySize;

            	MaxArraySize = std::max(nArraySize,MaxArraySize);

            }

            double dfPeriod = 1.0;

            if(!sEntry.empty())
            {
				if (!MOOSIsNumeric(sEntry))
				{
					std::cerr<<"Entry:"<<sEntry<<std::endl;
					std::cerr<<MOOS::ConsoleColours::red()<<"badly formatted publish directive name1@frequency name2@frequency 2\n"<<MOOS::ConsoleColours::reset();
					exit(1);
				}
				else
				{
					double dfF =atof(sEntry.c_str());
					dfF = std::max<double>(dfF,0.01);
					dfPeriod = 1.0/dfF;
				}
            }


            if(nArraySize==0)
            {
            	_Jobs.push(Job(dfPeriod,sName));
                std::cerr<<MOOS::ConsoleColours::Green()<<"+Publishing "<<sName<<" at "<<1.0/dfPeriod<<" Hz\n"<< MOOS::ConsoleColours::reset();
            }
            else
            {
            	_Jobs.push(Job(dfPeriod,sName,nArraySize));
                std::cerr<<MOOS::ConsoleColours::Green()<<"+Publishing "<<sName<<" ["<<nArraySize<<"] at "<<1.0/dfPeriod<<" Hz\n"<< MOOS::ConsoleColours::reset();
            }

        }

        _BinaryArray.resize(MaxArraySize);

        return true;

    }



    void OnPrintHelpAndExit()
    {
    	PrintHelp();
    	exit(0);
    }

    bool OnStartUp()
    {
        if(_SimulateNetworkFailure)
        {
            MOOSTrace("%f %f\n\n",_NetworkStallProb,_NetworkStallTime);
            m_Comms.ConfigureCommsTesting(_NetworkStallProb,_NetworkStallTime,_ApplicationExitProb);
        }

        Scheduler.Initialise(ScheduleDispatch,this);
        Scheduler.Start();

        return true;
    }
    bool OnNewMail(MOOSMSG_LIST & NewMail)
    {
        MOOSMSG_LIST::iterator q;

        if(_bVerbose && !NewMail.empty())
        {
        	std::cerr<<std::endl;
        	std::cerr<<MOOS::ConsoleColours::Yellow();
        	std::cerr<<std::left<<std::setw(20)<<"name";
        	std::cerr<<std::left<<std::setw(20)<<"source";
        	std::cerr<<std::left<<std::setw(30)<<"contents";
        	std::cerr<<std::endl;
        	std::cerr<<MOOS::ConsoleColours::reset();
        }

        for(q = NewMail.begin();q!=NewMail.end();q++)
        {
        	if(_bVerbose)
        	{
        		std::cerr<<std::left<<std::setw(20)<<q->GetKey();
        		std::cerr<<std::left<<std::setw(20)<<q->GetSource();
        		std::string sout = q->GetAsString();
        		if(sout.size()>GetScreenWidth()-43)
        			sout = sout.substr(0,GetScreenWidth()-43)+"...";
        		std::cerr<<std::left<<std::setw(30)<<sout;
        		std::cerr<<std::endl;

        	}
        	if(_bShowLatency)
        	{
        		double dfLatencyMS  = (MOOS::Time()-q->GetTime())*1000;
        		std::cerr<<MOOS::ConsoleColours::cyan()<<"        Latency "<<std::setprecision(2)<<dfLatencyMS<<" ms\n";
        		std::cerr<<MOOS::ConsoleColours::cyan()<<"           Tx: "<<std::setw(20)<<std::setprecision(14)<<q->GetTime()<<"\n";
        		std::cerr<<MOOS::ConsoleColours::cyan()<<"           Rx: "<<std::setw(20)<<std::setprecision(14)<<MOOS::Time()<<"\n";
                std::cerr<<MOOS::ConsoleColours::reset();
        	}
        }

        return true;
    }
    bool Iterate()
    {
		static double dfT = MOOS::Time();
		static long long unsigned int nByteInCounter=0;
		static long long unsigned int nByteOutCounter=0;
		if(MOOS::Time()-dfT>1.0)
		{
			long long unsigned int bi, bo;
			bi  = m_Comms.GetNumBytesReceived();
			bo = m_Comms.GetNumBytesSent();

			if(_bShowBandwidth)
			{
				std::cerr<<MOOS::ConsoleColours::yellow()<<"--Bandwidth--    ";
				std::cerr<<MOOS::ConsoleColours::green()<<"Incoming: "<<std::setw(8)<<  8*(bi-nByteInCounter)/(1024.0*1024.0)<<"  Mb/s  ";
				std::cerr<<MOOS::ConsoleColours::Green()<<  "Outgoing: "<<std::setw(8)<<  8*(bo-nByteOutCounter)/(1024.0*1024.0) <<"  Mb/s\r";
				std::cerr<<MOOS::ConsoleColours::reset();
			}
			nByteInCounter = bi;
			nByteOutCounter = bo;
			dfT = MOOS::Time();

		}



        return true;
    }
    bool OnConnectToServer()
    {
        DoSubscriptions();
        return true;
    }

    bool DoSubscriptions()
    {
        std::vector<std::string>::iterator q;

        std::cerr<<MOOS::ConsoleColours::Green();
        for(q = _vSubscribe.begin();q!=_vSubscribe.end();q++)
        {
			std::string sEntry = *q;
			std::string sVar = MOOSChomp(sEntry,"@");
			double dfPeriod = 0.0;
			if(MOOSIsNumeric(sEntry)&& !sEntry.empty())
			{

				double dfF = atof(sEntry.c_str());
				if(dfF>0.0)
					dfPeriod =1.0/dfF;
			}
			m_Comms.Register(sVar,dfPeriod);

            std::cerr<<"+Subscribing to "<<sVar<<"@"<<dfPeriod<<"\n";
        }
        std::cerr<<MOOS::ConsoleColours::reset();

        std::vector<std::string>::iterator w;

		for(w = _vWildSubscriptions.begin();w!=_vWildSubscriptions.end();w++)
		{
			//GPS_X:*@0.4
			std::string sEntry = *w;
			std::string sVarPattern = MOOSChomp(sEntry,":");
			std::string sAppPattern = MOOSChomp(sEntry,"@");

			double dfPeriod = 0.0;
			if(!sEntry.empty())
			{
				if(MOOSIsNumeric(sEntry))
				{
					double dfF = atof(sEntry.c_str());
					if(dfF>0.0)
						dfPeriod =1.0/dfF;
				}
				else
				{
					std::cerr<<MOOS::ConsoleColours::red()<<*w<<" does not have format var_pattern:app_pattern[@period]\n";
					continue;
				}
			}
			if(sVarPattern.empty()||sAppPattern.empty())
			{
				std::cerr<<MOOS::ConsoleColours::red()<<*w<<" does not have format var_pattern:app_pattern[@period]\n";
				continue;
			}

			m_Comms.Register(sVarPattern,sAppPattern,dfPeriod);

		}



        return true;
    }

public:
    static bool ScheduleDispatch(void * pParam)
    {
    	DBTestClient* pMe = (DBTestClient*)pParam;
    	return pMe->ScheduleLoop();
    }

    bool ScheduleLoop()
    {
    	while(!Scheduler.IsQuitRequested())
    	{
			while(!_Jobs.empty() && _Jobs.top().isActive(MOOS::Time()))
			{


				Job Active = _Jobs.top();
				_Jobs.pop();
				if(Active.IsBinary())
				{
					Notify(Active._sName,&_BinaryArray[0],Active._DataSize, MOOS::Time() );

					if(_bVerbose)
					{
						/*
						double T = MOOS::Time()-GetAppStartTime();
						std::cerr<<T<<MOOS::ConsoleColours::Yellow()<<" publishing binary data: "<<Active._sName<<"="
							<<Active._nCount<<" "<<Active._DataSize<<" bytes"<<std::endl<<MOOS::ConsoleColours::reset();*/
					}

				}
				else
				{
					Notify(Active._sName,Active._nCount,MOOSTime());
					if(_bVerbose)
					{
						//std::cerr<<T<<MOOS::ConsoleColours::Yellow()<<" publishing: "<<Active._sName<<"="<<Active._nCount<<std::endl<<MOOS::ConsoleColours::reset();
					}
				}
				Active.Reschedule();
				_Jobs.push(Active);
			}
			MOOSPause(5);
    	}

    	return true;
    }


private:
    std::vector<std::string> _vSubscribe;
    std::vector<std::string> _vWildSubscriptions;

    CMOOSThread Scheduler;

    bool _SimulateNetworkFailure ;
    double _NetworkStallProb ;
    double _NetworkStallTime ;
    double _ApplicationExitProb;

    struct Job
    {
        Job(double dfPeriod, std::string sName):_dfPeriod(dfPeriod),_sName(sName),_nCount(0), _pData(0)
        {
            _dfTimeScheduled = MOOSTime()+_dfPeriod;
            _DataSize = 0;
        }

        Job(double dfPeriod,std::string sName, unsigned int nSize):_dfPeriod(dfPeriod),_sName(sName),_nCount(0)
        {
        	_DataSize = nSize;
        	_dfTimeScheduled = MOOSTime()+_dfPeriod;
        }

        ~Job()
        {
        }
        bool operator < (const  Job &  a) const
        {
            return _dfTimeScheduled> a._dfTimeScheduled;
        }
        bool IsBinary()
        {
        	return _pData!=NULL;
        }

        void Reschedule()
        {
            _nCount++;
            _dfTimeScheduled = MOOSTime()+_dfPeriod;
        }

        bool isActive(double TimeNow) const
        {
            return _dfTimeScheduled < TimeNow;
        }

        double _dfPeriod;
        double _dfTimeScheduled;
        std::string _sName;
        int _nCount;
        unsigned char * _pData;
        unsigned int _DataSize;

    };
    std::priority_queue<Job> _Jobs;
    std::vector <unsigned char  >_BinaryArray;
    bool _bVerbose;
    bool _bShowLatency;
    bool _bShowBandwidth;




};

int main (int argc, char* argv[])
{

	//here we do some command line parsing...
	MOOS::CommandLineParser P(argc,argv);

	//we may want many instances run from command line so lets guess
	//a random name. This is just to stop users having to specify
	//--moos_name at the command line lots and lots...
	std::stringstream default_name;
	srand ( time(NULL) );
	default_name<<"umm-"<< rand() %1024;
	std::string app_name = P.GetFreeParameter(1, default_name.str());

    DBTestClient TC1;

    TC1.Run(app_name,argc,argv);
}
