/* Trabalho pratico 1
	Aléxei Felipe Paim (20250264)
	Fabricio Fabricio Zimmermann (19150429)
	
Codigo Adaptado de https://github.com/crmoratelli/tcp-server/blob/master/tcp-server-multithread.c
*/	

// inclui as bibliotecas necessárias:
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <pthread.h>


// **************************************************************************
// definicoes iniciais
#define BUFFER_SIZE 1024          // tamanho do buffer de recepcao
#define PORT 8080                    // porta de comunicacao

// **************************************************************************


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; //Define uma variável global mutex para sincronizar o acesso aos workers:

// Define a estrutura de dados para representar cada worker:
struct worker_c{
	int work;   // socket do worker
    	int flag;   // 0 livre - 1 Ocupado
    	int cria;   // 1 se o worker foi criado, 0 caso contrário
};

struct worker_c vet_worker[10]; // vetor de workers

// Define a estrutura de dados para representar cada conexão de cliente:
struct client_data{
    int sk;
    struct sockaddr_in *client_addr;
};

 int w=0; //Ininicia variavel global worker 
 
 void tempo_ale(){ // função para Gerar um tempo aleatori de espera 
 
 	srand(time(NULL)); //Inicializa o gerador de numeros aleatorios
 	int random_segu = rand() % 6; // gera um numero eleatorio entre 0 e 5
 	printf(" / / / / %d / / / / \n",random_segu);
    	fflush(stdout);
    
 	sleep (random_segu);
 }
 


//Implementa a função que lida com as conexões dos clientes:
void * connection_identifier(void* cd){

    struct client_data *client = (struct client_data *)cd; //socket
    
    char sendBuff[BUFFER_SIZE]; // BUffer
    
 
    int aux, i = 0,j=0; //Variaveis locais 
    
    memset(sendBuff, 0, sizeof(sendBuff)); //preeenche a memoria com zeros

    /* Imprime IP e porta de quem conectou. */
    printf("Received connection from %s:%d\n", inet_ntoa(client->client_addr->sin_addr), ntohs(client->client_addr->sin_port));
    fflush(stdout);
   
    
       //Recebe a mensagem enviada pela conexão lendo um caracter por vez
        
	while((aux = recv(client->sk,&sendBuff[i],1,0))>0){   
		if(sendBuff[i] == '\0'){ // Até encontar \0
	            break;
        	}
	   i++;
    	}
        
   if (strcmp(sendBuff, "worker") == 0) { // Verifica se conteudo recebido é igual a Worker 
   	if(w < 10){
   		printf("worker conectouu\n");
   		pthread_mutex_lock(&mutex); // Trava mutex
   	 	printf("-------------------");
   	 	fflush(stdout);
   	 	   
   	 	// Grava worker no Array de de workers    
       		vet_worker[w].work = client->sk;
        	vet_worker[w].cria = 1;
        	vet_worker[w].flag = 0;
     	
        	printf("criou worker na posição %d : e valor do flag é %d\n", w,vet_worker[w].flag);
        	fflush(stdout);
        
       		pthread_mutex_unlock(&mutex); //Libera mutex
       		w++;
        
    	}else{//Indica que array de workers ja esta cheio
     
        	printf("Sem espaço para workers");
        	 fflush(stdout);
        }	
     	
    }else if (strcmp(sendBuff, "client") == 0) { // Verifica se conteudo recebido é igual a Worker 
    	
    	 // Confirma a conexão com o client
   	 memset(sendBuff, 0, sizeof(sendBuff));
   	 snprintf(sendBuff, BUFFER_SIZE, "Server conectado\n");
   	 send(client->sk, sendBuff, strlen(sendBuff) + 1, 0);
   	 	 
    	i=0; //reseta 
    	 
    	     
     	memset(sendBuff, 0, sizeof(sendBuff));//preeenche a memoria com zeros
     	
     	//Recebe a mensagem enviada pela conexão lendo um caracter por vez
     	while((aux = recv(client->sk,&sendBuff[i],1,0))>0){
           	if(sendBuff[i] == '\0'){
              		break;
        	}
        i++;
       
    	}
   
   	pthread_mutex_lock(&mutex); //tranca mutex para realização a verificação do array de Workers
   	
	for (j; j <= (sizeof(vet_worker)/4);j++){ // Laço para percorrer as posiçoes do array de workers 
	   
	   	if(j == (sizeof(vet_worker)/4)){ // se percorreu até a ultima posição e não a worker vago, imprime :
	   		 memset(sendBuff, 0, sizeof(sendBuff));
   			 snprintf(sendBuff, BUFFER_SIZE, "Sistema ocupado. Tente mais tarde\n");
   			 send(client->sk, sendBuff, strlen(sendBuff) + 1, 0);
   			  printf("--- Sem Workers disponiveis! \n---");
   			 fflush(stdout);
   			 
   			 pthread_mutex_unlock(&mutex);// libera mutex para realização a verificação do array de Workers
    
	   	
	   		break;
	  	}
	   	else if( vet_worker[j].cria == 1 && vet_worker[j].flag == 0){ // Verifica se o worker ja foi criado e se a flag esta livre para o uso na operação
    		
    			printf ("conectado ao worker %d\n",j);
	   		fflush(stdout);
    		
    			//pthread_mutex_lock(&mutex); //tranca mutex
    			
    				vet_worker[j].flag = 1; // Muda flag
    				
    			pthread_mutex_unlock(&mutex);// libera mutex para realização a verificação do array de Workers
    			
    			tempo_ale(); //espera um tempo aletorio
    			
    			
    				
    			send(vet_worker[j].work, sendBuff, strlen(sendBuff) + 1, 0); // envia operação para worker 
   
    			memset(sendBuff, 0, BUFFER_SIZE);//preeenche a memoria com zeros
    			
        		recv(vet_worker[j].work, sendBuff, BUFFER_SIZE, 0);// recebe a resposta do Worker
        
       	 		send(client->sk, sendBuff, strlen(sendBuff) + 1, 0);// envia o resulatdo do worker para o client.
       	 		
       	 		vet_worker[j].flag = 0; // Libera o worker
       	 		
       	 		

       	 		
    			break;
       		 }
        } 
        
      
   }
    
    return NULL;
}

int main(int argc, char *argv[]){

	//Define as variaveis da main
    int listenfd = 0;
    struct sockaddr_in serv_addr; 
    int addrlen;
    struct client_data *cd;
    pthread_t thr;

    /* Cria o Socket: SOCK_STREAM = TCP */
    if ( (listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
    	perror("socket");
    	return 1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));//preeenche a memoria com zeros

	// Configura servidor para receber conexoes de qualquer endereço:
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(PORT); 

	/* Associa o socket a estrutura sockaddr_in */
    if (bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
    	perror("bind");
    	return 1;
    } 

	/* Inicia a escuta na porta */
    listen(listenfd, 10); 
    printf("Server Aguardando conexão!");
    fflush(stdout);
    

    while(1) {
    	/* Aloca a memória para a estrutura que armazena os dados de quem conectou */
        cd = (struct client_data *)malloc(sizeof(struct client_data));
        cd->client_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
        addrlen = sizeof(struct sockaddr_in);

	/* Aguarda a conexão */	
        cd->sk = accept(listenfd, (struct sockaddr*)cd->client_addr, (socklen_t*)&addrlen); 

	 /* Cria uma nova thread para lidar com a conexão */
        pthread_create(&thr, NULL, connection_identifier, (void *)cd);
        pthread_detach(thr); //a thread se torna autônoma e os recursos alocados por ela são liberados assim que ela termina.

     }
}
