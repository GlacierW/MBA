#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <monitor/monitor.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "agent.h"

#define MAX_REDIR_PORT 55535
#define GUEST_PORT 8888
#define REDIR_PORT_LENGHT 10
#define MAX_COMMAND_SIZE 256
#define AGENT_SYSTEM_RECEIVE 18
#define MAX_FILE_SIZE 1638400  
#define FILE_SIZE_LENGHT 11 
#define MESSAGE_SIZE_LENGHT 11
#define STAT_COMMAND_RETURN 37
#define THREAD_IDLE 0
#define THREAD_BUSY 1
#define AGENT_IDLE 0
#define TEN_SECOND 10
#define MBA_CMD_IMPO 1 //  Command begging with 'impo' from MBA     
#define MBA_CMD_EXPO 2 //  Command begging with 'expo' from MBA     
#define MBA_CMD_EXEC 3 //  Command begging with 'exec' from MBA     
#define MBA_CMD_INVO 4 //  Command begging with 'invo' from MBA     
#define MBA_CMD_STAT 5 //  Command begging with 'stat' from MBA     
#define MBA_CMD_LOGF 6 //  Command begging with 'logf' from MBA 

char* redir_Port;
char* agent_srcpath;
char* agent_despath;
char buf[MAX_FILE_SIZE], receive[AGENT_SYSTEM_RECEIVE];
unsigned int agent_action;
unsigned int agent_thread;
uint16_t port;
int sockfd, numbytes;
struct sockaddr_in servaddr;

char* getRandomRedirPort(void) {
    srand(time(NULL));
    int r = (rand() % MAX_REDIR_PORT) + 10001, rNum = 0, temp = r;
    port = r ;
    for ( ; temp != 0 ; temp /= 10)
        rNum++;
    char* randPort = calloc( rNum + REDIR_PORT_LENGHT, sizeof(char) );
    sprintf( randPort, "tcp:%d::%d", r, GUEST_PORT );
    printf( "randPort:%s   port:%d\n", randPort, port );
    return randPort;
} // getRandomRedirPort()

void initial_string(char* word, int len) {
    int run = 0;
    for ( ; run < len ; run++ ) 
        word[run] = '\0';
} // initial_string()

void init_socket(void) { 
    sockfd = socket( AF_INET,SOCK_STREAM, 0 );
    bzero(&servaddr,sizeof servaddr);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET,"127.0.0.1",&(servaddr.sin_addr));  
} // init_socket()

void print_system_receive(Monitor* mon, int receiveLength) {
    char receiveCommand[MAX_COMMAND_SIZE];
    initial_string( receiveCommand, MAX_COMMAND_SIZE);
    numbytes = read(sockfd, receive, AGENT_SYSTEM_RECEIVE );
    monitor_printf(mon, "%s", receive );
    numbytes = read(sockfd, receiveCommand, receiveLength );
    monitor_printf(mon, "%s\n", receiveCommand );
} // print_system_receive()

void import_cmd(Monitor* mon) {
    long long int file_size;
    char command[MAX_COMMAND_SIZE], char_file_size[FILE_SIZE_LENGHT];
    FILE *fp;
    fp = fopen(agent_srcpath, "rb" );
    if ( fp ) {
        initial_string( command, MAX_COMMAND_SIZE );
        initial_string( buf, MAX_FILE_SIZE );
        initial_string( char_file_size, FILE_SIZE_LENGHT );
        initial_string( receive, AGENT_SYSTEM_RECEIVE );
        init_socket();
        file_size = fread(buf, sizeof(char), sizeof(buf), fp);
        sprintf( char_file_size, "%lld", file_size );
        sprintf( command, "impo %s %s", agent_despath, char_file_size );   
        if ( file_size > MAX_FILE_SIZE ) {
            monitor_printf(mon, "Impo file size should be less than 16MB.\n(qemu)");
        } // if
        else {
            connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));         
            numbytes = write(sockfd, command, sizeof(command) );
            while ( file_size != 0 ) {
 	            numbytes = write(sockfd, buf, file_size );
                file_size -= numbytes;
            } // while

            monitor_printf(mon, "File_sending_over\n(qemu)");
 	        fclose(fp);
        } // else
 
        print_system_receive(mon, strlen(command));
        monitor_printf(mon, "get impo cmd From:%s To:%s numbytes:%d\n(qemu)", agent_srcpath, agent_despath, numbytes);
    } // if
    else {
        monitor_printf(mon, "Could not find the file\n(qemu)");
    } // else

    agent_action = AGENT_IDLE;
    close(sockfd);
} // import_cmd()

