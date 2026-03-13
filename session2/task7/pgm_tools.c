/**
 * @file pgm_tools.c
 * @brief Tools for manipulating PGM (Portable Gray Map) image files
 *
 * This code contains 5 distinct bugs, which you will need to find and fix,
 * using the tools & techniques discussed in Week 7.
 *
 * (COMP1850 Week 2.7, Session 2, Task 7)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* Function prototypes */
unsigned char **allocate_image_array(int height, int width);
void free_image_array(unsigned char **image, int height);
unsigned char **read_pgm_image(const char *filename, int *height, int *width, int *max_gray);
void print_image_info(const char *filename, int height, int width, int max_gray);
void print_image_values(unsigned char **pixels, int height, int width);
unsigned char **invert_image_colors(unsigned char **original_image, int height, int width, int max_gray);
unsigned char **rotate_image(unsigned char **original_image, int height, int width, int degrees);
int save_pgm_image(const char *filename, unsigned char **pixels, int height, int width, int max_gray);
int get_user_menu_choice(const char *message);
void display_menu(void);


int main(int argc, char **argv) {
    /* Check command line arguments */
    if (argc != 2) {
        printf("Usage: %s <image_path>\n", argv[0]);
        return 1;
    }

    /* Variables to store image dimensions and max gray value */
    int height, width, max_gray;
    unsigned char **image_pixels;

    /* Read the image */
    image_pixels = read_pgm_image(argv[1], &height, &width, &max_gray);

    /* Check if image reading was successful */
    if (image_pixels == NULL) {
        return 1;
    }

    /* Display image information */
    print_image_info(argv[1], height, width, max_gray);

    int choice = -1;
    char output_filename[100];
    int rotation_degrees;
    unsigned char **processed_image = NULL;

    /* Main program loop */
    do {
        choice = -1;
        display_menu();
        while (choice <= 0)
            choice = get_user_menu_choice("Enter choice");

        switch (choice) {
        case 1: /* View image */
            print_image_values(image_pixels, height, width);
            break;

        case 2: /* Invert image */
            printf("Inverting image colours...\n");
            processed_image = invert_image_colors(image_pixels, height, width, max_gray);

            printf("Enter output filename: ");
            scanf("%99s", output_filename);
            getchar(); /* Consume newline */

            if (save_pgm_image(output_filename, processed_image, height, width, max_gray)) {
                printf("Inverted image saved to %s\n", output_filename);
            } else {
                printf("Failed to save inverted image\n");
            }

            /* Free the processed image memory */
            free_image_array(processed_image, height);
            break;

        case 3: /* Rotate image */
            printf("Enter rotation in degrees (90, 180, or 270): ");
            scanf("%d", &rotation_degrees);
            getchar(); /* Consume newline */

            /* Validate rotation degrees */
            if (rotation_degrees != 90 && rotation_degrees != 180 && rotation_degrees != 270) {
                printf("Invalid rotation angle. Please use 90, 180, or 270 degrees.\n");
                break;
            }

            printf("Rotating image by %d degrees...\n", rotation_degrees);
            processed_image = rotate_image(image_pixels, height, width, rotation_degrees);

            printf("Enter output filename: ");
            scanf("%99s", output_filename);
            getchar(); /* Consume newline */

            /* For 90 and 270 degree rotations, height and width are swapped */
            if (rotation_degrees == 90 || rotation_degrees == 270) {
                if (save_pgm_image(output_filename, processed_image, width, height, max_gray)) {
                    printf("Rotated image saved to %s\n", output_filename);
                } else {
                    printf("Failed to save rotated image\n");
                }
                free_image_array(processed_image, width);
            } else { /* 180 degrees */
                if (save_pgm_image(output_filename, processed_image, height, width, max_gray)) {
                    printf("Rotated image saved to %s\n", output_filename);
                } else {
                    printf("Failed to save rotated image\n");
                }
                free_image_array(processed_image, height);
            }
            break;

        case 4: /* Quit */
            printf("Exiting program...\n");
            /* Free the original image memory before exiting */
            free_image_array(image_pixels, height);
            return 0;

        default:
            printf("Invalid choice, please try again\n");
            break;
        }
    } while (1);
}

/**
 * @brief Gets user menu choice from standard input
 * 
 * @param message The prompt message to display
 * @return int The user's choice, or -1 if invalid input
 */
int get_user_menu_choice(const char *message) {
    char buffer[100];
    int choice;
    printf("%s: ", message);
    fgets(buffer, 100, stdin);
    int check = sscanf(buffer, "%d", &choice);
    if (!check) {
        return -1;
    }
    return choice;
}

/**
 * @brief Displays the main program menu
 */
void display_menu(void) {
    printf("\n----- PGM Image Tool Menu -----\n");
    printf("1 - View PGM Image\n");
    printf("2 - Invert Image\n");
    printf("3 - Rotate Image\n");
    printf("4 - Quit\n");
}

/**
 * @brief Allocates a 2D array for storing the image pixels
 * 
 * @param height Number of rows in the image
 * @param width Number of columns in the image
 * @return unsigned char** 2D array of image pixels, or NULL if allocation fails
 */
