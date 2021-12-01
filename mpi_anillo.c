#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#include "./tsp/TSP.c"

void intercambioMejor(int *poblacion, int *mejores, int *peores,
                      int tamPoblacion, int nCiudades, int nIntercambios)
{
  for (int i = 0; i < nIntercambios; i++)
  {
    int *individuo = (int *)malloc(nCiudades * sizeof(int));
    extraerVector(poblacion, individuo, mejores[i], tamPoblacion, nCiudades);
    insertarVector(poblacion, individuo, peores[i], tamPoblacion, nCiudades);
  }
}

void intercambioEnAnillo(int *poblacionPrincipal, int *mejorCamino,
                         int *mejorCaminoAdyacente, int *peores,
                         int rank, MPI_Status estado,
                         int size, int tamPoblacion, int nCiudades, int nIntercambios)
{
  if (rank == 0)
  {
    MPI_Send(mejorCamino,               //variable con la referencia dell vector de elementos a enviar
             nCiudades * nIntercambios, // tamaño del vector que se envia
             MPI_INT,                   // Tipo de dato que se envia
             rank + 1,                  // identificador del proceso destino
             0,                         //etiqueta informativa
             MPI_COMM_WORLD);           //Comunicador utilizado

    MPI_Recv(mejorCaminoAdyacente,      // variable con la referencia al vector para almacenar el dato recibido
             nCiudades * nIntercambios, // tamaño del vector que se recibe
             MPI_INT,                   // Tipo de dato que se recibe
             size - 1,                  // identificador del proceso del que se reciben los datos
             0,                         // etiqueta informativa
             MPI_COMM_WORLD,            // Comunicador utilizados
             &estado);                  // estructura con información del estado

    // intercambio
    intercambioMejor(poblacionPrincipal, mejorCaminoAdyacente, peores,
                     tamPoblacion, nCiudades, nIntercambios);
  }
  else
  {
    MPI_Recv(mejorCaminoAdyacente,      // variable con la referencia al vector para almacenar el dato recibido
             nCiudades * nIntercambios, // tamaño del vector que se recibe
             MPI_INT,                   // Tipo de dato que se recibe
             rank - 1,                  // identificador del proceso del que se reciben los datos
             0,                         // etiqueta informativa
             MPI_COMM_WORLD,            // Comunicador utilizados
             &estado);                  // estructura con información del estado

    // intercambio
    intercambioMejor(poblacionPrincipal, mejorCaminoAdyacente, peores,
                     tamPoblacion, nCiudades, nIntercambios);

    if (rank != size - 1)
    {
      MPI_Send(mejorCamino,
               nCiudades * nIntercambios,
               MPI_INT,
               rank + 1,
               0,
               MPI_COMM_WORLD);
    }
    else
    {
      MPI_Send(mejorCamino,
               nCiudades * nIntercambios,
               MPI_INT,
               0,
               0,
               MPI_COMM_WORLD);
    }
  }
}

