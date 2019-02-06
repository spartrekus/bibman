
//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
#include <stdio.h>
#if defined(__linux__) //linux
#define MYOS 1
#elif defined(_WIN32)
#define MYOS 2
#elif defined(_WIN64)
#define MYOS 3
#elif defined(__unix__) 
#define MYOS 4  // freebsd
#define PATH_MAX 2500
#else
#define MYOS 0
#endif
#include <stdlib.h>
#include <string.h>
//#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>



#include <string.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define ESC "\033"

#define home() 			printf(ESC "[H") //Move cursor to the indicated row, column (origin at 1,1)
#define clrscr()		printf(ESC "[2J") //clear the screen, move to (1,1)
#define gotoxy(x,y)		printf(ESC "[%d;%dH", y, x);
//	gotoxy(2,2 + i);



int  fiche = 0;
int  viewmode = 2;
char ficheref[PATH_MAX]; 
char fichepdffile[PATH_MAX]; 




void nsystem( char *mycmd )
{
   printf( "<SYSTEM>\n" );
   printf( " >> CMD:%s\n", mycmd );
   system( mycmd );
   printf( "</SYSTEM>\n");
}

void editfichier( char *filesource )
{
           char cmdi[PATH_MAX];
           strncpy( cmdi , " vim " , PATH_MAX );
           strncat( cmdi , " " , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi , " \"" , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi ,  filesource , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi , "\" " , PATH_MAX - strlen( cmdi ) -1 );
           nsystem( cmdi ); 
}


void export_refkey( char *mystring )
{
           //nsystem( " vim --servername foo --remote-send \"<Esc>o\\cite{reference2134}<Esc>\"  " );
           char cmdi[PATH_MAX];
           strncpy( cmdi , " vim --servername foo " , PATH_MAX );
           strncat( cmdi , " --remote-send " , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi , " \"<Esc>o\\cite{" , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi ,  mystring , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi , "}<Esc>\" " , PATH_MAX - strlen( cmdi ) -1 );
           nsystem( cmdi ); 
}



void open_pdf( char *mystring )
{
           char cmdi[PATH_MAX];
           strncpy( cmdi , " mupdf ~/pdfs/" , PATH_MAX );
           strncat( cmdi ,  "\""  , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi ,  mystring , PATH_MAX - strlen( cmdi ) -1 );
           strncat( cmdi ,  "\"  "  , PATH_MAX - strlen( cmdi ) -1 );
           nsystem( cmdi ); 
}


///////////////
char* scan_line( char* buffer, int buffer_size) 
{
   char* p = buffer;
   int count = 0;
   do {
       char c;
       scanf("%c", &c); // scan a single character
       // break on end of line, string terminating NUL, or end of file

       if (c == '\r' || c == '\n' || c == 0 || c == EOF) 
       {
           *p = 0;
           break;
       }

       *p++ = c; // add the valid character into the buffer
   } while (count < buffer_size - 1);  // don't overrun the buffer
   // ensure the string is null terminated
   buffer[buffer_size - 1] = 0;
   return buffer;
}



static struct termios oldt;

void restore_terminal_settings(void)
{
    tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
}

void enable_waiting_for_enter(void)
{
    tcsetattr(0, TCSANOW, &oldt);  /* Apply saved settings */
}

void disable_waiting_for_enter(void)
{
    struct termios newt;

    /* Make terminal read 1 char at a time */
    tcgetattr(0, &oldt);  /* Save terminal settings */
    newt = oldt;  /* Init new settings */
    newt.c_lflag &= ~(ICANON | ECHO);  /* Change settings */
    tcsetattr(0, TCSANOW, &newt);  /* Apply settings */
    atexit(restore_terminal_settings); /* Make sure settings will be restored when program ends  */
}













