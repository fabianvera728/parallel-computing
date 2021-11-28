/*
Universidad de Pamplona
Facultad de Ingenier�as y Arquitectura
Programa de Ingenier�a de Sistemas
Fundamentos de Computaci�n paralela y distribuida

Implementaci�n de una eaquema de soluci�n al problema del TSP mediante un Algoritmo Gen�tico Simple
Version Serial y paralela
*/

/* #include <cstdlib> */
#include <cstdlib>
#include <stdio.h>
/* #include<windows.h> */
#include <time.h>
#include <math.h>
#include <omp.h>
#include <stdlib.h>
//#include<utilidades.h>
// #include<locale.h>  linux

void imprimirVectorEnteros(int V[], int n)
{
	for (int i = 0; i < n; i++)
		printf(" %d \t", *(V + i));
	printf("\n");
}

void imprimirVectorReales(double V[], int n)
{
	for (int i = 0; i < n; i++)
		printf(" %.2f \t", *(V + i));
	printf("\n");
}

void imprimirMatrizEnteros(int M[], int f, int c)
{
	for (int i = 0; i < f; i++)
	{
		for (int j = 0; j < c; j++)
			printf(" %d \t", *(M + (i * c + j)));
		printf("\n");
	}
}

void imprimirMatrizReales(double M[], int f, int c)
{
	for (int i = 0; i < f; i++)
	{
		for (int j = 0; j < c; j++)
			printf(" %.2f \t", *(M + (i * c + j)));
		printf("\n");
	}
}

/*

*/
void imprimirVectorEnteroFichero(int vector[], int n, char nombre[])
{
	FILE *fichero;
	fichero = fopen(nombre, "w");
	for (int i = 0; i < n; i++)
	{
		if (i < n - 1)
			fprintf(fichero, "%d,", vector[i]);
		else
			fprintf(fichero, "%d", vector[i]);
	}
	fclose(fichero);
}

/*

*/
void imprimirVectorRealFichero(float vector[], int n, char nombre[])
{
	FILE *fichero;
	fichero = fopen(nombre, "w");
	for (int i = 0; i < n; i++)
	{
		if (i < n - 1)
			fprintf(fichero, "%.2f,", vector[i]);
		else
			fprintf(fichero, "%.2f", vector[i]);
	}
	fclose(fichero);
}

void extraerVector(int matriz[], int vector[], int pos, int m, int n)
{
	for (int j = 0; j < n; j++)
		vector[j] = matriz[n * pos + j];
}

void insertarVector(int matriz[], int vector[], int pos, int m, int n)
{
	for (int j = 0; j < n; j++)
		matriz[n * pos + j] = vector[j];
}

/*
Funci�n que genera dos vectores con las coordenadas en X y Y, correspondientes a un caso particular del problema del TSP. 
Las coordenadas corresponden a valores enteros.

Par�metros de entrada:

nCiudades: N�mero de ciudades para el problema del transporte.
xMin, xMax : Valores m�nimo y m�ximo para las coordenadas en X.
yMin, yMax : Valores m�nimo y m�ximo para las coordenadas en Y.

Salida de la funci�n:

coordenadasX: Vector con los valores de las coordenadas en X.
coordenadasY: Vector con los valores de las coordenadas en Y.

*/

void generarCoordenadas(int coordenadasX[], int coordenadasY[], int nCiudades, int xMin, int xMax, int yMin, int yMax)
{
	// paraleliar esta funcion
	// #pragma omp parallel forma
	int i = 0, rango = 0;
#pragma omp parallel for private(i, rango) shared(coordenadasX, coordenadasY, nCiudades)
	for (i = 0; i < nCiudades; i++)
	{
		rango = xMax - xMin + 1;
		coordenadasX[i] = rand() % rango + xMin;
		rango = yMax - yMin + 1;
		coordenadasY[i] = rand() % rango + yMin;
	}

	// Se imprime el vector en un archivo de texto.
	char cX[] = "coordenadasX.txt";
	imprimirVectorEnteroFichero(coordenadasX, nCiudades, cX);
	char cY[] = "coordenadasY.txt";
	imprimirVectorEnteroFichero(coordenadasY, nCiudades, cY);
}

/*
Funci�n que genera la matriz de adyacencias del grafo determinado por los nodos 
cuyas coordenadas vienen dados en los vectores coordenadasX y coordenadasY. E
sta matriz contiene las distancias entre los nodos del grafo determinado por tales coordenadas.

Par�metros de entrada:

nCiudades: N�mero de ciudades para el problema del transporte.
coordenadasX: Vector con los valores de las coordenadas en X.
coordenadasY: Vector con los valores de las coordenadas en Y.

Salida de la funci�n:

matrizDistancia: Matriz que contiene las distancias entre las ciudades dadas por las coordenada de entradas. Es una matriz cuadrada, sim�trica y de diagonal 0.

*/
void crearMatrizDistancia(double matrizDistancias[], double coordenadasX[], double coordenadasY[], int nCiudades)
{
	int i = 0, j = 0;
	#pragma omp parallel for private(i, j) shared(matrizDistancias, coordenadasX, coordenadasY, nCiudades)
	for (i = 0; i < nCiudades; i++)
		for (j = 0; j < nCiudades; j++)
		{
			double distancia = sqrt(pow((coordenadasX[i] - coordenadasX[j]), 2) + pow((coordenadasY[i] - coordenadasY[j]), 2));
			matrizDistancias[i * nCiudades + j] = distancia;
		}
}

/*
Funci�n que retorna la longitud del camino dado como par�metro de entrada 
que es igual a la distancia recorrida al iniciar  en la ciudad camino(i) y regresar al mismo lugar. 

Par�metros de entrada:

camino: vector que muestra el orden de las ciudades recorridas en un circuito
matrizDistancia: requerido para obtener la distancia entre ciudades.

Salida de la funci�n

Longitud: valor real que corresponde a la longitud del recorrido correspondiente al circuito dado.

*/
float longitudCircuito(int camino[], double matrizDistancia[], int nCiudades)
{
	double suma = 0;
	for (int i = 0; i < nCiudades - 1; i++)
	{
		double distancia = matrizDistancia[(camino[i] - 1) * nCiudades + camino[i + 1] - 1];
		suma += distancia;
	}
	suma += matrizDistancia[(camino[nCiudades - 1] - 1) * nCiudades + camino[0] - 1];
	return (suma);
}

