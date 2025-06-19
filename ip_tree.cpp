#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
using namespace std;

ifstream archivo;
vector<string> meses = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

struct LogEntry {
    string month;
    int day;
    string time;
    string ip_address;
    string failure_reason;
};

struct TreeNode {
    string data;              // Puede ser la IP de red, IP de host o información de log.
    TreeNode *left;           // Árbol izquierdo para nodos de IP.
    TreeNode *right;          // Árbol derecho para nodos de IP.
    TreeNode *hostTree;       // Subárbol para IPs de host, si es nodo de red.
    TreeNode *logTree;        // Subárbol para logs, si es nodo de host.
};

struct redMax {
    string ID;       
    int hostCount;   
};

struct hostMax{
    string ID;
    int hostCount;
};
struct IPs{
    string IP;
    int count;
};

vector<int> dividirIP(string ip) {
    vector<int> ipNumeros(4, 0); 
    size_t pos = 0;
    int i = 0;
    while ((pos = ip.find('.')) != string::npos && i < 4) {
        ipNumeros[i++] = stoi(ip.substr(0, pos));
        ip.erase(0, pos + 1);
    }
    if (i < 4) { 
        ipNumeros[i] = stoi(ip);
    }
    return ipNumeros;
}

// Función para contar los hosts en el subárbol de hosts de una red
int contarHosts(TreeNode* hostNode) {
    if (hostNode == nullptr) return 0;
    return 1 + contarHosts(hostNode->left) + contarHosts(hostNode->right);
}

// Función para recorrer el árbol y contar la cantidad de hosts de cada red
void contarHostsRed(TreeNode* root, vector<redMax>& contadorRedes) {
    if (root == nullptr) return;
    int hostCount = contarHosts(root->hostTree);
    redMax redInfo = {root->data, hostCount};
    contadorRedes.push_back(redInfo);
    contarHostsRed(root->left, contadorRedes);
    contarHostsRed(root->right, contadorRedes);
}

// Función para encontrar y mostrar las redes con más hosts
void mostrarRedesConMasHosts(const vector<redMax>& contadorRedes) {
    int maxHosts = 0;
    vector<string> redesConMasHosts;
    for (int i = 0; i < contadorRedes.size(); i++) {
        if (contadorRedes[i].hostCount > maxHosts) {
            maxHosts = contadorRedes[i].hostCount;
            redesConMasHosts = {contadorRedes[i].ID};
        } else if (contadorRedes[i].hostCount == maxHosts) {
            redesConMasHosts.push_back(contadorRedes[i].ID);
        }
    }
    for (int i = 0; i < redesConMasHosts.size(); i++) {
        cout << redesConMasHosts[i] <<  endl;
    }
}

TreeNode* insertarLog(TreeNode* &root, const LogEntry &log) {
    // Extraer la IP y el puerto
    string ip = log.ip_address;
    string port;
    size_t pos = ip.find(':');
    if (pos != string::npos) {
        port = ip.substr(pos + 1); 
        ip = ip.substr(0, pos);    
    } else {
        port = "0000"; 
    }
    string logData = port + " " + log.month + " " + to_string(log.day) + " " + log.time + " " + log.failure_reason;
    if (root == nullptr) {
        root = new TreeNode{logData, nullptr, nullptr, nullptr, nullptr};
    } else if (logData < root->data) {
        insertarLog(root->left, log);
    } else {
        insertarLog(root->right, log);
    }
    return root;
}

void insertarHost(TreeNode *root, const string &ip_host, const LogEntry &log) {
    if (root->data == ip_host) {
        insertarLog(root->logTree, log);
    } else if (ip_host < root->data) {
        if (root->left == nullptr) {
            root->left = new TreeNode{ip_host, nullptr, nullptr, nullptr, nullptr};
        }
        insertarHost(root->left, ip_host, log);
    } else {
        if (root->right == nullptr) {
            root->right = new TreeNode{ip_host, nullptr, nullptr, nullptr, nullptr};
        }
        insertarHost(root->right, ip_host, log);
    }
}

