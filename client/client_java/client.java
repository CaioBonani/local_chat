import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.Socket;
import java.net.UnknownHostException;

public class client {

    public static void main(String[] args) {

        try (
            Socket echoSocket = new Socket("127.0.0.1", 8080);
            PrintWriter out = new PrintWriter(echoSocket.getOutputStream(), true);
            BufferedReader in = new BufferedReader(new InputStreamReader(echoSocket.getInputStream()));
            BufferedReader stdIn = new BufferedReader(new InputStreamReader(System.in))
        ) {
            System.out.print("Digite seu nome: ");
            String userName = stdIn.readLine();
            out.println(userName);

            Thread receiveThread = new Thread(() -> {
                try {
                    String serverResponse;
                    while ((serverResponse = in.readLine()) != null) {
                        System.out.println(serverResponse);
                    }
                } catch (IOException e) {
                    e.printStackTrace();
                }
            });
            receiveThread.start();

            String userInput;
            while (true) {
                userInput = stdIn.readLine();
                if (userInput == null || userInput.equals("quit")) {
                    break;
                }
                out.println(userInput);
            }
        } catch (UnknownHostException e) {
            System.err.println("Erro: não foi possível encontrar o host.");
            System.exit(1);
        } catch (IOException e) {
            System.err.println("Erro: não foi possível obter E/S para a conexão.");
            System.err.println("Certifique-se de que o servidor esteja em execução e verifique o nome do host e a porta.");
            System.exit(1);
        }
    }
}
