import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class client {

    public static void main(String[] args) {

        String ip = "172.21.219.101";
        // String ip = ".....";
        int porta = 39999;

        try (//cria um socket para se conectar ao servidor
            Socket echoSocket = new Socket(ip, porta);
            PrintWriter out = new PrintWriter(echoSocket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(echoSocket.getInputStream()));
            BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in))
        ) {
            System.out.print("Digite seu nome: ");
            String userName = stdIn.readLine();//lê o nome do usuário
            out.println(userName);//envia o nome do usuário para o servidor

            Thread receiveThread = new Thread(() -> {//thread para receber mensagens do servidor
                try {//recebe mensagens do servidor e imprime na tela
                    String serverResponse;
                    while ((serverResponse = in.readLine()) != null) {//enquanto o servidor enviar mensagens
                        System.out.println(serverResponse);//imprime na tela
                    }
                } catch (IOException e) {//caso ocorra algum erro
                    e.printStackTrace();//imprime o erro
                }
            });
            receiveThread.start();//inicia a thread

            String userInput;
            while (true) {//enquanto o usuário não digitar quit
                userInput = stdIn.readLine();
                if (userInput == null || userInput.equals("quit")) {
                    break;
                }
                out.println(userInput);//envia a mensagem para o servidor
            }
        } catch (UnknownHostException e) {//caso ocorra algum erro
            System.err.println("Erro: não foi possível encontrar o host.");
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Erro: não foi possível obter E/S para a conexão.");
            System.err.println("Certifique-se de que o servidor esteja em execução e verifique o nome do host e a porta.");
            System.exit(1);
        }
    }
}
