#include <iostream>
#include <queue>
#include <stack>
#include <cstdlib>
#include <pthread.h>
#include <unistd.h>

using namespace std;

const int NUM_CAJAS = 4;
const int MAX_NUMEROS = 200;

// Declaración de variables globales
queue<int> colas[NUM_CAJAS];  // Colas de cada caja
stack<int> pila;  // Pila de números atendidos
pthread_mutex_t mutexColas[NUM_CAJAS];
pthread_mutex_t mutexPila;

// Función para generar un número aleatorio entre min y max
int generarNumeroAleatorio(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Función ejecutada por cada hilo de caja
void* hiloCaja(void* cajaId) {
    int caja = *((int*)cajaId);

    while (true) {
        // Obtener el siguiente número de la cola
        pthread_mutex_lock(&mutexColas[caja]);
        if (!colas[caja].empty()) {
            int numero = colas[caja].front();
            colas[caja].pop();
            pthread_mutex_unlock(&mutexColas[caja]);

            // Atender el número (esperar 2 segundos)
            sleep(2);

            // Agregar el número a la pila
            pthread_mutex_lock(&mutexPila);
            pila.push(numero);
            pthread_mutex_unlock(&mutexPila);
        } else {
            pthread_mutex_unlock(&mutexColas[caja]);
            break;  // La cola está vacía, salir del bucle
        }
    }

    pthread_exit(NULL);
}

int main() {
    srand(time(NULL));

    pthread_t hilosCajas[NUM_CAJAS];
    int idCajas[NUM_CAJAS];

    // Inicializar los mutex de las colas y la pila
    for (int i = 0; i < NUM_CAJAS; i++) {
        pthread_mutex_init(&mutexColas[i], NULL);
    }
    pthread_mutex_init(&mutexPila, NULL);

    // Generar números aleatorios y asignar a las colas
    for (int i = 0; i < MAX_NUMEROS; i++) {
        int numero = generarNumeroAleatorio(1, 90);
        int caja = generarNumeroAleatorio(0, NUM_CAJAS - 1);

        pthread_mutex_lock(&mutexColas[caja]);
        colas[caja].push(numero);
        pthread_mutex_unlock(&mutexColas[caja]);

        sleep(1);
    }

    // Crear hilos para cada caja
    for (int i = 0; i < NUM_CAJAS; i++) {
        idCajas[i] = i;
        pthread_create(&hilosCajas[i], NULL, hiloCaja, (void*)&idCajas[i]);
    }

    // Esperar a que todos los hilos terminen
    for (int i = 0; i < NUM_CAJAS; i++) {
        pthread_join(hilosCajas[i], NULL);
    }

    // Imprimir la pila y calcular la suma
    int sumaTotal = 0;
    while (!pila.empty()) {
        int numero = pila.top();
        pila.pop();
        sumaTotal += numero;
        cout << numero << " ";
    }

    cout << "\nTotal: " << sumaTotal << endl;

    // Destruir los mutex de las colas y la pila
    for (int i = 0; i < NUM_CAJAS; i++) {
        pthread_mutex_destroy(&mutexColas[i]);
    }
    pthread_mutex_destroy(&mutexPila);

    return 0;
}

