#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <cstring>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

using namespace std;

int main() {
    WSADATA wsaData;
    SOCKET clientSocket = INVALID_SOCKET;
    sockaddr_in serverAddr{};
    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup basarisiz oldu." << endl;
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Soket olusturulamadi. Hata Kodu: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Sunucuya baglanilamadi. Hata Kodu: " << WSAGetLastError() << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    cout << "Sunucuya baglanildi." << endl;

    while (true) {
        string input;
        double num1, num2;
        char op;

        cout << "\nBirinci sayiyi giriniz: ";
        cin >> num1;

        cout << "Islem karakterini giriniz (+ veya -): ";
        cin >> op;

        cout << "Ikinci sayiyi giriniz: ";
        cin >> num2;

        input = to_string(num1) + " " + op + " " + to_string(num2);

        send(clientSocket, input.c_str(), input.length(), 0);

        memset(buffer, 0, BUFFER_SIZE);
        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            cout << "Sunucudan gelen cevap: " << buffer << endl;
        } else {
            cout << "Sunucudan veri alinamadi veya baglanti koptu." << endl;
            break;
        }

        string choice;
        cout << "Devam etmek istiyor musunuz? (e/h): ";
        cin >> choice;

        if (choice == "h" || choice == "H") {
            string exitMsg = "EXIT";
            send(clientSocket, exitMsg.c_str(), exitMsg.length(), 0);
            cout << "Cikis mesaji gonderildi." << endl;
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
