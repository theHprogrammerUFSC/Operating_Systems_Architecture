// Author: Helder Henrique da Silva
// Year: 2022/2
//
// Description: Implementação de atividade de sorteio de pids utilizando sockets.
//

#include <stdlib.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <locale.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 5001

void processoPai()
{
	// Variáveis para o socket
	int socketServidor;
	int socket_retorno[10];
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// Variáveis para análise dos pids
	char lista_PID[10][1024];
	int qtd_pidRecebido = 0;
	int option = 1;
	char buffer[1024] = {0};

	// socket() cria um novo socket
	socketServidor = socket(AF_INET, SOCK_STREAM, 0);

	// Configuração do socket
	setsockopt(socketServidor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &option, sizeof(option));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// bind() associa o socket a um endereço
	bind(socketServidor, (struct sockaddr *)&address, sizeof(address));

	// listen() coloca o socket em modo de escuta
	listen(socketServidor, 10);

	// Enquanto não receber todos os pids
	while (qtd_pidRecebido < 10)
	{
		// accept() aceita uma conexão
		socket_retorno[qtd_pidRecebido] = accept(socketServidor, (struct sockaddr *)&address, (socklen_t *)&addrlen);

		// read() lê os dados enviados pelo cliente
		read(socket_retorno[qtd_pidRecebido], buffer, 1024);

		// Salva o pid recebido e incrementa a quantidade de pids recebidos
		strcpy(lista_PID[qtd_pidRecebido], buffer);
		qtd_pidRecebido++;
	}

	// Sorteia um pid
	srand(time(NULL));
	const char *pidSorteado = lista_PID[rand() % 10];
	printf("PID sorteado: %s\n", pidSorteado);

	// Envia o pid sorteado para todos os filhos
	for (int i = 0; i < 10; i++)
	{
		send(socket_retorno[i], pidSorteado, strlen(pidSorteado), 0);
	}

	// waitpid() espera que o processo filho termine. WUNTRACED retorna se o processo filho foi parado
	int status;
	waitpid(-1, &status, WUNTRACED);
}

void processoFilho()
{
	// Variáveis para o socket
	int socketCliente = 0;
	struct sockaddr_in serv_addr;

	// Variáveis para análise dos pids
	const char *message = (std::to_string(getpid())).c_str();
	char buffer[1024] = {0};

	// socket() cria um novo socket
	socketCliente = socket(AF_INET, SOCK_STREAM, 0);

	// Configuração do socket
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(PORT);
	inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

	// Enquanto não conseguir conectar ao servidor
	while (true)
	{
		// connect() conecta o socket ao endereço
		if (connect(socketCliente, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) >= 0)
			break;
	}

	// send() envia dados para o servidor
	send(socketCliente, message, strlen(message), 0);

	// Enquanto não receber o pid sorteado
	int pidSorteado = 0;
	while (pidSorteado <= 0)
	{
		// read() lê os dados enviados pelo servidor
		pidSorteado = read(socketCliente, buffer, 1024);
	}

	// Converte o pid sorteado para inteiro e imprime
	if (stoi(std::string(buffer)) == getpid())
	{
		printf("%s: fui sorteado\n", buffer);
	}
}

int main()
{
	// Verifica se é o pai ou o filho
	bool isParent;

	// Cria os filhos
	for (int i = 0; i < 10; i++)
	{
		pid_t child = fork();
		isParent = false;
		if (child == 0)
		{
			processoFilho();
			break;
		}
		else
			isParent = true;
	}

	// Se for o pai, executa o processo pai
	if (isParent)
		processoPai();
	return 0;
}