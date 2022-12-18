/*-------------------------------------------
EJECUCIÓN


---------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
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

typedef struct ind_t {
    int* chromosome;
    double x;  //genes
    double fitness;   //aptitud
    int parents[2];   //padres del individuo
    //Dónde ocurre la mutación y la cruza de pares
    int mutation_place;
    int crossover_place;
} Individual;   //Para un solo individuo


//Poblaciones, elitista y la ruleta

Individual* parents;
Individual* offspring;
Individual the_best;
double* roulette;

 
  //Variables útiles

unsigned chromosome_length;
double crossover_probability;
double mutation_probability;

unsigned max_generations;
unsigned selected_father; // correción
unsigned selected_mother; //corrección

/*--------------------------------------------
PARAMETROS DEL ALGORITMO
--------------------------------------------*/

void getParameters()
{
    printf("\nNumero maximo de generaciones: "); scanf("%u", &max_generations);
    printf("\nProbabilidad de cruza: "); scanf("%lf", &crossover_probability);
    printf("\nProbabilidad de mutacion: "); scanf("%lf", &mutation_probability);
    
}

// RESERVAR MEMORIA PARA LAS POBLACIONES

void allocateMemory() 
{
  unsigned required_bytes = sizeof(Individual) * POPULATION_SIZE;
  parents = (Individual*) malloc(required_bytes);
  offspring = (Individual*) malloc(required_bytes);

  //Reserva memoria para los cromosomas. Para lo padres y la descendencia

  int i;
  chromosome_length = ceil (log2((FX_UPPER_BOUND - FX_LOWER_BOUND)*pow(10, PRECISION))); //Largo del cromosoma
 
  for (i=0; i<POPULATION_SIZE;  i++){

    parents[i].chromosome = (int*) calloc(chromosome_length, sizeof(int));
    offspring[i].chromosome = (int*) calloc(chromosome_length, sizeof(int));

     parents[i].x = offspring[i].x = RAND_MAX;
     parents[i].fitness = offspring[i].fitness = 0;

     parents[i].parents[0] = offspring[i].parents[0] = -1;
     parents[i].parents[1] = offspring[i].parents[1] = -1;

     parents[i].crossover_place = offspring[i].crossover_place = -1;
     parents[i].mutation_place = offspring[i].mutation_place = -1;
  }

  the_best.chromosome = (int*) calloc(chromosome_length, sizeof(int));
  the_best.fitness = 0;  //agregado para mostrar 

  roulette = (double*)malloc(sizeof(double) * POPULATION_SIZE);

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
    if(randomDouble(0,1) <= probability )
        return 1;
    else
        return 0;    
}


/*--------------------------------------------
INICIALIZAR PRIMERA GENERACIÓN
--------------------------------------------*/
void createFirstGeneration()
{ //Generamos el cromosoma aliatoriamente
    int i, j;
    for(i=0; i<POPULATION_SIZE; i++)     
            
        for(j=0; j<chromosome_length; j++)
            parents[i].chromosome[j] = flip(0.5);    
    
}

