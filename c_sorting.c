#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"c_sorting.h"

/* Array di puntatori a funzione, JUMP TABLE */
void (*doSort[3])(data dt) = {bubbleWrapper,insertionWrapper,selectionWrapper};

int main (int argc, char **argv)
{

/* variabile delegata alla gestione del programma e alla interpretazione
* dei possibili errori che potrebbero essere restituiti */
enum exitval status = 0;

/* Utilizzo questa variabile che passo a chooseSort per capire
 * che tipo di oggetto allocare, se array, matrice o matrice di puntatori */
int typeChoice = 0;

srand(12121);

status = chooseSort(argc,argv,&typeChoice);

int statusCopy = status;

if(status == INPUT_ERROR ||
   status == READ_ERROR  ||
   status == MALLOC_ERROR )
{
        fprintf(stderr,"%s\n", warnmsg[status]);
    return 1;
}

data mydata;
mydata = allocateData(typeChoice);
if(mydata.exitStatus != SUCCESS){
        fprintf(stderr,"%s\n",warnmsg[mydata.exitStatus]);
        return 1;
}
/* Questo indice serve a chiamare la funzione wrapper dell'algoritmo di ordinamento scelto.
 * Il calcolo poco intuitivo è dovuto ad un erorre di progettazione che ho fatto:
 * nella enum exitval ho mappato prima alcune ricorrenti situazione di errore, e solo dopo ho iniziato
 * ad enumerare i vari STATUS del programma... BUBBLE_ARRAY è il primo caso enumerato, per cui va sottratto per
 * operare correttamente la divisione, il cui modulo restituisce correttamente il wraper da chiamare. */
int algID = (status - BUBBLE_ARRAY) / 3;
if (algID < 0 || algID > 2) {
   fprintf(stderr, "ERRORE INTERNO: Indice algID errato (%d). Status: %d\n", algID, status); return 1;
}
doSort[algID](mydata);

status = displayCode(statusCopy);
if(status != SUCCESS){fprintf(stderr,"%s\n",warnmsg[status]);
        return 1;
}
freeData(mydata);

return 0;
}


enum exitval chooseSort(int argc, char** argv,int *typeChoice){
        /* controllo degli argomenti di argc, se l'input non è conforme a quanto
         * richiesto nelle istruzioni si restituisce l'errore */
        if(argc != 3){ return INPUT_ERROR; }

        int found = 0;
        for(unsigned i=0; i<SORTSNAMES; i++){
                if(strcmp(argv[1],sortsNames[i]) != 0){
                                found = 1; // FALSE //
                                continue;
                        }
                        else{
                                found = 0; // TRUE //
                                break;
                        }
                }
        if(found == 1){return INPUT_ERROR;}

        found = 0; // Reset //

        for(unsigned i=0;i<SORTSTYPES;i++){
                if(atoi(argv[2]) != sortsTypes[i]){
                        found=1;
                        continue;
                }
                else{
                        found=0;
                    break;
                }
        }

        if(found == 1){return INPUT_ERROR;}

        /* L'input a questo punto è valido: bisogna solo identificarlo e restituire
         * il valore di ritorno corretto per fare proseguire il programma */

                /* copiamo il nome dell'algortimo scelto dall'utente in questo buffer
                 * temporaneo sort, allochiamo, gestiamo eventuali errori etc... */

                char * sort = malloc(strlen(argv[1])+1);
        if (sort == NULL){ return MALLOC_ERROR; }
        sort[strlen(argv[1])] = '\0';
        strcpy(sort,argv[1]);

        int type = 0;


        if(sscanf(argv[2],"%d",&type)!= 1){
                return READ_ERROR;
        }

        int sortID = 0;
        for(unsigned i=0;i<SORTSNAMES;i++){
                if(strcmp(sort,sortsNames[i]) == 0){
                        sortID = i;
                        break;
                        }
                }

        int typeID = 0;
        for(unsigned i=0;i<SORTSTYPES;i++){
                if(type == sortsTypes[i]){
                        typeID = i;
                        break;
                        }
                }
           /*Salvo il tipo di oggetto da allocare grazie al puntatore che punta
                alla variabile che vive nel nostro main */
                *typeChoice = typeID;

/* In questo switch annidato il programma gestisce sortID, un numero che rappresenta
 * l'indice dell'array di stringhe "sortsNames", che contiene proprio i nomi dei 3 sort.
 * sortID viene impostato nel momento in cui la stringa inserita dall'utente, prima
 * contenuta in argv[1], poi copiata nel buffer temporaneo, corrisponde esattamente ad una
 * delle stringhe dell'array "sortsNames"; questo viene verificato grazie alla funzione
 * strcmp che restituisce 0 proprio quando le due stringhe sono identiche ;
 * Il secondo switch annidato gestisce in maniera del tutto analoga typeID */

