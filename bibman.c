

//////////////////////////////////////////
//////////////////////////////////////////
//////////////////////////////////////////
#include <stdio.h>
#define PATH_MAX 2500
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
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <time.h>


#define ESC "\033"
#define home() 			printf(ESC "[H") //Move cursor to the indicated row, column (origin at 1,1)
#define clrscr()		printf(ESC "[2J") //clear the screen, move to (1,1)
#define gotoxy(x,y)		printf(ESC "[%d;%dH", y, x);
//	gotoxy(2,2 + i);


#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

int  fiche = 0;
int  notefiche = 0;
int  viewmode = 3;
int  viewkey = 0;
char refclip[PATH_MAX]; 
char ficheref[PATH_MAX]; 
char pathbefore[PATH_MAX];
char fichepdffile[PATH_MAX]; 



void save_config_file( char *thefile )
{
   FILE *fpout; 
   char foostr[250];
   snprintf( foostr , 250 , "%d",  (int)time(NULL) );
   fpout = fopen( ".bibmanrc" , "ab+");
   fputs( "bibmanrc\n" , fpout );
   fputs( "fichier=" , fpout );
   fputs( thefile , fpout );
   fputs( "\n" , fpout );
   fputs( "time=" , fpout );
   fputs( foostr , fpout );
   fputs( "\n" , fpout );
   fputs( "path=" , fpout );
   fputs( pathbefore , fpout );
   fputs( "\n" , fpout );
   fputs( "edit=" , fpout );
   fputs( " vim " , fpout );
   fputs( "\"" , fpout );
   fputs( thefile , fpout );
   fputs( "\"" , fpout );
   fputs( "\n" , fpout );
   fclose( fpout );
}


///////////////////////////////////////////
void readfile( char *filesource )
{
   FILE *source; 
   int ch ; 
   source = fopen( filesource , "r");
   if ( source == NULL ) { printf( "File not found.\n" ); } else {
   while( ( ch = fgetc(source) ) != EOF )
   {
         printf( "%c", ch );
   }
   fclose(source);
   }
}







void nsystem( char *mycmd )
{
   printf( "<SYSTEM>\n" );
   printf( " >> CMD:%s\n", mycmd );
   system( mycmd );
   printf( "</SYSTEM>\n");
}


