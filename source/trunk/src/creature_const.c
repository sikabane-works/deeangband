#include "angband.h"

// Base Action point
s16b adj_dex_action_point[STAT_TABLE_SIZE] =
{
	400	/* 3 */,
	350	/* 4 */,
	320	/* 5 */,
	290	/* 6 */,
	260	/* 7 */,
	230	/* 8 */,
	200	/* 9 */,
	190	/* 10 */,
	180	/* 11 */,
	170	/* 12 */,
	160	/* 13 */,
	150	/* 14 */,
	140	/* 15 */,
	130	/* 16 */,
	120	/* 17 */,
	115	/* 18 */,
	110	/* 19 */,
	105	/* 20 */,
	100	/* 21 */,
	 95	/* 22 */,
	 90	/* 23 */,
	 86	/* 24 */,
	 82	/* 25 */,
	 78	/* 26 */,
	 75	/* 27 */,
	 72	/* 28 */,
	 69	/* 29 */,
	 66	/* 30 */,
	 63	/* 31 */,
	 60	/* 32 */,
	 57	/* 33 */,
	 55 /* 34 */,
	 53	/* 35 */,
	 51	/* 36 */,
	 49	/* 37 */,
	 47	/* 38 */,
	 45	/* 39 */,
	 44	/* 40 */,
	 43 /* 41 */,
	 42	/* 42 */,
	 41	/* 43 */,
	 40	/* 44 */,
	 40	/* 45 */,
	 39	/* 46 */,
	 39	/* 47 */,
	 38	/* 48 */,
	 38	/* 49 */,
	 37	/* 50 */,
	 37	/* 51 */,
	 36	/* 52 */,
	 36	/* 53 */,
	 35	/* 54 */,
	 35	/* 55 */,
	 35	/* 56 */,
	 34	/* 57 */,
	 34	/* 58 */,
	 34	/* 59 */,
	 33	/* 60 */,
};

s16b adj_weight_action_point[PERCENTAGE / 5 + 1] =
{
	100, // 00-04%
	102, // 05-09%
	105, // 10-14%
	110, // 15-19%
	120, // 20-24%

	140, // 25-29%
	160, // 30-34%
	190, // 35-39%
	240, // 40-44%
	300, // 45-49%

	360, // 50-54%
	420, // 55-59%
	500, // 60-64%
	600, // 65-69%
	700, // 70-74%

	800, // 75-79%
	900, // 80-84%
	1000, // 85-89%
	1200, // 90-94%
	1400, // 95-99%

	1400, // 100%
};




/*
 * Stat Table (INT/WIS) -- Number of half-spells per level
 */
byte adj_mag_study[STAT_TABLE_SIZE] =
{
	0	/* 3 */,
	0	/* 4 */,
	0	/* 5 */,
	0	/* 6 */,
	0	/* 7 */,
	1	/* 8 */,
	1	/* 9 */,
	1	/* 10 */,
	1	/* 11 */,
	2	/* 12 */,
	2	/* 13 */,
	2	/* 14 */,
	2	/* 15 */,
	2	/* 16 */,
	2	/* 17 */,
	2	/* 18 */,
	2	/* 19 */,
	2	/* 20 */,
	2	/* 21 */,
	2	/* 22 */,
	3	/* 23 */,
	3	/* 24 */,
	3	/* 25 */,
	3	/* 26 */,
	4	/* 27 */,
	4	/* 28 */,
	4	/* 29 */,
	5	/* 30 */,
	5	/* 31 */,
	5	/* 32 */,
	5	/* 33 */,
	5	/* 34 */,
	5	/* 35 */,
	5	/* 36 */,
	5	/* 37 */,
	5	/* 38 */,
	6	/* 39 */,
	6	/* 40 */,
	6	/* 41 */,
	6	/* 42 */,
	6	/* 43 */,
	6	/* 44 */,
	6	/* 45 */,
	6	/* 46 */,
	6	/* 47 */,
	6	/* 48 */,
	6	/* 49 */,
	6	/* 50 */,
	6	/* 51 */,
	6	/* 52 */,
	6	/* 53 */,
	6	/* 54 */,
	6	/* 55 */,
	6	/* 56 */,
	6	/* 57 */,
	6	/* 58 */,
	6	/* 59 */,
	6	/* 60 */,
};


/*
 * Stat Table (INT/WIS) -- extra 1/4-mana-points per level
 */
byte adj_mag_mana[STAT_TABLE_SIZE] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	2       /* 9 */,
	3       /* 10 */,
	4       /* 11 */,
	5       /* 12 */,
	5       /* 13 */,
	6       /* 14 */,
	7       /* 15 */,
	8       /* 16 */,
	9       /* 17 */,
	10      /* 18 */,
	11      /* 19 */,
	11      /* 20 */,
	12      /* 21 */,
	12      /* 22 */,
	13      /* 23 */,
	14      /* 24 */,
	15      /* 25 */,
	16      /* 26 */,
	17      /* 27 */,
	18      /* 28 */,
	19      /* 29 */,
	20      /* 30 */,
	21      /* 31 */,
	22      /* 32 */,
	23      /* 33 */,
	24      /* 34 */,
	25      /* 35 */,
	26      /* 36 */,
	27      /* 37 */,
	28      /* 38 */,
	29      /* 39 */,
	30      /* 40 */,
	31      /* 41 */,
	32      /* 42 */,
	33      /* 43 */,
	34      /* 44 */,
	35      /* 45 */,
	36      /* 46 */,
	37      /* 47 */,
	38      /* 48 */,
	39      /* 49 */,
	40      /* 50 */,
	41      /* 51 */,
	42      /* 52 */,
	43      /* 53 */,
	44      /* 54 */,
	45      /* 55 */,
	46      /* 56 */,
	47      /* 57 */,
	48      /* 58 */,
	49      /* 59 */,
	50      /* 60 */,
};


