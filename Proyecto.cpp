#include <iostream>
#include <windows.h>

using namespace std;

// Prioridad
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
void verCola();
// Podriamos reeemplazar estos tres con la opcion ver estado de documentos
void verImpresos();
void verEliminados();
void reporteFinal(Documento* documentos); // Al salir del sistema imprimir el reporte final de documentos impresos y eliminados

// Funciones secundarias
Documento* crearDocumento(string nombre, string tipo, Prioridad prioridad);
void agregarAImpresos(Documento* documento);
void agregarAEliminados(Documento* documento);

int main() {
    SetConsoleOutputCP (65001);

    

    return 0;
}

// Funciones principales

void agregarDocumento(string nombre, string tipo, Prioridad prioridad) {
    Documento* nuevo = crearDocumento(nombre, tipo, prioridad);

    // Insertamos el documneto según su nivel de prioridad
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
}

void eliminarDocumento(string nombre) {
    // Verificamos si la cola esta vacía
    if (!cola) {
        cout<<"La cola está vacía."<<endl;
        return;
    }

    // Buscamos el nodo a eliminar
    Documento* actual = cola;
    Documento* anterior = nullptr;
    while (actual->siguiente && actual->nombre != nombre) {
        anterior = actual;
        actual = actual->siguiente;
    }

    // Verificamos si hubo alguna coincidencia
    if (!actual->siguiente) {
        cout<<"El documento no se encuentra en la cola."<<endl;
        return;
    }

    if (!anterior) {
        cola = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    // Eliminamos el documento
    delete actual;
}

void verCola() {
    // Verificamos si la cola esta vacía
    if (!cola) {
        cout<<"La cola está vacía."<<endl;
        return;
    }

    // Imprimimos la cola de impresión
    Documento* actual = cola;
    for (int i = 0; actual; i++) {
        cout<<i<<". "<<actual->nombre<<" "<<"["<<actual->tipo<<"]"<<" "<<"("<<static_cast<int>(actual->prioridad)<<")"<<endl;
    }
}

void verImpresos() {
    // Verificamos si la cola esta vacía
    if (!impresos) {
        cout<<"La cola está vacía."<<endl;
        return;
    }

    // Imprimimos el reporte de documentos impresos
    Documento* actual = impresos;
    for (int i = 0; actual; i++) {
        cout<<i<<". "<<actual->nombre<<" "<<"["<<actual->tipo<<"]"<<" "<<"("<<static_cast<int>(actual->prioridad)<<")"<<endl;
    }
}

void verEliminados() {
    // Verificamos si la cola esta vacía
    if (!eliminados) {
        cout<<"La cola está vacía."<<endl;
        return;
    }

    // Imprimimos el reporte de documentos eliminados
    Documento* actual = eliminados;
    for (int i = 0; actual; i++) {
        cout<<i<<". "<<actual->nombre<<" "<<"["<<actual->tipo<<"]"<<" "<<"("<<static_cast<int>(actual->prioridad)<<")"<<endl;
    }
}

void reporteFinal(Documento* documento, int i = 1) {
    if (!documento) return;
    cout<<i<<". "<<documento->nombre<<" "<<"["<<documento->tipo<<"]"<<" "<<"("<<static_cast<int>(documento->prioridad)<<")"<<endl;
    reporteFinal(documento->siguiente, ++i);
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

void agregarAImpresos(Documento* documento) {
    documento->siguiente = impresos;
    impresos = documento;
}

void agregarAEliminados(Documento* documento) {
    documento->siguiente = eliminados;
    eliminados = documento;
}