        switch(sortID){
                        case 0: // BUBBLE
                   switch(typeID)
                                        {
                                case 0:return BUBBLE_ARRAY; break;
                                                                case 1:return BUBBLE_MATRIX; break;
                                case 2:return BUBBLE_POINTERS_MATRIX; break;
                        }
                                break;
            case 1: // INSERTION
                   switch(typeID)
                                   {
                                case 0:return INSERTION_ARRAY; break;
                                case 1:return INSERTION_MATRIX; break;
                                case 2:return INSERTION_POINTERS_MATRIX; break;
                                   }
                                break;

            case 2: // SELECTION
                   switch(typeID)
                                   {
                                case 0:return SELECTION_ARRAY; break;
                                case 1:return SELECTION_MATRIX; break;
                                case 2:return SELECTION_POINTERS_MATRIX; break;
                   }
                                                        break;
        } // END SWITCH //
}

/* il parametro chosenType rappresenta proprio l'oggetto che il programma deve allocare
 * e riempire con numeri generati in maniera pseudo-casuale.
 * Dentro la struct data è definita la union allocation_t.
 * Grazie a typeID, che abbiamo ottenuto nella precedente funzione chooseSort, possiamo
 * allocare selettivamente l'oggetto richiesto dall'utente e salvarlo all'interno della
 * struct. Quest'ultima contiene anche il parametro exitStatus embeddato al suo interno
 * che serve ad interpretare eventuali errori in cui potrebbe incappare la funzione.
 * Perché exitStatus dentro la struct? Perché ci serve che la funzione restituisca proprio
 * un oggetto struct data, e non un tipo enum exitval come per la precedente funzione. */

data allocateData(int chosenType){
        data ret;
        ret.chosenType = chosenType;
        switch(chosenType){
                case 0:
                        ret.allocation_t.arr = randomizeArray();
                        if(ret.allocation_t.arr == NULL){
                                ret.exitStatus = MALLOC_ERROR;
                        }
                        else{
                                printf("\n\n** Array non ordinato **\n\n");
                                printArray(ret.allocation_t.arr);
                                ret.exitStatus = SUCCESS;}
                        break;
                case 1:
                        ret.allocation_t.mat = randomizeMatrix();
                        if(ret.allocation_t.mat == NULL){
                                ret.exitStatus = MALLOC_ERROR;
                        }
                        else{
                                printf("\n\n** Matrice non ordinata **\n\n");
                                printMatrix(ret.allocation_t.mat);
                                ret.exitStatus = SUCCESS;
                                }
                        break;
                case 2:
                        ret.allocation_t.pmat = randomizePointersMatrix();
                        if(ret.allocation_t.pmat == NULL){
                                ret.exitStatus = MALLOC_ERROR;
                        }
                        else{
                        printf("\n\n** Matrice di puntatori a double non ordinata **\n\n");
                                printPointers(ret.allocation_t.pmat);
                                ret.exitStatus = SUCCESS;
                                }
                        break;
                        }
return ret;
}

double * randomizeArray(void){
        double * ret = malloc(sizeof(*ret) * ARRAYSIZE);
        if(ret == NULL){ return NULL; }
        double max = ARRAYSIZE; double min = ARRAYSIZE-(ARRAYSIZE/2);
        for(unsigned i=0;i<ARRAYSIZE;i++){
                ret[i] = rand()/(double)(RAND_MAX)*(max-min)+min;
        }
        return ret;
}

double** randomizeMatrix(void) {
    double **ret = malloc(sizeof(*ret) * ROWS);
    if (ret == NULL) return NULL;
        double max = ARRAYSIZE;double min = ARRAYSIZE-(ARRAYSIZE/2);

    for(unsigned i=0;i<ROWS;i++){
        ret[i] = malloc(sizeof(double) * COLS);

        if(ret[i] == NULL){ return NULL; }
        for(unsigned j=0;j< COLS;j++) {
            ret[i][j] = rand()/(double)(RAND_MAX)*(max-min)+min;
                        }
                }
        return ret;
}