/*
 * Stat Table (INT/WIS) -- Minimum failure rate (percentage)
 */
byte adj_mag_fail[STAT_TABLE_SIZE] =
{
	99      /* 3 */,
	99      /* 4 */,
	99      /* 5 */,
	99      /* 6 */,
	99      /* 7 */,
	50      /* 8 */,
	30      /* 9 */,
	20      /* 10 */,
	15      /* 11 */,
	12      /* 12 */,
	11      /* 13 */,
	10      /* 14 */,
	9       /* 15 */,
	8       /* 16 */,
	7       /* 17 */,
	6       /* 18 */,
	6       /* 19 */,
	5       /* 20 */,
	5       /* 21 */,
	5       /* 22 */,
	4       /* 23 */,
	4       /* 24 */,
	4       /* 25 */,
	4       /* 26 */,
	3       /* 27 */,
	3       /* 28 */,
	2       /* 29 */,
	2       /* 30 */,
	2       /* 31 */,
	2       /* 32 */,
	1       /* 33 */,
	1       /* 34 */,
	1       /* 35 */,
	1       /* 36 */,
	1       /* 37 */,
	0       /* 38 */,
	0       /* 39 */,
	0       /* 40 */,
	0       /* 41 */,
	0       /* 42 */,
	0       /* 43 */,
	0       /* 44 */,
	0       /* 45 */,
	0       /* 46 */,
	0       /* 47 */,
	0       /* 48 */,
	0       /* 49 */,
	0       /* 50 */,
	0       /* 51 */,
	0       /* 52 */,
	0       /* 53 */,
	0       /* 54 */,
	0       /* 55 */,
	0       /* 56 */,
	0       /* 57 */,
	0       /* 58 */,
	0       /* 59 */,
	0       /* 60 */,
};


/*
 * Stat Table (INT/WIS) -- Various things
 */
byte adj_mag_stat[STAT_TABLE_SIZE] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18 */,
	3       /* 19 */,
	3       /* 20 */,
	3       /* 21 */,
	3       /* 22 */,
	4       /* 23 */,
	4       /* 24 */,
	5       /* 25 */,
	6       /* 26 */,
	7       /* 27 */,
	8       /* 28 */,
	9       /* 29 */,
	10      /* 30 */,
	11      /* 31 */,
	12      /* 32 */,
	13      /* 33 */,
	14      /* 34 */,
	15      /* 35 */,
	16      /* 36 */,
	17      /* 37 */,
	18      /* 38 */,
	19      /* 39 */,
	20      /* 40 */,
	21      /* 41 */,
	22      /* 42 */,
	23      /* 43 */,
	24      /* 44 */,
	25      /* 45 */,
	26      /* 46 */,
	27      /* 47 */,
	28      /* 48 */,
	30      /* 49 */,
	32      /* 50 */,
	34      /* 51 */,
	36      /* 52 */,
	38      /* 53 */,
	40      /* 54 */,
	42      /* 55 */,
	44      /* 56 */,
	46      /* 57 */,
	48      /* 58 */,
	50      /* 59 */,
	52      /* 60 */,
};


/*
 * Stat Table (DEX) -- base initative
 */
POWER adj_dex_initiative[STAT_TABLE_SIZE] =
{
	1     /* 3 */,
	2     /* 4 */,
	3     /* 5 */,
	3     /* 6 */,
	3     /* 7 */,
	4     /* 8 */,
	4     /* 9 */,
	4     /* 10 */,
	5     /* 11 */,
	5     /* 12 */,
	6     /* 13 */,
	6     /* 14 */,
	7     /* 15 */,
	7     /* 16 */,
	8     /* 17 */,
	9     /* 18 */,
	10      /* 19 */,
	11      /* 20 */,
	12      /* 21 */,
	13      /* 22 */,
	14      /* 23 */,
	15      /* 24 */,
	16      /* 25 */,
	18      /* 26 */,
	20      /* 27 */,
	22      /* 28 */,
	24      /* 29 */,
	26      /* 30 */,
	28      /* 31 */,
	30      /* 32 */,
	33      /* 33 */,
	36      /* 34 */,
	39      /* 35 */,
	42      /* 36 */,
	45      /* 37 */,
	48      /* 38 */,
	51      /* 39 */,
	54      /* 40 */,
	57      /* 41 */,
	60      /* 42 */,
	64      /* 43 */,
	68      /* 44 */,
	72      /* 45 */,
	76      /* 46 */,
	80      /* 47 */,
	85      /* 48 */,
	90      /* 49 */,
	95      /* 50 */,
	100     /* 51 */,
	105     /* 52 */,
	110     /* 53 */,
	115     /* 54 */,
	120     /* 55 */,
	125     /* 56 */,
	130     /* 57 */,
	135     /* 58 */,
	140     /* 59 */,
	145     /* 60 */,
};


/*
 * Stat Table (INT) -- Magic devices
 */
