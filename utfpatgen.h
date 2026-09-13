#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef UTFPATGEN_VERSION
#define UTFPATGEN_VERSION "1.0"
#endif

typedef struct {
    size_t capacity;
    size_t occupied;
    size_t node_max;
    size_t base_max;
    size_t pattern_count;
    char *nodes;  // _c
    size_t *links;  // _l
    size_t *aux;  // _r
    char *taken;
} trie;

trie *init_trie(size_t capacity);
trie *resize_trie(trie *t, size_t new_capacity);
void relink_trie(trie *t);
void destroy_trie(trie *t);

bool put_first_level(trie *t);

bool copy_node(trie *from, size_t from_index, trie *to, size_t to_index);

bool get_base_used(trie *t, size_t index);
bool set_base_used(trie *t, size_t index, bool used);

void set_links(trie *t, size_t from, size_t to);

bool is_node_occupied(trie *t, size_t index);

bool find_base_for_first_fit(trie *t, trie *q, size_t *out_base);
bool first_fit(trie *t, trie *q, size_t *out_base);
bool unpack(trie *from, size_t base, trie *to);
size_t traverse_trie(trie *t, const char *pattern);

typedef struct {
    size_t value;
    size_t position;
    size_t next_op_index;
} output;

typedef struct {
    size_t capacity;
    size_t count;
    output *data;
    size_t lookup_cap;
    size_t lookup_cnt;
    size_t *lookup;
} outputs;

outputs *init_outputs(size_t capacity);
outputs *resize_outputs(outputs *ops, size_t capacity);
void destroy_outputs(outputs *ops);
bool resize_lookup(outputs *ops, size_t new_cap, trie *t);

typedef struct {
    trie *t;
    outputs *ops;
} pattern_trie;

pattern_trie *init_pattern_trie(size_t trie_capacity, size_t outputs_capacity);
void destroy_pattern_trie(pattern_trie *pt);

bool new_trie_output(pattern_trie *pt, size_t value, size_t position, size_t next, size_t *op_index);
size_t hash_trie_output(outputs *ops, size_t value, size_t position, size_t next);

bool insert_pattern(trie *t, const char *pattern, size_t *out_op_index, trie *helper_trie);
bool insert_substring(trie *t, const char *pattern, size_t end, size_t length, size_t *out_op_index, trie *helper_trie);
bool repack(trie *t, trie *q, size_t *node, size_t *link, char value);
bool set_output(pattern_trie *pt, size_t node, size_t value, size_t position);

typedef struct {
    size_t capacity;
    size_t size;
    size_t *good;
    size_t *bad;
} pattern_counts;

pattern_counts *init_pattern_counts(size_t capacity);
pattern_counts *resize_pattern_counts(pattern_counts *pc, size_t new_capacity);
void destroy_pattern_counts(pattern_counts *pc);

typedef struct {
    trie *t;
    pattern_counts *cnts;
} count_trie;

count_trie *init_count_trie(size_t trie_capacity, size_t counts_capacity);
void destroy_count_trie(count_trie *ct);

typedef enum { // 2-bit logic: upper = hyphen was found, lower = hyphen is present
    NO_HYF = 0,
    MISS_HYF = 1,
    BAD_HYF = 2,
    GOOD_HYF = 3,
} hyphen_type;

typedef struct {
    // global
    uint8_t left_hyphen_min;
    uint8_t right_hyphen_min;
    char bad_hyphen;
    char missed_hyphen;
    char good_hyphen;
    uint8_t hyph_start;
    uint8_t hyph_finish;
    uint8_t word_weight;
    FILE *dictionary_file;
    FILE *pattern_file;
    FILE *output_file;
    FILE *translate_file;
    // level specific
    uint8_t hyph_level;
    uint8_t pat_start;
    uint8_t pat_finish;
    uint8_t good_wt;
    uint8_t bad_wt;
    uint8_t thresh;
    hyphen_type good_dot;
    hyphen_type bad_dot;
    // pass specific
    uint8_t pat_len;
    uint8_t pat_dot;
} params;

params *init_params();
void reset_params(params *params);
void destroy_params(params *params);

typedef struct {
    size_t good_pat_cnt;
    size_t bad_pat_cnt;
    size_t good_cnt;
    size_t bad_cnt;
    size_t miss_cnt;
    size_t level_pattern_cnt;
    uint8_t max_level;
    bool more_to_come;
} pass_stats;

typedef struct {
    size_t capacity;
    size_t size;
    char *data;
    bool eof;
} string_buffer;

string_buffer *init_buffer(size_t capacity);
string_buffer *resize_buffer(string_buffer *buf, size_t new_capacity);
void reset_buffer(string_buffer *buf);
void destroy_buffer(string_buffer *buf);

typedef struct {
    size_t capacity;
    size_t top;
    size_t *data;
} stack;

