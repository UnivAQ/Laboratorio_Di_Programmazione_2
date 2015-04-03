/**
* fopen(), fprintf(), fscanf(), fgets(), fgetc(), FILE, stdout, stderr
*
* @include stdio.h
**/
#include <stdio.h>

/**
* calloc(), exit(), EXIT_FAILURE
*
* @include stdlib.h
**/
#include <stdlib.h>

/**
* strncmp()
*
* @include string.h
**/
#include <string.h>

#define true 1  /* Boolean value true */
#define false 0 /* Boolean value false */

/**
* Boolean type emulation
*
* @typedef short bool
**/
typedef short bool ;

/**
* Error raising interface
*
* @param char *     Message to print on the stderr
* @param char *     String parameter for the message
* @return void
**/
void raiseError(char *, char *) ;

/**
* Checks the file format and
* returns a vector containing the records
*
* @param char *     Path to the file
* @param int *      Returned vector length
* @return char **   Records container
**/
char ** checkArgument(char *, int *) ;

/**
* Counts the distinct vowels of a string
*
* @param char *     String to analyze
* @return int       Vowels number
**/
int countVowels(char *) ;

/**
* Checks if a string has more distinct vowels of a number
*
* @param int        Vowels number to confront with the string
* @param char *     String to confront with the vowels number
* @param int *      Cache vector
* @param int        Cache index
* @return bool      Check result
**/
bool isLessThan(int, char *, int *, int) ;

int main (int argv, char * args[])
{
    char ** vector1 ;   /* Container of the records of the first file */
    char ** vector2 ;   /* Container of the records of the second file */
    int vector1Length ; /* Number of records in the first container */
    int vector2Length ; /* Number of records in the second container */
    int i, ii, iii ;    /* Temp counters used for the iterations */
    bool status = true ;/* Status of the check of the iteration */
    char * message ;    /* Message to print out after the check */
    int * cache ;       /* Vowels cache for vector2 */

    /* It needs al least two parameters from the command line */
    if (argv < 3) {
        /* args[0] is the path of the executable */
        fprintf(stdout, "Usage: %s 'path/to/file1' 'path/to/file2'\n", args[0]) ;
        return -1 ;
    }

    vector1 =(char **) checkArgument(args[1], &vector1Length) ;
    vector2 =(char **) checkArgument(args[2], &vector2Length) ;

    /* It allocates space dynamically for the cache */
    cache =(int *) calloc(vector2Length, sizeof(int)) ;

    /*
    * It checks if every string of vector1
    * has less distinct vowels of every string of vector2
    */
    for (i = 0; i < vector1Length && status == true; ++i) {
        iii = countVowels(vector1[i]) ;
        for (ii = 0; ii < vector2Length && status == true; ++ii)
            status = isLessThan(iii, vector2[ii], cache, ii) ;
    }

    i = 1 ;
    message = "false" ;
    if (status == true) {
        i = 2 ;
        message = "true" ;
    }

    /* The result of the program is shown on the stdout */
    fprintf(stdout, "%s\n", message) ;
    /* fflush(stdout) ; */

    return i ;
}

/**
* Error raising interface
*
* @param char *     Message to print on the stderr
* @param char *     String parameter for the message
* @return void
**/
void raiseError(char * message, char * parameter)
{
    fprintf(stderr, message, parameter) ; /* It uses the stderr to output the error message */
    exit(EXIT_FAILURE) ; /* EXIT_FAILURE is the standard exit status in case of error */
}

