/*-------------------------------------------
GENERA LA POBLACIÓN INICIAL y CALCULA LA APTITUD DE LA GENERACIÓN
---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

// Definmos al individuo

// Estructura

// Constantes de la función y el algorítmo genético

#define POPULATION_SIZE 100
//Límites de la función
#define FX_LOWER_BOUND -20
#define FX_UPPER_BOUND 20
//PRECISIÓN
#define PRECISION 3

//Definición del individuo
typedef struct ind_t
{
    int* chromosome;
    double x;  //genes
    double fitness;   //aptitud
    int parents[2];   //padres del individuo
    //Dónde ocurre la mutación y la cruza de pares
    int mutation_place;
    int crossover_place;
}Individual;   //Para un solo individuo


//Poblaciones
Individual* parents;
Individual* offspring;
 
  //Variables útiles
unsigned chromosome_length;

// RESERVAR MEMORIA PARA LAS POBLACIONES

void allocateMemory() 
{
  unsigned required_bytes = sizeof(Individual) * POPULATION_SIZE;
  parents = (Individual*) malloc(required_bytes);
  offspring = (Individual*) malloc(required_bytes);

  //Reserva memoria para los cromosomas. Para lo padres y la descendencia

  int i;
  chromosome_length = ceil (log2((FX_UPPER_BOUND - FX_LOWER_BOUND)*pow(10, PRECISION))); //Largo del cromosoma
  required_bytes = chromosome_length * sizeof(int);

  for (i=0; i<POPULATION_SIZE;  i++){
    parents[i].chromosome = (int*) malloc(required_bytes);
    offspring[i].chromosome = (int*) malloc(required_bytes);
  }

}


/*--------------------------------------------
Devuelve un aleatorio sobre un intervalo
--------------------------------------------*/
double randomDouble(double a, double b)
{
   return (b-a)*(rand()/(double)RAND_MAX) + a;
}

//Simula lanzar una moneda al aire
int flip(double probability)
{
    if(randomDouble(0,1) < probability )
        return 0;
    else
        return 1;    
}


/*--------------------------------------------
INICIALIZAR PRIMERA GENERACIÓN
--------------------------------------------*/
void createFirstgeneration()
{
    int i;
    for(i=0; i<POPULATION_SIZE; i++){
        parents[i].x = RAND_MAX;
        parents[i].fitness = 0;
        parents[i].parents[0] = parents[i].parents[1] = -1;
        parents[i].mutation_place = parents[i].crossover_place = -1;

        //Generamos el cromosoma aliatoriamente
        int j;
        for(j=0; j<chromosome_length; j++)
            parents[i].chromosome[j] = flip(0.5);     



    }
}

/*--------------------------------------------
DECODIFICA EL GENOTIPO EN BINARIO A REAL Y APLICA AJUSTE AL RANGO
--------------------------------------------*/
doule binary2real(int* chromosome)
{
    int i;
    double aux = 0.0;
    for (i=chromosome_length-1; i>=0; i--) //hace el recorrido de derecha a izquierda
       aux += (double) pow(2, chromosome_length-i-1);
    return FX_LOWER_BOUND + ( (aux*(FX_UPPER_BOUND - FX_LOWER_BOUND))/(pow(2, chromosome_length)-1));   
}


/*--------------------------------------------
EVALUAR LA APTITUD DEL INDIVIDUO
--------------------------------------------*/

void evaluateTargetFunction(Individual* individual) //nos devuelve la variable con el ajuste al rango
{
    individual->x = binary2real(individual->chromosome);
    individual->fitness = 1/(pow(individual->x, 2)+0.001); // el inverso porque vamos a minimizar

}


/*--------------------------------------------
FUNCIÓN PRINCIPAL
--------------------------------------------*/
  int main()
  {
    srand((log)time(NULL)); //semilla para generar aleatorios
    allocateMemory();
    createFirstgeneration();
    return 0;
  }

 