stack *init_stack(size_t capacity);
stack *resize_stack(stack *s, size_t new_capacity);
void destroy_stack(stack *s);
bool put_on_stack(stack *s, size_t value);
size_t get_top_value(stack *s);
void set_top_value(stack *s, size_t value);

typedef struct {
    size_t capacity;
    size_t size;
    size_t length;
    char *translated;
    size_t *true_hyphens;
    uint8_t *found_hyphens;
    bool *no_more;
} word;

word *init_word(size_t capacity);
word *resize_word(word *word, size_t new_capacity);
void reset_word(word *word);
void destroy_word(word *word);

bool append_char_to_word(word *word, char c);

size_t get_true_hyphen(word *word, size_t index);
bool set_true_hyphen(word *word, size_t index, size_t value);

uint8_t get_found_hyphen(word *word, size_t index);
bool set_found_hyphen(word *word, size_t index, uint8_t value);

bool get_no_more(word *word, size_t index);
bool set_no_more(word *word, size_t index, bool value);

#ifndef BAD_OP_VALUE
#define BAD_OP_VALUE (size_t) 255
#endif

#ifndef EMPTY_OP_VALUE
#define EMPTY_OP_VALUE (size_t) 0
#endif
bool is_utf_start_byte(uint8_t byte);

bool collect_count_trie(count_trie *ct, pattern_trie *pt, params *params, pass_stats *ps);
bool traverse_count_trie(count_trie *ct, pattern_trie *pt, params *params, pass_stats *ps);

bool delete_bad_patterns(pattern_trie *pt);
void deallocate_node(trie *t, size_t t_index);
bool link_around_bad_outputs(pattern_trie *pt, size_t t_index);
bool delete_patterns(pattern_trie *pt);

typedef struct {
    trie *mapping;
    string_buffer *alphabet;
    size_t *index_to_alphabet;
    size_t letter_count;
    size_t letter_capacity;
} translate_table;

translate_table *init_tr_table(size_t mapping_capacity, size_t alphabet_capacity);
void destroy_tr_table(translate_table *tt);

bool read_translate(params *params, translate_table *tt);
bool parse_header(string_buffer *buf, params *params);
bool parse_letters(string_buffer *buf, translate_table *tt, trie *helper_trie);
bool default_ascii_mapping(translate_table *tt, trie *helper_trie);
char *get_lower(translate_table *tt, const char *letter);
size_t get_letter_index(translate_table *tt, char *letter);
bool convert_index(size_t index, word *word);
size_t convert_byte_sequence(char **sequence);

bool output_patterns(pattern_trie *pt, translate_table *tt, FILE *pattern_file);
void output_pattern(string_buffer *pattern, translate_table *tt, outputs *ops, size_t op_index, FILE *pattern_file);
size_t get_highest_level(outputs *ops, size_t start_index, size_t position);

bool read_line(FILE *stream, string_buffer *buf);
bool append_char(string_buffer *buf, char c);
bool append_string(string_buffer *buf, const char *str);

#ifndef EDGE_OF_WORD
// not used in UTF-8
#define EDGE_OF_WORD (char) 0xff
#endif

bool parse_word(string_buffer *buf, translate_table *tt, params *params, word *out_word);

bool hyphenate_word(word *word, pattern_trie *pt, params *params);
void count_dots(word *word, params *params, pass_stats *ps);
void output_hyphenated_word(FILE *pattmp, word *word, translate_table *tt, params *params);

bool process_word(word *word, count_trie *ct, params *params, trie *helper_trie);

bool process_dictionary(params *params, translate_table *tt, pattern_trie *pt, pass_stats *ps);
bool process_all_words(params *params, translate_table *tt, pattern_trie *pt, pass_stats *ps, count_trie *ct);
bool hyphenate_dictionary(params *params, translate_table *tt, pattern_trie *pt, bool output, pass_stats *ps);
bool hyphenate_all_words(params *params, translate_table *tt, pattern_trie *pt, FILE *pattmp, pass_stats *ps);

typedef struct {
    size_t capacity;
    size_t size;
    size_t length;
    char *text;
    uint8_t *hyphens;
} pattern;

#ifndef HYPHEN_FLAG
// not used in UTF-8
#define HYPHEN_FLAG (char) 0xfe
#endif

pattern *init_pattern(size_t capacity);
pattern *resize_pattern(pattern *pat, size_t new_capacity);
void reset_pattern(pattern *pat);
void destroy_pattern(pattern *pat);

bool convert_index_to_pattern(size_t index, pattern *pat);

uint8_t get_hyphen(pattern *pat, size_t index);
bool set_hyphen(pattern *pat, size_t index, uint8_t value);

bool read_patterns(params *params, pattern_trie *pt, translate_table *tt, pass_stats *ps);
bool parse_pattern(string_buffer *buf, pattern *out_pattern, translate_table *tt);
bool insert_new_pattern(pattern *pat, pattern_trie *pt, pass_stats *ps, trie *helper_trie);

bool parse_input(char *argv[], int argc, params *params);
void print_help();
void print_version();
