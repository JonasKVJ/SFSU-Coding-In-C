// Name: Jonas Vinter-Jensen
// CSC 415 [01] | Fall-17
// San Francisco State University
// HW6 : CustomCP

#include <fcntl.h> //open(), read()
#include <stdio.h>
#include <stdlib.h> //exit()
#include <string.h> //strlen()
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> //write(), close()

int main(int argc, const char * argv[])
{
    char buffer[1024];
    int file_descriptor_read = -1;
    int file_descriptor_write = -1;
    size_t bytes_read = -1;
    size_t bytes_written = -1;
    
    //char* will make the filename1 read-only in memory
    const char* filename1;
    const char* filename2;
    
    if(argc != 3) //accept two arguments
    {
        printf("Expected format: ./mycp filename1 filename2\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        filename1 = argv[1];
        filename2 = argv[2];
    }
    
    /*Open filename1 in read-only mode and file_descriptor is a small integer which represents
     a reference to filename1*/
    file_descriptor_read = open(filename1, O_RDONLY);
    if(file_descriptor_read == -1)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    
    /*Create filename2 with the following properties --
     WRONLY: records write-only in the open file description,
     O_CREAT: Create the new file if it does not exist
     O_EXCL: If filename2 already exists, open() will fail and throw an error
     mode: S_IRWXU, the following access to filename2 will give the user read, write and execute permission to it
     */
    file_descriptor_write = open(filename2, O_WRONLY|O_CREAT|O_EXCL, S_IRWXU);
    if(file_descriptor_write == -1)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }
    
    //Reads the first byte of filename1 into buffer
    bytes_read = read(file_descriptor_read, buffer, 1);
    
    //One byte at a time, this loop while read a character from filename1 and write it to
    //filename2
    while(bytes_read != 0)
    {
        if(bytes_read == -1)
        {
            perror("Error reading file");
            exit(EXIT_FAILURE);
        }
        else
        {
            /*Writes however many bytes was read into the newly created file, which should be
             exactly one byte per iteration
             */
            bytes_written += write( file_descriptor_write, buffer, bytes_read );
            if(bytes_written == -1)
            {
                perror("Error writing file");
                exit(EXIT_FAILURE);
            }
            
            /*bytes_read is 0 when EOF is reached and each subsequent read from filename1
             into buffer will overwrite the previous byte which was read into buffer
             */
            bytes_read = read(file_descriptor_read, buffer, 1);
        }
    }

    //close filename1
    if( close(file_descriptor_read) == -1)
    {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
    
    //close newly created copy of filename1
    if( close(file_descriptor_write) == -1)
    {
        perror("Error closing file");
        exit(EXIT_FAILURE);
    }
    
    printf("copied %zu bytes\n", bytes_written); //zu: unsigned long for size_t datatype

    return 0;
}