void nruncmd( char *filesource , char *cmdapp )
{
           char cmdi[PATH_MAX];
           strncpy( cmdi , "  " , PATH_MAX );
           strncat( cmdi , cmdapp , PATH_MAX - strlen( cmdi ) -1 );
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




void export_clipboard( char *mystring , int themode )
{
           FILE *fpout;
           char file[PATH_MAX];

     if ( themode == 9 ) 
     {  
           strncpy( file , getenv( "HOME" ) , PATH_MAX );
           strncat( file , "/" ,              PATH_MAX - strlen( file ) -1 );
           strncat( file , ".clipboard" ,     PATH_MAX - strlen( file ) -1 );
           readfile( file );
     }

     else if ( themode == 0 ) 
     {   // new file
           strncpy( file , getenv( "HOME" ) , PATH_MAX );
           strncat( file , "/" ,              PATH_MAX - strlen( file ) -1 );
           strncat( file , ".clipboard" ,     PATH_MAX - strlen( file ) -1 );
           fpout = fopen( file , "wb+");
           fclose( fpout );
           strncpy( refclip, "", PATH_MAX );
     }
     else if ( themode == 1 ) 
     {  // copy one citation
           strncpy( file , getenv( "HOME" ) , PATH_MAX );
           strncat( file , "/" ,              PATH_MAX - strlen( file ) -1 );
           strncat( file , ".clipboard" ,     PATH_MAX - strlen( file ) -1 );
           fpout = fopen( file , "wb+");
           fputs( "\\cite{", fpout );
           fputs( mystring  , fpout );
           fputs( "}\n", fpout );
           fclose( fpout );
           strncpy( refclip, "", PATH_MAX );
           strncat( refclip , "\\cite{" , PATH_MAX - strlen( refclip ) -1 );
           strncat( refclip , mystring , PATH_MAX - strlen( refclip ) -1 );
           strncat( refclip , "}" , PATH_MAX - strlen( refclip ) -1 );
     }
     else if ( themode == 2 ) 
     {  // append several references
           strncpy( file , getenv( "HOME" ) , PATH_MAX );
           strncat( file , "/" ,              PATH_MAX - strlen( file ) -1 );
           strncat( file , ".clipboard" ,     PATH_MAX - strlen( file ) -1 );
           fpout = fopen( file , "ab+");
           fputs( mystring  , fpout );
           fputs( ",", fpout );
           fclose( fpout );
           strncat( refclip , mystring , PATH_MAX - strlen( refclip ) -1 );
           strncat( refclip , ", " , PATH_MAX - strlen( refclip ) -1 );
     }
     else if ( themode == 8 ) 
     {     // add cite, to create a new cite index
           strncpy( file , getenv( "HOME" ) , PATH_MAX );
           strncat( file , "/" ,              PATH_MAX - strlen( file ) -1 );
           strncat( file , ".clipboard" ,     PATH_MAX - strlen( file ) -1 );
           fpout = fopen( file , "wb+");
           fputs( "\\cite{", fpout );
           fclose( fpout );
           strncat( refclip , "\\cite{" , PATH_MAX - strlen( refclip ) -1 );
     }
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

   if ( source == NULL ) { printf( "File not found.\n" ); } else {
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
             {
                if ( viewmode == 4 )  
                {
                   if ( strcmp( lline, "" ) != 0 ) 
                      printf( "%s\n" , lline );
                }
                else
                {
                  printf( "%s\n" , lline );
                }
             }

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
}






///////////////////////////////////////////
int readfile_export( char *filesource ) 
{
   int readsearchi;
   FILE *source; 
   int ch ; 
   char lline[PATH_MAX];
   char string[PATH_MAX];
   int pcc = 0;
   int artcount = 0;

   FILE *fpout;
   source = fopen( filesource , "r");
   if ( source == NULL ) { printf( "File not found.\n" ); } else 
   {
   fpout = fopen( "export.bib" , "wb+");
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
                 if ( ( strstr( lline , "nref2019" ) != 0 ) 
                    && ( strstr( lline, "{" ) != 0 ) )
                 {
                   printf( "%s\n" , lline );   
                   fputs( lline , fpout );
                   fputs( "\n" ,  fpout );
                 }
                 else
                 {
                   printf( "Article %d\n" , artcount );
                   snprintf( string , 250 , "%dA0%d",  (int)time(NULL) , artcount );
                     printf( " .Changing... %s\n" , lline );   
                     printf( " .To ... refpub%s\n"     , string );   
                   fputs( "@article{refpub" , fpout );
                   fputs( string , fpout );
                   fputs( "," , fpout );
                   fputs( "\n" ,  fpout );
                 }
             }
             else
             {
                printf( "%s\n" , lline );   
                fputs( lline , fpout );
                fputs( "\n" ,  fpout );
             }

             if ( lline[0] == '}' )
                 printf( "Article %d\n" , artcount );

             lline[0]='\0';
             pcc = 0;
         }
   }
   fclose(fpout);
   fclose(source);
   }
}




///////////////////////////////////////////
int readfilemk( char *filesource ) 
{
   int readsearchi;
   FILE *source; 
   int ch ; 
   char lline[PATH_MAX];
   int pcc = 0;
   int artcount = 0;
   source = fopen( filesource , "r");
   if ( source == NULL ) { printf( "File not found.\n" ); } else {
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
                  printf( "Article %d\n" , artcount );

             printf( "%s\n" , lline );   

             if ( lline[0] == '}' )
                 printf( "Article %d\n" , artcount );

             lline[0]='\0';
             pcc = 0;
         }
   }
   fclose(source);
   }
   return readsearchi;
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
   if ( source == NULL ) { printf( "File not found.\n" ); } else {
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
   }
   return readsearchi;
}





