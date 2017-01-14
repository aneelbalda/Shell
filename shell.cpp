
#include <stdio.h>
#include <unistd.h> //for getcwd
#include <iostream>
#include <string>
#include <string.h> 
#include <stdlib.h>  //atoi
#include <vector>
#include <fstream>
#include <algorithm> //to use erase
#include <regex> //to use regex
#include <iterator>
#include <sys/types.h>
#include <sys/wait.h>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>

using namespace std;

vector< pair<int ,string> > hist;
string echo_trimmer(string s);
void Parse(string input);
string trimmer(string input);
string echo_trimmer(string s);
void cd(char *inputPath);
void history(int n);
void pwd();
void echo_(string name);
void export_(string name);
void mypipe(char ***argcmd);

/*........trimmer........*/

string trimmer(string input)
{

	input = regex_replace(input,regex("^[ \t]+"), "");
	input = regex_replace(input,regex("[ \t]+$"), "");
	input = echo_trimmer(input);
	return input;
}

string echo_trimmer(string s)
{

	string qoute="";
	string temp="";
	int len=s.length();
	int status=0;
	char c;
	string outut="";
	int count=0;
	int flag=0;

	while(len--)
	{
		c=s[count++];
		if (status==0 && (c=='"' || c=='\'' ))
		{
			status=1;
			outut+=regex_replace(temp,regex("[ \t]+"), " ");
			if (c=='\'')
			{
				qoute="single";
				outut+='\'';
			}
			else
			{
				outut+='"';
				qoute="double";
			}

			temp="";
		}
		else if (status==1 && c=='"' && qoute=="double")
		{
			if (qoute=="double")
			{
				status=0;
				temp+=c;
				outut+=temp;
				temp="";
				flag=1;
			}
		}
		else if (status==1 && c=='\'' && qoute=="single" )
		{
			if (qoute=="single")
			{
				status=0;
				temp+=c;
				outut+=temp;
				temp="";
				flag=1;
			}
		}
		else if((c==' ' || c=='\t')&& (s[count]==' ' || s[count]=='\t') && status==0)
		{

		}
		else if(flag==0)
		{
			if (c=='\t' && status==0)
			{
				temp+=' ';
			}else
			temp+=c;
		}
		flag=0;
		
	}
	outut+=temp;
	return outut;
}




/*............cd command......*/
void cd(char *inputPath)
{
	int x;
	char path[1024];
	memset(path,0,1024);
	char crntWrkDir[1024];
	memset(crntWrkDir,0,1024);

	cout.flush();
	if(inputPath == NULL)
	{

		strcpy(path, getenv("HOME"));
	}
	else
		strcpy(path,inputPath);

	if(path[0]=='/')
	{
		x = chdir(path);
	}
	else
	{
		getcwd(crntWrkDir,sizeof(crntWrkDir));
		strcat(crntWrkDir,"/");
		strcat(crntWrkDir,path);
		x = chdir(crntWrkDir);
	}

	if(x!=0)
	{
		cout<<"bash:"<< path <<": No such file or directory"<<endl;
	}
}



/*.................pwd.................*/
void pwd()
{
	char crntWrkDir[1024];
	memset(crntWrkDir,0,1024);
	getcwd(crntWrkDir,sizeof(crntWrkDir));
	cout<<crntWrkDir<<endl;
}


/*...............history..............*/

void history(int n)
{

	vector<pair<int ,string> >::iterator it;
		if(n==0)
		{
			for(it=hist.begin();it!=hist.end();it++)
			{
				cout<<it->first <<" "<<(*it).second<<endl;
			}
		}
		else 
		{
			int len = hist.size();

			for(int i=len-n;i<len;i++)
			{
				cout<<hist[i].first <<" "<<hist[i].second<<endl;
			}

		}


	}
/*................echo............*/
	void echo_(string name)
	{

		if(name[0]=='$')
		{
			name=name.substr(1);
			char *val = getenv(name.c_str());
			printf("%s",val);
			cout<<endl;
		}
		else if(name[0]=='\''||name[0]=='"')
		{
			int len = name.length();
			string x = name.substr(1,len-2);
			cout<<x<<endl;
		}
		else 
			cout<<name<<endl;

	}



/*................export............*/
	void export_(string name)
	{
		string x,y;
		int i;
		i = name.find('=');
		x = name.substr(0,i);
		y = name.substr(i+1);
		if(name[i-1]==' ')
			cout<<"bash: export: `=': not a valid identifier"<<endl;
		else
			setenv(x.c_str(),y.c_str(),1);

	}