byte adj_int_dev[STAT_TABLE_SIZE] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18 */,
	3       /* 19 */,
	4       /* 20 */,
	4       /* 21 */,
	5       /* 22 */,
	5       /* 23 */,
	6       /* 24 */,
	6       /* 25 */,
	7       /* 26 */,
	7       /* 27 */,
	8       /* 28 */,
	9       /* 29 */,
	10      /* 30 */,
	11      /* 31 */,
	12      /* 32 */,
	13      /* 33 */,
	14      /* 34 */,
	15      /* 35 */,
	16      /* 36 */,
	17      /* 37 */,
	18      /* 38 */,
	19      /* 39 */,
	20      /* 40 */,
	22      /* 41 */,
	24      /* 42 */,
	26      /* 43 */,
	28      /* 44 */,
	30      /* 45 */,
	32      /* 46 */,
	34      /* 47 */,
	36      /* 48 */,
	38      /* 49 */,
	40      /* 50 */,
	42      /* 51 */,
	44      /* 52 */,
	46      /* 53 */,
	48      /* 54 */,
	50      /* 55 */,
	52      /* 56 */,
	54      /* 57 */,
	56      /* 58 */,
	58      /* 59 */,
	60      /* 60 */,
};


/*
 * Stat Table -- Saving throw
 */
int adj_sav[STAT_TABLE_SIZE] =
{
	-10     /* 3 */,
	-9      /* 4 */,
	-8      /* 5 */,
	-7      /* 6 */,
	-6      /* 7 */,
	-5      /* 8 */,
	-4      /* 9 */,
	-3      /* 10 */,
	-2      /* 11 */,
	-1      /* 12 */,
	0       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	3       /* 16 */,
	4       /* 17 */,
	5       /* 18 */,
	6       /* 19 */,
	7       /* 20 */,
	8       /* 21 */,
	9       /* 22 */,
	10       /* 23 */,
	11       /* 24 */,
	12       /* 25 */,
	14       /* 26 */,
	16       /* 27 */,
	18       /* 28 */,
	20       /* 29 */,
	22       /* 30 */,
	24      /* 31 */,
	26      /* 32 */,
	28      /* 33 */,
	30      /* 34 */,
	32      /* 35 */,
	34      /* 36 */,
	37      /* 37 */,
	40      /* 38 */,
	43      /* 39 */,
	46      /* 40 */,
	49      /* 41 */,
	52      /* 42 */,
	55      /* 43 */,
	58      /* 44 */,
	61      /* 45 */,
	64      /* 46 */,
	67      /* 47 */,
	70      /* 48 */,
	73      /* 49 */,
	76      /* 50 */,
	80      /* 51 */,
	84      /* 52 */,
	88      /* 53 */,
	92      /* 54 */,
	96      /* 55 */,
	100     /* 56 */,
	104     /* 57 */,
	108     /* 58 */,
	112     /* 59 */,
	116     /* 60 */,
};


/*
 * Stat Table (DEX) -- disarming
 */
byte adj_dex_dis[STAT_TABLE_SIZE] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	0       /* 8 */,
	0       /* 9 */,
	0       /* 10 */,
	0       /* 11 */,
	0       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	1       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	4       /* 18 */,
	4       /* 19 */,
	4       /* 20 */,
	4       /* 21 */,
	5       /* 22 */,
	5       /* 23 */,
	5       /* 24 */,
	6       /* 25 */,
	6       /* 26 */,
	7       /* 27 */,
	7       /* 28 */,
	8       /* 29 */,
	8       /* 30 */,
	9       /* 31 */,
	9       /* 32 */,
	10       /* 33 */,
	10       /* 34 */,
	11       /* 35 */,
	11       /* 36 */,
	12       /* 37 */,
	12      /* 38 */,
	13      /* 39 */,
	14      /* 40 */,
	15      /* 41 */,
	16      /* 42 */,
	17      /* 43 */,
	18      /* 44 */,
	19      /* 45 */,
	20      /* 46 */,
	21      /* 47 */,
	22      /* 48 */,
	23      /* 49 */,
	24      /* 50 */,
	25      /* 51 */,
	26      /* 52 */,
	27      /* 53 */,
	28      /* 54 */,
	29      /* 55 */,
	30      /* 56 */,
	32      /* 57 */,
	34      /* 58 */,
	37      /* 59 */,
	40      /* 60 */,
};


/*
 * Stat Table (INT) -- disarming
 */
byte adj_int_dis[STAT_TABLE_SIZE] =
{
	0       /* 3 */,
	0       /* 4 */,
	0       /* 5 */,
	0       /* 6 */,
	0       /* 7 */,
	1       /* 8 */,
	1       /* 9 */,
	1       /* 10 */,
	1       /* 11 */,
	1       /* 12 */,
	1       /* 13 */,
	1       /* 14 */,
	2       /* 15 */,
	2       /* 16 */,
	2       /* 17 */,
	3       /* 18 */,
	3       /* 19 */,
	3       /* 20 */,
	4       /* 21 */,
	4       /* 22 */,
	5       /* 23 */,
	6       /* 24 */,
	7       /* 25 */,
	8       /* 26 */,
	9       /* 27 */,
	10      /* 28 */,
	10      /* 29 */,
	11      /* 30 */,
	12      /* 31 */,
	13      /* 32 */,
	14      /* 33 */,
	15      /* 34 */,
	16      /* 35 */,
	17      /* 36 */,
	18      /* 37 */,
	19      /* 38 */,
	19      /* 39 */,
	20      /* 40 */,
	20      /* 41 */,
	21      /* 42 */,
	22      /* 43 */,
	23      /* 44 */,
	24      /* 45 */,
	25      /* 46 */,
	26      /* 47 */,
	27      /* 48 */,
	28      /* 49 */,
	29      /* 50 */,
	30      /* 51 */,
	31      /* 52 */,
	32      /* 53 */,
	33      /* 54 */,
	34      /* 55 */,
	35      /* 56 */,
	36      /* 57 */,
	37      /* 58 */,
	38      /* 59 */,
	40      /* 60 */,
};