double*** randomizePointersMatrix(void){
        double*** ret = malloc(sizeof(double **)*ROWS);
        if(ret == NULL) return NULL;
        double max = ARRAYSIZE;double min = ARRAYSIZE-(ARRAYSIZE/2);

        for(unsigned i=0;i<ROWS;i++){
                ret[i] = malloc(sizeof(double *)* COLS);
                        if(ret[i] == NULL){ return NULL;}
                for(unsigned j=0;j<COLS;j++){
                        ret[i][j] = malloc(sizeof(double));
                                if(ret[i][j] == NULL){return NULL;}
                        ret[i][j][0] = rand()/(double)(RAND_MAX)*(max-min)+min;
                }
        }
        return ret;
}
void freeData(data dt) {
    if (dt.exitStatus == MALLOC_ERROR) return; /* Nulla da liberare */

    switch(dt.chosenType) {
        case 0: /* Array */
            free(dt.allocation_t.arr);
            break;
        case 1: /* Matrice */
            for(int i=0; i<ROWS; i++) free(dt.allocation_t.mat[i]);
            free(dt.allocation_t.mat);
            break;
        case 2: /* Matrice Puntatori */
            for(int i=0; i<ROWS; i++) {
                for(int j=0; j<COLS; j++){
                                        free(dt.allocation_t.pmat[i][j]); /* Libero il double */
                                }
                free(dt.allocation_t.pmat[i]);            /* Libero la riga di puntatori */
            }
            free(dt.allocation_t.pmat);
            break;
    }
}
void printArray(double*arr){
        for(unsigned i=0;i<ARRAYSIZE;i++){
                printf("%.3lf ",arr[i]);
       }
}

void printMatrix(double**mat){
        for(unsigned i=0;i<ROWS;i++){
                for(unsigned j=0;j<COLS;j++){
                        printf("%.3lf ",mat[i][j]);
                }
                printf("\n");
        }
}
void printPointers(double***pmat){
        for(unsigned i=0;i<ROWS;i++){
                for(unsigned j=0;j<COLS;j++){
                        printf("%.3lf ",pmat[i][j][0]);
                }
                printf("\n");
        }
}

void bubbleWrapper(data dt){
        switch(dt.chosenType){
                case 0:
                        bubbleArray(dt.allocation_t.arr);
                        printf("\n\n** Array ordinato **\n\n");
                        printArray(dt.allocation_t.arr);
                        break;
                case 1:
                        bubbleMatrix(dt.allocation_t.mat);
                        printf("\n\n** Matrice ordinata **\n\n");
                        printMatrix(dt.allocation_t.mat);
                        break;
                case 2:
                        bubblePointerMatrix(dt.allocation_t.pmat);
                printf("\n\n** Matrice di puntatori a double ordinata **\n\n");
                        printPointers(dt.allocation_t.pmat);
                        break;
        }
}

void insertionWrapper(data dt){
        switch(dt.chosenType){
                case 0:
                        insertionArray(dt.allocation_t.arr);
                        printf("\n\n** Array ordinato **\n\n");
                        printArray(dt.allocation_t.arr);
                        break;
                case 1:
                        insertionMatrix(dt.allocation_t.mat);
                        printf("\n\n** Matrice ordinata **\n\n");
                        printMatrix(dt.allocation_t.mat);
                        break;
                case 2:
                        insertionPointersMatrix(dt.allocation_t.pmat);
                printf("\n\n** Matrice di puntatori a double ordinata **\n\n");
                        printPointers(dt.allocation_t.pmat);
                        break;
        }
}
void selectionWrapper(data dt){
        switch(dt.chosenType){
                case 0:
                        selectionArray(dt.allocation_t.arr);
                        printf("\n\n** Array ordinato **\n\n");
                        printArray(dt.allocation_t.arr);
                        break;
                case 1:
                        selectionMatrix(dt.allocation_t.mat);
                        printf("\n\n** Matrice ordinata **\n\n");
                        printMatrix(dt.allocation_t.mat);
                        break;
                case 2:
                        selectionPointersMatrix(dt.allocation_t.pmat);
                printf("\n\n** Matrice di puntatori a double ordinata **\n\n");
                        printPointers(dt.allocation_t.pmat);
                        break;
        }
}