TreeNode* insertar(TreeNode *root, const LogEntry &log) {
    vector<int> ip = dividirIP(log.ip_address);
    string ip_red = to_string(ip[0]) + "." + to_string(ip[1]);
    string ip_host = to_string(ip[2]) + "." + to_string(ip[3]);
    
    if (root == nullptr) {
        root = new TreeNode{ip_red, nullptr, nullptr, nullptr, nullptr};
    }

    if (root->data == ip_red) {
        if (root->hostTree == nullptr) {
            root->hostTree = new TreeNode{ip_host, nullptr, nullptr, nullptr, nullptr};
        }
        insertarHost(root->hostTree, ip_host, log);
    } else if (ip_red < root->data) {
        root->left = insertar(root->left, log);
    } else {
        root->right = insertar(root->right, log);
    }
    
    return root;
}

int contarLogs(TreeNode* logNode) {
    if (logNode == nullptr) return 0;
    return 1 + contarLogs(logNode->left) + contarLogs(logNode->right);
}

// Función para recorrer el árbol y contar la cantidad de logs de cada host
void contarLogsHost(TreeNode* root, vector<hostMax>& contadorHosts) {
    if (root == nullptr) return;
    if (root->logTree != nullptr) {
        int logCount = contarLogs(root->logTree);
        hostMax hostInfo = {root->data, logCount};
        contadorHosts.push_back(hostInfo);
    }
    contarLogsHost(root->left, contadorHosts);
    contarLogsHost(root->right, contadorHosts);
}

// Función para recorrer el árbol de hosts de cada red y contar los logs de cada host
void contarLogsRed(TreeNode* root, vector<hostMax>& contadorHosts) {
    if (root == nullptr) return;
    contarLogsHost(root->hostTree, contadorHosts);
    contarLogsRed(root->left, contadorHosts);
    contarLogsRed(root->right, contadorHosts);
}

void mostrarIPsConMasCounts(const vector<IPs>& structsIPs) {
    int maxCount = 0;
    vector<string> ipsConMasCounts;
    for (const auto& ipStruct : structsIPs) {
        if (ipStruct.count > maxCount) {
            maxCount = ipStruct.count;
            ipsConMasCounts = {ipStruct.IP}; 
        } else if (ipStruct.count == maxCount) {
            ipsConMasCounts.push_back(ipStruct.IP); 
        }
    }
    for (const auto& ip : ipsConMasCounts) {
        string formatted_ip;
        size_t pos = 0;
        string segment;
        string ip_copy = ip;
        for (int i = 0; i < 4; ++i) {
            pos = ip_copy.find('.');
            if (pos != string::npos) {
                segment = ip_copy.substr(0, pos);
                ip_copy.erase(0, pos + 1);
            } else {
                segment = ip_copy;
            }
            if (segment.length() == 2) {
                segment = "0" + segment;
            }
            
            formatted_ip += segment;
            if (i < 3) {
                formatted_ip += ".";
            }
        }
        cout << formatted_ip << endl;
    }
}

int main() {
    TreeNode *root = nullptr;
    string line;
    vector<IPs> structsIPs;
    archivo.open("b3.txt", ios::in);
    if (!archivo.is_open()) {
        cout << "Error abriendo el archivo" << endl;
        return 1;
    }
    string month, hour, IP, message;
    int day;
    while (getline(archivo, line)) {
        size_t pos = line.find(' ');
        month = line.substr(0, pos);
        line.erase(0, pos + 1);
        pos = line.find(' ');
        day = stoi(line.substr(0, pos));
        line.erase(0, pos + 1);
        pos = line.find(' ');
        hour = line.substr(0, pos);
        line.erase(0, pos + 1);
        pos = line.find(' ');
        IP = line.substr(0, pos);
        line.erase(0, pos + 1);
        message = line;
        LogEntry log = {month, day, hour, IP, message};
        vector<int> ip = dividirIP(IP);
        string ip_red = to_string(ip[0]) + "." + to_string(ip[1]);
        string ip_host = to_string(ip[2]) + "." + to_string(ip[3]);
        string full_ip = ip_red + "." + ip_host;
        bool found = false;
        for (int i = 0; i<structsIPs.size();++i) {
            if (structsIPs[i].IP == full_ip) {
                structsIPs[i].count++;
                found = true;
                break;
            }
        }
        if (!found) {
            structsIPs.push_back({full_ip, 1}); 
        }
        root = insertar(root, log);
        
    }
    archivo.close();
    vector<redMax> contadorRedes;
    contarHostsRed(root, contadorRedes);
    mostrarRedesConMasHosts(contadorRedes);
    vector<hostMax> contadorHosts;
    contarLogsRed(root, contadorHosts);
    mostrarIPsConMasCounts(structsIPs);
    return 0;
}
