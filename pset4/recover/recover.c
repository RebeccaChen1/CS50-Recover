#include <stdio.h>
#include <stdint.h>

#define BYTE1 0xff
#define BYTE2 0xd8
#define BYTE41 0xe0
#define BYTE42 0xef
#define BLOCK 512


int main(int argc, char *argv[])
{
    // opens card.raw file
    FILE *infile = fopen("card.raw", "r");
    // initialize file count
    int fcount = 0;
    // declare outfile
    FILE *outfile = NULL;
    
    // returns 1 if infile cannot be opened
    if (infile == NULL)
    {
        fprintf(stderr, "could not open\n");
        return 1;
    }
    
    // 
    uint8_t buffer[BLOCK];
    
    // iterates over file
    while(fread(buffer, BLOCK, 1, infile))
    {
        
        // creates filename for jpeg if found
        char filename[8];
        
        // checks if first four bytes are jpeg
        if (buffer[0] == BYTE1 && buffer[1] == BYTE2 && buffer[2] == BYTE1 
            && (buffer[3] >= BYTE41 || buffer[3] <= BYTE42))
        {
            // checks if a jpg is already open
            if (outfile != NULL)
            {
                fclose(outfile);
            }
            
            // makes new jpeg
            sprintf(filename, "%03i.jpg", fcount);
            // opens new file to write in
            outfile = fopen(filename, "w");
            // increments file number
            fcount++;
        
        }
        // writes bytes to new file
        if (outfile != NULL)
        {
            fwrite(buffer, BLOCK, 1, outfile);
        }
        
    }
    
    // closes files
    fclose(outfile);
    fclose(infile);
    return 0;
}