#include <stdio.h>
#include <string.h>

int main() {
    char input[500];
    int consecutive_enters = 0;

    printf("--- Sentence Divider Program ---\n");
    printf("Type your sentences below. Press [ENTER] twice to exit.\n\n");

    while (1) {
        // Prompt for input
        printf("> ");
        
        // Read input including spaces
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break; 
        }

        // Check if the input is just a newline character (Enter key)
        if (strcmp(input, "\n") == 0) {
            consecutive_enters++;
        } else {
            consecutive_enters = 0; // Reset if text is typed
            
            // Visual Divider
            printf("--------------------------------------------------\n");
        }

        // Exit condition
        if (consecutive_enters >= 1) {
            printf("\nExiting program...\n");
            break;
        }
    }

    return 0;
}