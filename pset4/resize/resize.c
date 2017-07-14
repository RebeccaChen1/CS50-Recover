/**
 * Copies a BMP piece by piece, just because.
 */
       
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    // ensure proper usage
    if (argc != 4)
    {
        printf("Usage: ./copy n infile outfile\n");
        return 1;
    }
    
    int factor = atoi(argv[1]);
    if (factor < 0 || factor > 100)
    {
        printf("needs to be positive integer less than 100\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];

    // open input file 
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        printf("Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf, new_bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi, new_bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 || 
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    
    new_bf = bf;
    new_bi = bi;
    new_bi.biWidth = bi.biWidth * factor;
    new_bi.biHeight = bi.biHeight * factor;
    
    // determine padding for scanlines
    int padding =  (4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    int newPadding = (4 - (new_bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4;
    
    // new size image 
    new_bi.biSizeImage = ((sizeof(RGBTRIPLE) * new_bi.biWidth) + newPadding)
                         * abs(new_bi.biHeight);
    new_bf.bfSize = new_bi.biSizeImage + sizeof(BITMAPFILEHEADER)
                    + sizeof(BITMAPINFOHEADER);
    
    // write outfile's BITMAPFILEHEADER
    fwrite(&new_bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&new_bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    // loop through input file, store pixels in an array
    // array of bi.biWidth * bi.biHeight 
    // iterate over infile's scanlines (number of rows in infile)
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // copy n - 1 times
        for (int cur_row = 0; cur_row < factor ; cur_row++)
        {
            // iterate over pixels in scanline
            for (int cur_pixel = 0; cur_pixel < bi.biWidth; cur_pixel++)
            {
                // temporary storage
                RGBTRIPLE triple;
                
                // read RGB triple from infile
                fread(&triple, sizeof(RGBTRIPLE), 1, inptr);
     
                for (int cur_outPix = 0; cur_outPix < factor; cur_outPix++)
                {
                    // write RGB triple to outfile
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            
            }
            
            // add padding to outfile
            for (int cur_pad = 0; cur_pad < newPadding; cur_pad++)
            {
                fputc(0x00, outptr);
            }
            
            // checks how many times row was repeated
            if (cur_row < factor - 1)
            {
                fseek(inptr, -bi.biWidth * sizeof(RGBTRIPLE), SEEK_CUR);
            }
            
        }
        
        // skip over padding, if any
        fseek(inptr, padding, SEEK_CUR);
        
    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