/*
 * Stat bonus to AC(STR&CON)/EV(DEX&INT)/VO(WIS&CHA)
 */
s16b adj_stat_to_saving[STAT_TABLE_SIZE] =
{
	-4    /* 3 */,
	-3    /* 4 */,
	-2    /* 5 */,
	-1    /* 6 */,
	0     /* 7 */,
	0     /* 8 */,
	0     /* 9 */,
	0     /* 10 */,
	0     /* 11 */,
	0     /* 12 */,
	0     /* 13 */,
	0     /* 14 */,
	1     /* 15 */,
	1     /* 16 */,
	1     /* 17 */,
	2     /* 18 */,
	2     /* 19 */,
	2     /* 20 */,
	2     /* 21 */,
	2     /* 22 */,
	3     /* 23 */,
	3     /* 24 */,
	3     /* 25 */,
	4     /* 26 */,
	5     /* 27 */,
	6     /* 28 */,
	7     /* 29 */,
	8     /* 30 */,
	9     /* 31 */,
	9     /* 32 */,
	10    /* 33 */,
	11    /* 34 */,
	12    /* 35 */,
	13    /* 36 */,
	14    /* 37 */,
	15    /* 38 */,
	15    /* 39 */,
	16    /* 40 */,
	16    /* 41 */,
	17    /* 42 */,
	17    /* 43 */,
	18    /* 44 */,
	18    /* 45 */,
	19    /* 46 */,
	19    /* 47 */,
	20    /* 48 */,
	20    /* 49 */,
	21    /* 50 */,
	21    /* 51 */,
	22    /* 52 */,
	22    /* 53 */,
	23    /* 54 */,
	23    /* 55 */,
	24    /* 56 */,
	24    /* 57 */,
	25    /* 58 */,
	25    /* 59 */,
	26    /* 60 */,
};

/*
 * Stat Table (DEX) -- base initiative
 */
byte adj_chr_gold[STAT_TABLE_SIZE] =
{
	130     /* 3 */,
	125     /* 4 */,
	122     /* 5 */,
	120     /* 6 */,
	118     /* 7 */,
	116     /* 8 */,
	114     /* 9 */,
	112     /* 10 */,
	110     /* 11 */,
	108     /* 12 */,
	106     /* 13 */,
	104     /* 14 */,
	103     /* 15 */,
	102     /* 16 */,
	101     /* 17 */,
	100     /* 18 */,
	99      /* 19 */,
	98      /* 20 */,
	97      /* 21 */,
	96      /* 22 */,
	95      /* 23 */,
	94      /* 24 */,
	93      /* 25 */,
	92      /* 26 */,
	91      /* 27 */,
	90      /* 28 */,
	89      /* 29 */,
	88      /* 30 */,
	87      /* 31 */,
	86      /* 32 */,
	85      /* 33 */,
	84      /* 34 */,
	83      /* 35 */,
	82      /* 36 */,
	81      /* 37 */,
	80      /* 38 */,
	79      /* 39 */,
	78      /* 40 */,
	76      /* 41 */,
	74      /* 42 */,
	72      /* 43 */,
	70      /* 44 */,
	68      /* 45 */,
	66      /* 46 */,
	64      /* 47 */,
	62      /* 48 */,
	60      /* 49 */,
	58      /* 50 */,
	56      /* 51 */,
	54      /* 52 */,
	52      /* 53 */,
	50      /* 54 */,
	50      /* 55 */,
	50      /* 56 */,
	50      /* 57 */,
	50      /* 58 */,
	50      /* 59 */,
	50      /* 60 */,
};


/*
 * Stat Table (STR) -- bonus to dam (plus 128)
 */
s16b adj_str_to_damage[STAT_TABLE_SIZE] =
{
	-2    /* 3 */,
	-2    /* 4 */,
	-1    /* 5 */,
	-1    /* 6 */,
	0     /* 7 */,
	0     /* 8 */,
	0     /* 9 */,
	0     /* 10 */,
	0     /* 11 */,
	0     /* 12 */,
	0     /* 13 */,
	0     /* 14 */,
	0     /* 15 */,
	1     /* 16 */,
	2     /* 17 */,
	2     /* 18 */,
	2     /* 19 */,
	3     /* 20 */,
	3     /* 21 */,
	3     /* 22 */,
	3     /* 23 */,
	3     /* 24 */,
	4     /* 25 */,
	5     /* 26 */,
	5     /* 27 */,
	6     /* 28 */,
	7     /* 29 */,
	8     /* 30 */,
	9     /* 31 */,
	10    /* 32 */,
	11    /* 33 */,
	12    /* 34 */,
	13    /* 35 */,
	14    /* 36 */,
	15    /* 37 */,
	16    /* 38 */,
	18    /* 39 */,
	19    /* 40 */,
	19    /* 41 */,
	20    /* 42 */,
	20    /* 43 */,
	21    /* 44 */,
	21    /* 45 */,
	22    /* 46 */,
	22    /* 47 */,
	23    /* 48 */,
	23    /* 49 */,
	24    /* 50 */,
	24    /* 51 */,
	25    /* 52 */,
	25    /* 53 */,
	26    /* 54 */,
	26    /* 55 */,
	27    /* 56 */,
	27    /* 57 */,
	28    /* 58 */,
	30    /* 59 */,
	32    /* 60 */,
};


