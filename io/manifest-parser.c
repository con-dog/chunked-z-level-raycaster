#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Main function to read the file
extern char *read_asset_manifest_file(const char *filename)
{
  FILE *file = fopen(filename, "r");
  if (!file)
  {
    fprintf(stderr, "Could not open file %s\n", filename);
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *buffer = malloc(file_size + 1); // extra for null terminator
  if (!buffer)
  {
    fprintf(stderr, "Memory allocation failed for manifest\n");
    fclose(file);
    return NULL;
  }

  size_t read_size = fread(buffer, 1, file_size, file);
  if (read_size != file_size)
  {
    fprintf(stderr, "Failed to read file %s\n", filename);
    free(buffer);
    fclose(file);
    return NULL;
  }

  buffer[file_size] = '\0';
  fclose(file);
  return buffer;
}
