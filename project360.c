
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
 
#define MAX_TEXT_LEN   5000
#define MAX_SENTENCES  300
#define MAX_SENT_LEN   600
 
typedef enum {
    DECLARATIVE,
    EXCLAMATORY,
    INTERROGATIVE
} SentenceType;
 
typedef struct {
    char         text[MAX_SENT_LEN];
    SentenceType type;
    int          word_count;
    int          char_count;
} Sentence;
 
static const char *ABBREVS[] = {
    "Mr","Mrs","Ms","Dr","Prof","Sr","Jr","St",
    "Ave","Blvd","Dept","vs","etc","Jan","Feb",
    "Mar","Apr","Jun","Jul","Aug","Sep","Oct",
    "Nov","Dec","Fig","No","Vol", NULL
};
 
void trim(char *s) {
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;
    int len = (int)strlen(start);
    while (len > 0 && isspace((unsigned char)start[len-1])) len--;
    memmove(s, start, len);
    s[len] = '\0';
}
 
int count_words(const char *s) {
    int count = 0, in_word = 0;
    while (*s) {
        if (isspace((unsigned char)*s)) in_word = 0;
        else if (!in_word) { in_word = 1; count++; }
        s++;
    }
    return count;
}
 
int count_chars(const char *s) {
    int count = 0;
    while (*s) { if (!isspace((unsigned char)*s)) count++; s++; }
    return count;
}
 
int is_abbreviation(const char *text, int i) {
    int start = i - 1;
    while (start > 0 && !isspace((unsigned char)text[start-1])) start--;
    int wlen = i - start;
    if (wlen <= 0 || wlen > 20) return 0;
    char word[21];
    strncpy(word, text + start, wlen);
    word[wlen] = '\0';
    for (int k = 0; ABBREVS[k] != NULL; k++) {
        const char *a = word, *b = ABBREVS[k];
        int match = 1;
        while (*a && *b) {
            if (tolower((unsigned char)*a) != tolower((unsigned char)*b))
                { match = 0; break; }
            a++; b++;
        }
        if (match && *a == '\0' && *b == '\0') return 1;
    }
    return 0;
}
 
int divide_sentences(const char *text, Sentence sentences[], int max) {
    int count = 0, len = (int)strlen(text), start = 0;
    for (int i = 0; i < len && count < max; i++) {
        char ch = text[i];
        if (ch == '.' || ch == '!' || ch == '?') {
            if (ch == '.' && is_abbreviation(text, i)) continue;
            if (ch == '.' && i+1 < len && text[i+1] == '.') continue;
            int seg_len = i - start + 1;
            if (seg_len <= 0) { start = i+1; continue; }
            if (seg_len >= MAX_SENT_LEN) seg_len = MAX_SENT_LEN - 1;
            strncpy(sentences[count].text, text + start, seg_len);
            sentences[count].text[seg_len] = '\0';
            trim(sentences[count].text);
            if (strlen(sentences[count].text) == 0) { start = i+1; continue; }
            if      (ch == '!') sentences[count].type = EXCLAMATORY;
            else if (ch == '?') sentences[count].type = INTERROGATIVE;
            else                sentences[count].type = DECLARATIVE;
            sentences[count].word_count = count_words(sentences[count].text);
            sentences[count].char_count = count_chars(sentences[count].text);
            count++;
            start = i + 1;
            while (start < len && isspace((unsigned char)text[start])) start++;
            i = start - 1;
        }
    }
    if (start < len && count < max) {
        char tail[MAX_SENT_LEN];
        int tlen = len - start;
        if (tlen >= MAX_SENT_LEN) tlen = MAX_SENT_LEN - 1;
        strncpy(tail, text + start, tlen);
        tail[tlen] = '\0';
        trim(tail);
        if (strlen(tail) > 0) {
            strncpy(sentences[count].text, tail, MAX_SENT_LEN - 1);
            sentences[count].type       = DECLARATIVE;
            sentences[count].word_count = count_words(tail);
            sentences[count].char_count = count_chars(tail);
            count++;
        }
    }
    return count;
}
 
void print_line(char ch, int width) {
    for (int i = 0; i < width; i++) putchar(ch);
    putchar('\n');
}
 
