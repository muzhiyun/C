#include <stdio.h>
#include <stdlib.h>



                                         
int  main()                                                   
{                                                                 
    printf("Content-type: text/html\n\n");                        
    printf("<html>\n");                                            
    printf("<head>\n");                                            
    printf("<title>CGI Output</title>\n");                         
    printf("</head>\n");                                           
 
    printf("<body>");                                             
    printf("<h1> Hellow, world. </h1>"); 
	printf("%s",getenv("QUERY_STRING"));	
    printf("</body>");                                           
    printf("</html>\n");                                         
    return 0;                                                    
}