int main( int argc, char *argv[])
{
    ////////////////////////////////////////////////////////
    if ( argc == 2)
    if ( strcmp( argv[1] , "-y" ) ==  0 ) 
    {
       printf("%syellow\n", KYEL);
       return 0;
    }

    ////////////////////////////////////////////////////////
    if ( argc == 2)
    if ( strcmp( argv[1] , "-t" ) ==  0 ) 
    {
       printf("%d\n", (int)time(NULL));
       return 0;
    }

    struct winsize w; // need ioctl and unistd 
    ioctl( STDOUT_FILENO, TIOCGWINSZ, &w );
    char string[PATH_MAX];
    ////////////////////////////////////////////////////////
    if ( argc == 2)
    if ( strcmp( argv[1] , "-s" ) ==  0 ) 
    {
       printf("Screen\n" );
       printf("Env HOME:  %s\n", getenv( "HOME" ));
       printf("Env PATH:  %s\n", getcwd( string, PATH_MAX ) );
       printf("Env TERM ROW:  %d\n", w.ws_row );
       printf("Env TERM COL:  %d\n", w.ws_col );
       return 0;
    }

    printf("Env HOME:  %s\n", getenv( "HOME" ));
    printf("Env PATH:  %s\n", getcwd( string, PATH_MAX ) );
    printf("Env TERM ROW:  %d\n", w.ws_row );
    printf("Env TERM COL:  %d\n", w.ws_col );

    strncpy( refclip, "" , PATH_MAX );
    int key = 0;  int fooi;
    char fichier[PATH_MAX];
    strncpy( pathbefore, getcwd( string, PATH_MAX), PATH_MAX );

    strncpy( ficheref, "", PATH_MAX);
    strncpy( fichepdffile, "", PATH_MAX);


    ///////////////
    if ( argc == 1)
    {
       printf("\n");
       printf("Usage: please enter a Bib file." );
       printf("\n");
       //return 0;
    }


    ///////////////
    if ( argc == 2)
      strncpy( fichier, argv[ 1 ] , PATH_MAX );
    else
      strncpy( fichier, "biblio.bib" , PATH_MAX );

    chdir( pathbefore );
    chdir( getenv( "HOME" ) );
    save_config_file( fichier );
    chdir( pathbefore );

    printf("\n");
    printf("-==========-\n");
    printf("-= BIBMAN =-\n");
    printf("-==========-\n");
    printf("\n");
    printf("FILE: %s\n", fichier);

    int ch;
    disable_waiting_for_enter();
    /* Key reading loop */
    while ( 1 ) 
    {
        if ( viewkey == 1 ) printf( "   You just pressed Key = C%c D%d.\n", ch , ch );

        printf("\n");
        //printf("=> |FICHE:%d|", fiche );
        printf("==>");
        printf(" Press Key:");
        ch = getchar();
        if ( ch == '[' ) 
        {
           printf(" Arrow Key\n");
           ch = getchar(); 
           if      ( ch == 'A' ) ch = 258 ;
           else if ( ch == 'B' ) ch = 259 ;
        }
        printf( "%c\n", ch );

        if (ch == 'Q') return 0;  /* Press 'Q' to quit program */
        if (ch == 'q') return 0;  /* Press 'Q' to quit program */

        if (ch == '0') 
        {
            fiche = 0;
            printf("   |FICHE:%d|", fiche );
        }

        else if (ch == 'g') 
        {
            fiche = 1;
            printf("   |FICHE:%d|", fiche );
        }




        else if (ch == 4 )  // c-d
        {
            notefiche = notefiche -10;
            printf("   |NOTE FICHE:%d|", notefiche );
            snprintf( string , 250 , "note = {not%d}",  notefiche );
            fooi = readsearch( fichier , string , 1 );
            fiche = fooi;
            clear_screen( );
            readfiche( fichier , fiche ); 
        }
        else if (ch == 21 )  //c-u
        {
            notefiche = notefiche + 10;
            printf("   |NOTE FICHE:%d|", notefiche );
            snprintf( string , 250 , "note = {not%d}",  notefiche );
            fooi = readsearch( fichier , string , 1 );
            fiche = fooi;
            clear_screen( );
            readfiche( fichier , fiche ); 
        }
        else if (ch == '+') 
        {
            notefiche++;
            printf("   |NOTE FICHE:%d|", notefiche );
            snprintf( string , 250 , "note = {not%d}",  notefiche );
            fooi = readsearch( fichier , string , 1 );
            fiche = fooi;
            clear_screen( );
            readfiche( fichier , fiche ); 
        }
        else if (ch == '-') 
        {
            notefiche--;
            printf("   |NOTE FICHE:%d|", notefiche );
            snprintf( string , 250 , "note = {not%d}",  notefiche );
            fooi = readsearch( fichier , string , 1 );
            fiche = fooi;
            clear_screen( );
            readfiche( fichier , fiche ); 
        }

        else if (ch == 'u') 
        {    fiche = fiche -10;
             if ( viewmode >= 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode >= 3 ) readfiche( fichier , fiche ); 
        }



        else if (ch == 'd') 
        {    fiche = fiche +10;
             if ( viewmode >= 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode >= 3 ) readfiche( fichier , fiche ); 
        }


        else if ( (ch == 'j') || ( ch == 259 ) ) 
        {    fiche++;  
             if ( viewmode >= 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode >= 3 ) readfiche( fichier , fiche ); 
        }

        else if ( (ch == 'k') || ( ch == 258 ) ) 
        {    fiche--;  
             if ( viewmode >= 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode >= 3 ) readfiche( fichier , fiche ); 
        }

        else if (ch == 32 ) 
        {
             if ( viewmode >= 3 ) clear_screen( );
             if ( viewmode == 2 ) readfiche( fichier , fiche ); 
             if ( viewmode >= 3 ) readfiche( fichier , fiche ); 
        }

        else if (ch == 'p') 
          readfile( fichier );

        else if (ch == 'P') 
          readfilemk( fichier );

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

        else if (ch == 'n') 
        {
           export_clipboard( ficheref , 0 );
           printf( "|=> New to File ~/.clipboard|\n" );
        }

        else if (ch == 'y') 
        {
           printf( "|Fiche Reference %s|\n", ficheref );
           export_clipboard( ficheref , 1 );
           printf( "|=> Export to File ~/.clipboard|\n" );
        }

        else if ( ch == 'x' )
        {
           printf( "|=> View to File ~/.clipboard|\n" );
           export_clipboard( ficheref , 9 );
           printf( "|=> View to File ~/.clipboard|\n" );
        }

        else if (ch == 'a') 
        {
           printf( "|Fiche Reference %s|\n", ficheref );
           if ( strcmp( refclip, "" ) == 0 )   export_clipboard( ficheref , 8 );
           export_clipboard( ficheref , 2 );
           printf( "|=> Append to File ~/.clipboard|\n" );
        }

        else if (ch == 'e') 
        {
            enable_waiting_for_enter();
            strncpy( string, "" , PATH_MAX );
            printf("Enter yes to export : ");
            scan_line( string , PATH_MAX);
            printf("got: \"%s\"\n", string );
            if ( strcmp( string, "yes" ) == 0 ) 
               readfile_export( fichier );
            disable_waiting_for_enter();
        }

        else if (ch == 'E') 
        {
           printf( "|Fiche Reference %s|\n", ficheref );
           export_refkey( ficheref );
        }

        else if (ch == 'c') 
          clear_screen();


        else if ( ch == 'T' )     printf("%syellow\n", KYEL);
        else if ( ch == 't' )     printf("%syellow\n", KYEL);

        else if (ch == 'i') 
        {
          printf("-------------\n");
          printf("- Settings  -\n" );
          printf("-------------\n");
          size_screen();
          printf("-------------\n");
          printf("TIME (d): %d\n", (int)time(NULL));
          snprintf( string , 250 , "%d",  (int)time(NULL) );
          printf("TIME (d): %d\n", (int)time(NULL));
          printf("Time (s): %s\n", string );
          printf("Env HOME:  %s\n", getenv( "HOME" ));
          printf("Env PATH:  %s\n", getcwd( string, PATH_MAX ) );
          printf("Var refclip:  %s\n", refclip );
          printf("-------------\n");
        }

        else if (ch == 'l') 
          readfiche( fichier , fiche );

        else if (ch == 'v') 
          nruncmd( fichier , " vim " );

        else if (ch == 'V') 
          nruncmd( fichier , " nedit " );

        else if (ch == 's')  // like show
        {
            if ( viewmode == 1 ) 
               viewmode = 2; 
            else if ( viewmode == 2 ) 
               viewmode = 3; 
            else if ( viewmode == 3 ) 
               viewmode = 4; 
            else
               viewmode = 1; 
            printf( "Viewmode = %d\n", viewmode );
        }

        else if (ch == 'S')  // like show
        {
            if ( viewkey == 1 ) 
               viewkey = 0; 
            else 
               viewkey = 1; 
            printf( "Viewkey = %d\n", viewkey );
        }

        else if (ch == '#') 
        {
            enable_waiting_for_enter();
            strncpy( string, "" , PATH_MAX );
            printf("Enter a fiche number: ");
            scan_line( string , PATH_MAX);
            printf("got: \"%s\"\n", string );
            printf("=> Set Fiche Number: \"%s\"\n", string );
            fiche = atoi( string );
            disable_waiting_for_enter();
            if ( viewmode == 2 ) 
                 readfiche( fichier , fiche ); 
            if ( viewmode >= 3 ) 
                 readfiche( fichier , fiche ); 
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



        else if (ch == '$') 
        {
            enable_waiting_for_enter();
            strncpy( string, "" , PATH_MAX );
            printf("Enter a sys application: ");
            scan_line( string , PATH_MAX);
            printf("got: \"%s\"\n", string );
            nsystem( string );
            disable_waiting_for_enter();
        }


        else if (ch == '!') 
        {
            enable_waiting_for_enter();
            strncpy( string, "" , PATH_MAX );
            printf("Enter a cmd application: ");
            scan_line( string , PATH_MAX);
            printf("got: \"%s\"\n", string );
            nruncmd( fichier , string );
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