void export_cmd(Monitor* mon) {
    char command[MAX_COMMAND_SIZE], get_file_size[FILE_SIZE_LENGHT];
    FILE *fp;   
    long long int file_size;
    init_socket();
    initial_string( command, MAX_COMMAND_SIZE );
    initial_string( buf, MAX_FILE_SIZE );
    initial_string( get_file_size, FILE_SIZE_LENGHT );
    sprintf( command, "expo %s", agent_srcpath );
    fp = fopen(agent_despath, "w" );
    if ( fp ) { 
        connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
        numbytes = write(sockfd, command, sizeof(command) ); 
        numbytes = read(sockfd, get_file_size, FILE_SIZE_LENGHT );
        file_size = atoi(get_file_size);
        monitor_printf(mon, "file_size:%lld\n(qemu)", file_size);
        if ( atoi(get_file_size) > MAX_FILE_SIZE ) {
            monitor_printf(mon, "Expo file size should be less than 16MB.\n(qemu)");
        } // if
        else {                
            while ( file_size > 0 ) { 
                numbytes = read(sockfd, buf, file_size );
                fwrite(buf, sizeof(char), numbytes, fp);	
                file_size -= numbytes;
            } // while

            print_system_receive(mon, strlen(command));
            monitor_printf(mon, "get expo cmd From:%s To:%s bytes:%d\n(qemu)", agent_srcpath, agent_despath, numbytes);
        } // else 

        fclose(fp);
    } // if
    else {
        monitor_printf(mon, "export file failed\n(qemu)");
    } // else
 	
    agent_action = AGENT_IDLE;
    close(sockfd);
} // export_cmd()

void execute_cmd(Monitor* mon) {
    char command[MAX_COMMAND_SIZE], execReturn[MAX_COMMAND_SIZE], getMessageSize[MESSAGE_SIZE_LENGHT],
    getMessage[MAX_FILE_SIZE], readMessageBuf[MAX_FILE_SIZE];
    init_socket();
    initial_string( command, MAX_COMMAND_SIZE);
    initial_string( execReturn, MAX_COMMAND_SIZE);
    initial_string( getMessageSize, MESSAGE_SIZE_LENGHT);
    initial_string( getMessage, MAX_FILE_SIZE);
    sprintf( command, "exec %s", agent_despath );
    
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    numbytes = write(sockfd, command, sizeof(command) );
    monitor_printf(mon, "ready for reading message\n");
    numbytes = read(sockfd, getMessageSize, MESSAGE_SIZE_LENGHT );
    monitor_printf(mon, "[getMessageSize:%s] [numbytes:%d]\n", getMessageSize, numbytes);
    while ( strcmp( getMessageSize, "0" ) != 0 ) {
        initial_string( readMessageBuf, MAX_FILE_SIZE );
        numbytes = read(sockfd, readMessageBuf, atoi(getMessageSize) );
        monitor_printf(mon, "getMessage:%s\n", readMessageBuf);
        strcat( getMessage, readMessageBuf );
        numbytes = read(sockfd, getMessageSize, MESSAGE_SIZE_LENGHT );
        monitor_printf(mon, "[getMessageSize:%s] [numbytes:%d]\n", getMessageSize, numbytes);
    } // while
    
    monitor_printf(mon, "Reading Message Done\n");
    print_system_receive(mon,  strlen(command) );
    monitor_printf(mon, "get exec cmd %s bytes:%d\n(qemu)", agent_despath, numbytes);
    agent_action = AGENT_IDLE;
    close(sockfd);
} // exec_cmd()

void invoke_cmd(Monitor* mon) {
    char command[MAX_COMMAND_SIZE];
    init_socket();
    initial_string( command, MAX_COMMAND_SIZE);
    sprintf( command, "invo %s", agent_despath );
 
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    numbytes = write(sockfd, command, sizeof(command) );
    print_system_receive(mon, strlen(command));
    monitor_printf(mon, "get invo cmd %s bytes:%d\n(qemu)", agent_despath, numbytes);
    agent_action = AGENT_IDLE;
    close(sockfd);
} // invoke_cmd()

