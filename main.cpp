#include "inicpp.h"
#include <iostream>
#include <string>
#include <fstream>
#include <strings.h>
#include <unistd.h>
using namespace std;

//nssm install serviceName filePath [role], when role is empty,means role is root(in such case 4 parameters)
//nssm remove serviceName
//nssm start servicename
//nssm stop serviceName
//nssm status serviceName

static int readFromFile(string& data, const string& filePath)
{
    ifstream Osread(filePath);
    if(Osread.fail())
    {
	    cout << "error open file " << filePath << endl;
	    return -1;
    }
    string line="";
    while(getline(Osread,line))
    {
        data+=line;
        data+="\n";
    }

    Osread.close();
    return 0;
}

void serviceInstalled(const string& serviceName,int& installed)
{
	string cmd="",data="";
	cmd = "ls /etc/systemd/system/|grep "+serviceName+".service |wc -l > log.dat";
	system(cmd.c_str());
	readFromFile(data,"log.dat");
	if(atoi(data.c_str()))
	{
		installed=1;
	}
	else
	{
		installed = 0;
	}
	remove("log.dat");	
}

int installService(const string& serviceName)
{
	int ret = -1;
	string cmd = "chmod +x " + serviceName+ ".service";
	system(cmd.c_str());
	cmd = "mv " + serviceName + ".service /etc/systemd/system/";
	ret = system(cmd.c_str());
	if(0==ret)
	{
		cmd = "systemctl enable " + serviceName;
		ret = system(cmd.c_str());
	}
	return ret;	
}

int stopService(const string& serviceName)
{
	int ret = -1;
	string cmd = "systemctl stop " + serviceName;
	ret = system(cmd.c_str());
	return ret;
}

int startService(const string& serviceName)
{
	int ret = -1;
	string cmd = "systemctl start " + serviceName;
	ret = system(cmd.c_str());
	return ret;
}

int serviceStatus(const string& serviceName)
{
	int ret = 0;
	string data = "";
	string cmd = "systemctl status " + serviceName + "|grep \"Main PID\"|awk \'{print $3}\' > log.dat";
	ret = system(cmd.c_str());
	if(0==ret)
	{
		readFromFile(data,"./log.dat");
		if(!data.empty())
		{
			cout<<"main pid is "<<data;
			ret = 1;
		}
		remove("./log.dat");
	}
	return ret;	
}

int restartService(const string& serviceName)
{
	stopService(serviceName);
	return startService(serviceName);
}

int generateCfgFile(const string& serviceName, const string& appPath, const string& role)
{
	int ret = -1;
	ini::IniFile myIni;
	myIni["Unit"]["Description"] = serviceName;
	myIni["Unit"]["After"] = "network.target";
	myIni["Service"]["ExecStart"]=appPath;
	myIni["Service"]["ExecStop"]="/bin/kill -s TERM $MAINPID";
	myIni["Service"]["User"]=role;
	myIni["Service"]["Group"]=role;
	myIni["Service"]["Type"]="simple";
	myIni["Service"]["TimeoutStopSec"]=60;
	myIni["Service"]["RestartSec"]=3;
	myIni["Service"]["Restart"]="always";
	myIni["Install"]["WantedBy"]="default.target";
	string fileName = serviceName+".service";
	myIni.save(fileName);
	return ret;
}

int main(int argc, char* argv[])
{
	int ret = -1;
	string cmd = "";
	string data = "";
	string serviceName="";
	if(argc<3)
	{
		cout<<"invalid command, at least 3 parameters,like :"<<endl;
		cout<<"nssm install serviceName appPath"<<endl;
	}
	if(argc == 3)
	{
		serviceName = argv[2];
		int installed=0;
		serviceInstalled(serviceName,installed);
		if(installed)
		{
			if(0==strcasecmp("remove",argv[1]))
			{
				ret = stopService(serviceName);
				if(ret==0)
				{
					cmd = "systemctl disable " + serviceName;
					ret = system(cmd.c_str());
					if(ret==0)
					{
						cmd = "rm -f /etc/systemd/system/"+serviceName+".service";
						ret = system(cmd.c_str());	
						if(ret==0)
						{
							cout<<"succedd"<<endl;
						}
						else
						{
							cout<<"failed to remove service configuration file"<<endl;
						}
					}
					else
					{
						cout<<"disable "+serviceName<<" fail"<<endl;
					}
				}
				else
				{
					cout<<"failed to stop "+serviceName<<endl;
				}
			}
			else if(0==strcasecmp("stop",argv[1]))
			{
				ret = stopService(serviceName);
				if(ret==0)
				{
					cout<<"succedd"<<endl;
				}
				else
				{
					cout<<"failed"<<endl;
				}
			}
			else if(0==strcasecmp("start",argv[1]))
			{
				ret = startService(serviceName);
				if(ret==0)
				{
					cout<<"succedd"<<endl;
				}
				else
				{
					cout<<"failed"<<endl;
				}				
			}
			else if(0==strcasecmp("status",argv[1]))
			{
				ret = serviceStatus(serviceName);
				if(0==ret)
				{
					cout<<"service stopped"<<endl;
				}
				else
				{
					cout<<"service is running"<<endl;
				}
			}
			else if(0==strcasecmp("restart",argv[1]))
			{
				ret = restartService(serviceName);
				if(0==ret)
				{
					cout<<"service started"<<endl;
				}
				else
				{
					cout<<"service can't started"<<endl;
				}				
			}			
			else
			{
				cout<<"operation not support, now only support install, start, stop ,remove"<<endl;
			}
		}
		else
		{
			cout<<serviceName<<" is not a service"<<endl;
		}
	}
	else if(argc==4 || argc==5)
	{
		string role="";
		if(argc==4)
		{
			role = "root";
		}
		else
		{
			role = argv[4];
		}
		if(0==strcasecmp("install",argv[1]))
		{
			serviceName = argv[2];
			generateCfgFile(serviceName,argv[3],role);
			ret = installService(serviceName);

			//ret = startService(serviceName);
			if(0==ret)
			{
				cout<<"succedd"<<endl;
			}
			else
			{
				cout<<"install service fail"<<endl;
			}
		}
		else
		{
			cout<<"operation not support, now only support install, start, stop ,remove"<<endl;
		}
	}
	return ret;
}