/*
 * Stat Table (DEX) -- bonus to hit (plus 128)
 */
s16b adj_dex_to_hit[STAT_TABLE_SIZE] =
{
	-3    /* 3 */,
	-2    /* 4 */,
	-2    /* 5 */,
	-1    /* 6 */,
	-1    /* 7 */,
	0     /* 8 */,
	0     /* 9 */,
	0     /* 10 */,
	0     /* 11 */,
	0     /* 12 */,
	0     /* 13 */,
	0     /* 14 */,
	0     /* 15 */,
	1     /* 16 */,
	2     /* 17 */,
	3     /* 18 */,
	3     /* 19 */,
	3     /* 20 */,
	3     /* 21 */,
	3     /* 22 */,
	4     /* 23 */,
	4     /* 24 */,
	4     /* 25 */,
	4     /* 26 */,
	5     /* 27 */,
	6     /* 28 */,
	7     /* 29 */,
	8     /* 30 */,
	9     /* 31 */,
	9     /* 32 */,
	10    /* 33 */,
	11    /* 34 */,
	12    /* 35 */,
	13    /* 36 */,
	14    /* 37 */,
	15    /* 38 */,
	15    /* 39 */,
	16    /* 40 */,
	16    /* 41 */,
	17    /* 42 */,
	17    /* 43 */,
	18    /* 44 */,
	18    /* 45 */,
	19    /* 46 */,
	19    /* 47 */,
	20    /* 48 */,
	20    /* 49 */,
	21    /* 50 */,
	21    /* 51 */,
	22    /* 52 */,
	22    /* 53 */,
	23    /* 54 */,
	23    /* 55 */,
	24    /* 56 */,
	24    /* 57 */,
	25    /* 58 */,
	25    /* 59 */,
	25    /* 60 */,
};


/*
 * Stat Table (STR) -- bonus to hit (plus 128)
 */
s16b adj_str_to_hit[STAT_TABLE_SIZE] =
{
	-3    /* 3 */,
	-2    /* 4 */,
	-1    /* 5 */,
	-1    /* 6 */,
	0     /* 7 */,
	0     /* 8 */,
	0     /* 9 */,
	0     /* 10 */,
	0     /* 11 */,
	0     /* 12 */,
	0     /* 13 */,
	0     /* 14 */,
	0     /* 15 */,
	0     /* 16 */,
	0     /* 17 */,
	1     /* 18 */,
	1     /* 19 */,
	1     /* 20 */,
	1     /* 21 */,
	1     /* 22 */,
	1     /* 23 */,
	1     /* 24 */,
	2     /* 25 */,
	3     /* 26 */,
	4     /* 27 */,
	5     /* 28 */,
	6     /* 29 */,
	7     /* 30 */,
	8     /* 31 */,
	9     /* 32 */,
	10    /* 33 */,
	11    /* 34 */,
	12    /* 35 */,
	13    /* 36 */,
	14    /* 37 */,
	15    /* 38 */,
	15    /* 39 */,
	16    /* 40 */,
	16    /* 41 */,
	16    /* 42 */,
	16    /* 43 */,
	16    /* 44 */,
	17    /* 45 */,
	17    /* 46 */,
	17    /* 47 */,
	17    /* 48 */,
	18    /* 49 */,
	18    /* 50 */,
	18    /* 51 */,
	18    /* 52 */,
	19    /* 53 */,
	19    /* 54 */,
	19    /* 55 */,
	19    /* 56 */,
	20    /* 57 */,
	20    /* 58 */,
	20    /* 59 */,
	20    /* 60 */,
};


/*
 * Stat Table (STR) -- weight limit in deca-pounds
 */
byte adj_str_carrying_weight[STAT_TABLE_SIZE] =
{
	10      /* 3 */,
	11      /* 4 */,
	12      /* 5 */,
	13      /* 6 */,
	14      /* 7 */,
	15      /* 8 */,
	16      /* 9 */,
	17      /* 10 */,
	18      /* 11 */,
	19      /* 12 */,
	20      /* 13 */,
	21      /* 14 */,
	22      /* 15 */,
	23      /* 16 */,
	24      /* 17 */,
	25      /* 18 */,
	26      /* 19 */,
	27      /* 20 */,
	28      /* 21 */,
	29      /* 22 */,
	30      /* 23 */,
	31      /* 24 */,
	31      /* 25 */,
	32      /* 26 */,
	32      /* 27 */,
	33      /* 28 */,
	33      /* 29 */,
	34      /* 30 */,
	34      /* 31 */,
	35      /* 32 */,
	35      /* 33 */,
	36      /* 34 */,
	36      /* 35 */,
	37      /* 36 */,
	37      /* 37 */,
	38      /* 38 */,
	38      /* 39 */,
	39      /* 40 */,
	39      /* 41 */,
	40      /* 42 */,
	40      /* 43 */,
	41      /* 44 */,
	41      /* 45 */,
	42      /* 46 */,
	42      /* 47 */,
	43      /* 48 */,
	43      /* 49 */,
	44      /* 50 */,
	44      /* 51 */,
	45      /* 52 */,
	45      /* 53 */,
	46      /* 54 */,
	46      /* 55 */,
	47      /* 56 */,
	47      /* 57 */,
	48      /* 58 */,
	49      /* 59 */,
	50      /* 60 */,
};


/*
 * Stat Table (STR) -- weapon weight limit in pounds
 */