/*

Funci�n que genera una permutaci�n con el recorrido por n ciudades, sin repetir ciudad. 

Par�metros de entrada: 

nCiudades: n�mero de ciudades bajo an�lisis.

Salida de la funci�n:

camino:  vector de enteros consistente en una permutaci�n de n ciudades entre 1 y n.

*/
void generaIndividuo(int camino[], int nCiudades)
{
	int existe, i, j, ciudad;
	for (i = 0; i < nCiudades; i++)
	{
		existe = 1;
		while (existe == 1)
		{
			existe = 0;
			ciudad = rand() % nCiudades + 1;
			j = 0;
			while ((j < i) && (existe == 0))
			{
				if (camino[j] == ciudad)
					existe = 1;
				j++;
			}
			if (existe == 0)
				camino[i] = ciudad;
		}
	}
}

/*
Funci�n que retorna el valor de la adaptaci�n, que permitir� determinar quien es el individuo mejor adaptado. 
Se calcula como la longitud m�xima del recorrido en la poblaci�n actual 
menos el valor de la longitud del individuo bajo an�lisis: 

adaptaci�n = longMaxima+1 - longitud

Par�metros de entrada:

longutid: Valor de la longitud del individuo
longMaxima: Distancia de mayor recorrido entre todos los individuos de la poblaci�n

Salida de la funci�n 

aptitud: Valor que correponde a la adaptaci�n al problema del transporte. Es un entero positivo, a mayor valor, mejor adaptaci�n
*/

float adaptacion(float longitud, float longitudMaxima)
{
	float aptitud = (longitudMaxima + 1) - longitud;
	return aptitud;
}

float maxElementoVector(double vector[], int n)
{
	float max = -1;
	for (int i = 0; i < n; i++)
	{
		if (vector[i] > max)
			max = vector[i];
	}
	return max;
}
/*
Funci�n que genera la poblaci�n inicial en un arreglo bidimensional (matriz). 
El n�mero de filas corresponde al tama�o de la poblaci�n. 
Cada fila es una soluci�n posible al problema del TSP,  
siendo una combinaci�n que indica el orden del recorrido sobre n ciudades, 
es decir un vector num�rico con n valores aleatorios entre 1 y n, 
sin que se repita ninguna valor. 
Adem�s de la poblaci�n se deben generar dos vectores adicionales, 
el uno corresponde a la distancia del recorrido de cada individuo 
y el otro a su aptitud, obtenido de la funci�n de adaptaci�n.  

Par�metros de entrada:

tamPob: entero que indica el tama�o de la poblaci�n a generar
nCiudades: entero que indica el n�mero de ciudades correspondiente al problema.
matrizDistancia: matriz de adyacencias correspondiente a las distancias entre las ciudades a modelar.

Salida de la funci�n

poblacion: matriz con los individuos que conforman la poblaci�n.
distancias: arreglo unidimensional (vector) correspondiente a la distancia que corresponde 
            al recorrido de cada individuo en la poblaci�n
aptitud: vector correspondiente a la aptitud o adaptaci�n de cada individuo.
*/

void poblacionInicial(int poblacion[], double distancias[], double aptitud[], int tamPoblacion, int nCiudades, double matrizDistancia[])
{
	int i, j, camino[nCiudades];
	double longitud, longMaxima = -1;
#pragma omp parallel for private(i, j, camino) shared(poblacion, tamPoblacion, nCiudades, matrizDistancia, distancias)
	for (i = 0; i < tamPoblacion; i++)
	{
		// generaIndividuo(camino, nCiudades);
		generaIndividuo(camino, nCiudades);
		distancias[i] = longitudCircuito(camino, matrizDistancia, nCiudades);
		if (distancias[i] > longMaxima)
			longMaxima = distancias[i];
		for (j = 0; j < nCiudades; j++)
		{
			poblacion[i * nCiudades + j] = camino[j];
		}
	}
	for (i = 0; i < tamPoblacion; i++)
	{
		aptitud[i] = adaptacion(distancias[i], longMaxima);
	}
}

/*
Funci�n que permite calcular para cada individuo de la poblaci�n: 
la puntuaci�n, que corresponde a la probabilidad de ser seleccionado en la siguiente generaci�n, 
la puntuaci�n acumulada, obtenida con los valores anteriores 
y la posici�n (indice de la fila) del individuo mejor adaptado. 

Par�metros de entrada:

poblacion: array bidimensional (matriz) con los individuos en representaci�n adecuada.
aptitud: array unidimensional, con el valor de la aptitud (adaptaci�n) de cada individuo

Salida de la funci�n:

puntuacion: puntuaci�n de cada individuo que corresponde a la probabilidad de selecci�n
			calculada como la adaptaci�n del individuo sobre la suma total 
			de las adaptaciones de toda la poblaci�n.
puntAcumulada: obtenida como la suma acumulada de las adaptaciones individuales. Este valor se
               utilizar� para el proceso de selecci�n de la poblaci�n.
posMejor: Valor entero que representa el �ndice de la fila en 
		  la tabla de la poblaci�n con el mejor.

*/