// [BUBBLE_ARRAY]
void bubbleArray(double *arr){

        /* ciclo per il numero di passate */
        for(unsigned p=1;p<ARRAYSIZE;p++){
        /* ciclo per il numero di confronti ad ogni passata */
                for(size_t i=0;i<ARRAYSIZE-1;i++){
                        if(arr[i]>arr[i+1]){
                                double temp = arr[i];
                                arr[i] = arr[i+1];
                                arr[i+1] = temp;
                        }
                }
        }
}
// [/BUBBLE_ARRAY]

// [BUBBLE_MATRIX]
void bubbleMatrix(double **mat){
        for(unsigned i=0;i<ROWS;i++){
                for(unsigned p=1;p<COLS;p++){
                        for(unsigned j=0;j<COLS-1;j++){
                                if(mat[i][j] > mat[i][j+1]){
                                        double temp = mat[i][j];
                                        mat[i][j] = mat[i][j+1];
                                        mat[i][j+1] = temp;
                                }
                        }
                }
        }

/*===========================================================================================================================*/

/* ORDINAMENTO PER COLONNE */

/*===========================================================================================================================*/

/*
    for(unsigned j=0;j<COLS;j++){
        for(unsigned p=1;p<ROWS;p++){
            for(unsigned i=0;i<ROWS-1;i++){
                if(mat[i][j] > mat[i+1][j]){
                    double temp = mat[i][j];
                    mat[i][j] = mat[i+1][j];
                    mat[i+1][j] = temp;
                }
            }
        }
    }
                                                                                                                                               */
}
// [/BUBBLE_MATRIX]

// [BUBBLE_POINTERS]
void bubblePointerMatrix(double ***pmat){
        for(unsigned i=0;i<ROWS;i++){
                for(unsigned p=1;p<COLS;p++){
                        for(unsigned j=0;j<COLS-1;j++){
                                if(*pmat[i][j]>*pmat[i][j+1]){
                                        double* temp = pmat[i][j];
                                        pmat[i][j] = pmat[i][j+1];
                                        pmat[i][j+1] = temp;
                                }
                        }
                }
        }

/*===========================================================================================================================*/

/* VERSIONE CON SINTASSI [0] */

/*===========================================================================================================================*/

/*
    for(unsigned i=0;i<ROWS;i++){
        for(unsigned p=1;p<COLS;p++){
            for(unsigned j=0;j<COLS-1;j++){
                if(pmat[i][j][0] > pmat[i][j+1][0]){
                    double *temp = pmat[i][j];
                    pmat[i][j] = pmat[i][j+1];
                    pmat[i][j+1] = temp;
                }
            }
        }
    }
                                                                                                                                        */

/*===========================================================================================================================*/

/* ORDINAMENTO PER COLONNE */

/*===========================================================================================================================*/

/*
    for(unsigned j=0;j<COLS;j++){
        for(unsigned p=1;p<ROWS;p++){
            for(unsigned i=0;i<ROWS-1;i++){
                if(*pmat[i][j] > *pmat[i+1][j]){
                    double *temp = pmat[i][j];
                    pmat[i][j] = pmat[i+1][j];
                    pmat[i+1][j] = temp;
                }
            }
        }
    }
                                                                                                                                            */

/*===========================================================================================================================*/

/* ORDINAMENTO PER COLONNE CON SINTASSI [0] */

/*===========================================================================================================================*/

/*
    for(unsigned j=0;j<COLS;j++){
        for(unsigned p=1;p<ROWS;p++){
            for(unsigned i=0;i<ROWS-1;i++){
                if(pmat[i][j][0] > pmat[i+1][j][0]){
                    double *temp = pmat[i][j];
                    pmat[i][j] = pmat[i+1][j];
                    pmat[i+1][j] = temp;
                }
            }
        }
    }
                                                                                                                                            */
}
// [/BUBBLE_POINTERS]

// [INSERTION_ARRAY]
void insertionArray(double *arr){
        int prev; double key;

        for(unsigned i=1;i<ARRAYSIZE;i++){
                 key = arr[i];
                 prev = i-1;
        while(prev>= 0 && arr[prev] > key){
                arr[prev+1] = arr[prev];
/* si scorre a ritroso fino a quando prev non è -1 */
                prev--;
                }
/* se l'indice prev è arrivato a -1, si inserisce l'elemento più piccolo all'indice 0 */
        arr[prev+1] = key;
        }
}
// [/INSERTION_ARRAY]