byte adj_str_equipping_weight[STAT_TABLE_SIZE] =
{
	4      /* 3 */,
	5      /* 4 */,
	6      /* 5 */,
	7      /* 6 */,
	8      /* 7 */,
	9      /* 8 */,
	10     /* 9 */,
	11     /* 10 */,
	12     /* 11 */,
	13     /* 12 */,
	14     /* 13 */,
	15     /* 14 */,
	16     /* 15 */,
	17     /* 16 */,
	18     /* 17 */,
	19     /* 18 */,
	20     /* 19 */,
	21     /* 20 */,
	22     /* 21 */,
	23     /* 22 */,
	24     /* 23 */,
	25     /* 24 */,
	26     /* 25 */,
	27     /* 26 */,
	28     /* 27 */,
	30     /* 28 */,
	31     /* 29 */,
	32     /* 30 */,
	33     /* 31 */,
	34     /* 32 */,
	35     /* 33 */,
	37     /* 34 */,
	40     /* 35 */,
	44     /* 36 */,
	48     /* 37 */,
	50     /* 38 */,
	52     /* 39 */,
	54     /* 40 */,
	55     /* 41 */,
	56     /* 42 */,
	57     /* 43 */,
	58     /* 44 */,
	59     /* 45 */,
	60     /* 46 */,
	61     /* 47 */,
	62     /* 48 */,
	63     /* 49 */,
	64     /* 50 */,
	65     /* 51 */,
	66     /* 52 */,
	67     /* 53 */,
	68     /* 54 */,
	69     /* 55 */,
	70     /* 56 */,
	71     /* 57 */,
	72     /* 58 */,
	73     /* 59 */,
	74     /* 60 */,
};


/*
 * Stat Table (STR) -- digging value
 */
byte adj_str_dig[STAT_TABLE_SIZE] =
{
	0      /* 3 */, 
	0      /* 4 */, 
	1      /* 5 */, 
	2      /* 6 */, 
	3      /* 7 */, 
	4      /* 8 */, 
	4      /* 9 */, 
	5      /* 10 */, 
	5      /* 11 */, 
	6      /* 12 */, 
	6      /* 13 */, 
	7      /* 14 */, 
	7      /* 15 */, 
	8      /* 16 */, 
	8      /* 17 */, 
	9      /* 18 */, 
	10     /* 19 */, 
	12     /* 20 */, 
	15     /* 21 */, 
	20     /* 22 */, 
	25     /* 23 */, 
	30     /* 24 */, 
	35     /* 25 */, 
	40     /* 26 */, 
	45     /* 27 */, 
	50     /* 28 */, 
	55     /* 29 */, 
	60     /* 30 */, 
	65     /* 31 */, 
	70     /* 32 */, 
	75     /* 33 */, 
	80     /* 34 */, 
	85     /* 35 */, 
	90     /* 36 */, 
	95     /* 37 */, 
	100    /* 38 */, 
	100    /* 39 */, 
	100    /* 40 */, 
	100    /* 41 */, 
	100    /* 42 */, 
	100    /* 43 */, 
	100    /* 44 */, 
	100    /* 45 */, 
	100    /* 46 */, 
	100    /* 47 */, 
	100    /* 48 */, 
	100    /* 49 */, 
	100    /* 50 */, 
	100    /* 51 */, 
	100    /* 52 */, 
	100    /* 53 */, 
	100    /* 54 */, 
	100    /* 55 */, 
	100    /* 56 */, 
	100    /* 57 */, 
	100    /* 58 */, 
	100    /* 59 */, 
	100    /* 60 */, 
};


/*
 * Stat Table (STR) -- help index into the "blow" table
 */
byte adj_str_blow[STAT_TABLE_SIZE] =
{
	3     /* 3 */, 
	4     /* 4 */, 
	5     /* 5 */, 
	6     /* 6 */, 
	7     /* 7 */, 
	8     /* 8 */, 
	9     /* 9 */, 
	10    /* 10 */, 
	11    /* 11 */, 
	12    /* 12 */, 
	13    /* 13 */, 
	14    /* 14 */, 
	15    /* 15 */, 
	16    /* 16 */, 
	17    /* 17 */, 
	20    /* 18 */, 
	30    /* 19 */, 
	40    /* 20 */, 
	50    /* 21 */, 
	60    /* 22 */, 
	70    /* 23 */, 
	80    /* 24 */, 
	90    /* 25 */, 
	100   /* 26 */, 
	110   /* 27 */, 
	120   /* 28 */, 
	130   /* 29 */, 
	140   /* 30 */, 
	150   /* 31 */, 
	160   /* 32 */, 
	170   /* 33 */, 
	180   /* 34 */, 
	190   /* 35 */, 
	200   /* 36 */, 
	210   /* 37 */, 
	220   /* 38 */, 
	230   /* 39 */, 
	240   /* 40 */, 
	240   /* 41 */, 
	240   /* 42 */, 
	240   /* 43 */, 
	240   /* 44 */, 
	240   /* 45 */, 
	240   /* 46 */, 
	240   /* 47 */, 
	240   /* 48 */, 
	240   /* 49 */, 
	240   /* 50 */, 
	240   /* 51 */, 
	240   /* 52 */, 
	240   /* 53 */, 
	240   /* 54 */, 
	240   /* 55 */, 
	240   /* 56 */, 
	240   /* 57 */, 
	240   /* 58 */, 
	240   /* 59 */, 
	240   /* 60 */, 
};


/*
 * Stat Table (DEX) -- index into the "blow" table
 */