int main(int argc, char *argv[])
{
  // DATOS INICIALES
  srand(time(NULL));
  // double time_g_1 = omp_get_wtime();

  int nCiudades = 29;      // nodos del grafo
  int tamPoblacion = 3000; // tama�o de la poblaci�n
  int numMaxGen = 1300;
  int pasosIntercambio = 10;
  int nIntercambios = 1;

  int posMejor = 0; // posici�n del individuo mejor adaptado
  double probCruce = 0.7;
  double probMut = 0.004;

  double *coordenadasX = (double *)malloc(nCiudades * sizeof(double));
  double *coordenadasY = (double *)malloc(nCiudades * sizeof(double));

  char nombreCX[] = "coordenadasX_S.txt";
  char nombreCY[] = "coordenadasY_S.txt";
  leerFicheroCordenadas(nombreCX, coordenadasX);
  leerFicheroCordenadas(nombreCY, coordenadasY);

  int *poblacionPrincipal = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
  int *mejorCamino, *mejorCaminoAdyacente, *pobAuxiliar,
      *padre1, *padre2, *hijo1, *hijo2;
  double *matrizDistancias, *distancias, *aptitud, *puntuacion,
      *puntAcumulada, *nuevaAptitud, *nuevaDistancias;
  double longitud;

  mejorCamino = (int *)malloc(nCiudades * sizeof(int));
  mejorCaminoAdyacente = (int *)malloc(nCiudades * sizeof(int));
  distancias = (double *)malloc(tamPoblacion * sizeof(double));
  nuevaDistancias = (double *)malloc(tamPoblacion * sizeof(double));
  aptitud = (double *)malloc(tamPoblacion * sizeof(double));
  nuevaAptitud = (double *)malloc(tamPoblacion * sizeof(double));
  puntuacion = (double *)malloc(tamPoblacion * sizeof(double));
  puntAcumulada = (double *)malloc(tamPoblacion * sizeof(double));
  matrizDistancias = (double *)malloc(nCiudades * nCiudades * sizeof(double));
  pobAuxiliar = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));

  crearMatrizDistancia(matrizDistancias, coordenadasX, coordenadasY, nCiudades);

  poblacionInicial(poblacionPrincipal, distancias, aptitud, tamPoblacion, nCiudades, matrizDistancias);
  posMejor = evaluacion(aptitud, puntuacion, puntAcumulada, tamPoblacion);
  extraerVector(poblacionPrincipal, mejorCamino, posMejor, tamPoblacion, nCiudades);

  char nombreCamino[] = "caminoInicial.txt";
  imprimirVectorEnteroFichero(mejorCamino, nCiudades, nombreCamino);
  longitud = longitudCircuito(mejorCamino, matrizDistancias, nCiudades);

  // DATOS MPI
  int rank, size, *enviado, *recibido;

  MPI_Status estado;
  // Se inicia el entorno de comunicación MPI
  MPI_Init(&argc, &argv);
  // Se obtiene el total de procesos
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  // Se obtiene el identificador de cada proceso
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  for (int i = 0; i < numMaxGen; i++)
  {
    if (i % pasosIntercambio == 0)
    {
      int *mejores = (int *)malloc(nIntercambios * sizeof(int));
      obtenerMejores(aptitud, tamPoblacion, mejores, nIntercambios);

      int *peores = (int *)malloc(nIntercambios * sizeof(int));
      obtenerMejores(aptitud, tamPoblacion, peores, nIntercambios);

      intercambioEnAnillo(poblacionPrincipal,
                          mejores, mejorCaminoAdyacente, peores,
                          rank, estado, size, tamPoblacion,
                          nCiudades, nIntercambios);
      recalcularAptitud(poblacionPrincipal, aptitud, matrizDistancias,
                        distancias, tamPoblacion, nCiudades);
    }

    isla(poblacionPrincipal, pobAuxiliar, puntAcumulada,
         aptitud, distancias,
         nuevaAptitud, nuevaDistancias,
         matrizDistancias, puntuacion,
         probMut, probCruce,
         tamPoblacion, nCiudades);
  }

  posMejor = evaluacion(aptitud, puntuacion, puntAcumulada, tamPoblacion); // datos entrada
  // double time_2 = omp_get_wtime();
  printf("Mejor distancia : %d \t\t %.2f \n", posMejor, distancias[posMejor]);
  // imprimirVectorReales(aptitud, nCiudades);
  extraerVector(poblacionPrincipal, mejorCamino, posMejor, tamPoblacion, nCiudades);
  char nombreCaminoFinal[] = "caminoFinal.txt";
  imprimirVectorEnteroFichero(mejorCamino, nCiudades, nombreCaminoFinal);

  char nombreCaminoFinalTour[] = "caminoFinalTour.txt";
  int *mejorCaminoTour;
  mejorCaminoTour = (int *)malloc(nCiudades * sizeof(int));
  leerFicheroCamino(nombreCaminoFinalTour, mejorCaminoTour);

  float losc = longitudCircuito(mejorCaminoTour, matrizDistancias, nCiudades);
  printf("TSPLib long best tour : %.2f \n", losc);
  printf("Error : %.2f \n\n", (abs(distancias[posMejor] - losc) / losc) * 100);

  // double time_g_2 = omp_get_wtime();
  // double t = (time_g_2 - time_g_1);
  // printf("\n - **_** Suma secuencial = %d - tiempo = %f \n", 0, t);

  MPI_Finalize();
}