int evaluacion(double aptitud[],														// datos entrada
							 double puntuacion[], double puntAcumulada[], // Salida de la funcion
							 int tamPoblacion)														// datos entrada
{
	// Se suma calcula la suma de todas las aptitudes y
	// se busca cual es la mejor aptitud
	float sumaAptitud = 0;
	float aptitudMejor = -1;
	float acumulada = 0;
	int posMejor = 0;
	for (int i = 0; i < tamPoblacion; i++)
	{
		sumaAptitud += aptitud[i];
		if (aptitud[i] > aptitudMejor)
		{
			posMejor = i;
			aptitudMejor = aptitud[i];
		}
	}
	// Se calcula la puntuacion y la puntuacion acumulada
	for (int i = 0; i < tamPoblacion; i++)
	{
		puntuacion[i] = aptitud[i] / sumaAptitud;
		puntAcumulada[i] = puntuacion[i] + acumulada;
		acumulada += puntuacion[i];
	}
	return posMejor;
}
/*
Funci�n que  selecciona los individuos que van a trascender a la siguiente generaci�n 
o que van a cruzarse para generar nuevos individuos. 
La selecci�n se realiza mediante la t�cnica de la ruleta, 
teniendo en cuenta la puntuaci�n acumulada. 

Par�metros de entrada:

tamPoblaci�n: Tama�o de la poblaci�n ( filas de la matriz poblacion)
nCiudades: N�mero de ciudades de la poblacion ( columnas de la matriz poblacion

poblacion: la tabla (matriz) con los individuos en forma de cadena binaria (genotipos), 
           de la generaci�n actual.
puntAcumulada: Vector con la puntuaci�n acumulada de cada individuo que se utilizar�
               para seleccionar los individuos que formar�n la pr�xima generaci�n,
aptitud: Vector de aptitud de la poblaci�n, se usar� para actualizar la nueva poblacion
distancia: vector de distancias de la poblaci�n, se usar� para actualizar la nueva problaci�n

Salida de la funci�n:

pobAuxiliar: Tabla (matriz) con los individuos de la nueva generaci�n.

nuevaAptitud: vector con la aptitud (adaptaci�n) de cada individuo de la nueva generaci�n.

nuevaDistancias: vector con las distancias de cada individuo de la nueva generaci�n.

*/
void seleccion(int poblacion[], double puntAcumulada[],
							 double aptitud[], double distancias[],															 // datos entrada
							 int pobAuxiliar[], double nuevaAptitud[], double nuevaDistancias[], // Salida de la funcion
							 int tamPoblacion, int nCiudades)																		 // datos entrada
{
	/* printf("\nantes de la seleccion\n"); */
	/* 	imprimirMatrizEnteros(poblacion, tamPoblacion, nCiudades); */
	/* printf("\nantes de la seleccion"); */

	int *selSuper; // vector que guarda los indices de los seleccionados para sobrevivir
	int *camino;	 // almacena los individuos que se insertaran en la nueva poblacion
	camino = (int *)malloc(nCiudades * sizeof(int));
	selSuper = (int *)malloc(tamPoblacion * sizeof(int));
	double probSel; // Almacenar la probabilidad de seleccion
	int posSuper;		// posici�n del superviviente

	// Se seleccionan las posiciones de los supervivientes
	for (int i = 0; i < tamPoblacion; i++)
	{
		probSel = (rand() % 100 + 1) / 100.0;
		//printf("\n probabilidad = %.2f ",probSel);
		posSuper = 0;
		while ((probSel > puntAcumulada[posSuper]) && (posSuper < tamPoblacion - 1))
			posSuper++;
		selSuper[i] = posSuper;
	}
	// Se genera la poblaci�n superviviente
	for (int i = 0; i < tamPoblacion; i++)
	{
		int pos = selSuper[i];
		/* printf("\n -- pos_sel_super: %d -- \n", pos); */
		extraerVector(poblacion, camino, pos, tamPoblacion, nCiudades);
		insertarVector(pobAuxiliar, camino, i, i, nCiudades);
		nuevaAptitud[i] = aptitud[pos];
		nuevaDistancias[i] = distancias[pos];
	}
	/* printf("\n -- despues de la seleccion -- \n"); */
	/* imprimirVectorEnteros(camino, nCiudades); */
	/* imprimirMatrizEnteros(pobAuxiliar, tamPoblacion, nCiudades); */
	/* printf("\n -- despues de la seleccion -- \n"); */
	free(selSuper);
	free(camino);
}

/*
Esta funci�n verifica si el valor dado en el par�metro ciudad ya existe en el camino
Los parametros de entrada son:
camino: Un individuo que correponde a un camino valido
ciudad: numero entero que cuyo valor se busca dentro del cromosoma
nCiudades: tama�o del vector camino

Valor retornado: 1, si el num existe en el cromosoma, 0 en caso contrario
*/
int existe(int camino[], int ciudad, int nCiudades)
{
	for (int i = 0; i < nCiudades; i++)
	{
		if (camino[i] == ciudad)
		{
			return 1;
		}
	}
	return 0;
}

/*
Esta funcion relaciona dos vectores (segmentos de caminos) 
para el operador de cruce multipunto.

Los par�metros de entrada son:

vector1, vector 2: vectores del mismo tama�o.
num: valor entero en vector1 del que se busca su valor relacionado en vector2
tam: tama�o de los vectores.

*/
int buscarPareja(int vector1[], int vector2[], int num, int tam)
{
	/* printf("| buscarPareja | num: %d \ttam: %d\n", num, tam); */
	/* imprimirVectorEnteros(vector1, tam); */
	/* imprimirVectorEnteros(vector2, tam); */
	for (int i = 0; i < tam; i++)
	{
		if (vector1[i] == num)
		{
			return vector2[i];
		}
	}
	printf("\nError en buscarPareja %d\n", num);
	exit(0);
	return 1;
}

void crucePadreHijo(int pInicial, int pFinal, int padre[], int hijo[], int vector1[], int vector2[], int tam, int nCiudades)
{
	for (int i = pInicial; i < pFinal; i++)
	{
		int num = padre[i];
		int numTemp = padre[i];
		int e = existe(hijo, num, nCiudades);
		while (e == 1)
		{
			numTemp = num;
			num = buscarPareja(vector1, vector2, num, tam);
			e = existe(hijo, num, nCiudades);
			if (num == numTemp)
			{
				break;
			}
		}
		hijo[i] = num;
	}
}

