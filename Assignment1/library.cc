#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

/**
 * Populate a random array (which is already
 * allocated with enough memory to hold n bytes.
 */
void random_array(char *array, long bytes)
{
  for(long i = 0; i < bytes; i++)
  {
    array[i] = 'A' + rand() % 26;
  }
}

/**
  * file_ptr : the file pointer, ready to be read from.
  * hist: an array to hold 26 long integers.  hist[0] is the
  *       number of 'A', and hist[1] is the number of 'B', etc.
  * block_size: the buffer size to be used.
  * milliseconds: time it took to complete the file scan
  * total_bytes_read: the amount data in bytes read
  *
  * returns: -1 if there is an error.
  */
int get_histogram(FILE *file_ptr, long hist[], int block_size, long *milliseconds, long *total_bytes_read)
{
  /* Clear values before using them and allocate buffer */
  memset(hist, 0, sizeof(long)*26);
  *milliseconds = 0;
  *total_bytes_read = 0;
  char *buffer = (char *)calloc(1, block_size);
  
  /* Record the start time */
  struct timeb t;
  ftime(&t);
  long start_time = t.time * 1000 + t.millitm;

  /* Keeping reading bytes from file and update the record */
  int i;
  size_t bytes_read = fread(buffer, 1, block_size, file_ptr);
  while(bytes_read == block_size)
  {
    *total_bytes_read += bytes_read;
    for(i=0; i<bytes_read; i++)
    {
      hist[*(buffer+i)-'A']++;
    }
    memset(buffer, 0, block_size);
    bytes_read = fread(buffer, 1, block_size, file_ptr);
  }

  /* An error occured when reading from the file */
  if(bytes_read < block_size && ferror(file_ptr)){
    fprintf(stderr, "Byte write failed");
    return -1;
  }

  /* Last read from the file and update the record */
  *total_bytes_read += bytes_read;
  for(i=0; i<bytes_read; i++)
  {
    hist[*(buffer+i)-'A']++;
  }

  /* Record the end time and record time consumed */
  ftime(&t);
  long end_time = t.time * 1000 + t.millitm;
  *milliseconds = end_time - start_time;
  return 0;
}