///////////////////////////////////////////
void readfiche( char *filesource , int myitem )
{
   strncpy( ficheref, "" , PATH_MAX );
   strncpy( fichepdffile, "" , PATH_MAX );

   FILE *source; 
   int ch ; 
   char lline[PATH_MAX];
   int pcc = 0; int tcc;  int ucc;  int occ;
   int artcount = 0;
   source = fopen( filesource , "r");
   while( ( ch = fgetc(source) ) != EOF )
   {
         if ( ch != '\n' )
            lline[pcc++]=ch;

         else if ( ch == '\n' ) 
         {
             lline[pcc++]='\0';

             if ( lline[0] == '@' )
             if ( strstr( lline, "{" ) != 0 ) 
                  artcount++;

              if ( lline[0] == '@' )
              if ( artcount == myitem )
              {  
               //  printf( "Article %d\n" , artcount );
                  printf( "==========================\n");
                  printf( "FICHE %d\n" , artcount );
                  printf( "==========================\n");
              }



             if ( lline[0] == '@' )
             if ( artcount == myitem )
             {  
                 tcc = 0;   ucc = 0; occ = 0;
                 strncpy( ficheref, "" , PATH_MAX );
                 while( lline[tcc] != '\0' )
                 {
                    if ( lline[ tcc ] == '{' )
                       occ = 1;
                    else if ( lline[ tcc ] == ',' )
                       occ = 0; 

                    if ( occ == 1 )
                     if ( lline[ tcc ] != '{' )
                       ficheref[ ucc++ ] = lline[ tcc ];

                    tcc++;
                 }
                 ficheref[ ucc ] = '\0';
             }


             if ( artcount == myitem )
             if ( strstr( lline, "pdffile" ) != 0 ) 
             if ( strstr( lline, "{" ) != 0 ) 
             if ( strstr( lline, "}" ) != 0 ) 
             {  
                 tcc = 0;   ucc = 0; occ = 0;
                 strncpy( fichepdffile, "" , PATH_MAX );
                 while( lline[tcc] != '\0' )
                 {
                    if ( lline[ tcc ] == '{' )
                       occ = 1;
                    else if ( lline[ tcc ] == '}' )
                       occ = 0; 

                    if ( occ == 1 )
                     if ( lline[ tcc ] != '=' )
                      if ( lline[ tcc ] != '{' )
                       if ( lline[ tcc ] != ' ' )
                        fichepdffile[ ucc++ ] = lline[ tcc ];

                    tcc++;
                 }
                 fichepdffile[ ucc ] = '\0';
             }


             if ( artcount == myitem )
                printf( "%s\n" , lline );

             if ( lline[0] == '}' )
             if ( artcount == myitem )
             {
               //  printf( "Article %d\n" , artcount );
             }

             lline[0]='\0';
             pcc = 0;
         }
   }
   fclose(source);
}




///////////////////////////////////////////
void readfile( char *filesource )
{
   FILE *source; 
   int ch ; 
   source = fopen( filesource , "r");
   while( ( ch = fgetc(source) ) != EOF )
   {
         printf( "%c", ch );
   }
   fclose(source);
}





void clear_screen()
{
    int fooi;
    struct winsize w; // need ioctl and unistd 
    ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
    for ( fooi = 1 ; fooi <= w.ws_row ; fooi++ ) 
       printf( "\n" );
    home();
}

void size_screen()
{
    struct winsize w; // need ioctl and unistd 
    ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
    printf( "Size %d x %d \n" , w.ws_col , w.ws_row );
}









///////////////////////////////////////////
int readsearch( char *filesource  , char *mystring , int viewit )
{
   int readsearchi;
   FILE *source; 
   int ch ; 
   char lline[PATH_MAX];
   int pcc = 0;
   int artcount = 0;
   source = fopen( filesource , "r");
   while( ( ch = fgetc(source) ) != EOF )
   {

         if ( ch != '\n' )
            lline[pcc++]=ch;

         else if ( ch == '\n' ) 
         {
             lline[pcc++]='\0';

             if ( lline[0] == '@' )
             if ( strstr( lline, "{" ) != 0 ) 
                  artcount++;

              if ( lline[0] == '@' )
              {  
                 //  printf( "Article %d\n" , artcount );
              }

             //if ( artcount == myitem )
             if ( strstr(  lline, mystring ) != 0 )
             {  
                 //  printf( "%s\n" , lline );   
                 readsearchi = artcount;
                 printf( "   => Found %s in %d\n" , mystring, artcount  );
                 if ( viewit == 1 )  readfiche( filesource , artcount ); 
             }

             if ( lline[0] == '}' )
             {
               //  printf( "Article %d\n" , artcount );
             }

             lline[0]='\0';
             pcc = 0;
         }
   }
   fclose(source);
   return readsearchi;
}