/*
El operador de cruce toma dos padres y genera dos cadenas hijas. 
Los par�metros requeridos son: 
padre1, padre2: Vectores que contiene la cadena  correpondiente los camunos padre
La funci�n debe retornar:
hijo1, hijo2: Vectores correspondientes a los decendientes de los caminos padre
     
*/
void cruce(int padre1[], int padre2[], int hijo1[], int hijo2[], int nCiudades)
{
	// se generan aleatoriamente los puntos de cruce
	int pCruce1 = rand() % (nCiudades - 2) + 1;
	int pCruce2 = (rand() % (nCiudades - pCruce1 - 1)) + pCruce1 + 1;

	int *vector1, *vector2;
	int tam = pCruce2 - pCruce1;
	vector1 = (int *)malloc(tam * sizeof(int));
	vector2 = (int *)malloc(tam * sizeof(int));
	// iniciamos vectores hijos
	for (int i = 0; i < nCiudades; i++)
	{
		hijo1[i] = 0;
		hijo2[i] = 0;
	}
	int k = 0;
	for (int i = pCruce1; i < pCruce2; i++)
	{
		hijo1[i] = padre2[i];
		hijo2[i] = padre1[i];
		vector1[k] = padre1[i];
		vector2[k] = padre2[i];
		k++;
	}

	//primera parte del hijo1, se toma la primera parte del padre 1, pero si algun
	//valor se repite, es reemplazado seg�n la relacion establecida entre los genes
	//de la parte central.
	crucePadreHijo(0, pCruce1, padre1, hijo1, vector2, vector1, tam, nCiudades);

	// Segunda parte del hijo1, se toma la segunda parte del padre 1, pero si algun
	// valor se repite, es reemplazado seg�n la relacion establecida entre los genes
	// de la parte central.
	crucePadreHijo(pCruce2, nCiudades, padre1, hijo1, vector2, vector1, tam, nCiudades);

	//Primera parte del hijo2, se toma la primera parte del padre 2, pero si algun
	//valor se repite, es reemplazado seg�n la relacion establecida entre los genes
	//de la parte central.
	crucePadreHijo(0, pCruce1, padre2, hijo2, vector1, vector2, tam, nCiudades);

	//Segunda parte del hijo2, se toma la segunda parte del padre 2, pero si algun
	//valor se repite, es reemplazado seg�n la relacion establecida entre los genes
	//de la parte central.
	crucePadreHijo(pCruce2, nCiudades, padre2, hijo2, vector1, vector2, tam, nCiudades);

	free(vector1);
	free(vector2);
}
/*
Esta funci�n selecciona parejas de individuos de la poblaci�n dada como par�metro 
(poblacion seleccionada ) y de acuerdo a la probabilidad de cruce, 
realiza el cruce obteniendo dos nuevos individuos que reemplazan a sus padres. 
Retorna la poblaci�n con los individuos nuevos insertados.
La funcion tiene los siguientes par�metros:
    pobacion:  tabla de datos con los caminos en forma de combinacion de enteros
    prob_cruce: Valor que determina la probabilidad de que una pareja de individuos se cruce

La funci�n debe retornar:
    nuevaPoblacion:  la nueva poblacion con los individuos obtenidos como resultado del cruce
    aptitud: el vector con el valor de la aptitud de cada camino
    distancia: el vector con el valor de la distancia de cada camino
*/
void reproduccion(int poblacion[], double distancia[], double aptitud[],
									double probCruce, double matrizDistancias[],
									int tamPoblacion, int nCiudades)
{

	int *selCruce, *hijo1, *hijo2, *padre1, *padre2, i; // vector que guarda los indices de los seleccionados para cruce
	int numSelCruce = 0;																// contador de seleccionados para cruce

	hijo1 = (int *)malloc(nCiudades * sizeof(int));
	hijo2 = (int *)malloc(nCiudades * sizeof(int));
	padre1 = (int *)malloc(nCiudades * sizeof(int));
	padre2 = (int *)malloc(nCiudades * sizeof(int));
	selCruce = (int *)malloc(tamPoblacion * sizeof(int));
	// Este ciclo elije los individuos para cruzar
	for (i = 0; i < tamPoblacion; i++)
	{
		double prob = (rand() % 100 + 1) / 100.0;

		// Se eligen los individuos de las posiciones i si prob < proCruce
		if (prob < probCruce)
		{
			selCruce[numSelCruce] = i;
			numSelCruce += 1;
		}
	}
	// El numero de seleccionados se hace par
	if ((numSelCruce % 2) == 1)
		numSelCruce -= 1;
	double mayorDistancia = -1;
	for (i = 0; i < tamPoblacion; i++)
		if (distancia[i] > mayorDistancia)
			mayorDistancia = distancia[i];
	i = 0;
	while (i < numSelCruce)
	{
		extraerVector(poblacion, padre1, selCruce[i], tamPoblacion, nCiudades);
		extraerVector(poblacion, padre2, selCruce[i + 1], tamPoblacion, nCiudades);
		cruce(padre1, padre2, hijo1, hijo2, nCiudades);
		insertarVector(poblacion, hijo1, selCruce[i], tamPoblacion, nCiudades);
		distancia[selCruce[i]] = longitudCircuito(hijo1, matrizDistancias, nCiudades);
		if (distancia[selCruce[i]] > mayorDistancia)
			mayorDistancia = distancia[selCruce[i]];
		insertarVector(poblacion, hijo2, selCruce[i + 1], tamPoblacion, nCiudades);
		distancia[selCruce[i + 1]] = longitudCircuito(hijo2, matrizDistancias, nCiudades);
		if (distancia[selCruce[i + 1]] > mayorDistancia)
			mayorDistancia = distancia[selCruce[i + 1]];
		aptitud[selCruce[i]] = adaptacion(distancia[selCruce[i]], mayorDistancia);
		aptitud[selCruce[i + 1]] = adaptacion(distancia[selCruce[i + 1]], mayorDistancia);
		i += 2;
	}
	/* printf("| reproduccion | FIN DE LA REPRODUCCION\n"); */
	free(hijo1);
	free(hijo2);
	free(padre1);
	free(padre2);
	free(selCruce);
}

double obtenerLongitudMaxima(double matrizDistancias[], int nCiudades)
{
	double mayorDistancia = -1;
	for (int i = 0; i < nCiudades; i++)
		for (int j = 0; j < nCiudades; j++)
			if (matrizDistancias[i * nCiudades + j] > mayorDistancia)
				mayorDistancia = matrizDistancias[i * nCiudades + j];
	return mayorDistancia;
}

/* 
	Dado un individuo (camino) este se debe mutar, intercambiando dos ciudades del camino aleatoriamente. La
	mutación debe ocurrir en todos los individuos de la población de acuerdo a una probabilidad. Se debe
	generar un numero aleatorio por cada individuo de la población, y si este es mejor que la probabilidad de
	mutación, el individuo debe mutar. 
 */
