#include <iostream>
#include <string>
#include <limits> // Para las validaciones
#include <thread> // Para hilos
#include <mutex> // Para mutex
#include <atomic> // Para proteger variables
#include <chrono> // Para tiempos
#include <iomanip> // Para setw();
#include <windows.h>

using namespace std;

#define limpiar system("cls");
#define pausar system("pause");

mutex mtx; // Definimos un mutex global para evitar errores entre los hilos
atomic<bool> detenerHilo(false); // Definimos una variable booleana protegida para detener el hilo de impresión

// Enumeración de prioridad
enum class Prioridad {
    Alta = 1,
    Media,
    Baja
};

// Estructura de documento
struct Documento {
    string nombre;
    string tipo;
    Prioridad prioridad;
    Documento* siguiente;
};

// Colas a manejar en el sistema
Documento* cola = nullptr;
Documento* impresos = nullptr;
Documento* eliminados = nullptr;

// Funciones principales
void impresionAutomatica();
void agregarDocumento(string nombre, string tipo, Prioridad prioridad);
void eliminarDocumento();
void verColaDeImpresion();
void verEstadoDeDocumentos();

// Funciones secundarias
Documento* crearDocumento(string nombre, string tipo, Prioridad prioridad);
void agregarAImpresos(Documento* documento);
void agregarAEliminados(Documento* documento);
void imprimirCola(Documento* documento, int i = 1);
string asignarNombre();
string seleccionarTipo();
Prioridad seleccionarPrioridad();
void limpiarCola(Documento*& inicio);

int main() {
    SetConsoleOutputCP (65001);

    // Iniciamos el hilo de impresión automática
    thread hiloDeImpresion(impresionAutomatica);

    int opcion;
    do {
        limpiar; // Limpiamos la terminal

        // Mostramos el menu de inicio
        cout<<"\n\t\033[31m-- SISTEMA DE IMPRESION --\033[0m\n\n";
        cout<<"1. Agregar documento\n";
        cout<<"2. Eliminar documento\n";
        cout<<"3. Ver cola de impresión\n";
        cout<<"4. Ver estado de documentos\n";
        cout<<"5. Salir\n";
        cout<<"\n\033[34mIngrese una opción: \033[0m";
        cin>>opcion;

        // Validamos la entrada del usuario
        while (cin.fail() || opcion < 1 || opcion > 5) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"\n\033[34mIngrese una opción válida: \033[0m";
            cin>>opcion;
        }
        cin.ignore();

        // Declaramos las variables necesarias
        string nombre, tipo;
        Prioridad prioridad;

        limpiar;

        // Evaluamos la opción seleccionada
        switch (opcion) {
            case 1:
                nombre = asignarNombre();
                tipo = seleccionarTipo();
                prioridad = seleccionarPrioridad();
                agregarDocumento(nombre, tipo, prioridad);
                break;
            case 2:
                eliminarDocumento();
                break;
            case 3:
                verColaDeImpresion();
                break;
            case 4:
                verEstadoDeDocumentos();
                break;
            case 5:
                cout<<"\n\033[32mSaliendo del sistema...\033[0m"<<endl;
                break;
            default:
                cout<<"\n\033[32mOpcion inválida...\033[0m"<<endl;
                break;
        }
        cout<<"\n";
        pausar; // Esperamos a que el usuario presione una tecla
    } while (opcion != 5);

    detenerHilo = true; // Para que el hilo de impresión se detenga
    hiloDeImpresion.join(); // Esperamos que el hilo termine correctamente

    // Liberamos la memoria utilizada
    limpiarCola(cola);
    limpiarCola(impresos);
    limpiarCola(eliminados);

    return 0;
}

// Funciones principales

void impresionAutomatica() {
    while (!detenerHilo) {
        // Intervalo de tiempo para imprimir un documento
        this_thread::sleep_for(chrono::seconds(20));

        // Usamos mutex antes de modificar la cola
        lock_guard<mutex> lock(mtx);

        // Imprimimos si la cola no esta vacía
        if (cola) {
            Documento* impreso = cola;
            cola = cola->siguiente;

            // Movemos el documento a la cola de impresos
            agregarAImpresos(impreso);
        }
    }
}

void agregarDocumento(string nombre, string tipo, Prioridad prioridad) {
    Documento* nuevo = crearDocumento(nombre, tipo, prioridad);

    lock_guard<mutex> lock(mtx);

    // Insertamos el documento según su nivel de prioridad
    if (!cola || nuevo->prioridad < cola->prioridad) {
        nuevo->siguiente = cola;
        cola = nuevo;
    } else {
        Documento* actual = cola;
        while (actual->siguiente && actual->siguiente->prioridad <= nuevo->prioridad) {
            actual = actual->siguiente;
        }
        // Insertamos el documento en la cola
        nuevo->siguiente = actual->siguiente;
        actual->siguiente = nuevo;
    }

    cout<<"\n\033[32m-> Documento '"<<nuevo->nombre<<"' agregado a la cola con éxito.\033[0m"<<endl;
}