int main( int argc, char *argv[])
{
    int key = 0;  int fooi;
    char fichier[PATH_MAX];
    char string[PATH_MAX];
    strncpy( ficheref, "", PATH_MAX);
    strncpy( fichepdffile, "", PATH_MAX);

    struct winsize w; // need ioctl and unistd 
    ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
    printf("Env HOME:  %s\n", getenv( "HOME" ));
    printf("Env PATH:  %s\n", getcwd( string, PATH_MAX ) );
    //printf("Env LINES: %s\n", getenv( "LINES" ));
    //printf("Env COLS:  %s\n", getenv( "COLUMNS" ));
    printf("Env TP ROW:  %d\n", w.ws_row );
    printf("Env TP COL:  %d\n", w.ws_col );


    ///////////////
    if ( argc == 1)
       return 0;


    ///////////////
    if ( argc == 2)
     strncpy( fichier, argv[ 1 ] , PATH_MAX );

    printf("-==========-\n");
    printf("-= BIBMAN =-\n");
    printf("-==========-\n");
    printf("\n");
    printf("FILE: %s\n", fichier);

    int ch;
    disable_waiting_for_enter();
    /* Key reading loop */
    while (1) 
    {
        printf("\n");
        //printf("=> |FICHE:%d|", fiche );
        printf("==>");
        printf(" Press Key:");
        ch = getchar();
        printf( "%c\n", ch );

        if (ch == 'Q') return 0;  /* Press 'Q' to quit program */
        if (ch == 'q') return 0;  /* Press 'Q' to quit program */

        if (ch == '0') 
        {
            fiche = 0;
            printf("   |FICHE:%d|", fiche );
        }

        else if (ch == '+') 
        {
            fiche++;
            printf("   |FICHE:%d|", fiche );
        }
        else if (ch == '-') 
        {
            fiche--;
            printf("   |FICHE:%d|", fiche );
        }

        else if (ch == 'u') 
        {    fiche = fiche -10;
             if ( viewmode == 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode == 3 ) readfiche( fichier , fiche ); 
        }

        else if (ch == 'd') 
        {    fiche = fiche +10;
             if ( viewmode == 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode == 3 ) readfiche( fichier , fiche ); 
        }

        else if (ch == 'j') 
        {    fiche++;  
             if ( viewmode == 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode == 3 ) readfiche( fichier , fiche ); 
        }

        else if (ch == 'k') 
        {    fiche--;  
             if ( viewmode == 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode == 3 ) readfiche( fichier , fiche ); 
        }

        else if (ch == 'p') 
          readfile( fichier );

        else if (ch == 'r') 
        {
           printf( "|Fiche Reference: %s|\n", ficheref );
           printf( "|Fiche Pdf: %s|\n", fichepdffile );
        }

        else if (ch == 'o') 
        {
           printf( "|Fiche Pdf: %s|\n", fichepdffile );
           if ( strcmp( fichepdffile , "" ) != 0 ) open_pdf( fichepdffile );
        }

        else if (ch == 'e') 
        {
           printf( "|Fiche Reference %s|\n", ficheref );
           export_refkey( ficheref );
        }

        else if (ch == 'c') 
          clear_screen();

        else if (ch == 't') 
          size_screen();

        else if (ch == 'l') 
          readfiche( fichier , fiche );

        else if (ch == 'v') 
          editfichier( fichier );

        else if (ch == 'w') 
        {
            if ( viewmode == 1 ) 
               viewmode = 2; 
            else if ( viewmode == 2 ) 
               viewmode = 3; 
            else
               viewmode = 1; 
            printf( "Viewmode = %d\n", viewmode );
        }


        else if ( (ch == '#') || (ch == 'g') )
        {
            enable_waiting_for_enter();
            strncpy( string, "" , PATH_MAX );
            printf("Enter a fiche number: ");
            scan_line( string , PATH_MAX);
            printf("got: \"%s\"\n", string );
            fiche = atoi( string );
            disable_waiting_for_enter();
            if ( viewmode == 2 ) readfiche( fichier , fiche ); 
        }

        else if (ch == 'f') 
        {
            enable_waiting_for_enter();
            strncpy( string, "" , PATH_MAX );
            printf("Enter a string to search : ");
            scan_line( string , PATH_MAX);
            printf("got: \"%s\"\n", string );
            fooi = readsearch( fichier , string , 1 );
            disable_waiting_for_enter();
        }

        else if (ch == '/') 
        {
            enable_waiting_for_enter();
            strncpy( string, "" , PATH_MAX );
            printf("Enter a string to search : ");
            scan_line( string , PATH_MAX);
            printf("got: \"%s\"\n", string );
            fooi = readsearch( fichier , string , 0 );
            disable_waiting_for_enter();
        }

        else if (ch == ':') 
        {
            enable_waiting_for_enter();
            strncpy( string, "" , PATH_MAX );
            printf("Enter a string: ");
            scan_line( string , PATH_MAX);
            printf("got: \"%s\"\n", string );
            disable_waiting_for_enter();
        }

        else 
          printf("You pressed %c\n", ch);


    }

    return 0;
}