// [INSERTION_MATRIX]
void insertionMatrix(double **mat) {
        /* ORDINAMENTO PER COLONNE */

    for (int j = 0; j < COLS; j++) {
        for (int i = 1; i < ROWS; i++) {
            double key = mat[i][j];
            int prev = i - 1;
            while (prev >= 0 && mat[prev][j] > key) {
                mat[prev + 1][j] = mat[prev][j];
                prev--;
            }
            mat[prev + 1][j] = key;
        }
    }

/* PRECISAZIONE: Questa versione appena presentata è quella che viene quasi sempre richiesta nei compiti di prog 1.
 * La versione che esegue un ordinamento globale è riportata di seguito */

/*===========================================================================================================================*/

/* PREMESSA: l'ordinamento globale richiede linearità ma la struttura dati double ** è fisicamente discontinua in memoria.
 * Bisogna capire come poter permettere all'algoritmo di funzionare globalmente... */

/*===========================================================================================================================*/

/*
         Scorriamo tutta la matrice cella per cella
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {

            // Salto il primo elemento assoluto (0,0) perché non ha precedenti //
            if (i == 0 && j == 0) continue;
            double key = mat[i][j];
            // Coordinate del "precedente" //
            int prev_r = i;
            int prev_c = j - 1;
                        // Se la colonna va sotto zero, risalgo alla fine della riga sopra //
                if (prev_c < 0) {
                prev_r--;
                prev_c = COLS - 1;
            }

            // Ciclo while: finché ho righe valide E il valore precedente è maggiore della key
            while (prev_r >= 0 && mat[prev_r][prev_c] > key) {

                // Sposto l'elemento "avanti"
                // Calcolo la posizione "successiva" rispetto a p_r, p_c
                int next_r = prev_r;
                int next_c = prev_c + 1;

                // Se 'avanti' esce dalla riga, vado a capo
                if (next_c >= COLS) {
                    next_r++;
                    next_c = 0;
                }

                mat[next_r][next_c] = mat[prev_r][prev_c];

                // Decremento gli indici per guardare ancora più indietro
                prev_c--;
                if (prev_c < 0) {
                    prev_r--;
                    prev_c = COLS - 1;
                }
            }

            // Inserisco la key nel "buco" che ho creato (posizione successiva all'ultimo prev controllato)
            int place_r = prev_r;
            int place_c = prev_c + 1;

            // Gestione wrap-around per l'inserimento
            if (place_c >= COLS) {
                place_r++;
                place_c = 0;
            }

            mat[place_r][place_c] = key;
        }
    }                                                                                                                                          */
}
// [/INSERTION_MATRIX]

