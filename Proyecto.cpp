#include <iostream>
#include <string>
#include <limits>
#include <windows.h>

using namespace std;

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
void agregarDocumento(string nombre, string tipo, Prioridad prioridad);
void eliminarDocumento(string nombre);
void verColaDeImpresion();
void verEstadoDeDocumentos();

// Funciones secundarias
Documento* crearDocumento(string nombre, string tipo, Prioridad prioridad);
void agregarAImpresos(Documento* documento);
void agregarAEliminados(Documento* documento);
void imprimirCola(Documento* documento, int i = 1);
string seleccionarTipo();
Prioridad seleccionarPrioridad();

int main() {
    SetConsoleOutputCP (65001);

    int opcion;
    do {
        system("cls"); // Limpiamos la terminal

        // Mostramos el menu de inicio
        cout<<"\n\t-- SISTEMA DE IMPRESION --\n\n";
        cout<<"1. Agregar documento\n";
        cout<<"2. Eliminar documento\n";
        cout<<"3. Ver cola de impresión\n";
        cout<<"4. Ver estado de documentos\n";
        cout<<"5. Salir\n";
        cout<<"\nIngrese una opción: ";
        cin>>opcion;

        // Validamos la entrada del usuario
        while (cin.fail() || opcion < 1 || opcion > 5) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout<<"\nIngrese una opción válida: ";
            cin>>opcion;
        }
        cin.ignore();

        // Declaramos las variables necesarias
        string nombre, tipo;
        Prioridad prioridad;

        system("cls");

        // Evaluamos la opción seleccionada
        switch (opcion) {
            case 1:
                cout<<"\nNombre del documento: ";
                getline(cin, nombre);
                tipo = seleccionarTipo();
                prioridad = seleccionarPrioridad();
                agregarDocumento(nombre, tipo, prioridad);
                break;
            case 2:
                cout<<"\nNombre del documento: ";
                getline(cin, nombre);
                eliminarDocumento(nombre);
                break;
            case 3:
                verColaDeImpresion();
                break;
            case 4:
                verEstadoDeDocumentos();
                break;
            case 5:
                cout<<"\n Saliendo del sistema..."<<endl;
                break;
            default:
                cout<<"\n Opcion inválida\n"<<endl;
                break;
        }
        system("pause"); // Esperamos a que el usuario presione una tecla
    } while (opcion != 5);

    return 0;
}

// Funciones principales

void agregarDocumento(string nombre, string tipo, Prioridad prioridad) {
    Documento* nuevo = crearDocumento(nombre, tipo, prioridad);

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

    cout<<"\n-> Documento '"<<nuevo->nombre<<"' agregado a la cola con éxito."<<endl;
}

void eliminarDocumento(string nombre) {
    // Verificamos si la cola esta vacía
    if (!cola) {
        cout<<"\n-> La cola está vacía."<<endl;
        return;
    }

    // Buscamos el nodo a eliminar
    Documento* actual = cola;
    Documento* anterior = nullptr;
    while (actual && actual->nombre != nombre) {
        anterior = actual;
        actual = actual->siguiente;
    }

    // Verificamos si hubo alguna coincidencia
    if (!actual) {
        cout<<"\n-> El documento no se encuentra en la cola de impresión."<<endl;
        return;
    }

    if (!anterior) {
        cola = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    // Agregamos el documento a la cola de eliminados
    agregarAEliminados(actual);

    cout<<"\n-> Documento '"<<actual->nombre<<"' eliminado exitosamente."<<endl;

    // Eliminamos el documento
    delete actual;
}

void verColaDeImpresion() {
    // Verificamos si la cola esta vacía
    if (!cola) {
        cout<<"-> La cola de impresión está vacía."<<endl;
        return;
    }

    // Imprimimos la cola de impresión
    cout<<"-> Cola de impresión actual: "<<endl;
    imprimirCola(cola);
}

void verEstadoDeDocumentos() {
    cout<<"\n-> Documentos en cola: "<<endl;
    if (!cola) cout<<"No hay documentos en cola."<<endl;
    imprimirCola(cola);

    cout<<"\n-> Documentos impresos: "<<endl;
    if (!impresos) cout<<"No hay documentos impresos."<<endl;
    imprimirCola(impresos);

    cout<<"\n-> Documentos eliminados: "<<endl;
    if (!eliminados) cout<<"No hay documentos eliminados."<<endl;
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

    cout<<i<<". "<<documento->nombre<<" "<<"["<<documento->tipo<<"]"<<" "<<"("<<static_cast<int>(documento->prioridad)<<")"<<endl;
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

string seleccionarTipo() {
    // Arreglo con los tipos de documento
    string arr[] = {"PDF", "DOCX", "JPEG"};

    int opcion;
    cout<<"\n-- Tipo de Documento --\n";
    cout<<" 1. PDF\n 2. DOCX \n 3. JPEG\n";
    cout<<"Seleccione una opción (1-3): ";
    cin>>opcion;

    // Retornamos el tipo de documento
    return arr[opcion - 1];
}

Prioridad seleccionarPrioridad() {
    // Arreglo con los niveles de prioridad
    Prioridad arr[] = {Prioridad::Alta, Prioridad::Media, Prioridad::Baja};

    int opcion;
    cout<<"\n-- Prioridad del Documento --\n";
    cout<<" 1. Alta\n 2. Media \n 3. Baja\n";
    cout<<"Seleccione una opción (1-3): ";
    cin>>opcion;

    // Retornamos la prioridad del documento
    return arr[opcion - 1];
}