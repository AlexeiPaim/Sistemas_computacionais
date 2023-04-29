// implementa um cliente

/* Trabalho pratico 1
	Aléxei Felipe Paim (20250264)
	Fabricio Zimmermann (19150429)
*/
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <netdb.h>

// **************************************************************************
// definicoes iniciais
#define TAMANHO_BUFFER 1024          // tamanho do buffer de recepcao
#define PORT 8080                    // porta de comunicacao

// **************************************************************************

int main(int argc, char *argv[]) {

  char buff[TAMANHO_BUFFER]; // Buffer

    if (argc < 5){
        printf("Usage: %s <ip address> <list of parameters>\n", argv[0]);
        exit(0);
    }

    // variaveis
    int sockfd, env_opr;
    struct sockaddr_in serv_addr;
    const char client_hello[] = "client"; // mensagem de sinal para server
    struct hostent *serv;


    // cria o socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        printf("Erro ao abrir o socket.\n");
        return 1;
    }
    
    // configura o endereco do servidor
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT); 
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // conecta ao servidor
    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        printf("Erro ao conectar.\n");
        return 1;
    }
    
    /* Mensagem cliente */
    if (send(sockfd, client_hello, strlen(client_hello) + 1, 0) < 0) {
        perror("Error sending client");
        exit(EXIT_FAILURE);
    }
    
     /*Recebe confirmação de conexão do servidor*/
    memset(buff, 0, TAMANHO_BUFFER);
    recv(sockfd, buff, TAMANHO_BUFFER, 0);
    
    /*Confirmação de conexão*/
    printf("%s\n", buff);
    fflush(stdout);
    
    // Estabeleceu conexao com o servidor e pode trocar dado    
	//extrai os dados do argumento argv
       int num1 = atoi(argv[3]);
       int num2 = atoi(argv[4]);
       
 	//Monta a mensagem de operação para enviar ao server
       sprintf(buff, "%s %d %d", argv[2], num1, num2);
       
	//Envia mensagem de operaçao para server
        env_opr = send(sockfd,buff,strlen(buff)+1, 0);
        if (env_opr < 0) {
            printf("Erro ao enviar mensagem.\n");
            return 1;
        }

        memset(buff, 0, TAMANHO_BUFFER); //preeenche a memoria com zeros
        recv(sockfd, buff, TAMANHO_BUFFER, 0); // Recebe a menagem final do server
        printf("Resultado: %s\n", buff); // imprime na tela o resultado final
        fflush(stdout);

  
    // Encerra a conexao
    close(sockfd);
    return 0;
}