/*.................bang...........*/
	void bang(string name)
	{

		if(name[0]=='!')
		{
			vector<pair<int ,string> >::iterator it;
			int len = hist.size();
			string input2 = hist[len-2].second;
			input2 = trimmer(input2);
			string input = input2;
			string temp = input;
			string namehist = temp.substr(0,7);
			string nameecho = temp.substr(0,4);
			string nameexpo = temp.substr(0,6);
			string namepwd  = temp.substr(0,3);
			string namebng  = temp.substr(0,1);
			
			if(nameecho == "echo")
			{
				int slen=input.length();
				if(slen>5)
				{
					string name = temp.substr(5);
					echo_(name);
				}
				else
					cout<<endl;

			}
			else if( nameexpo == "export")
			{
				int slen=input.length();
				if(slen>7)
				{
					string name = temp.substr(7);
					export_(name);
				}
				else
					cout<<endl;
			}
			else if(namehist == "history")
			{
				int slen=input.length();
				if(slen>8)
				{
					int n=10;
					string name = temp.substr(8);
					n = atoi(name.c_str());

					history(n);
				}
				else
					history(0);
				
			}
			else if(namebng == "!")
			{
				//cout<<"yahan"<<endl;
				int slen=input.length();
				if(slen>1)
				{
					string name = temp.substr(1);
					bang(name);

				}

			}
			else if(namepwd == "pwd")
			{
				pwd();
			}
			else
				Parse(input);



		}
		else if(name[0]=='-')
		{
			string cmd = name.substr(1);
			int n = atoi(cmd.c_str());
			vector<pair<int ,string> >::iterator it;
			int len = hist.size();
			string input2 = hist[len-1-n].second;
			input2 = trimmer(input2);
			
			string input = input2;

			string temp = input;
			string namehist = temp.substr(0,7);
			string nameecho = temp.substr(0,4);
			string nameexpo = temp.substr(0,6);
			string namepwd  = temp.substr(0,3);
			string namebng  = temp.substr(0,1);
			
			if(nameecho == "echo")
			{
				int slen=input.length();
				if(slen>5)
				{
					string name = temp.substr(5);
					echo_(name);
				}
				else
					cout<<endl;

			}
			else if( nameexpo == "export")
			{
				int slen=input.length();
				if(slen>7)
				{
					string name = temp.substr(7);
					export_(name);
				}
				else
					cout<<endl;
			}
			else if(namehist == "history")
			{
				int slen=input.length();
				if(slen>8)
				{
					int n=10;
					string name = temp.substr(8);
					n = atoi(name.c_str());

					history(n);
				}
				else
					history(0);
				
			}
			else if(namebng == "!")
			{

				int slen=input.length();
				if(slen>1)
				{
					string name = temp.substr(1);
					bang(name);

				}

			}
			else if(namepwd == "pwd")
			{
				pwd();
			}
			else
				Parse(input);

		}
		else 
		{
			string cmd = name.substr(0);
			int n = atoi(cmd.c_str());
			vector<pair<int ,string> >::iterator it;
			string input2 = hist[n+1-2].second;
			input2 = trimmer(input2);
			string input = input2;
			string temp = input;
			string namehist = temp.substr(0,7);
			string nameecho = temp.substr(0,4);
			string nameexpo = temp.substr(0,6);
			string namepwd  = temp.substr(0,3);
			string namebng  = temp.substr(0,1);
			
			if(nameecho == "echo")
			{
				int slen=input.length();
				if(slen>5)
				{
					string name = temp.substr(5);
					echo_(name);
				}
				else
					cout<<endl;

			}
			else if( nameexpo == "export")
			{
				int slen=input.length();
				if(slen>7)
				{
					string name = temp.substr(7);
					export_(name);
				}
				else
					cout<<endl;
			}
			else if(namehist == "history")
			{
				int slen=input.length();
				if(slen>8)
				{
					int n=10;
					string name = temp.substr(8);
					n = atoi(name.c_str());

					history(n);
				}
				else
					history(0);
				
			}
			else if(namebng == "!")
			{
				int slen=input.length();
				if(slen>1)
				{
					string name = temp.substr(1);
					bang(name);

				}

			}
			else if(namepwd == "pwd")
			{
				pwd();
			}
			else
				Parse(input);

		}




	}



/*..........signal..handler....*/
	void sgnlHndler( int signl )
	{

	
	}

/*..............pipe................*/


	void mypipe(char ***argcmd) 
	{
		int   pipefd[2];
		pid_t cpid;
		int   fd_in = 0;
		int cmdn=0;
		while (argcmd[cmdn]!= NULL)
		{

			if (pipe(pipefd) != 0) 
			{
				perror("pipe");
				exit(EXIT_FAILURE);
			}

			cpid = fork();
			if (cpid == -1)
			{
				perror("fork");
				exit(EXIT_FAILURE);
			}
			else if(cpid == 0)
			{
          dup2(fd_in, 0); //copyng the last input to stdin...
          if (argcmd[cmdn+1] != NULL)
          	dup2(pipefd[1], 1); //to make stdout...
          close(pipefd[0]);
          if(execvp(argcmd[cmdn][0],argcmd[cmdn]))
          {
          	//cout<<"command not found";
          	perror(" ");
          }
          _exit(EXIT_SUCCESS);
      }
      else
      {
      	wait(NULL); //Wait for child
      	close(pipefd[1]);
      	fd_in = pipefd[0]; 
      	cmdn++;
      }
  }
}




