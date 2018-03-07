/***************************************************************
* AUTHOR	: Vijoy Sunil Kumar
* DATE		: 03/02/2018
* DESCRITPTION	: This program demostrates different IPC mechanisms
		  that is used to transfer a message structure
		  between two processes which is also used to control 
		  user LED0 on BBB board.

* HEADER FILES	: ipc_demo.h
****************************************************************/
#include "ipc_demo.h"

packet child_to_parent[1];
packet parent_to_child[1];
packet* buffer;

int setup_message_struct(void)
{
	//set up message packet from child to parent
        
        strcpy(child_to_parent->msg,"LED ON");
        child_to_parent->led_control = 1;	//child sends msg to turn on led

	//set up message packet from parent to child
        
        strcpy(parent_to_child->msg,"LED OFF");
        parent_to_child->led_control = 0;	//parent sends msg to turn off led

        buffer = (packet*)malloc(sizeof(packet));
        if(buffer == NULL) 
		return 0;

	return 1;
        
}

int main()
{
	int rt;
	rt = setup_message_struct();
	if(!rt)
	{
		printf("ERROR: malloc message buffer fail\n");
		return 0;
	}

	READY_LED;


#ifdef MQU
	int priority, len_bytes;
        struct mq_attr attr = {
				.mq_maxmsg = 16, 
                                .mq_msgsize = BUFFER_DEPTH, 
                                .mq_flags = 0
				};

        mqd_t msg_queue;



        switch(fork()) 
	{
		case 0:   
			sleep(1);     
		        printf("CHILD: running\n");
		        

		        msg_queue = mq_open(MSQ, O_RDWR, S_IRWXU, &attr);

		        if(msg_queue < 0) 
			{ 
				printf("ERROR: message queue open fail\n"); 
				return 0;
			}

		        len_bytes = mq_receive(msg_queue, (char*)buffer, BUFFER_DEPTH, &priority);
		        if(len_bytes < 0) 
			{
				printf("ERROR: parent_to_child message receiving fail\n");
				return 0;
			}

		        else 
			{
				printf("RECEIVED: parent_to_child string: %s\t led_control: %d\n", buffer->msg, buffer->led_control);
				SET_LED(buffer->led_control);
		                sleep(1);
		        }

		        len_bytes = mq_send(msg_queue, (const char*)child_to_parent, sizeof(child_to_parent), priority);
		        if(len_bytes < 0) 
			{
				printf("ERROR: child_to_parent message sending fail\n"); 
				return 0;
			}
		        else 
				printf("SENT: message from child to parent\n");

			printf("CHILD: exiting\n");
		        break;


		default:
		        printf("PARENT: running\n");

		        priority = 13;

			// parent process creates the message queue
		        msg_queue = mq_open(MSQ, O_CREAT| O_RDWR, S_IRWXU, &attr);
		        if(msg_queue < 0) 
			{ 
				printf("ERROR: message queue open fail\n"); 
				return 0;
			}

		        len_bytes = mq_send(msg_queue, (const char*)parent_to_child, sizeof(parent_to_child), priority);
		        if(len_bytes < 0) 
			{
				//printf("ERROR: parent_to_child message sending fail\n"); 
				//return 0;
			}
		        else 
				printf("SENT: message from parent_to_child\n");

			sleep(4);



		        len_bytes = mq_receive(msg_queue, (char*)buffer, BUFFER_DEPTH, &priority);
		        if(len_bytes < 0) 
			{
				printf("ERROR: child_to_parent message receiving fail\n");
				return 0;
			}

		        else 
			{
				printf("RECEIVED: child_to_parent string: %s\t led_control: %d\n", buffer->msg, buffer->led_control);
				SET_LED(buffer->led_control);
		        }
			printf("PARENT: exiting\n");
			break;			
			
		case -1:
		        printf("ERROR: fork() fail\n"); 
			return 0;


        }
	mq_close(msg_queue);

        mq_unlink(MSQ);
#endif


#ifdef SHM

        int desc_file_shm;
        void* shm_obj;

	char shm_name[] = "my_shm";

        switch(fork()) 
	{
		case 0:
			printf("CHILD: running\n");
		        sleep(1);

		        desc_file_shm = shm_open(shm_name,O_CREAT|O_RDWR,0666);

		        ftruncate(desc_file_shm,BUFFER_DEPTH);
		        shm_obj = mmap(0, BUFFER_DEPTH, PROT_READ|PROT_WRITE, MAP_SHARED, desc_file_shm, 0);

		        if(shm_obj == NULL) 
				printf("ERROR: shared memory map error\n");

			printf("RECEIVED: parent_to_child string: %s\t led_control: %d\n", ((packet*)shm_obj)->msg, ((packet*)shm_obj)->led_control);
			SET_LED(((packet*)shm_obj)->led_control);

		        sleep(1);

		        bzero(shm_obj,sizeof(packet));
		        
			if(memcpy(shm_obj, child_to_parent, sizeof(child_to_parent))== NULL)
				printf("ERROR: memcpy fail\n");
			else
		        	printf("SENT: message from child_to_parent\n");

		        sleep(1);
		        shm_unlink(shm_name);
			printf("CHILD: exiting\n");
		        break;


		default:
			printf("PARENT: running\n");
		
			// parent process creates shared memory
		        desc_file_shm = shm_open(shm_name,O_CREAT|O_RDWR,0666);

		        ftruncate(desc_file_shm,BUFFER_DEPTH);
		        shm_obj = mmap(0, BUFFER_DEPTH ,PROT_WRITE | PROT_READ, MAP_SHARED,desc_file_shm, 0);

		        if(shm_obj == NULL) 
				printf("ERROR: shared memory map error\n");

		        bzero(shm_obj,sizeof(packet));
		        
			if(memcpy(shm_obj, parent_to_child, sizeof(parent_to_child))== NULL)
				printf("ERROR: memcpy fail\n");
			else
		        	printf("SENT: message from parent_to_child\n");

		        sleep(4);
  
			printf("RECEIVED: child_to_parent string: %s\t led_control: %d\n", ((packet*)shm_obj)->msg, ((packet*)shm_obj)->led_control);
			SET_LED(((packet*)shm_obj)->led_control);
		        shm_unlink(shm_name);
			printf("PARENT: exiting\n");
		        break;

		case -1:
		        printf("ERROR: fork() fail\n"); 
			return 0;
        }
#endif

#ifdef PIPES
        int desc_file[2];

        rt = pipe(desc_file);
        if (rt == -1) 
	{
		printf("ERROR: pipe file descriptor error\n"); 
		return 0;
	}

	// fork() this process to create a child process
        switch(fork()) 
	{

		case 0:
			printf("CHILD: running\n");

		        rt = read(desc_file[0], buffer, BUFFER_DEPTH);

		        if(rt == -1) 
			{
				printf("ERROR: parent_to_child message receiving fail\n"); 
				return 0;
			}

		        else 
			{
				printf("RECEIVED: parent_to_child string: %s\t led_control: %d\n", buffer->msg, buffer->led_control);
				SET_LED(buffer->led_control);
			}
				
		        rt = write(desc_file[1], child_to_parent, sizeof(child_to_parent));
		                  

		        if(rt == -1) 
			{
				printf("ERROR: child_to_parent message sending fail\n"); 
				return 0;
			}

		        else 
				printf("SENT: message from child to parent\n");

		        close(desc_file[0]); 
			close(desc_file[1]);

		        printf("CHILD: exiting\n");
		        break;


		default:
		        printf("PARENT: running\n");
		        rt = write(desc_file[1], parent_to_child, sizeof(parent_to_child));
		                  

		        if(rt == -1) 
			{
				printf("ERROR: parent_to_child message sending fail\n"); 
				return 0;
			}

		        else 
				printf("SENT: message from parent to child \n");

			sleep(1);

		        rt = read(desc_file[0], buffer, BUFFER_DEPTH);
		        if(rt == -1) 
			{
				printf("ERROR: child_to_parent message receiving fail\n"); 
				return 0;
			}

		        else 
			{
				printf("RECEIVED: child_to_parent string: %s\t led_control: %d\n", buffer->msg, buffer->led_control);
				SET_LED(buffer->led_control);
			}

		        sleep(1);
		        close(desc_file[0]); 
			close(desc_file[1]);

		        printf("PARENT: exiting\n");
			break;

		case -1:
		        printf("ERROR: fork() fail\n"); 
			return 0;

        }
#endif

// Reference: https://www.geeksforgeeks.org/socket-programming-cc/
//	      http://beej.us/guide/bgipc/html/single/bgipc.html#unixsock
#ifdef SOCKETS

	int s, s2, t, len, done, len_bytes;
	struct sockaddr_un local, remote;

        switch(fork()) 
	{
		case 0:
			printf("CHILD: running\n");
			if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
			{
				printf("ERROR: socket open fail\n");
			   	return 0;
			}

			printf("Trying to connect...\n");

			remote.sun_family = AF_UNIX;
			strcpy(remote.sun_path, SOCK_PATH);
			len = strlen(remote.sun_path) + sizeof(remote.sun_family);
			if (connect(s, (struct sockaddr *)&remote, len) == -1) 
			{
				printf("ERROR: socket connection fail\n");
			 	return 0;
			}

			printf("Connected.\n");

			if (send(s, child_to_parent, sizeof(child_to_parent), 0) == -1) 
			{
				printf("ERROR: child_to_parent message sending fail\n"); 
				return 0;
			}


			len_bytes = recv(s, buffer, sizeof(buffer), 0);
			if (len_bytes < 0) 
			{
				printf("ERROR: parent_to_child message receiving fail\n"); 
				return 0;
			}
			printf("RECEIVED: parent_to_child string: %s\t led_control: %d\n", buffer->msg, buffer->led_control);
			SET_LED(buffer->led_control);

			if (send(s, "DONE", strlen("DONE"), 0) == -1) 
			{
				printf("ERROR: child_to_parent message sending fail\n"); 
				return 0;
			}

			printf("CHILD: exiting\n");
			close(s);

			break;

		default:
			printf("PARENT: running\n");

			if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) 
			{
				printf("ERROR: socket open fail\n");
			   	return 0;
			}

			local.sun_family = AF_UNIX;
			strcpy(local.sun_path, SOCK_PATH);
			unlink(local.sun_path);
			len = strlen(local.sun_path) + sizeof(local.sun_family);

			if (bind(s, (struct sockaddr *)&local, len) == -1) {
				printf("ERROR: socket bind fail\n");
				return 0;
			}

			if (listen(s, 5) == -1) 
			{
			   	printf("ERROR: socket listen fail\n");
				return 0;
			}

			//for(;;) 
			//{
		        	printf("Waiting for a connection...\n");
			    	t = sizeof(remote);
			    	if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) 
				{
					printf("ERROR: socket accept fail\n");
					return 0;
			 	}

		    		printf("Connected.\n");


		    		len_bytes = recv(s2, buffer, sizeof(buffer), 0);
		    		if (len_bytes < 0) 
				{
					printf("ERROR: child_to_parent message receiving fail\n");
					return 0;					
		    		}

				printf("RECEIVED: child_to_parent string: %s\t led_control: %d\n", buffer->msg, buffer->led_control);
				SET_LED(buffer->led_control);


				if (send(s2, parent_to_child, sizeof(parent_to_child), 0) < 0) 
				{
					perror("send");
					done = 1;
				}

		    		len_bytes = recv(s2, buffer, sizeof(buffer), 0);
		    		if (len_bytes < 0) 
				{
					printf("ERROR: child_to_parent message receiving fail\n"); 
					return 0;
		    		}

		    		close(s2);
		   // }
		break;
		case -1:
		        printf("ERROR: fork() fail\n"); 
			return 0;

        }

#endif

        free(buffer);
        return 0;
}