byte adj_dex_blow[STAT_TABLE_SIZE] =
{
	0      /* 3 */, 
	0      /* 4 */, 
	0      /* 5 */, 
	0      /* 6 */, 
	0      /* 7 */, 
	0      /* 8 */, 
	0      /* 9 */, 
	1      /* 10 */, 
	1      /* 11 */, 
	1      /* 12 */, 
	1      /* 13 */, 
	1      /* 14 */, 
	2      /* 15 */, 
	2      /* 16 */, 
	2      /* 17 */, 
	2      /* 18 */, 
	3      /* 19 */, 
	3      /* 20 */, 
	3      /* 21 */, 
	4      /* 22 */, 
	4      /* 23 */, 
	5      /* 24 */, 
	5      /* 25 */, 
	6      /* 26 */, 
	6      /* 27 */, 
	7      /* 28 */, 
	7      /* 29 */, 
	8      /* 30 */, 
	8      /* 31 */, 
	9      /* 32 */, 
	9      /* 33 */, 
	10     /* 34 */, 
	10     /* 35 */, 
	11     /* 36 */, 
	11     /* 37 */, 
	12     /* 38 */, 
	12     /* 39 */, 
	13     /* 40 */, 
	13     /* 41 */, 
	13     /* 42 */, 
	13     /* 43 */, 
	13     /* 44 */, 
	13     /* 45 */, 
	13     /* 46 */, 
	13     /* 47 */, 
	13     /* 48 */, 
	13     /* 49 */, 
	13     /* 50 */, 
	13     /* 51 */, 
	13     /* 52 */, 
	13     /* 53 */, 
	13     /* 54 */, 
	13     /* 55 */, 
	13     /* 56 */, 
	13     /* 57 */, 
	13     /* 58 */, 
	13     /* 59 */, 
	13     /* 60 */, 
};


/*
 * Stat Table (DEX) -- chance of avoiding "theft" and "falling"
 */
byte adj_dex_safe[STAT_TABLE_SIZE] =
{
	0       /* 3 */, 
	1       /* 4 */, 
	2       /* 5 */, 
	3       /* 6 */, 
	4       /* 7 */, 
	5       /* 8 */, 
	5       /* 9 */, 
	6       /* 10 */, 
	6       /* 11 */, 
	7       /* 12 */, 
	7       /* 13 */, 
	8       /* 14 */, 
	8       /* 15 */, 
	9       /* 16 */, 
	9       /* 17 */, 
	10      /* 18 */, 
	10      /* 19 */, 
	15      /* 20 */, 
	15      /* 21 */, 
	20      /* 22 */, 
	25      /* 23 */, 
	30      /* 24 */, 
	35      /* 25 */, 
	40      /* 26 */, 
	45      /* 27 */, 
	50      /* 28 */, 
	60      /* 29 */, 
	70      /* 30 */, 
	80      /* 31 */, 
	90      /* 32 */, 
	100     /* 33 */, 
	100     /* 34 */, 
	100     /* 35 */, 
	100     /* 36 */, 
	100     /* 37 */, 
	100     /* 38 */, 
	100     /* 39 */, 
	100     /* 40 */, 
	100     /* 41 */, 
	100     /* 42 */, 
	100     /* 43 */, 
	100     /* 44 */, 
	100     /* 45 */, 
	100     /* 46 */, 
	100     /* 47 */, 
	100     /* 48 */, 
	100     /* 49 */, 
	100     /* 50 */, 
	100     /* 51 */, 
	100     /* 52 */, 
	100     /* 53 */, 
	100     /* 54 */, 
	100     /* 55 */, 
	100     /* 56 */, 
	100     /* 57 */, 
	100     /* 58 */, 
	100     /* 59 */, 
	100     /* 60 */, 
};


/*
 * Stat Table (CON) -- base regeneration rate
 */
byte adj_con_fix[STAT_TABLE_SIZE] =
{
	0       /* 3 */, 
	0       /* 4 */, 
	0       /* 5 */, 
	0       /* 6 */, 
	0       /* 7 */, 
	0       /* 8 */, 
	0       /* 9 */, 
	0       /* 10 */, 
	0       /* 11 */, 
	0       /* 12 */, 
	0       /* 13 */, 
	1       /* 14 */, 
	1       /* 15 */, 
	1       /* 16 */, 
	1       /* 17 */, 
	2       /* 18 */, 
	2       /* 19 */, 
	2       /* 20 */, 
	2       /* 21 */, 
	2       /* 22 */, 
	3       /* 23 */, 
	3       /* 24 */, 
	3       /* 25 */, 
	3       /* 26 */, 
	3       /* 27 */, 
	4       /* 28 */, 
	4       /* 29 */, 
	5       /* 30 */, 
	6       /* 31 */, 
	6       /* 32 */, 
	7       /* 33 */, 
	7       /* 34 */, 
	8       /* 35 */, 
	8       /* 36 */, 
	8       /* 37 */, 
	9       /* 38 */, 
	9       /* 39 */, 
	9       /* 40 */, 
	9       /* 41 */, 
	9       /* 42 */, 
	9       /* 43 */, 
	9       /* 44 */, 
	9       /* 45 */, 
	9       /* 46 */, 
	9       /* 47 */, 
	9       /* 48 */, 
	9       /* 49 */, 
	9       /* 50 */, 
	9       /* 51 */, 
	9       /* 52 */, 
	9       /* 53 */, 
	9       /* 54 */, 
	9       /* 55 */, 
	9       /* 56 */, 
	9       /* 57 */, 
	9       /* 58 */, 
	9       /* 59 */, 
	9       /* 60 */, 
};


/*
 * Stat Table (CON) -- extra 1/4-hitpoints per level (plus 128)
 */