void eliminarDocumento() {
    lock_guard<mutex> lock(mtx);

    string nombre;

    // Verificamos si la cola esta vacía
    if (!cola) {
        cout<<"\n\033[32m-> La cola está vacía.\033[0m"<<endl;
        return;
    }

    cout<<"\nNombre del documento: ";
    getline(cin, nombre);

    // Buscamos el nodo a eliminar
    Documento* actual = cola;
    Documento* anterior = nullptr;
    while (actual && actual->nombre != nombre) {
        anterior = actual;
        actual = actual->siguiente;
    }

    // Verificamos si hubo alguna coincidencia
    if (!actual) {
        cout<<"\n\033[32m-> El documento '"<<nombre<<"' no se encuentra en la cola de impresión.\033[0m"<<endl;
        return;
    }

    if (!anterior) {
        cola = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    // Movemos el documento a la cola de eliminados
    agregarAEliminados(actual);

    cout<<"\n\033[32m-> Documento '"<<actual->nombre<<"' eliminado exitosamente.\033[0m"<<endl;
}

void verColaDeImpresion() {
    lock_guard<mutex> lock(mtx);

    // Verificamos si la cola esta vacía
    if (!cola) {
        cout<<"\n\033[32m-> La cola de impresión está vacía.\033[0m"<<endl;
        return;
    }

    // Imprimimos la cola de impresión
    cout<<"\n\033[32m-> Cola de impresión actual: \033[0m\n"<<endl;
    imprimirCola(cola);
}

void verEstadoDeDocumentos() {
    lock_guard<mutex> lock(mtx);

    cout<<"\n\033[32m-> Documentos en cola: \033[0m"<<endl;
    if (!cola) cout<<"\tNo hay documentos en cola."<<endl;
    imprimirCola(cola);

    cout<<"\n\033[32m-> Documentos impresos: \033[0m"<<endl;
    if (!impresos) cout<<"\tNo hay documentos impresos."<<endl;
    imprimirCola(impresos);

    cout<<"\n\033[32m-> Documentos eliminados: \033[0m"<<endl;
    if (!eliminados) cout<<"\tNo hay documentos eliminados."<<endl;
    imprimirCola(eliminados);
}

// Funciones secundarias

Documento* crearDocumento(string nombre, string tipo, Prioridad prioridad) {
    Documento* nuevo = new Documento;
    nuevo->nombre = nombre;
    nuevo->tipo = tipo;
    nuevo->prioridad = prioridad;
    nuevo->siguiente = nullptr;
    return nuevo;
}

void imprimirCola(Documento* documento, int i) {
    // Caso base de la recursión
    if (!documento) return;

    cout<<"\t"<<i<<". "
        <<left<<setw(20)<<documento->nombre<<"["<<setw(13)<<documento->tipo<<"]"
        <<right<<setw(5)<<"("<<static_cast<int>(documento->prioridad)<<")"<<endl;

    imprimirCola(documento->siguiente, i + 1);
}

void agregarAImpresos(Documento* documento) {
    documento->siguiente = impresos;
    impresos = documento;
}

void agregarAEliminados(Documento* documento) {
    documento->siguiente = eliminados;
    eliminados = documento;
}

string asignarNombre() {
    string nombre;

    // Pedimos el nommbre del documento
    do {
        cout<<"\nNombre del documento (sin espacios): ";
        getline(cin, nombre);

        // Validamos que no tenga espacios
        for (int i = 0; i < nombre.length(); i++) {
            if (isspace(nombre[i])) {
                nombre = "";
                break;
            }
        }
    } while (nombre.empty());

    do {
        lock_guard<mutex> lock(mtx);

        // Verificamos si el nombre está en uso dentro de la cola de impresión
        Documento* actual = cola;
        while (actual && actual->nombre != nombre) {
            actual = actual->siguiente;
        }

        if (!actual) {
            break;
        }

        cout<<"\nNombre en uso, ingrese otro nombre: ";
        getline(cin, nombre);
    } while (true);

    // Retornamos el nombre válido
    return nombre;
}

string seleccionarTipo() {
    // Arreglo con los tipos de documento
    string arr[] = {"\033[31mPDF\033[0m", "\033[34mDOCX\033[0m", "\033[33mJPEG\033[0m"};

    int opcion;

    cout<<"\n\033[31m-- Tipo de Documento --\033[0m\n";
    cout<<" 1. PDF\n 2. DOCX \n 3. JPEG\n";

    do {
        cout<<"\033[34mSeleccione una opción (1-3): \033[0m";
        cin>>opcion;

        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
    } while (opcion < 1 || opcion > 3);

    // Retornamos el tipo de documento
    return arr[opcion - 1];
}

Prioridad seleccionarPrioridad() {
    // Arreglo con los niveles de prioridad
    Prioridad arr[] = {Prioridad::Alta, Prioridad::Media, Prioridad::Baja};

    int opcion;
    cout<<"\n\033[31m-- Prioridad del Documento --\033[0m\n";
    cout<<" 1. Alta\n 2. Media \n 3. Baja\n";

    do {
        cout<<"\033[34mSeleccione una opción (1-3): \033[0m";
        cin>>opcion;
        
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
    } while (opcion < 1 || opcion > 3);

    // Retornamos la prioridad del documento
    return arr[opcion - 1];
}

void limpiarCola(Documento*& inicio) {
    while (inicio) {
        Documento* temp = inicio;
        inicio = inicio->siguiente;

        // Liberamos el bloque de memoria al que apunta el puntero
        delete temp;
    }
}