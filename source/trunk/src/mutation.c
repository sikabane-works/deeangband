/* File: mutation.c */

/*
 * Copyright (c) 1997 Ben Harrison, James E. Wilson, Robert A. Koeneke
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.  Other copyrights may also apply.
 */

/* Purpose: Mutation effects (and racial powers) */

#include "angband.h"


bool gain_trait(creature_type *creature_ptr, int choose_mut, bool messsage)
{
	//TODO
	return TRUE;
}


bool lose_trait(creature_type *creature_ptr, int choose_mut)
{
	//TODO
	return TRUE;
}


void dump_traits(creature_type *creature_ptr, FILE *OutFile)
{
	if(!OutFile) return;

	//TODO
}

void remove_all_acquired_traits(creature_type *creature_ptr)
{
	int i;

#ifdef JP
	msg_print("‘S‚Ä‚Ì“Ë‘R•ÏˆÙ‚ª¡‚Á‚½B");
#else
	msg_print("You are cured of all mutations.");
#endif

	for(i = 0; i < TRAIT_FLAG_MAX; i++) creature_ptr->acquired_trait[i] = 0L;
	creature_ptr->creature_update |= CRU_BONUS;
	creature_ptr->regenerate_mod = calc_regenerate_mod(creature_ptr);
	handle_stuff();
	return;
}


// List traits_precondition we have...
void do_cmd_knowledge_traits(creature_type *creature_ptr)
{
	FILE *fff;
	char file_name[1024];

	/* Open a new file */
	fff = my_fopen_temp(file_name, 1024);

	/* Dump the mutations to file */
	if(fff) dump_traits(creature_ptr, fff);

	/* Close the file */
	my_fclose(fff);

	/* Display the file contents */
#ifdef JP
    show_file(TRUE, file_name, "“Á«", 0, 0);
#else
	show_file(TRUE, file_name, "Trait", 0, 0);
#endif

	/* Remove the file */
	fd_kill(file_name);
}


int count_bits(u32b x)
{
	int n = 0;

	if(x) do
	{
		n++;
	}
	while (0 != (x = x&(x-1)));

	return (n);
}


static int count_mutations(creature_type *creature_ptr)
{
	//TODO
	return 0;
}


/*
 * Return the modifier to the regeneration rate
 * (in percent)
 */
int calc_regenerate_mod(creature_type *creature_ptr)
{
	int regen;
	int mod = 10;
	int count = count_mutations(creature_ptr);

	/*
	 * Beastman get 10 "free" mutations and
	 * only 5% decrease per additional mutation
	 */

	if(creature_ptr->chara_idx == CHARA_LUCKY) count--;
	if(has_trait(creature_ptr, TRAIT_KALEIDOSCOPIC_RACE))
	{
		count -= 10;
		mod = 5;
	}

	/* No negative modifier */
	if(count <= 0) return 100;

	regen = 100 - count * mod;

	/* Max. 90% decrease in regeneration speed */
	if(regen < 10) regen = 10;

	return (regen);
}


void mutation_stop_mouth(creature_type *creature_ptr)
{
	if(MUSIC_SINGING_ANY(creature_ptr)) stop_singing(creature_ptr);
	if(HEX_SPELLING_ANY(creature_ptr)) stop_hex_spell_all(creature_ptr);
}

void get_acquired_trait(creature_type *creature_ptr, int trait)
{
	add_flag(creature_ptr->acquired_trait, trait);
}

void lose_acquired_trait(creature_type *creature_ptr, int trait)
{
	remove_flag(creature_ptr->acquired_trait, trait);
}