unsigned char **allocate_image_array(int height, int width) {
    unsigned char **array = calloc(height, sizeof(unsigned char *));
    if (array == NULL) {
        printf("Error: Memory allocation failed for image array\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        array[i] = calloc(width, sizeof(unsigned char));
        if (array[i] == NULL) {
            /* Free previously allocated memory if this allocation fails */
            for (int j = 0; j < i; j++) {
                free(array[j]);
            }
            free(array);
            printf("Error: Memory allocation failed for image row %d\n", i);
            return NULL;
        }
    }

    return array;
}

/**
 * @brief Frees memory allocated for a 2D image array
 * 
 * @param image The 2D image array to free
 * @param height Number of rows in the image
 */
void free_image_array(unsigned char **image, int height) {
    if (image == NULL) return;

    for (int i = 0; i < height; i++) {
        free(image[i]);
    }
    free(image);
}

/**
 * @brief Reads a PGM image from a file
 * 
 * @param filename Path to the PGM file
 * @param height Pointer to store the image height
 * @param width Pointer to store the image width
 * @param max_gray Pointer to store the max gray value
 * @return unsigned char** 2D array of image pixels, or NULL if reading fails
 */
unsigned char **read_pgm_image(const char *filename, int *height, int *width, int *max_gray) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error: Could not open file %s\n", filename);
        return NULL;
    }

    /* Read PGM header */
    char magic_number[3];
    if (fscanf(file, "%2s", magic_number) != 1) {
        printf("Error: Invalid PGM file format\n");
        fclose(file);
        return NULL;
    }

    if (strcmp(magic_number, "P2") != 0) {
        printf("Error: File is not a valid PGM file (P2 format)\n");
        fclose(file);
        return NULL;
    }

    if (fscanf(file, "%d %d", width, height) != 2) {
        printf("Error: Could not read image dimensions\n");
        fclose(file);
        return NULL;
    }

    if (fscanf(file, "%d", max_gray) != 1) {
        printf("Error: Could not read max gray value\n");
        fclose(file);
        return NULL;
    }

    /* Allocate memory for the pixel array */
    unsigned char **pixels = allocate_image_array(*height, *width);
    if (pixels == NULL) {
        fclose(file);
        return NULL;
    }

    /* Read pixel values */
    int pixel_value;
    for (int i = 0; i < *height; i++) {
        for (int j = 0; j < *width; j++) {
            if (fscanf(file, "%d", &pixel_value) != 1) {
                printf("Error: Could not read pixel value at position (%d,%d)\n", j, i);
                free_image_array(pixels, *height);
                fclose(file);
                return NULL;
            }
            pixels[i][j] = (unsigned char)pixel_value;
        }
    }

    fclose(file);
    return pixels;
}

/**
 * @brief Prints information about the PGM image
 * 
 * @param filename Name of the image file
 * @param height Image height
 * @param width Image width
 * @param max_gray Maximum gray value
 */
void print_image_info(const char *filename, int height, int width, int max_gray) {
    printf("Image: %s\n", filename);
    printf("Dimensions: %d x %d pixels\n", width, height);
    printf("Max Gray Value: %d\n", max_gray);
}

/**
 * @brief Prints the pixel values of the image
 * 
 * @param pixels The 2D array of image pixels
 * @param height Image height
 * @param width Image width
 */
void print_image_values(unsigned char **pixels, int height, int width) {
    printf("\nImage Pixel Values (%dx%d):\n", width, height);
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            printf("%3d ", pixels[i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Inverts the colours of the image (255-value)
 * 
 * @param original_image Original image pixels
 * @param height Image height
 * @param width Image width
 * @param max_gray Maximum gray value
 * @return unsigned char** New image with inverted colours
 */
unsigned char **invert_image_colors(unsigned char **original_image, int height, int width, int max_gray) {
    /* Allocate memory for the inverted image */
    unsigned char **inverted_image = allocate_image_array(height, width);
    if (inverted_image == NULL) {
        return NULL;
    }

    /* Invert each pixel (max_gray - value) */
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            inverted_image[i][j] = max_gray - original_image[i][j];
        }
    }

    return inverted_image;
}

/**
 * @brief Rotates the image by the specified angle
 * 
 * @param original_image Original image pixels
 * @param height Image height
 * @param width Image width
 * @param degrees Rotation angle (90, 180, or 270 degrees)
 * @return unsigned char** New rotated image
 */
unsigned char **rotate_image(unsigned char **original_image, int height, int width, int degrees) {
    unsigned char **rotated_image;

    switch (degrees) {
        case 90:
            /* For 90 degrees, width and height are swapped */
            rotated_image = allocate_image_array(width, height);
            if (rotated_image == NULL) {
                return NULL;
            }

            /* 90 degree rotation algorithm */
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    rotated_image[j][height-1-i] = original_image[i][j];
                }
            }
            break;

        case 180:
            rotated_image = allocate_image_array(height, width);
            if (rotated_image == NULL) {
                return NULL;
            }

            /* 180 degree rotation algorithm */
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    rotated_image[height-1-i][width-1-j] = original_image[i][j];
                }
            }
            break;

        case 270:
            /* For 270 degrees, width and height are swapped */
            rotated_image = allocate_image_array(width, height);
            if (rotated_image == NULL) {
                return NULL;
            }

            /* 270 degree rotation algorithm */
            for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    rotated_image[width-1-j][i] = original_image[i][j];
                }
            }
            break;

        default:
            printf("Error: Invalid rotation angle\n");
            return NULL;
    }

    return rotated_image;
}

/**
 * @brief Saves a PGM image to a file
 * 
 * @param filename Output filename
 * @param pixels Image pixel array
 * @param height Image height
 * @param width Image width
 * @param max_gray Maximum gray value
 * @return int 1 if successful, 0 if failed
 */
int save_pgm_image(const char *filename, unsigned char **pixels, int height, int width, int max_gray) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error: Could not create output file %s\n", filename);
        return 0;
    }

    /* Write PGM header */
    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n", width, height);
    fprintf(file, "%d\n", max_gray);

    /* Write pixel values */
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            fprintf(file, "%d ", pixels[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return 1;
}
