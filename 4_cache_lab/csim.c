#include "cachelab.h"
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#define LRU_NUM 999

/* line and cache data structure */
typedef struct {
    int valid;
    int lru_number;
    long tag;
} Line;

Line *cache;

/* function declaration */
void error(char *msg);

Line *index2line(int s, int i);

void get_input_opts(int argc, char **argv);

void modify_data(long addr, int size, int set, long tag);

void store_data(long addr, int size, int set, long tag);

void load_data(long addr, int size, int set, long tag);

int update_cache(int set, long tag);

int find_min_lru(int set);

int is_hit(int set, long tag);

void update_lru(int set, int index);


/* global variables */
int s, S, E, b, verbose = 0;
char *trace_file;
int misses = 0, hits = 0, evictions = 0;

int main(int argc, char **argv) {

    // get input arguments and initialize cache
    get_input_opts(argc, argv);
    cache = (Line *) calloc(sizeof(Line), S * E);
    if (!cache)
        error("Can't allocate memory for cache");

    // simulate operation line by line
    FILE *file = fopen(trace_file, "r");
    if (!file)
        error("Can't open trace file");

    char type;
    int size;
    long addr;
    while (fscanf(file, "%c %lx, %d ", &type, &addr, &size) != EOF) {
        if (type == 'I') continue;
        int set = (addr >> b) & ((1 << s) - 1);
        long tag = addr >> (s + b);
        if (verbose) printf("\n%c %lx, %d", type, addr, size);

        if (type == 'S')
            store_data(addr, size, set, tag);
        else if (type == 'M')
            modify_data(addr, size, set, tag);
        else if (type == 'L')
            load_data(addr, size, set, tag);
    }

    printSummary(hits, misses, evictions);
    fclose(file);
    free(cache);
    return 0;
}

void error(char *msg) {
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(1);
}


Line *index2line(int s, int i) {
    return cache + E * s + i;
}


void update_lru(int set, int index) {
    index2line(set, index)->lru_number = LRU_NUM;

    for (int i = 0; i < E; ++i)
        if (i != index)
            index2line(set, i)->lru_number--;
}


int is_hit(int set, long tag) {
    int hit = 0;
    for (int i = 0; i < E; ++i) {
        Line *line = index2line(set, i);
        if (line->valid && line->tag == tag) {
            hit = 1;
            update_lru(set, i);
        }
    }
    return hit;
}


int find_min_lru(int set) {
    int min_lru = LRU_NUM;
    int min_index = 0;
    for (int i = 0; i < E; ++i) {
        Line *line = index2line(set, i);
        if (line->lru_number < min_lru) {
            min_index = i;
            min_lru = line->lru_number;
        }
    }
    return min_index;
}


int update_cache(int set, long tag) {
    int is_full = 1;
    int i;
    for (i = 0; i < E; ++i) {
        Line *line = index2line(set, i);
        if (!line->valid) {
            is_full = 0;
            break;
        }
    }

    if (is_full) {
        int eviction_index = find_min_lru(set);
        index2line(set, eviction_index)->tag = tag;
        update_lru(set, eviction_index);
    } else {
        Line *line = index2line(set, i);
        line->valid = 1;
        line->tag = tag;
        update_lru(set, i);
    }
    return is_full;
}


void load_data(long addr, int size, int set, long tag) {
    if (is_hit(set, tag)) {
        hits++;
        if (verbose) printf("hit ");
    } else {
        misses++;
        if (verbose) printf("miss ");
        if (update_cache(set, tag) == 1) {
            evictions++;
            if (verbose) printf("eviction ");
        }
    }
}

void store_data(long addr, int size, int set, long tag) {
    load_data(addr, size, set, tag);
}

void modify_data(long addr, int size, int set, long tag) {
    load_data(addr, size, set, tag);
    store_data(addr, size, set, tag);
}


void get_input_opts(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "hvs:E:b:t:")) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            case 's':
                s = atoi(optarg);
                S = 1 << s;
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 't':
                trace_file = optarg;
                break;
            case 'h':
            default:
                printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
                exit(-1);
        }
    }
}