void status_cmd(Monitor* mon) {
    char command[MAX_COMMAND_SIZE], stat[STAT_COMMAND_RETURN];
    init_socket();
    initial_string( command, MAX_COMMAND_SIZE);
    initial_string( stat, STAT_COMMAND_RETURN );
    sprintf( command, "stat" );
    connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
    numbytes = write(sockfd, command, sizeof(command) );
	monitor_printf(mon, "get status cmd bytes:%d\n", numbytes);
                
    struct timeval timeout;
    timeout.tv_sec = TEN_SECOND;
    timeout.tv_usec = 0;
    if (setsockopt (sockfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
        sizeof(timeout)) < 0)
            monitor_printf(mon,"setsockopt failed\n");

    numbytes = read(sockfd, stat, STAT_COMMAND_RETURN );
    if ( numbytes > 0 )
        monitor_printf(mon, "%s\n(qemu)", stat );
    else
        monitor_printf(mon, "agent not working\n(qemu)" );
    print_system_receive(mon, strlen(command));
    agent_action = AGENT_IDLE;
    close(sockfd);
} // status_cmd()

void logf_cmd(Monitor* mon) {
    char command[MAX_COMMAND_SIZE], get_file_size[FILE_SIZE_LENGHT];
    FILE *fp;   
    long long int file_size;
    init_socket();
    initial_string( command, MAX_COMMAND_SIZE );
    initial_string( buf, MAX_FILE_SIZE );
    initial_string( get_file_size, FILE_SIZE_LENGHT );
    sprintf( command, "logf" );
    fp = fopen(agent_despath, "w" );
    if ( fp ) { 
        connect(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
        numbytes = write(sockfd, command, sizeof(command) ); 
        numbytes = read(sockfd, get_file_size, FILE_SIZE_LENGHT );
        file_size = atoi(get_file_size);
        monitor_printf(mon, "file_size:%lld\n", file_size);
        if ( atoi(get_file_size) > MAX_FILE_SIZE ) {
            monitor_printf(mon, "file size should be less than 16MB.\n(qemu)");
        } // if
        else {                
            while ( file_size > 0 ) { 
                numbytes = read(sockfd, buf, file_size );
                fwrite(buf, sizeof(char), numbytes, fp);	
                file_size -= numbytes;
            } // while

            print_system_receive(mon, strlen(command));
            monitor_printf(mon, "get logf cmd\n(qemu)");
        } // else 

        fclose(fp);
    } // if
    else {
        monitor_printf(mon, "export log failed\n(qemu)");
    } // else
 	
    agent_action = AGENT_IDLE;
    close(sockfd);
} // logf_cmd()
void* agent_thread_main(void *monitor)
{
	Monitor* mon = (Monitor* )monitor;
    while(true) {
        if ( agent_action == MBA_CMD_IMPO ) {
            import_cmd(mon);                    
        } // if
        else if ( agent_action == MBA_CMD_EXPO ) {
            export_cmd(mon);  
        } // else if
        else if ( agent_action == MBA_CMD_EXEC ) {
            execute_cmd(mon);
        } // else if
        else if ( agent_action == MBA_CMD_INVO ) {
            invoke_cmd(mon);
        } // else if
        else if ( agent_action == MBA_CMD_STAT ) {
            status_cmd(mon);
        } // else if
        else if ( agent_action == MBA_CMD_LOGF ) {
            status_cmd(mon);
        } // else if
		sleep(1);
    } // while
}

void create_agent_thread(Monitor *mon)
{
    if ( agent_thread == THREAD_IDLE ) {
        agent_srcpath = calloc( MAX_COMMAND_SIZE, sizeof(char) );
        agent_despath = calloc( MAX_COMMAND_SIZE, sizeof(char) );    
        agent_action = AGENT_IDLE, agent_thread = THREAD_BUSY;
        pthread_t agent_thread;	
        pthread_create(&agent_thread, NULL, agent_thread_main,(void *)mon);
        monitor_printf(mon, "Agent Thread Starting.\n");
    } // if
    else {
        monitor_printf(mon, "Agent Thread had started.\n(qemu) ");
    }
}
