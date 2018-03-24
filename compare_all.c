#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* Use https://github.com/JamesRamm/csv_parser for CSV parsing */
#include "csvparser.h"

/*
Num samples: 624
Num SNPs: 499170
AA: 103209822
NC: 4829294
AB: 78232741
BB: 125210223
*/

#define NUM_SAMPLES 624
#define NUM_SNPS 499170
#define INPUT_FILE "fcctx-snp-genotype.csv"
/* first 3 fields are Name,Chr,Position, we skip them */
#define START_SAMPLE_POS 3

typedef enum genotype { AA, AB, BB, NC } Genotype;

void init_samples(void);
Genotype get_genotype(char *);
char *show_genotype(Genotype);
void compare_all_samples(void);
double compare_2_samples(int, int);
CsvRow *read_samples(void);
void trim_sample_id(char *);

Genotype samples[NUM_SAMPLES][NUM_SNPS];
double comparisons[NUM_SAMPLES][NUM_SAMPLES] = {{0}};

char *show_genotype(Genotype gt)
{
    switch(gt)
    {
        case AA:
            return "AA";
            break;

        case AB:
            return "AB";
            break;

        case BB:
            return "BB";
            break;

        case NC:
            return "NC";
            break;

        default:
            return "?";
            break;
    }
}


Genotype get_genotype(char *field)
{
    if (strcmp("AA", field) == 0)
    {
        return AA;
    }
    else if (strcmp("AB", field) == 0)
    {
        return AB;
    }
    else if (strcmp("BB", field) == 0)
    {
        return BB;
    }
    else if (strcmp("NC", field) == 0)
    {
        return NC;
    }
    else
    {
        fprintf(stderr, "Bad genotype: %s\n", field);
        exit(1);
    }
}

CsvRow *read_samples(void)
{
    int snp_pos = 0;
    int field_pos = 0;
    int sample_pos = 0;
    int num_fields = 0;

    CsvParser *csvparser = CsvParser_new(INPUT_FILE, ",", 1);
    CsvRow *header;
    CsvRow *row;
    char **rowFields;
    Genotype gt;

    header = CsvParser_getHeader(csvparser);
    if (header == NULL)
    {
        printf("%s\n", CsvParser_getErrorMessage(csvparser));
    }

    while ((row = CsvParser_getRow(csvparser)))
    {
        rowFields = CsvParser_getFields(row);
        num_fields = CsvParser_getNumFields(row); 
        for (field_pos = START_SAMPLE_POS; field_pos < num_fields; field_pos++)
        {
            gt = get_genotype(rowFields[field_pos]); 
            sample_pos = field_pos - START_SAMPLE_POS;
            samples[sample_pos][snp_pos] = gt; 
        }
        CsvParser_destroy_row(row);
        snp_pos++; 
    }
    return header;
}

double compare_2_samples(int i, int j)
{
    int num_equal_snps = 0;
    int snp_pos;
    Genotype gt1, gt2;

    for (snp_pos = 0; snp_pos < NUM_SNPS; snp_pos ++)
    {
        gt1 = samples[i][snp_pos];
        gt2 = samples[j][snp_pos];
        // We don't consider NC SNPs to be the same
        if (gt1 != NC && gt2 != NC && gt1 == gt2)
        {
            num_equal_snps ++;
        }
    }

    return ((double) num_equal_snps) / ((double) NUM_SNPS);
}

void compare_all_samples(void)
{
    int i, j;
    double c;

    for (i = 0; i < NUM_SAMPLES; i++)
    {
        for (j = i; j < NUM_SAMPLES; j++)
        {
            c = compare_2_samples(i, j);    
            comparisons[i][j] = c;
        }
    }
}

// remove the .GType extension on the end of the sample ID
void trim_sample_id(char *str)
{
    str[strlen(str) - strlen(".GType")] = '\0';
}

void print_results(CsvRow *header)
{
    char **headerFields;
    int i, j;
    int num_header_fields;

    headerFields = CsvParser_getFields(header);
    num_header_fields = CsvParser_getNumFields(header);
    printf("SAMPLE,");
    for (i = START_SAMPLE_POS; i < num_header_fields; i++)
    {
        trim_sample_id(headerFields[i]);
        if (i < num_header_fields - 1)
        { 
            printf("%s,", headerFields[i]);
        }
        else
        { 
            printf("%s\n", headerFields[i]);
        }
    }
    for (i = 0; i < NUM_SAMPLES; i++)
    {
        printf("%s", headerFields[i+START_SAMPLE_POS]);
        for (j = 0; j < NUM_SAMPLES; j++)
        {
            printf(",%f", comparisons[i][j]);
        }
        printf("\n");
    }
}

int main(void)
{
   CsvRow *header;

   header = read_samples();
   compare_all_samples();
   print_results(header);
   return 0;
}