void mutacion(int poblacion[], double aptitud[], double matrizDistancias[], double distancias[], int tamPoblacion, double probMut, int nCiudades)
{
	/* printf("| mutacion | INICIO DE LA MUTACION\n"); */
	int i, j, mutado, *individuo;
	int nmMut = 0;
	individuo = (int *)malloc(nCiudades * sizeof(int));
	double prob;
	for (i = 0; i < tamPoblacion; i++)
	{
		mutado = 0;
		for (j = 0; j < nCiudades; j++)
		{
			prob = (rand() % 1000 + 1) / 1000.0;
			if (prob < probMut)
			{
				int pos2 = rand() % nCiudades;
				int aux = poblacion[i * nCiudades + j];
				poblacion[i * nCiudades + j] = poblacion[i * nCiudades + pos2];
				poblacion[i * nCiudades + pos2] = aux;
				mutado = 1;
			}
		}
		if (mutado == 1)
		{
			nmMut++;
			extraerVector(poblacion, individuo, i, tamPoblacion, nCiudades);
			distancias[i] = longitudCircuito(individuo, matrizDistancias, nCiudades);
			// double mayorDistancia = obtenerLongitudMaxima(matrizDistancias, nCiudades);
			double mayorDistancia = maxElementoVector(distancias, tamPoblacion);
			aptitud[i] = adaptacion(distancias[i], mayorDistancia);
		}
	}
	// printf("| mutacion | %d individuos mutados\n", nmMut);
	/* printf("| mutacion | poblacion al salir \n"); */
	/* imprimirMatrizEnteros(poblacion, tamPoblacion, nCiudades); */
	/* printf("| mutacion | FIN DE LA MUTACION\n"); */
}

void extraerMatriz(int matriz[], int matriz_2[], int m, int n)
{
	for (int i = 0; i < m; i++)
		for (int j = 0; j < n; j++)
			*(matriz_2 + (n * i + j)) = *(matriz + (n * i + j));
}

void extraerVectorDouble(double vector[], double vector_2[], int m)
{
	for (int j = 0; j < m; j++)
		*(vector_2 + j) = *(vector + j);
}

int mayorAptitud(double aptitud[], double indice, int tamPoblacion)
{
	int i, mayor = 0;
	for (i = 0; i < tamPoblacion; i++)
	{
		if (aptitud[i] > aptitud[mayor] && (indice != i || indice == -1))
		{
			mayor = i;
		}
	}
	return mayor;
}
void rellenarPeores(int peores[], int tamPeores)
{
	int i;
	for (i = 0; i < tamPeores; i++)
	{
		peores[i] = 0;
	}
}
void obtenerPeores(double aptitud[], int tamPoblacion, int peores[], int nPeores)
{
	rellenarPeores(peores, nPeores);
	int i, j, menor = 0, e = 0;
	for (i = 0; i < nPeores; i++)
	{
		for (j = 0; j < tamPoblacion; j++)
		{
			e = existe(peores, j, nPeores);
			if (aptitud[j] < aptitud[menor] && e == 0)
			{
				menor = j;
			}
		}
		peores[i] = menor;
	}
}

void obtenerMejores(double aptitud[], int tamPoblacion, int mejores[], int nMejores)
{
	int i, j, mejor = 0, e = 0;
	for (i = 0; i < nMejores; i++)
	{
		for (j = 0; j < tamPoblacion; j++)
		{
			e = existe(mejores, j, nMejores);
			if (aptitud[j] > aptitud[mejor] && e == 0)
			{
				mejor = j;
			}
		}
		mejores[i] = mejor;
	}
}

void recalcularAptitud(int poblacionPrincipal[], double aptitud[],
 double matrizDistancias[], double distancias[], int tamPoblacion, int nCiudades) {
		int *camino = (int *)malloc(nCiudades * sizeof(int));
	double longMaxima = 0;
	for (int i = 0; i < tamPoblacion; i++)
	{
		extraerVector(poblacionPrincipal, camino, i, tamPoblacion, nCiudades);
		distancias[i] = longitudCircuito(camino, matrizDistancias, nCiudades);
		if (distancias[i] > longMaxima)
			longMaxima = distancias[i];
		for (int j = 0; j < nCiudades; j++)
		{
			poblacionPrincipal[i * nCiudades + j] = camino[j];
		}
	}
	for (int i = 0; i < tamPoblacion; i++)
	{
		aptitud[i] = adaptacion(distancias[i], longMaxima);
	}
}

void intercambioMaestroIsla(int *poblacionObjetivo, int *poblacionBase, double *aptitudMejor,
														double *aptitudPeor, int *peores, int tamPoblacion, 
														int nCiudades, int nMejores, int nPeores,  int indice)
{
	int *mejores = (int *)malloc(nMejores * sizeof(int));
	obtenerMejores(aptitudMejor, tamPoblacion, mejores, nMejores);

	if(peores == nullptr){
		peores = (int *)malloc(nPeores * sizeof(int));
		obtenerPeores(aptitudPeor, tamPoblacion, peores, nPeores);	
	}
	// int *peoresI1 = (int *)malloc(nPeores * sizeof(int));

	for (int i = 0; i < nMejores; i++)
	{
		int *individuo = (int *)malloc(nCiudades * sizeof(int));
		extraerVector(poblacionBase, individuo, mejores[i], tamPoblacion, nCiudades);
		insertarVector(poblacionObjetivo, individuo,  peores[i+indice], tamPoblacion, nCiudades);
	}
}

void isla(int poblacion[],int pobAux[], double puntAcumulada[],
							 double aptitud[], double distancias[],														
							 double nuevaAptitud[], double nuevaDistancias[],
							 double matrizDistancias[],double puntuacion[],
							 double probMut, double probCruce, 
							 int tamPoblacion, int nCiudades)
{
	seleccion(poblacion, puntAcumulada, aptitud, distancias, // datos entrada
					pobAux, nuevaAptitud, nuevaDistancias,					 // Salida de la funcion
					tamPoblacion, nCiudades);													 // datos entrada
	extraerMatriz(pobAux, poblacion, tamPoblacion, nCiudades);
	extraerVectorDouble(nuevaDistancias, distancias, tamPoblacion);
	extraerVectorDouble(nuevaAptitud, aptitud, tamPoblacion);
	// printf("\nHilo: %d\n", hilo);
	reproduccion(poblacion, distancias, aptitud, probCruce,
								matrizDistancias, tamPoblacion, nCiudades);
	
	mutacion(poblacion, aptitud, matrizDistancias, distancias, tamPoblacion, probMut, nCiudades);
	int posMejor = evaluacion(aptitud, puntuacion, puntAcumulada, tamPoblacion); // datos entrada
}