STAT adj_con_mhp[STAT_TABLE_SIZE] =
{
	-16 /* 3.0 */,
	-14 /* 4.0 */,
	-12 /* 5.0 */,
	-10/* 6.0 */,
	-8 /* 7.0 */,
	-6 /* 8.0 */,
	-4 /* 9.0 */,
	-2 /* 10.0 */,
	0 /* 11.0 */,
	0 /* 12.0 */,
	0 /* 13.0 */,
	1 /* 14.0 */,
	1 /* 15.0 */,
	2 /* 16.0 */,
	3 /* 17.0 */,
	4 /* 18.0 */,
	5 /* 19.0 */,
	6 /* 20.0 */,
	7 /* 21.0 */,
	8 /* 22.0 */,
	9 /* 23.0 */,
	10 /* 24.0 */,
	11 /* 25.0 */,
	12 /* 26.0 */,
	13 /* 27.0 */,
	14 /* 28.0 */,
	15 /* 29.0 */,
	17 /* 30.0 */,
	19 /* 31.0 */,
	21 /* 32.0 */,
	23 /* 33.0 */,
	25 /* 34.0 */,
	27 /* 35.0 */,
	29 /* 36.0 */,
	31 /* 37.0 */,
	33 /* 38.0 */,
	35 /* 39.0 */,
	37 /* 40.0 */,
	39 /* 41.0 */,
	42 /* 42.0 */,
	45 /* 43.0 */,
	48 /* 44.0 */,
	51 /* 45.0 */,
	54 /* 46.0 */,
	57 /* 47.0 */,
	60 /* 48.0 */,
	63 /* 49.0 */,
	66 /* 50.0 */,
	69 /* 51.0 */,
	72 /* 52.0 */,
	75 /* 53.0 */,
	78 /* 54.0 */,
	81 /* 55.0 */,
	85 /* 56.0 */,
	89 /* 57.0 */,
	94 /* 58.0 */,
	99 /* 59.0 */,
	104 /* 60.0 */
};

/*
 * Stat Table (DR) -- plus maximum of hit dices)
 */
int adj_dr_mhp[DR_TABLE_SIZE] =
{
	1, /* 0 */

	2,
	2,
	3,
	3,
	4, /* 5 */

	8,
	9,
	10,
	11,
	12, /* 10 */

	17,
	18,
	19,
	20,
	21, /* 15 */

	26,
	27,
	28,
	29,
	30, /* 20 */

	41,
	42,
	43,
	44,
	45, /* 25 */

	61,
	62,
	63,
	64,
	65, /* 30 */

};

/*
 * Stat Table (DR) -- plus speed)
 */
s16b adj_dr_speed[DR_TABLE_SIZE] =
{
	0, /* 0 */

	2,
	2,
	2,
	2,
	3, /* 5 */

	4,
	4,
	4,
	4,
	5, /* 10 */

	6,
	6,
	7,
	7,
	8, /* 15 */

	10,
	11,
	12,
	13,
	14, /* 20 */

	17,
	18,
	19,
	20,
	21, /* 25 */

	25,
	26,
	27,
	28,
	30, /* 30 */

};


/*
 * Stat Table (DR) -- plus AC)
 */
int adj_dr_saving[DR_TABLE_SIZE] =
{
	10, /* 0 */

	12,
	14,
	16,
	18,
	20, /* 5 */

	32,
	34,
	36,
	38,
	40, /* 10 */

	52,
	54,
	56,
	58,
	60, /* 15 */

	72,
	74,
	76,
	78,
	80, /* 20 */

	92,
	94,
	96,
	98,
	100, /* 25 */

	120,
	130,
	140,
	150,
	160, /* 30 */

};



/*
 * Stat Table (CHR) -- charm
 */
byte adj_chr_chm[STAT_TABLE_SIZE] =
{
	0      /* 3 */, 
	0      /* 4 */, 
	1      /* 5 */, 
	2      /* 6 */, 
	3      /* 7 */, 
	4      /* 8 */, 
	4      /* 9 */, 
	5      /* 10 */, 
	5      /* 11 */, 
	6      /* 12 */, 
	6      /* 13 */, 
	7      /* 14 */, 
	7      /* 15 */, 
	8      /* 16 */, 
	8      /* 17 */, 
	9      /* 18 */, 
	10     /* 19 */, 
	12     /* 20 */, 
	15     /* 21 */, 
	18     /* 22 */, 
	21     /* 23 */, 
	24     /* 24 */, 
	28     /* 25 */, 
	32     /* 26 */, 
	36     /* 27 */, 
	39     /* 28 */, 
	42     /* 29 */, 
	45     /* 30 */, 
	49     /* 31 */, 
	53     /* 32 */, 
	57     /* 33 */, 
	61     /* 34 */, 
	65     /* 35 */, 
	69     /* 36 */, 
	73     /* 37 */, 
	77     /* 38 */, 
	81     /* 39 */, 
	85     /* 40 */, 
	85     /* 41 */, 
	85     /* 42 */, 
	85     /* 43 */, 
	85     /* 44 */, 
	85     /* 45 */, 
	85     /* 46 */, 
	85     /* 47 */, 
	85     /* 48 */, 
	85     /* 49 */, 
	85     /* 50 */, 
	85     /* 51 */, 
	85     /* 52 */, 
	85     /* 53 */, 
	85     /* 54 */, 
	85     /* 55 */, 
	85     /* 56 */, 
	85     /* 57 */, 
	85     /* 58 */, 
	85     /* 59 */, 
	85     /* 60 */, 
};