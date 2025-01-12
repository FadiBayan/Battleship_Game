#include <stdio.h>
#include <stdbool.h>  

bool checkforoverlap(char samir[10][10],int bounds[]) {
    int hb0= bounds[0], hb1=bounds[1], vb0=bounds[2] , vb1= bounds[3];
    for (int i = hb0; i <= hb1; i++) {
        for (int j = vb0; j <= vb1; j++) {
            
            if (samir[i][j] != 0) {
                return false; 
            }
        }
    }
    return true;  
}
int main() {
    // Initialize the 10x10 array with all zeroes
    char samir[10][10] = { 0 };

    // Set some non-zero values to simulate filled positions
    samir[2][3] = 'X';  // Example filled position

    int bounds[] = {1, 3, 2, 4}; 
    if (checkforoverlap(samir, bounds)) {
        printf("No overlap found in the specified region.\n");
    } else {
        printf("Overlap found in the specified region.\n");
    }

    return 0;
}