/**
* Checks the file format and
* returns a vector containing the records
*
* @param char *     Path to the file
* @param int *      Returned vector length
* @return char **   Records container
**/
char ** checkArgument(char * arg, int * rowsNumber)
{
    FILE * fileStream ; /* Stream of the file */
    char fileHeader[8] ;/* File header container */
    char ** vector ;    /* Container of the records of the file */

    int i, ii, iii ;    /* Temp counters used for the iterations */
    char c ;            /* Temp character used during the read of the file */

    /* The file is opened in read and binary mode */
    fileStream = fopen(arg, "rb") ;
    /* The path to the file maybe wrong or not readable */
    if (fileStream == NULL)
        raiseError("Errore: Il file '%s' non esiste o non è leggebile.\n", arg) ;

    /* It loads the first seven characters of the file used as header */
    fgets(fileHeader, 8, fileStream) ;
    /* The file header must be a specific one */
    if (strncmp(fileHeader, "Prj2_08\0", 7) != 0)
        raiseError("Errore: [ %s ] Il file deve iniziare con la stringa 'Prj2_08'.\n", arg) ;

    c = fgetc(fileStream) ;
    /* The file header must be followed by a space or a new line */
    if (c != ' ' && c != '\n' )
        raiseError("Errore: [ %s ] La sringa 'Prj2_08' deve essere seguita da un carattere di spazio o new line (unix).\n", arg) ;

    /* The file header must be followed by an integer */
    fscanf(fileStream, "%i", rowsNumber);
    /* The number must me be greater than zero */
    if (*rowsNumber < 1)
        raiseError("Errore: [ %s ] Il numero di stringhe deve essere > 0.\n", arg) ;

    /* The number must me be followed only by spaces and two new line */
    for (i = 0; i < 2; ++i)
        while ((c = fgetc(fileStream)) != '\n')
            if (c != ' ')
                raiseError("Errore: [ %s ] Dopo il numero di stringhe sono ammessi solo spazi e due new line.\n", arg) ;

    /* It allocates space dynamically to contain the records */
    vector =(char **) calloc(*rowsNumber, sizeof(char *)) ;
    /* It reads the file until the end */
    for (i = 0; (c = fgetc(fileStream)) != EOF; ++i) {
        /* It avoids to go out from the allocated space */
        if (*rowsNumber < i+1)
            raiseError("Errore: [ %s ] Il file contiene più righe di quelle specificate.\n", arg) ;
        ii = 0 ; /* Chars counter */
        iii = 0 ; /* Number of spaces after a char */
        fseek(fileStream, -1, SEEK_CUR) ;
        /* It counts the record length and checks for inner spaces */
        while ((c = fgetc(fileStream)) != '\n') {
            if (c != ' ' && iii > 0)
                raiseError("Errore: [ %s ] Le stringhe non possono contenere spazi al loro interno.\n", arg) ;
            else if (c != ' ')
                ++ii ;
            else if (ii > 0)
                ++iii ;
        }
        /* It restores the stream pointer to the start of the record */
        fseek(fileStream, -(ii+iii+1), SEEK_CUR) ;
        /* It allocates space dynamically to contain the string */
        vector[i] =(char *) calloc(ii, sizeof(char));
        /* It reads a string and checks if the line is empty */
        if (fscanf(fileStream, "%s", vector[i]) < 1)
            raiseError("Errore: [ %s ] Le righe non possono essere vuote.\n", arg) ;
        /* It places the the stream pointer to the next line */
        fseek(fileStream, iii+1, SEEK_CUR) ;
    }
    /* The file must have exactly the specified number of strings */
    if (*rowsNumber > i)
        raiseError("Errore: [ %s ] Il file contiene meno righe di quelle specificate.\n", arg) ;

    /* It closes the stream */
    fclose(fileStream) ;
    return vector ;
}

/**
* Counts the distinct vowels of a string
*
* @param char *     String to analyze
* @return int       Vowels number
**/
int countVowels(char * string)
{
    int vowelA = 0, vowelE = 0, vowelI = 0, vowelO = 0, vowelU = 0 ;
    int counter = 0 ;
    while (*string != '\0') {
        if (vowelA == 0 && (*string == 'a' || *string == 'A')) {
            ++vowelA ; ++counter ;
        }
        else if (vowelE == 0 && (*string == 'e' || *string == 'E')) {
            ++vowelE ; ++counter ;
        }
        else if (vowelI == 0 && (*string == 'i' || *string == 'I')) {
            ++vowelI ; ++counter ;
        }
        else if (vowelO == 0 && (*string == 'o' || *string == 'O')) {
            ++vowelO ; ++counter ;
        }
        else if (vowelU == 0 && (*string == 'u' || *string == 'U')) {
            ++vowelU ; ++counter ;
        }
        ++string ;
    }
    return counter ;
}

/**
* Checks if a string has more distinct vowels of a number
*
* @param int        Vowels number to confront with the string
* @param char *     String to confront with the vowels number
* @param int *      Cache vector
* @param int        Cache index
* @return bool      Check result
**/
bool isLessThan(int vowelsNumber, char * string, int * cache, int index)
{
    int vNumber ;
    /*
    * If the cache contains the number of distinct vowels
    * there is no need to recalculate it
    */
    if (cache[index] == 0)
        cache[index] = countVowels(string) ;
    vNumber = cache[index] ;
    if (vowelsNumber < vNumber)
        return true ;
    return false ;
}