// [INSERTION_POINTERS]
void insertionPointersMatrix(double ***pmat) {

/* ORDINAMENTO PER COLONNE */

    for (int j = 0; j < COLS; j++) {
        for (int i = 1; i < ROWS; i++) {
            double *keyPtr = pmat[i][j];
            int prev = i - 1;
            while (prev >= 0 && *pmat[prev][j] > *keyPtr) {
                pmat[prev + 1][j] = pmat[prev][j];
                prev--;
            }
            pmat[prev + 1][j] = keyPtr;
        }
    }

/* PRECISAZIONE: Questa versione appena presentata è quella che viene quasi sempre richiesta nei compiti di prog 1.
 * La versione che esegue un ordinamento globale è riportata dopo l'implementazione con sintassi [0] */

/*===========================================================================================================================*/

/* ORDINAMENTO PER COLONNE CON SINTASSI [0] */

/*===========================================================================================================================*/

/*
    for (int j = 0; j < COLS; j++) {
        for (int i = 1; i < ROWS; i++) {
            double *keyPtr = pmat[i][j];
            int prev = i - 1;
            while (prev >= 0 && pmat[prev][j][0] > keyPtr[0]) {
                pmat[prev + 1][j] = pmat[prev][j];
                prev--;
            }
            pmat[prev + 1][j] = keyPtr;
        }
    }
*/

/*===========================================================================================================================*/

/* PREMESSA: Anche qui, come nel caso della matrice double**, la memoria è frammentata.
 *
 * DIFFERENZA CRUCIALE: Sposteremo gli indirizzi di memoria, non i valori double! */

/*===========================================================================================================================*/
/*
    // Scorriamo tutta la matrice cella per cella //
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {

            // Salto il primo elemento assoluto //
            if (i == 0 && j == 0) continue;

            // SALVO IL PUNTATORE (l'indirizzo), non il valore.
            // Questo è il "tassello" che stiamo cercando di collocare. //
            double *keyPtr = pmat[i][j];

            // Coordinate del "precedente"
            int prev_r = i;
            int prev_c = j - 1;

            // Se la colonna va sotto zero, risalgo alla fine della riga sopra
            if (prev_c < 0) {
                prev_r--;
                prev_c = COLS - 1;
            }

            // Ciclo while:
            // 1. Controllo bounds (prev_r >= 0)
            // 2. CONFRONTO I VALORI: Uso l'asterisco * per vedere i numeri reali (*pmat > *keyPtr)
            while (prev_r >= 0 && *pmat[prev_r][prev_c] > *keyPtr) {

                // Calcolo la posizione "successiva" (dove spostare il puntatore corrente)
                int next_r = prev_r;
                int next_c = prev_c + 1;

                // Gestione wrap-around in avanti
                if (next_c >= COLS) {
                    next_r++;
                    next_c = 0;
                }

                // SPOSTAMENTO: Copio l'INDIRIZZO del puntatore precedente nella posizione successiva.
                // Non uso l'asterisco qui! Sto muovendo le "frecce", non i numeri.
                pmat[next_r][next_c] = pmat[prev_r][prev_c];

                // Decremento gli indici per guardare ancora più indietro
                prev_c--;
                if (prev_c < 0) {
                    prev_r--;
                    prev_c = COLS - 1;
                }
            }

            // Calcolo la posizione finale dove inserire la key
            int place_r = prev_r;
            int place_c = prev_c + 1;

            // Gestione wrap-around per l'inserimento
            if (place_c >= COLS) {
                place_r++;
                place_c = 0;
            }

            // INSERIMENTO: Rimetto il puntatore keyPtr al suo posto ordinato.
            // Anche qui, senza asterisco.
            pmat[place_r][place_c] = keyPtr;
        }
    }
                                                                                                                                               */
}
// [/INSERTION_POINTERS]

// [SELECTION_ARRAY]
void selectionArray(double *arr){

        for(unsigned i=0;i<ARRAYSIZE-1;i++){

                unsigned smallest = i;

                for(unsigned j=i+1;j<ARRAYSIZE;j++){
                        if(arr[j]<arr[smallest]){
                                smallest = j;
                                }
                        }
                double temp = arr[i];
                arr[i] = arr[smallest];
                arr[smallest] = temp;
        }
}
// [/SELECTION_ARRAY]

// [SELECTION_MATRIX]
void selectionMatrix(double **mat){

        for(unsigned i=0;i<ROWS;i++){
                for(unsigned j=0;j<COLS;j++){

                        unsigned min_r = i;             /* si assume che l'elemento più piccolo sia il corrente */
                        unsigned min_c = j;             /* ne salviamo la posizione con due coordinate */

        /* L'indice delle righe nel ciclo interno che scorre la parte restante della matrice riparte proprio dall'indice i.
         * La ricerca del minimo non deve ripartire da 0!
         * Gli elementi della matrice che precedono quello corrente sono stati già correttamente ordinati.
         * Ripartire da 0 sarebbe un errore gravissimo...*/

                        for(unsigned r=i;r<ROWS;r++){

        /* Per le colonne si pone un problema che ci porte di fronte a due possibilità:
        1) Se siamo scorrende la riga corrente (r==i) la ricerca parte da destra (j+1)...
        2) Se stiamo scorrendo le righe successsive (r != i), invece, bisogna ripartire dall'inizio (0) per controllare
        *  correttamente tutti gli elementi presenti nella parte ancora non ordinata delle matrice */

                                int c_start = (r==i ? j+1 : 0);

                                for(unsigned c=c_start;c<COLS;c++){
                                        if(mat[r][c]<mat[min_r][min_c]){
                                                min_r = r;
                                                min_c = c;
                                                }
                                        }
                                }
                double temp = mat[i][j];
                mat[i][j] = mat[min_r][min_c];
                mat[min_r][min_c] = temp;

                }
        }
/*===========================================================================================================================*/

/* ORDINAMENTO PER COLONNE */

/*===========================================================================================================================*/
/*
    for(unsigned j=0;j<COLS;j++){
        for(unsigned i=0;i<ROWS;i++){
            unsigned min = i;
            for(unsigned k=i+1;k<ROWS;k++){
                if(mat[k][j]<mat[min][j]){
                    min = k;
                }
            }
            double temp = mat[i][j];
            mat[i][j] = mat[min][j];
            mat[min][j] = temp;
        }
    }
                                                                                                                         */
}
// [/SELECTION_MATRIX]