void leerFicheroCordenadas(char filename[], double coordenadas[]){
    FILE *input_file = fopen(filename, "r");
    if (!input_file)
		{
			printf("Error al abrir el fichero\n");
			exit(EXIT_FAILURE);
    }
		for(int i = 0; i < 30; i++){
      fscanf(input_file, "%lf", &coordenadas[i]);
    }
    // for(int i = 0; i < 29; i++)
    //   printf("%f\n", coordenadas[i]);
    /* printf("se escribió\n"); */
    fclose(input_file);
}

void leerFicheroCamino(char filename[], int camino[], int nCiudades){
    FILE *input_file = fopen(filename, "r");
    if (!input_file)
		{
			printf("Error al abrir el fichero\n");
			exit(EXIT_FAILURE);
    }
		for(int i = 0; i < nCiudades; i++){
      fscanf(input_file, "%d", &camino[i]);
    }
    // for(int i = 0; i < 29; i++)
    //   printf("%f\n", coordenadas[i]);
    /* printf("se escribió\n"); */
    fclose(input_file);
}
int main(int argc, char **argv)
{
  int thread_num = 8;
  int stop = 0;

  /* int* thread_num = (int *) thread_num_c; */
  if (stop == 0)
  {
    printf("\n\n**__** ITERACION = %d \n", thread_num);
  }
	double time_g_1 = omp_get_wtime();
	// Semilla para n�meros aleatorios
	srand(time(NULL));

	// Declaracion variables - par�metros del algoritmo
	int nCiudades = 29;			 // nodos del grafo
	int tamPoblacion = 150; // tama�o de la poblaci�n
	int numMaxGen = 200;
	// int xMin = 1, xMax = 10, yMin = 1, yMax = 10; // limites de las coordenadas del grafo
	
	// int nIslas = 8;
	int nMigraciones = 50;
	int nPeores = 16;
	int nHilos = thread_num;
	// printf("Numero de hilos: %d\n", nHilos);

	int posMejor = 0;				 // posici�n del individuo mejor adaptado
	double probCruce = 0.8;
	double probMut = 0.004;

	// Variables dinamicas para almacenamiento de vectores y matroces
	double *coordenadasX = (double *)malloc(nCiudades * sizeof(double));
  double *coordenadasY = (double *)malloc(nCiudades * sizeof(double));
	/* int *coordenadasX = (int *)malloc(nCiudades * sizeof(int)); */
	// printf("\n\n**__** Coordenadas X\n");
	char nombreCX[] = "coordenadasX_S.txt";
	char nombreCY[] = "coordenadasY_S.txt";
	leerFicheroCordenadas(nombreCX, coordenadasX);
	leerFicheroCordenadas(nombreCY, coordenadasY);

	int *poblacionPrincipal = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	int *mejorCamino, *pobAuxiliar,
			*padre1, *padre2, *hijo1, *hijo2;
	double *matrizDistancias, *distancias, *aptitud, *puntuacion,
			*puntAcumulada, *nuevaAptitud, *nuevaDistancias;
	double longitud;

	mejorCamino = (int *)malloc(nCiudades * sizeof(int));
	distancias = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud = (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada = (double *)malloc(tamPoblacion * sizeof(double));
	matrizDistancias = (double *)malloc(nCiudades * nCiudades * sizeof(double));
	pobAuxiliar = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));

	int *poblacion1, *poblacion2, *pobAux1, *pobAux2;
	poblacion1 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	poblacion2 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	pobAux1 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	pobAux2 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));

	double *distancias1, *aptitud1, *puntuacion1, *puntAcumulada1, *nuevaAptitud1, *nuevaDistancias1;
	distancias1 = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud1 = (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion1 = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada1 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud1 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias1 = (double *)malloc(tamPoblacion * sizeof(double));

	double *distancias2, *aptitud2, *puntuacion2, *puntAcumulada2, *nuevaAptitud2, *nuevaDistancias2;
	distancias2 = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud2 = (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion2 = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada2 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud2 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias2 = (double *)malloc(tamPoblacion * sizeof(double));

	int *poblacion3, *pobAux3;
	poblacion3 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	pobAux3 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	double *distancias3, *aptitud3, *puntuacion3, *puntAcumulada3, *nuevaAptitud3, *nuevaDistancias3;
	distancias3 = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud3= (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion3 = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada3 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud3 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias3 = (double *)malloc(tamPoblacion * sizeof(double));

	int *poblacion4, *pobAux4;
	poblacion4 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	pobAux4 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	double *distancias4, *aptitud4, *puntuacion4, *puntAcumulada4, *nuevaAptitud4, *nuevaDistancias4;
	distancias4 = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud4= (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion4 = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada4 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud4 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias4 = (double *)malloc(tamPoblacion * sizeof(double));

	int *poblacion5, *pobAux5;
	poblacion5 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	pobAux5 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	double *distancias5, *aptitud5, *puntuacion5, *puntAcumulada5, *nuevaAptitud5, *nuevaDistancias5;
	distancias5 = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud5= (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion5 = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada5 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud5 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias5 = (double *)malloc(tamPoblacion * sizeof(double));

	int *poblacion6, *pobAux6;
	poblacion6 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	pobAux6 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	double *distancias6, *aptitud6, *puntuacion6, *puntAcumulada6, *nuevaAptitud6, *nuevaDistancias6;
	distancias6 = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud6= (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion6 = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada6 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud6 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias6 = (double *)malloc(tamPoblacion * sizeof(double));

	int *poblacion7, *pobAux7;
	poblacion7 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	pobAux7 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	double *distancias7, *aptitud7, *puntuacion7, *puntAcumulada7, *nuevaAptitud7, *nuevaDistancias7;
	distancias7 = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud7= (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion7 = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada7 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud7 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias7 = (double *)malloc(tamPoblacion * sizeof(double));

	int *poblacion8, *pobAux8;
	poblacion8 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	pobAux8 = (int *)malloc(tamPoblacion * nCiudades * sizeof(int));
	double *distancias8, *aptitud8, *puntuacion8, *puntAcumulada8, *nuevaAptitud8, *nuevaDistancias8;
	distancias8 = (double *)malloc(tamPoblacion * sizeof(double));
	aptitud8= (double *)malloc(tamPoblacion * sizeof(double));
	puntuacion8 = (double *)malloc(tamPoblacion * sizeof(double));
	puntAcumulada8 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaAptitud8 = (double *)malloc(tamPoblacion * sizeof(double));
	nuevaDistancias8 = (double *)malloc(tamPoblacion * sizeof(double));

	/* generarCoordenadas(coordenadasX, coordenadasY, nCiudades, xMin, xMax, yMin, yMax); */
	crearMatrizDistancia(matrizDistancias, coordenadasX, coordenadasY, nCiudades);
	// imprimirMatrizReales(matrizDistancias, nCiudades, nCiudades);

	poblacionInicial(poblacionPrincipal, distancias, aptitud, tamPoblacion, nCiudades, matrizDistancias);
	posMejor = evaluacion(aptitud, puntuacion, puntAcumulada, tamPoblacion);
	extraerVector(poblacionPrincipal, mejorCamino, posMejor, tamPoblacion, nCiudades);

	char nombreCamino[] = "caminoInicial.txt";
	imprimirVectorEnteroFichero(mejorCamino, nCiudades, nombreCamino);
	longitud = longitudCircuito(mejorCamino, matrizDistancias, nCiudades);

	double time_1 = omp_get_wtime();
	for (int generacion = 1; generacion <= numMaxGen; generacion++)
	{
		if (generacion == 1)
		{
			extraerMatriz(poblacionPrincipal, poblacion1, tamPoblacion, nCiudades);
			extraerVectorDouble(distancias, distancias1, tamPoblacion);
			extraerVectorDouble(aptitud, aptitud1, tamPoblacion);
			extraerVectorDouble(puntuacion, puntuacion1, tamPoblacion);
			extraerVectorDouble(puntAcumulada, puntAcumulada1, tamPoblacion);

			extraerMatriz(poblacionPrincipal, poblacion2, tamPoblacion, nCiudades);
			extraerVectorDouble(distancias, distancias2, tamPoblacion);
			extraerVectorDouble(aptitud, aptitud2, tamPoblacion);
			extraerVectorDouble(puntuacion, puntuacion2, tamPoblacion);
			extraerVectorDouble(puntAcumulada, puntAcumulada2, tamPoblacion);

			extraerMatriz(poblacionPrincipal, poblacion3, tamPoblacion, nCiudades);
			extraerVectorDouble(distancias, distancias3, tamPoblacion);
			extraerVectorDouble(aptitud, aptitud3, tamPoblacion);
			extraerVectorDouble(puntuacion, puntuacion3, tamPoblacion);
			extraerVectorDouble(puntAcumulada, puntAcumulada3, tamPoblacion);

			extraerMatriz(poblacionPrincipal, poblacion4, tamPoblacion, nCiudades);
			extraerVectorDouble(distancias, distancias4, tamPoblacion);
			extraerVectorDouble(aptitud, aptitud4, tamPoblacion);
			extraerVectorDouble(puntuacion, puntuacion4, tamPoblacion);
			extraerVectorDouble(puntAcumulada, puntAcumulada4, tamPoblacion);

			extraerMatriz(poblacionPrincipal, poblacion5, tamPoblacion, nCiudades);
			extraerVectorDouble(distancias, distancias5, tamPoblacion);
			extraerVectorDouble(aptitud, aptitud5, tamPoblacion);
			extraerVectorDouble(puntuacion, puntuacion5, tamPoblacion);
			extraerVectorDouble(puntAcumulada, puntAcumulada5, tamPoblacion);
/*  */
			extraerMatriz(poblacionPrincipal, poblacion6, tamPoblacion, nCiudades);
			extraerVectorDouble(distancias, distancias6, tamPoblacion);
			extraerVectorDouble(aptitud, aptitud6, tamPoblacion);
			extraerVectorDouble(puntuacion, puntuacion6, tamPoblacion);
			extraerVectorDouble(puntAcumulada, puntAcumulada6, tamPoblacion);
/*  */
			extraerMatriz(poblacionPrincipal, poblacion7, tamPoblacion, nCiudades);
			extraerVectorDouble(distancias, distancias7, tamPoblacion);
			extraerVectorDouble(aptitud, aptitud7, tamPoblacion);
			extraerVectorDouble(puntuacion, puntuacion7, tamPoblacion);
			extraerVectorDouble(puntAcumulada, puntAcumulada7, tamPoblacion);
/*  */
			extraerMatriz(poblacionPrincipal, poblacion8, tamPoblacion, nCiudades);
			extraerVectorDouble(distancias, distancias8, tamPoblacion);
			extraerVectorDouble(aptitud, aptitud8, tamPoblacion);
			extraerVectorDouble(puntuacion, puntuacion8, tamPoblacion);
			extraerVectorDouble(puntAcumulada, puntAcumulada8, tamPoblacion);
		}

		intercambioMaestroIsla(poblacion1, poblacionPrincipal, aptitud, aptitud1, nullptr, tamPoblacion, nCiudades, 2, 2, 0);
		intercambioMaestroIsla(poblacion2, poblacionPrincipal, aptitud, aptitud2, nullptr, tamPoblacion, nCiudades, 2, 2, 0);
		intercambioMaestroIsla(poblacion3, poblacionPrincipal, aptitud, aptitud3, nullptr, tamPoblacion, nCiudades, 2, 2, 0);
		intercambioMaestroIsla(poblacion4, poblacionPrincipal, aptitud, aptitud4, nullptr, tamPoblacion, nCiudades, 2, 2, 0);
		intercambioMaestroIsla(poblacion4, poblacionPrincipal, aptitud, aptitud5, nullptr, tamPoblacion, nCiudades, 2, 2, 0);
		intercambioMaestroIsla(poblacion4, poblacionPrincipal, aptitud, aptitud6, nullptr, tamPoblacion, nCiudades, 2, 2, 0);
		intercambioMaestroIsla(poblacion4, poblacionPrincipal, aptitud, aptitud7, nullptr, tamPoblacion, nCiudades, 2, 2, 0);
		intercambioMaestroIsla(poblacion4, poblacionPrincipal, aptitud, aptitud8, nullptr, tamPoblacion, nCiudades, 2, 2, 0);
		
		for (int migracion = 0; migracion < nMigraciones; migracion++)
		{
			time_1 += omp_get_wtime();
			#pragma omp parallel num_threads(nHilos)
			#pragma omp sections
			{
				#pragma omp section
				{
					int hilo = omp_get_thread_num();
					isla(poblacion1,pobAux1, puntAcumulada1,
							 aptitud1, distancias1,														
							 nuevaAptitud1, nuevaDistancias1,
							 matrizDistancias,puntuacion1,
							 probMut, probCruce, 
							 tamPoblacion, nCiudades);
				}
				#pragma omp section
				{
					isla(poblacion2,pobAux2, puntAcumulada2,
							 aptitud2, distancias2,														
							 nuevaAptitud2, nuevaDistancias2,
							 matrizDistancias,puntuacion2,
							 probMut, probCruce, 
							 tamPoblacion, nCiudades);
				}
				#pragma omp section
				{
					isla(poblacion3,pobAux3, puntAcumulada3,
							 aptitud3, distancias3,														
							 nuevaAptitud3, nuevaDistancias3,
							 matrizDistancias,puntuacion3,
							 probMut, probCruce, 
							 tamPoblacion, nCiudades);
				}
				#pragma omp section
				{
					isla(poblacion4,pobAux4, puntAcumulada4,
							 aptitud4, distancias4,														
							 nuevaAptitud4, nuevaDistancias4,
							 matrizDistancias,puntuacion4,
							 probMut, probCruce, 
							 tamPoblacion, nCiudades);
				}
				#pragma omp section
				{
					isla(poblacion5,pobAux5, puntAcumulada5,
							 aptitud5, distancias5,														
							 nuevaAptitud5, nuevaDistancias5,
							 matrizDistancias,puntuacion5,
							 probMut, probCruce, 
							 tamPoblacion, nCiudades);
				}
				#pragma omp section
				{
					isla(poblacion6,pobAux6, puntAcumulada6,
							 aptitud6, distancias6,														
							 nuevaAptitud6, nuevaDistancias6,
							 matrizDistancias,puntuacion6,
							 probMut, probCruce, 
							 tamPoblacion, nCiudades);
				}
				#pragma omp section
				{
					isla(poblacion7,pobAux7, puntAcumulada7,
							 aptitud7, distancias7,														
							 nuevaAptitud7, nuevaDistancias7,
							 matrizDistancias,puntuacion7,
							 probMut, probCruce, 
							 tamPoblacion, nCiudades);
				}
				#pragma omp section
				{
					isla(poblacion8,pobAux8, puntAcumulada8,
							 aptitud8, distancias6,														
							 nuevaAptitud8, nuevaDistancias8,
							 matrizDistancias,puntuacion8,
							 probMut, probCruce, 
							 tamPoblacion, nCiudades);
				}
			}

			int *peoresPI = (int *)malloc(nPeores * sizeof(int));
			obtenerPeores(aptitud, tamPoblacion, peoresPI, nPeores);
			intercambioMaestroIsla(poblacionPrincipal, poblacion1, aptitud1,aptitud, peoresPI, tamPoblacion, nCiudades, 2, -1, 0);
			intercambioMaestroIsla(poblacionPrincipal, poblacion2, aptitud2,aptitud, peoresPI, tamPoblacion, nCiudades, 2, -1, 2);
			intercambioMaestroIsla(poblacionPrincipal, poblacion3, aptitud3,aptitud, peoresPI, tamPoblacion, nCiudades, 2, -1, 4);
			intercambioMaestroIsla(poblacionPrincipal, poblacion4, aptitud4,aptitud, peoresPI, tamPoblacion, nCiudades, 2, -1, 6);
			intercambioMaestroIsla(poblacionPrincipal, poblacion5, aptitud5,aptitud, peoresPI, tamPoblacion, nCiudades, 2, -1, 8);
			intercambioMaestroIsla(poblacionPrincipal, poblacion6, aptitud6,aptitud, peoresPI, tamPoblacion, nCiudades, 2, -1, 10);
			intercambioMaestroIsla(poblacionPrincipal, poblacion7, aptitud7,aptitud, peoresPI, tamPoblacion, nCiudades, 2, -1, 12);
			intercambioMaestroIsla(poblacionPrincipal, poblacion8, aptitud8,aptitud, peoresPI, tamPoblacion, nCiudades, 2, -1, 14);
			
			recalcularAptitud(poblacionPrincipal, aptitud, matrizDistancias, distancias, tamPoblacion, nCiudades);
		}
	}
	posMejor = evaluacion(aptitud, puntuacion, puntAcumulada, tamPoblacion); // datos entrada
	double time_2 = omp_get_wtime();
printf("Mejor distancia : %d %.2f \n",posMejor, distancias[posMejor]); 
  // imprimirVectorReales(aptitud, nCiudades);
	extraerVector(poblacionPrincipal, mejorCamino, posMejor, tamPoblacion, nCiudades);
	char nombreCaminoFinal[] = "caminoFinal.txt";
	imprimirVectorEnteroFichero(mejorCamino, nCiudades, nombreCaminoFinal);
	imprimirVectorEnteros(mejorCamino, nCiudades);

	// double t = (time_2 - time_1) / (nMigraciones * numMaxGen);

	char nombreCaminoFinalTour[] = "caminoFinalTour.txt";
	int *mejorCaminoTour;
	mejorCaminoTour = (int *)malloc(nCiudades * sizeof(int));
	leerFicheroCamino(nombreCaminoFinalTour, mejorCaminoTour, nCiudades);
	float losc = longitudCircuito(mejorCaminoTour, matrizDistancias, nCiudades);
	printf("\nTSPLib long best tour : %.2f \n", losc); 
	imprimirVectorEnteros(mejorCaminoTour, nCiudades);
	double e = (distancias[posMejor]-losc) / losc;
	printf("Error : %.4f \n", (abs(e)*100)); 


	// printf("Tiempo de ejecucion: %f\n", t);          

	double time_g_2 = omp_get_wtime();
	double t = (time_g_2 - time_g_1);
	// printf("Tiempo: %f \n", time_g_2-time_g_1);
	printf("\n - **_** Suma secuencial = %d - tiempo = %f \n", 0, t);
  printf(" - Suma paralela = %d - tiempo = %f \n", 0, t);
  printf(" - Rendimiento = %d  \n", 100 - (1 / 1) * 100); 
}
