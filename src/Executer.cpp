#include "Executer.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <sys/stat.h>
#include <fcntl.h>

#define MAXLINE 2048
#define FIFO "stdout_fifo"

using namespace std;

Executer::Executer(){

}

Executer::~Executer(){

}

string Executer::exec(string cmd, int pipeflag, string argv){
    string res = "";
    try{
        if(pipeflag == 1){
            res = Executer::execute(cmd);
        }else{
            Executer::forkAndExec(cmd, argv);
        }
        
    }catch(exception e){
        cout << "execute failed\n" << e.what() << endl;
        return "error";
    }
    return res;
}

void Executer::forkAndExec(string cmd, string argv){
    pid_t pid;
    int status;
    int cnt = 1;
    char* args[MAXLINE];
    string arg = argv;
    vector<string> vec;
    if(vec.size()>0) vec.clear();
    int length=arg.length();
    int start=0;
    for(int i=0;i<length;i++){
        if(arg[i]==' ' && i==0)
            start+=1;
        else if(arg[i]==' '){
            vec.push_back(arg.substr(start,i-start));
            start=i+1;
        }
        else if(i==length-1){
            vec.push_back(arg.substr(start,i+1-start));
        }
    }
    printf("command:【");
    cout << cmd ;
    vector<string>::iterator iter = vec.begin();
    while(iter != vec.end()){
        cout << " " << *iter;
        args[cnt] = (char*)(*iter).c_str();
        cnt ++ ;
        iter++ ;
    }
    printf("】");
    args[0] = (char*)(cmd.c_str());
    // Fork and exec the command
    pid = fork();
    if (pid == 0) {
        // Child process 
        if (execvp(cmd.c_str(), args) == -1) {
            cout << endl << "EXEC :ERROR with system call execve" << endl;
        }
        // never reached
    }
    // Parent process
    waitpid(pid, NULL, 0);
    printf("exit\n");
}

string Executer::execute(string cmd){
    /** 
     * version 1.0 begin
     * 
    int ret = 0;
    ret = system(cmd.c_str());
    return ret;
    *
    * version 1.0 end
    * */

    /**
     * version 2.0 begin
     */
    char result_buf[MAXLINE];
    string res = "";
    int ret = 0;
    FILE *fp;
    //popen_res res_t;
    //res_t = Executer::mypopen((char*)cmd.c_str(), 'r');
    //fp = res_t.fp;
    printf("command:【%s】", cmd.c_str());
    fp = popen(cmd.c_str(), "r");
    if(fp == NULL){
        perror("popen error");
        exit(1);
    }
    while(fgets(result_buf, sizeof(result_buf), fp) != NULL){
        res.append(result_buf);
    }

    //wait and close fp&pipe
    ret = fclose(fp);
    if(ret == -1){
        perror("close file pointer failed");
        exit(1);
    }
    else{
        printf("exit with status【%d】command return【%d】\n", ret, WEXITSTATUS(ret));
    }
    //res = res + "pid" + to_string(res_t.pid);
    return res;
    /*
    * version 2.0 end
    * */

   /**
    * version 3.0 begin
    * 
    pid_t pid;
    int outfds[2];
    pipe(outfds);
    pid = fork();
    if(pid==0){
        Executer::child(cmd, outfds[1]);
    }else{
        string res = Executer::parent(outfds[0]);
        return string("pid") + to_string(pid) + string("\n") + res;
    }
    *
    * version 3.0 end
    * */
   /**
    * version 4.0 begin
    * 
    int fpipe[2] = {0};
	pid_t fpid;
	char message[MAXLINE] = {0};
    string res = "";
	memset(message, 0, 20);
	if (pipe(fpipe) < 0)
	{
		printf("Create pipe error!\n");
	}
	fpid = fork();
	if (fpid == 0)
	{
		close(fpipe[0]);
		dup2(fpipe[1],STDOUT_FILENO);
		//system("ls");
        printf("%d\n", getpid());
        execvp(cmd.c_str(), NULL);
	}
	else if (fpid > 0)
	{
		wait(NULL);
		close(fpipe[1]);
		read(fpipe[0], message, MAXLINE);
        res = res + message;
        cout<<"res:"<<res<<endl<<"end"<<endl;
		//printf("%s\n",massage);
	}
	else
	{
		printf("create fork error!\n");
	}
	return res;
    *
    * version 4.0 end
    * */
    /**
     * version 5.0 begin
     * 
    string res = "";
    unlink(FIFO);
    int ret = mkfifo(FIFO, 0777);
    if(ret == -1){
        perror("mkfifo error");
        exit(-1);
    }
    if(fork()==0){
        int fd = open(FIFO, O_WRONLY);
        dup2(fd, 1);
        printf("pid%d\n", getpid());
        execl((char*)cmd.c_str(), "-a", (char*)0);
    }else{
        char message[MAXLINE] = {0};
        int fd = open(FIFO, O_RDONLY);
        if(fd == -1){
            perror("pipe open error");
            exit(-1);
        }
        wait(NULL);
        read(fd, message, sizeof(message));
        res = message;
        return res;
    }
    return "";
    *
    * version 5.0 end
    * */
}

string Executer::parent(int in){
    ssize_t len;
    char buf[MAXLINE];
    string res = "";
    len = read(in, buf, strlen(buf));
    if(len > 0){
        buf[len] = '\0';
        res = res + buf;
    }
    return res;
}

void Executer::child(string cmd, int out){
    dup2(out, STDOUT_FILENO);
    int ret = system(cmd.c_str());
    printf("command:【%s】exit with status【%d】command return【%d】\n", cmd.c_str(), ret, WEXITSTATUS(ret));
}

popen_res Executer::mypopen(char *cmd,char type)
{
    
    
	int pipefd[2];           //管道描述符
	int pid_t;               //进程描述符

    popen_res res;

	if(type !='r' && type != 'w')
	{
		printf("popen flag error/n");
        res.fp=NULL;
		return res;
	}

	if(pipe(pipefd)<0)        //建立管道
	{
		printf("popen pipe create error/n");
		res.fp=NULL;
		return res;
	}
	
	pid_t=fork();             //建立子进程

	if(pid_t < 0) {
        res.fp=NULL;
		return res;
    }
 		

	if(0 == pid_t)            //子进程中......
	{   
		if(type == 'r')
		{
			close(pipefd[0]);               //此函数返回文件指针是用于读，因此，在子进程中应该将结果写入管道，这样父进程返回的文件指针才能读，所以这里将读端关闭
			if(dup2(pipefd[1],STDOUT_FILENO) == -1){
                perror("dup failed!");
                exit(-1);
            } 
            close(pipefd[1]);		

		}
		else{
			close(pipefd[1]);
			dup2(pipefd[0],STDIN_FILENO);
			close(pipefd[0]);
		}
		char *argv[] = {cmd,NULL};	
		if(execvp(cmd,argv)<0) {//用exec族函数执行命令
            res.fp=NULL;
            
		    return res;
        }   	
	}
	
	wait(0);                                //等待子进程返回

	if(type=='r'){
		close(pipefd[1]);
        
        res.fp = fdopen(pipefd[0],"r");
        res.pid = pid_t;
		return res;	//由于程序需要返回的参数是文件指针，因此需要用fdopen函数将描述符打开，其返回值为相应的文件指针	
	}else{
		close(pipefd[0]);
        
        res.fp = fdopen(pipefd[1],"w");
        res.pid = pid_t;
		return res;
	}
}

