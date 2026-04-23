#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TEXT_LEN 4096
#define MAX_SENTENCES 256
#define MAX_SENT_LEN 512

typedef enum {
    DECLARATIVE,
    EXCLAMATORY,
    INTERROGATIVE
} SentenceType;

typedef struct {
    char text[MAX_SENT_LEN];
    SentenceType type;
    int word_count;
    int char_count;
} Sentence;

static const char *ABBREVIATIONS[] = {
    "Mr","Mrs","Ms","Dr","Prof","Sr","Jr",
    "St","Ave","Blvd","Dept","govt","vs",
    "etc","e.g","i.e","Jan","Feb","Mar",
    "Apr","Jun","Jul","Aug","Sep","Oct",
    "Nov","Dec",NULL
};

void trim(char *s) {
    char *start = s;
    while (*start && isspace((unsigned char)*start)) start++;

    int len = (int)strlen(start);
    while (len > 0 && isspace((unsigned char)start[len - 1])) len--;

    memmove(s, start, len);
    s[len] = '\0';
}

int count_words(const char *s) {
    int count = 0, in_word = 0;
    while (*s) {
        if (isspace((unsigned char)*s)) {
            in_word = 0;
        } else if (!in_word) {
            in_word = 1;
            count++;
        }
        s++;
    }
    return count;
}

int count_chars(const char *s) {
    int count = 0;
    while (*s) {
        if (!isspace((unsigned char)*s)) count++;
        s++;
    }
    return count;
}

int is_abbreviation(const char *text, int dot_pos) {
    int start = dot_pos - 1;
    while (start > 0 && !isspace((unsigned char)text[start - 1])) start--;

    char word[64];
    int wlen = dot_pos - start;
    if (wlen <= 0 || wlen >= 63) return 0;

    strncpy(word, text + start, wlen);
    word[wlen] = '\0';

    for (int i = 0; ABBREVIATIONS[i] != NULL; i++) {
        int match = 1;
        const char *a = word, *b = ABBREVIATIONS[i];
        while (*a && *b) {
            if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) {
                match = 0;
                break;
            }
            a++; b++;
        }
        if (match && !*a && !*b) return 1;
    }
    return 0;
}

int divide_sentences(const char *text, Sentence sentences[], int max_sentences) {
    int count = 0, len = (int)strlen(text), start = 0;

    for (int i = 0; i < len && count < max_sentences; i++) {
        char ch = text[i];

        if (ch == '.' || ch == '!' || ch == '?') {

            if (ch == '.' && is_abbreviation(text, i)) continue;
            if (ch == '.' && i + 1 < len && text[i + 1] == '.') continue;

            int seg_len = i - start + 1;
            if (seg_len <= 0) { start = i + 1; continue; }
            if (seg_len >= MAX_SENT_LEN) seg_len = MAX_SENT_LEN - 1;

            strncpy(sentences[count].text, text + start, seg_len);
            sentences[count].text[seg_len] = '\0';
            trim(sentences[count].text);

            if (strlen(sentences[count].text) == 0) { start = i + 1; continue; }

            if (ch == '!') sentences[count].type = EXCLAMATORY;
            else if (ch == '?') sentences[count].type = INTERROGATIVE;
            else sentences[count].type = DECLARATIVE;

            sentences[count].word_count = count_words(sentences[count].text);
            sentences[count].char_count = count_chars(sentences[count].text);

            count++;
            start = i + 1;

            while (start < len && isspace((unsigned char)text[start])) start++;
            i = start - 1;
        }
    }

    if (start < len && count < max_sentences) {
        char tail[MAX_SENT_LEN];
        int tlen = len - start;
        if (tlen >= MAX_SENT_LEN) tlen = MAX_SENT_LEN - 1;

        strncpy(tail, text + start, tlen);
        tail[tlen] = '\0';
        trim(tail);

        if (strlen(tail) > 0) {
            strncpy(sentences[count].text, tail, MAX_SENT_LEN - 1);
            sentences[count].type = DECLARATIVE;
            sentences[count].word_count = count_words(tail);
            sentences[count].char_count = count_chars(tail);
            count++;
        }
    }

    return count;
}

void print_rule(char ch, int width) {
    for (int i = 0; i < width; i++) putchar(ch);
    putchar('\n');
}

void display_report(const Sentence sentences[], int count) {
    int total_words = 0, total_chars = 0;
    int decl = 0, excl = 0, inter = 0;
    int longest_idx = 0, shortest_idx = 0;

    for (int i = 0; i < count; i++) {
        total_words += sentences[i].word_count;
        total_chars += sentences[i].char_count;

        if (sentences[i].type == DECLARATIVE) decl++;
        else if (sentences[i].type == EXCLAMATORY) excl++;
        else if (sentences[i].type == INTERROGATIVE) inter++;

        if (sentences[i].word_count > sentences[longest_idx].word_count)
            longest_idx = i;
        if (sentences[i].word_count < sentences[shortest_idx].word_count)
            shortest_idx = i;
    }

    printf("\n");
    print_rule('=', 60);
    printf("          SENTENCE DIVIDER SIMULATOR - RESULTS\n");
    print_rule('=', 60);

    for (int i = 0; i < count; i++) {
        const char *type_str;
        const char *symbol;

        if (sentences[i].type == DECLARATIVE) { type_str = "Declarative"; symbol = "[.]"; }
        else if (sentences[i].type == EXCLAMATORY) { type_str = "Exclamatory"; symbol = "[!]"; }
        else { type_str = "Interrogative"; symbol = "[?]"; }

        printf("\n Sentence #%d  %s  %s\n", i + 1, symbol, type_str);
        print_rule('-', 60);
        printf("  \"%s\"\n", sentences[i].text);
        printf("  Words : %-4d  |  Characters (no spaces): %d\n",
               sentences[i].word_count, sentences[i].char_count);
    }

    printf("\n");
    print_rule('=', 60);
    printf("  SUMMARY STATISTICS\n");
    print_rule('-', 60);
    printf("  Total Sentences      : %d\n", count);
    printf("  Total Words          : %d\n", total_words);
    printf("  Total Characters     : %d\n", total_chars);
    printf("  Avg Words/Sentence   : %.2f\n",
           count > 0 ? (double)total_words / count : 0.0);

    printf("\n");
    printf("  Declarative  (.)     : %d\n", decl);
    printf("  Exclamatory  (!)     : %d\n", excl);
    printf("  Interrogative(?)     : %d\n", inter);

    printf("\n");
    printf("  Longest  sentence    : #%d (%d words)\n",
           longest_idx + 1, sentences[longest_idx].word_count);
    printf("  Shortest sentence    : #%d (%d words)\n",
           shortest_idx + 1, sentences[shortest_idx].word_count);

    print_rule('=', 60);
    printf("\n");
}

void get_text_from_user(char *buffer, int max_len) {
    printf("\nEnter your paragraph (press ENTER twice to finish):\n");

    buffer[0] = '\0';
    char line[512];
    int total = 0;

    while (fgets(line, sizeof(line), stdin)) {
        if (line[0] == '\n') break;

        int len = strlen(line);
        if (total + len + 1 >= max_len) break;

        strcat(buffer, line);
        total += len;
    }
}

int main() {
    Sentence sentences[MAX_SENTENCES];
    char text[MAX_TEXT_LEN];

    get_text_from_user(text, MAX_TEXT_LEN);

    int count = divide_sentences(text, sentences, MAX_SENTENCES);

    if (count == 0) {
        printf("No sentences found.\n");
    } else {
        display_report(sentences, count);
    }

    return 0;
}
 
