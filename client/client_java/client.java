import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class client {

    public static void main(String[] args) {

        try (
            Socket echoSocket = new Socket("127.0.0.1", 8080);// Cria um socket para se conectar ao servidor
            PrintWriter out = new PrintWriter(echoSocket.getOutputStream(), true);// Cria um PrintWriter para enviar mensagens ao servidor
            BufferedReader in = new BufferedReader(new InputStreamReader(echoSocket.getInputStream()));// Cria um BufferedReader para ler mensagens do servidor
            BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in))// Cria um BufferedReader para ler mensagens do usuário
        ) {
            System.out.print("Digite seu nome: ");
            String userName = stdIn.readLine(); // Solicita ao usuário que digite seu nome

            String userInput;
            while (true) {// Enquanto o usuário não digitar "quit", o loop continua
                userInput = stdIn.readLine();
                if (userInput == null || userInput.equals("quit")) {
                    break;
                }
                out.println(userName + ": " + userInput); // Envia o nome do usuário junto com a mensagem
                //String serverResponse = in.readLine(); // Lê a resposta do servidor
                //System.out.println("Resposta do servidor: " + serverResponse);
            }
        } catch (UnknownHostException e) {// Tratamento de exceções
            System.err.println("Erro: não foi possível encontrar o host.");
            System.exit(1);
        } catch (IOException e) {// Tratamento de exceções
            System.err.println("Erro: não foi possível obter E/S para a conexão.");
            System.err.println("Certifique-se de que o servidor esteja em execução e verifique o nome do host e a porta.");
            System.exit(1);
        }
    }
}
