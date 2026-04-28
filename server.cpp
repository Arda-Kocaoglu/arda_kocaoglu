#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

using namespace std;

string processMessage(const string& message) {
    stringstream ss(message);
    double num1, num2;
    char op;

    ss >> num1 >> op >> num2;

    if (ss.fail()) {
        return "HATA: Gecersiz veri formati. Ornek kullanim: 12 + 5";
    }

    double result;

    if (op == '+') {
        result = num1 + num2;
    } else if (op == '-') {
        result = num1 - num2;
    } else {
        return "HATA: Sadece + ve - islemleri destekleniyor.";
    }

    return "Sonuc: " + to_string(result);
}

int main() {
    WSADATA wsaData;
    SOCKET serverSocket = INVALID_SOCKET;
    SOCKET clientSocket = INVALID_SOCKET;

    sockaddr_in serverAddr{}, clientAddr{};
    int clientAddrLen = sizeof(clientAddr);

    char buffer[BUFFER_SIZE];

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "WSAStartup basarisiz oldu." << endl;
        return 1;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cout << "Soket olusturulamadi. Hata Kodu: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cout << "Bind islemi basarisiz. Hata Kodu: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, 1) == SOCKET_ERROR) {
        cout << "Listen islemi basarisiz. Hata Kodu: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Sunucu baslatildi. Port: " << PORT << endl;
    cout << "Bir istemci baglantisi bekleniyor..." << endl;

    clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);
    if (clientSocket == INVALID_SOCKET) {
        cout << "Accept islemi basarisiz. Hata Kodu: " << WSAGetLastError() << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Istemci baglandi." << endl;

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);

        int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);

        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            string receivedMessage(buffer);

            cout << "Istemciden gelen mesaj: " << receivedMessage << endl;

            if (receivedMessage == "EXIT") {
                cout << "Istemci cikis yapti. Baglanti kapatiliyor..." << endl;
                break;
            }

            string response = processMessage(receivedMessage);

            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else if (bytesReceived == 0) {
            cout << "Istemci baglantiyi kapatti." << endl;
            break;
        }
        else {
            cout << "Recv hatasi. Hata Kodu: " << WSAGetLastError() << endl;
            break;
        }
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