// [SELECTION_POINTERS]
void selectionPointersMatrix(double ***pmat){

        for(unsigned i=0;i<ROWS;i++){
                for(unsigned j=0;j<COLS;j++){
                        unsigned min_r = i;             /* si assume che l'elemento più piccolo sia il corrente */
                        unsigned min_c = j;             /* ne salviamo la posizione con due coordinate */
        for(unsigned r=i;r<ROWS;r++){
                unsigned c_start = (r==i ? j+1 : 0);

                for(unsigned c = c_start;c<COLS;c++){
                        if(*pmat[r][c]<*pmat[min_r][min_c]){
                                        min_r = r;
                                        min_c = c;
                                        }
                                }
                        }

        /* Swap dei puntatori, non dei valori */

                double * temp = pmat[i][j];
                pmat[i][j] = pmat[min_r][min_c];
                pmat[min_r][min_c] = temp;
        }
}

/*===========================================================================================================================*/

/* VERSIONE CON SINTASSI [0] */

/*===========================================================================================================================*/
/*
    for(unsigned i=0;i<ROWS;i++){
        for(unsigned j=0;j<COLS;j++){
            unsigned min_r = i;
            unsigned min_c = j;
            for(unsigned r=i;r<ROWS;r++){
                unsigned c_start = (r==i ? j+1 : 0);
                for(unsigned c = c_start;c<COLS;c++){
                    if(pmat[r][c][0] < pmat[min_r][min_c][0]){
                        min_r = r;
                        min_c = c;
                    }
                }
            }
            double * temp = pmat[i][j];
            pmat[i][j] = pmat[min_r][min_c];
            pmat[min_r][min_c] = temp;
        }
    }
                                                                                                                                */

/*===========================================================================================================================*/

/* ORDINAMENTO PER COLONNE */

/*===========================================================================================================================*/

/*
    for(unsigned j=0;j<COLS;j++){
        for(unsigned i=0;i<ROWS;i++){
            unsigned min = i;
            for(unsigned k=i+1;k<ROWS;k++){
                if(*pmat[k][j]<*pmat[min][j]){
                    min = k;
                }
            }
            double * temp = pmat[i][j];
            pmat[i][j] = pmat[min][j];
            pmat[min][j] = temp;
        }
    }
                                                                                                                                */

/*===========================================================================================================================*/

/* ORDINAMENTO PER COLONNE CON SINTASSI [0] */

/*===========================================================================================================================*/

/*
    for(unsigned j=0;j<COLS;j++){
        for(unsigned i=0;i<ROWS;i++){
            unsigned min = i;
            for(unsigned k=i+1;k<ROWS;k++){
                if(pmat[k][j][0] < pmat[min][j][0]){
                    min = k;
                }
            }
            double * temp = pmat[i][j];
            pmat[i][j] = pmat[min][j];
            pmat[min][j] = temp;
        }
    }
                                                                                                                                          */
}
// [/SELECTION_POINTERS]
enum exitval displayCode(int status){
    FILE * fp = fopen(__FILE__,"r");
    if(fp == NULL){ return READ_ERROR;}

    int index = status - BUBBLE_ARRAY;
    char line[500];
    char startTag[50];
    char endTag[50];
    int  print = 0;

    snprintf(startTag,50,"[%s]",tags[index]);
    snprintf(endTag,50,"[/%s]",tags[index]);

    /* --- MODIFICA STILE HEADER --- */
    printf("\n\n/*===========================================================================================================================*/\n");
    fprintf(stdout,"\t\t>>> %s! <<<", warnmsg[status]);
    printf("\n/*===========================================================================================================================*/\n");
    /* ----------------------------- */
        fprintf(stdout,"\nCodice sorgente...\n\n");
    while(fgets(line,sizeof(line),fp) != NULL){

        /* end tag trovato, bisogna smettere di stampare */
        if(strstr(line,endTag) != NULL){
            print = 0;
            break;
        }
        if(print == 1){
            fprintf(stdout,"%s",line);
        }
        if(strstr(line,startTag) != NULL){
            print = 1;
        }
    }

    fclose(fp);
    return SUCCESS;
}
