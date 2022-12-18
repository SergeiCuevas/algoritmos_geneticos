/*-------------------------------------------
GENERA LA POBLACIÓN INICIAL
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
}individual;   //Para un solo individuo


//Poblaciones
individual* parents;
individual* offspring;
 
  //Variables útiles
unsigned chromosome_length;

// RESERVAR MEMORIA PARA LAS POBLACIONES

void allocateMemory() 
{
  unsigned required_bytes = sizeof(individual) * POPULATION_SIZE;
  parents = (individual*) malloc(required_bytes);
  offspring = (individual*) malloc(required_bytes);

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
FUNCIÓN PRINCIPAL
--------------------------------------------*/
  int main()
  {
    //srand((log)time(NULL)); //semilla para generar aleatorios
    allocateMemory();
    createFirstgeneration();
    return 0;
  }

 



