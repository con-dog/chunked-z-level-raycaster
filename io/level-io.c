#include "./level-io.h"

static void process_row(char *line, Jagged_Row *row);

// TODO ! Strip empty final rows if there are any
extern Jagged_Grid *read_grid_csv_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "Could not open file %s\n", filename);
    return NULL;
  }

  Jagged_Grid *grid = malloc(sizeof(Jagged_Grid));
  if (!grid) {
    fclose(file);
    return NULL;
  }

  // First pass: count the number of rows
  grid->length = 0;
  char c;
  while ((c = fgetc(file)) != EOF) {
    if (c == '\n')
      grid->length++;
  }
  if (c != '\n' && grid->length > 0)
    grid->length++; // Handle last line without newline

  grid->rows = malloc(grid->length * sizeof(Jagged_Row));
  if (!grid->rows) {
    free(grid);
    fclose(file);
    return NULL;
  }

  rewind(file);

  char   *line = NULL;
  size_t  len  = 0;
  ssize_t read;
  int     row_index = 0;

  // Read and process each line
  while ((read = getline(&line, &len, file)) != -1 &&
         (size_t)row_index < grid->length) {
    process_row(line, &grid->rows[row_index]);
    row_index++;
  }

  free(line);
  fclose(file);
  return grid;
}

static void process_row(char *line, Jagged_Row *row) {
  int last_content = -1;
  int current_pos  = 0;
  int has_content  = 0;

  char *line_copy = strdup(line);
  char *pos       = line_copy;
  char *token;

  // First pass: find last non-empty position
  while ((token = strsep(&pos, ",")) != NULL) {
    // Trim whitespace and newline
    while (*token && (isspace(*token) || *token == '\n'))
      token++;
    char *end = token + strlen(token);
    while (end > token && (isspace(*(end - 1)) || *(end - 1) == '\n'))
      end--;
    *end = '\0';

    if (*token) {
      has_content  = 1;
      last_content = current_pos;
    }
    current_pos++;
  }
  free(line_copy);

  if (!has_content) {
    row->length             = 0;
    row->world_object_names = NULL;
    return;
  }

  row->length             = last_content + 1;
  row->world_object_names = malloc(row->length * sizeof(Object_Name));
  if (!row->world_object_names) {
    row->length = 0;
    return;
  }

  // Second pass: store strings
  line_copy   = strdup(line);
  pos         = line_copy;
  current_pos = 0;

  while ((token = strsep(&pos, ",")) != NULL &&
         (size_t)current_pos < row->length) {
    while (*token && (isspace(*token) || *token == '\n'))
      token++;
    char *end = token + strlen(token);
    while (end > token && (isspace(*(end - 1)) || *(end - 1) == '\n'))
      end--;
    *end = '\0';

    if (!*token) {
      row->world_object_names[current_pos] = strdup("EMPTY");
    } else {
      row->world_object_names[current_pos] = strdup(token);
    }

    if (!row->world_object_names[current_pos]) {
      for (size_t i = 0; i < (size_t)current_pos; i++) {
        free(row->world_object_names[i]);
      }
      free(row->world_object_names);
      row->world_object_names = NULL;
      row->length             = 0;
      free(line_copy);
      return;
    }
    current_pos++;
  }

  free(line_copy);
}

extern void free_jagged_grid(Jagged_Grid *grid) {
  if (!grid)
    return;

  for (size_t i = 0; i < grid->length; i++) {
    if (grid->rows[i].world_object_names) {
      for (size_t j = 0; j < grid->rows[i].length; j++) {
        free(grid->rows[i].world_object_names[j]);
      }
      free(grid->rows[i].world_object_names);
    }
  }
  free(grid->rows);
  free(grid);
}

extern void print_jagged_grid(const Jagged_Grid *grid) {
  if (!grid) {
    printf("Grid is NULL\n");
    return;
  }

  printf("Grid has %zu rows:\n", grid->length);
  for (size_t i = 0; i < grid->length; i++) {
    printf("Row %zu: length=%zu, elements=", i, grid->rows[i].length);

    if (grid->rows[i].world_object_names) {
      for (size_t j = 0; j < grid->rows[i].length; j++) {
        printf("%s", grid->rows[i].world_object_names[j]);
        if (j < grid->rows[i].length - 1) {
          printf(",");
        }
      }
    }
    printf("\n");
  }
}