/*--------------------------------------------
DECODIFICA EL GENOTIPO EN BINARIO A REAL Y APLICA AJUSTE AL RANGO
--------------------------------------------*/
double binary2real(int* chromosome)
{
    int i;
    double aux = 0.0;
    for (i=chromosome_length-1; i>=0; i--) {//hace el recorrido de derecha a izquierda
      if(chromosome[i] == 1)
        aux += (double) pow(2, chromosome_length-i-1);
    }
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
EVALUAR APTITUD DE POBLACIÓN
--------------------------------------------*/
void evaluatePopulation(Individual* population)
{
 int i;
 for(i=0; i<POPULATION_SIZE; i++)
    evaluateTargetFunction(&population[i]);   
}

/*--------------------------------------------
LLENA LA RULETA CON LA PROBABILIDAD DE CADA INDIVIDUO DE SER SELECCIONADO
--------------------------------------------*/
void updateRoulette(Individual* population)
{
    int i;
    double sum_fitness = 0.0;
    for(i=0; i<POPULATION_SIZE; i++)
        sum_fitness += population[i].fitness;
    for(i=0; i<POPULATION_SIZE; i++)
        roulette[i] = population[i].fitness / sum_fitness;
}

/*--------------------------------------------
SELECCIÓN POR RULETA
se crea un avariable individuo actual
Se crea un for que empieza desde el tamalo de la población
--------------------------------------------*/
unsigned rouletteWheelSelection()
{
    double r = randomDouble(0,1);
    double sum = 0.0;
    int i, current_idividual;
    for(i= POPULATION_SIZE; sum<r; i++){
       current_idividual = i % POPULATION_SIZE;
       sum += roulette[current_idividual];
    }    
    return current_idividual;

}
/*--------------------------------------------
RECOMBINACIÓN DE LOS PADRES SELECIONADOS
--------------------------------------------*/
void crossover (Individual* father, Individual* mother, Individual* child1, Individual* child2)
{
    int i;
    if(flip(crossover_probability)){
        unsigned p = (unsigned) randomDouble(1, chromosome_length-2);  //p es el pivote
        for(i=0; i<p; i++) {
            child1->chromosome[i] = father->chromosome[i];
            child2->chromosome[p+i] = mother->chromosome[i];
        }
        for(i=p+1; i<chromosome_length; i++){
            child1->chromosome[i] = mother->chromosome[i];
            child2->chromosome[i-p-1] = father->chromosome[i];
        }
        child1->crossover_place = child2->crossover_place = p;

        child1->parents[0] = child2->parents[0] = selected_father + 1;
        child1->parents[1] = child2->parents[1] = selected_mother + 1;
   
    }else {
        for(i=0; i<chromosome_length; i++){
            child1->chromosome[i] = father->chromosome[i];
            child2->chromosome[i] = mother->chromosome[i]; 
         }
         child1->crossover_place = child2->crossover_place = -1;
        //si no se hace la cruza, queda en 0
         child1->parents[0] = child2->parents[0] = 0;
         child1->parents[1] = child2->parents[1] = 0;

    }
}

/*--------------------------------------------
    REALIZA LA MUTACION         
--------------------------------------------*/
void mutation(Individual* individual)
{
    if(flip(mutation_probability)){
        unsigned p = (unsigned) randomDouble(0, chromosome_length-1);
        individual->chromosome[p] = 1 - individual->chromosome[p];
        individual->mutation_place = p;
    }   else{
            individual->mutation_place = -1;
    }
}


/*--------------------------------------------
ELITISMO (LEY DEL MAS FUERTE)
--------------------------------------------*/
void elitism()
{
    unsigned best_parent; // índice del mejor padre
    unsigned worst_child1, worst_child2;
    int i;

    best_parent = worst_child1 = worst_child2 = 0;
    //the_best = parents[0];

    for (i=0; i<POPULATION_SIZE; i++){
        if(offspring[i].fitness < offspring[worst_child1].fitness )
        worst_child1 = i;
        else if(offspring[i].fitness < offspring[worst_child2].fitness)
             worst_child2 = i;

        if(parents[i].fitness > parents[best_parent].fitness );
            best_parent = i;
    }

    offspring[worst_child1] = parents[best_parent];
    offspring[worst_child2] = parents[best_parent];
}

/*--------------------------------------------
IMPRIMIR CROMOSOMA CON INDICADORES
--------------------------------------------*/
void printChromosome(Individual* individual)
{
    int i;
    for(i=0; i<chromosome_length; i++){
        if(i== individual->mutation_place) printf("(");
        printf("%d", individual->chromosome[i]);
        if(i == individual->mutation_place ) printf(")");
        if(i == individual->crossover_place) printf("/");
    }
}

/*--------------------------------------------
MOSTRAR LA INFORMACIÓN DE UNA POBLACIÓN
--------------------------------------------*/
void printPopulationDetail(Individual* population)
{
    int i, current_best=0; //agregado para mostrar-> quien es el mejor en esta generación
    double fitness_avg = 0.0; //agregado-> promedio de la aptitud

    printf("\n\n-----------------------------------\n");
    printf("#\tChromosome\tx\tFitness\tParents");
    printf("\n-----------------------------------\n");

    for(i=0; i<POPULATION_SIZE; i++){
        printf("\n%03d  ", i+1);
        printChromosome(&population[i]);
        printf(" %.3f\t%.3f\t(%d,%d)", population[i].x, population[i].fitness,
                                       population[i].parents[0],population[i].parents[1]);
        //-----Agregado para mostrar el promedio-------->
        fitness_avg += population[i].fitness;
        if (population[i].fitness > population[current_best].fitness)
            current_best = i;
        if (population[current_best].fitness > the_best.fitness)
            the_best = population[i];                                       
    }
    fitness_avg /= POPULATION_SIZE;
    printf("\n--------------------------------------------------\n");
    printf("\nAptitud promedio (poblacion): %.3f", fitness_avg );
    printf("\nMejor aptitud: %.3f", population[current_best].fitness );
}




/*--------------------------------------------
FUNCIÓN PRINCIPAL
--------------------------------------------*/
//Preparación
  int main() {
    int op;
    op=1;
    while(op == 1) {
  
    getParameters();
    srand((long)time(NULL)); //semilla para generar aleatorios
    allocateMemory();
    createFirstGeneration();
    evaluatePopulation(parents);
    Individual* temp_helper; //->  corrección
    
    int generation, i; //iteración sobre toda la población
    for(generation=0; generation<max_generations; generation++){
        updateRoulette(parents);
        printPopulationDetail(parents);
        //Proceso de generación
        for(i=0; i< POPULATION_SIZE-1; i+=2 ) {
            selected_father = rouletteWheelSelection();
            selected_mother = rouletteWheelSelection();

            crossover(&parents[selected_father], &parents[selected_mother], &offspring[i], &offspring[i+1]);
            mutation(&offspring[i]);
            mutation(&offspring[i+1]);
            evaluateTargetFunction(&offspring[i]); //->corrección
            evaluateTargetFunction(&offspring[i+1]); //->corrección
        } 
       
        elitism();
        temp_helper = parents;
        parents = offspring;
        offspring = temp_helper;
        printf("\n\n\tTermino con exito generacion %d\n", generation+1);
              
    }
     //Agregamos   
    printf("\n\n*****************************************************************");
    printf("\n\t\t\tEL MEJOR DE TODOS");
    printf("\n*****************************************************************");

    printf ("\n\tCadena binaria: "); printChromosome(&the_best);
    printf("\n\tx = %.f\tFitness = %.3f", the_best.x, the_best.fitness);
    printf("\n\tPadres: (%d, %d)\n", the_best.parents[0], the_best.parents[1]);
    
    free(parents);
    free(offspring);
    free(roulette);
    
    printf("\n---------------------------------------------------------------------------------");
    printf ("\nEscribe 1 para Volver a ejecutar el programa, cualquier numero para salir\n Opcion: ");
    scanf  ("%d", &op); 

    }
    printf("Adios Mundo\n");
    
    return 0;
   
  }

 



