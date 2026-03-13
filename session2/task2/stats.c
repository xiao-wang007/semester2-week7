/* Program to compute summary statistics for numbers in a file */

#include <math.h>
#include <stdio.h>

#define MAX_LENGTH 80
#define MAX_SIZE 10000


int read_data(const char* filename, float data[], int* size)
{
    char line[MAX_LENGTH];
    float value;

    *size = 0;
    FILE* infile = fopen(filename, "r");
    if (infile == NULL) {
        fprintf(stderr, "Error: could not open file '%s'\n", filename);
        return 0;
    }

    while (*size < MAX_SIZE) {
        if (fgets(line, MAX_LENGTH, infile) == NULL) {
            break;
        }

        if (sscanf(line, "%f", &value) != 1) {
            continue;
        }

        if (value < 0.0f) {
            continue;
        }

        data[*size] = value;
        (*size)++;
    }

    fclose(infile);
    return 1;
}


float mean_value(const float data[], int size)
{
    float sum_values = 0.0f;

    for (int i = 0; i < size; ++i) {
        sum_values += data[i];
    }

    return sum_values / size;
}


float standard_deviation(const float data[], int size, float mean)
{
    float sum_squared_diff = 0.0f;

    for (int i = 0; i < size; ++i) {
        float diff_mean = data[i] - mean;
        sum_squared_diff += diff_mean * diff_mean;
    }

    return sqrtf(sum_squared_diff / size);
}


void display_stats(float mean, float std_dev)
{
    printf("Mean    = %.3f\n", mean);
    printf("Std dev = %.3f\n", std_dev);
}


int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: ./stats <filename>\n");
        return 1;
    }

    int size;
    float data[MAX_SIZE];

    if (!read_data(argv[1], data, &size)) {
        return 1;
    }

    if (size == 0) {
        fprintf(stderr, "Error: no valid data\n");
        return 1;
    }

    float mean = mean_value(data, size);
    float std_dev = standard_deviation(data, size, mean);

    display_stats(mean, std_dev);

    return 0;
}