/*...............parse................*/
void Parse(string input)
{
	
	vector<string> v;
	int arr[1000],j=0;
	regex r("[a-zA-Z0-9\\-\"\'\\<\\?\\=\\<<\\+\\>>\\$\\*\\>\\!\\_\\/\\@\\. ]+|$");
	for(sregex_iterator i = sregex_iterator(input.begin(), input.end(), r);i != sregex_iterator();++i )
	{
		smatch m = *i;
		arr[j++]=m.position();
	}
	
	int p=0;
	string abc;
	for( p=0;p<j-2;p++)
	{
		abc =  trimmer(input.substr(arr[p],arr[p+1]-arr[p]-1)) ;
		//cout<<abc<<endl;
		v.push_back(abc);
	}
	abc = trimmer(input.substr(arr[p]));
	//cout<<abc<<endl;
	v.push_back(abc);
	char ***argcmd;
	argcmd =new char **[10000];
	int tcmd=0;
	int count=0;


	for(unsigned int Index=0;Index<v.size();Index++)
	{
		string temp = v[Index];
		string token;
		string temp2="";
		
		char **argv;
		int idx=0;

		for(unsigned int i1=0;i1<temp.length();i1++)
			if(temp[i1] == ' ')
				count++;
			count++;
			argv = new char * [count+1];

			for(unsigned int i1=0;i1<temp.length();i1++)
			{
				if(temp[i1]!=' ')
					token += temp[i1];
				else
				{
					argv[idx] = new char [token.length()+1];
					//cout<<"Storing "<<token<<endl;
					strcpy(argv[idx], token.c_str());
					idx++;
					token="";
				}
			}
			argv[idx] = new char [token.length()+1];
			strcpy(argv[idx], token.c_str());
			//cout<<"Storing outside"<<token<<endl;
			idx++;
			argv[idx] = NULL;	
			argcmd[tcmd++]=argv;
			argcmd[tcmd]=NULL;
		}
		
		string check = argcmd[0][0];
		if(check== "cd")
		{
			
			cd(argcmd[0][1]);
		}
		else
			mypipe(argcmd);


	}



	int main()
	{
		int count = 1;
		while(1)
		{

			
			string input;
			char crntWrkDir[1024];
			memset(crntWrkDir,0,1024);
			char hostName[1024];
			memset(hostName,0,1024);
			gethostname(hostName,sizeof(hostName));
			getcwd(crntWrkDir,sizeof(crntWrkDir));
			cout<<getenv("LOGNAME")<<"@"<<hostName<<":~"<<crntWrkDir<<"$";

			signal(SIGINT,sgnlHndler);
			signal(SIGTSTP,sgnlHndler);
			getline(cin,input);


			input = trimmer(input);

			if(hist.size() > 0)
			{
				vector<pair<int ,string> >::reverse_iterator rhit;
				rhit = hist.rbegin();
				if(rhit->second != input  )
				{	
					hist.push_back(pair<int ,string> (count ,input));
					count++;
				}
			}
			else
			{
				hist.push_back(pair<int ,string> (count ,input));
				count++;			
			}

			string temp = input;
			string namehist = temp.substr(0,7);
			string nameecho = temp.substr(0,4);
			string nameexpo = temp.substr(0,6);
			string namepwd  = temp.substr(0,3);
			string namebng  = temp.substr(0,1);
			

			if( input == "exit")
			{
				break;
			}
			if(nameecho == "echo")
			{
				int slen=input.length();
				if(slen>5)
				{
					string name = temp.substr(5);
					echo_(name);
				}
				else
					cout<<endl;

			}
			else if( nameexpo == "export")
			{
				int slen=input.length();
				if(slen>7)
				{
					string name = temp.substr(7);
					export_(name);
				}
				else
					cout<<endl;
			}
			else if(namehist == "history")
			{
				int slen=input.length();
				if(slen>8)
				{
					int n=10;
					string name = temp.substr(8);
					n = atoi(name.c_str());

					history(n);
				}
				else
					history(0);
				
			}
			else if(namebng == "!")
			{
				int slen=input.length();
				if(slen>1)
				{
					string name = temp.substr(1);
					bang(name);

				}

			}
			else if(namepwd == "pwd")
			{
				pwd();
			}
			else
				Parse(input);

			
		}
		return 0;
	}