void show_results(const Sentence sentences[], int count) {
    int total_words = 0, total_chars = 0;
    int decl = 0, excl = 0, inter = 0;
    int longest = 0, shortest = 0;
    for (int i = 0; i < count; i++) {
        total_words += sentences[i].word_count;
        total_chars += sentences[i].char_count;
        if (sentences[i].type == DECLARATIVE)   decl++;
        if (sentences[i].type == EXCLAMATORY)   excl++;
        if (sentences[i].type == INTERROGATIVE) inter++;
        if (sentences[i].word_count > sentences[longest].word_count)  longest = i;
        if (sentences[i].word_count < sentences[shortest].word_count) shortest = i;
    }
    printf("\n");
    print_line('=', 62);
    printf("        SENTENCE DIVIDER SIMULATOR  --  RESULTS\n");
    print_line('=', 62);
    for (int i = 0; i < count; i++) {
        const char *type_name, *symbol;
        switch (sentences[i].type) {
            case DECLARATIVE:   type_name = "Declarative";   symbol = "[.]"; break;
            case EXCLAMATORY:   type_name = "Exclamatory";   symbol = "[!]"; break;
            case INTERROGATIVE: type_name = "Interrogative"; symbol = "[?]"; break;
            default:            type_name = "Unknown";       symbol = "[ ]"; break;
        }
        printf("\n  Sentence #%-3d  %s  %s\n", i+1, symbol, type_name);
        print_line('-', 62);
        printf("  \"%s\"\n", sentences[i].text);
        printf("  Words: %-5d  Characters (no spaces): %d\n",
               sentences[i].word_count, sentences[i].char_count);
    }
    printf("\n");
    print_line('=', 62);
    printf("  SUMMARY\n");
    print_line('-', 62);
    printf("  Total sentences        : %d\n", count);
    printf("  Total words            : %d\n", total_words);
    printf("  Total characters       : %d\n", total_chars);
    printf("  Average words/sentence : %.2f\n",
           count > 0 ? (double)total_words / count : 0.0);
    printf("\n");
    printf("  Declarative   [.]      : %d\n", decl);
    printf("  Exclamatory   [!]      : %d\n", excl);
    printf("  Interrogative [?]      : %d\n", inter);
    printf("\n");
    printf("  Longest  sentence      : #%d  (%d words)\n",
           longest+1, sentences[longest].word_count);
    printf("  Shortest sentence      : #%d  (%d words)\n",
           shortest+1, sentences[shortest].word_count);
    print_line('=', 62);
    printf("\n");
}
 
/* Read lines until user types "done" or "quit" */
int read_input(char *buffer, int max_len) {
    buffer[0] = '\0';
    int total = 0;
    char line[512];
    while (1) {
        if (!fgets(line, sizeof(line), stdin)) break;
        int llen = (int)strlen(line);
        if (llen > 0 && line[llen-1] == '\n') { line[llen-1] = '\0'; llen--; }
        char lower[512];
        strncpy(lower, line, sizeof(lower)-1);
        lower[sizeof(lower)-1] = '\0';
        for (int i = 0; lower[i]; i++) lower[i] = tolower((unsigned char)lower[i]);
        if (strcmp(lower, "done") == 0) return 1;
        if (strcmp(lower, "quit") == 0) return 0;
        if (total + llen + 2 < max_len) {
            if (total > 0) { buffer[total++] = ' '; buffer[total] = '\0'; }
            strcat(buffer, line);
            total += llen;
        }
    }
    return 1;
}
 
int main(void) {
    Sentence sentences[MAX_SENTENCES];
    char     text[MAX_TEXT_LEN];
 
    printf("\n");
    print_line('*', 62);
    printf("*%60s*\n", "");
    printf("*        SENTENCE DIVIDER SIMULATOR  (C Project)        *\n");
    printf("*%60s*\n", "");
    print_line('*', 62);
    printf("\n");
    printf("  HOW TO USE:\n");
    printf("  1. Type or paste your paragraph and press ENTER\n");
    printf("  2. Type  done  and press ENTER  -> to see results\n");
    printf("  3. Type  quit  and press ENTER  -> to exit program\n");
    printf("\n");
 
    while (1) {
        print_line('-', 62);
        printf("  YOUR TEXT:\n");
        print_line('-', 62);
        printf("  ");
 
        int go = read_input(text, MAX_TEXT_LEN);
        if (!go) {
            printf("\n  Goodbye! Thank you for using Sentence Divider.\n\n");
            break;
        }
 
        trim(text);
 
        if (strlen(text) == 0) {
            printf("\n  [!] You did not enter any text. Please try again.\n\n");
            continue;
        }
 
        int count = divide_sentences(text, sentences, MAX_SENTENCES);
 
        if (count == 0) {
            printf("\n  [!] No sentences found.\n");
            printf("      Tip: End your sentences with  .  !  or  ?\n\n");
        } else {
            show_results(sentences, count);
        }
 
        printf("  Press ENTER to try another text, or type quit + ENTER to exit.\n  ");
        char again[64];
        if (fgets(again, sizeof(again), stdin)) {
            char lower[64];
            strncpy(lower, again, sizeof(lower)-1);
            for (int i = 0; lower[i]; i++) lower[i] = tolower((unsigned char)lower[i]);
            if (strncmp(lower, "quit", 4) == 0) {
                printf("\n  Goodbye! Thank you for using Sentence Divider.\n\n");
                break;
            }
        }
    }
 
    return 0;
}