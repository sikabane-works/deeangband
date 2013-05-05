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
#include "files.h"
#include "mutation.h"


bool get_individual_trait(creature_type *creature_ptr, TRAIT_ID choose_mut, bool message)
{
	add_flag(creature_ptr->mutative_trait, choose_mut);
	if(message) msg_print(trait_name + trait_info[choose_mut].get_text);
	return TRUE;
}


bool lose_individual_trait(creature_type *creature_ptr, TRAIT_ID choose_mut, bool message)
{
	remove_flag(creature_ptr->mutative_trait, choose_mut);
	if(message) msg_print(trait_name + trait_info[choose_mut].lost_text);
	return TRUE;
}


void dump_traits(creature_type *creature_ptr, FILE *OutFile)
{
	if(!is_valid_creature(creature_ptr)) return;
	if(!OutFile) return;

	//TODO
}

void remove_all_mutative_traits(creature_type *creature_ptr)
{
	int i;

#ifdef JP
	msg_print("‘S‚Ä‚Ì“Ë‘R•ÏˆÙ‚ª¡‚Á‚½B");
#else
	msg_print("You are cured of all mutations.");
#endif

	for(i = 0; i < MAX_TRAITS_FLAG; i++) creature_ptr->mutative_trait[i] = 0L;
	prepare_update(creature_ptr, CRU_BONUS);
	creature_ptr->regenerate_mod = calc_regenerate_mod(creature_ptr);
	handle_stuff(creature_ptr);
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

static int count_mutations(creature_type *creature_ptr)
{
	if(!is_valid_creature(creature_ptr)) return 0;
	//TODO
	return 0;
}

// Return the modifier to the regeneration rate
// (in percent)
int calc_regenerate_mod(creature_type *creature_ptr)
{
	int regen, mod;
	int count = count_mutations(creature_ptr);

	// Kaleidoscopic race get 10 "free" mutations and
	// only 5% decrease per additional mutation
	if(has_trait(creature_ptr, TRAIT_KALEIDOSCOPIC_RACE))
	{
		count -= 10;
		mod = 5;
	}
	else mod = 10;

	if(count <= 0) return 100;	// No negative modifier

	regen = 100 - count * mod;
	if(regen < 10) regen = 10;	// Max. 90% decrease in regeneration speed

	return (regen);
}

bool get_mutative_trait(creature_type *creature_ptr, TRAIT_ID trait_id, bool message)
{
	add_flag(creature_ptr->mutative_trait, trait_id);
	if(message) msg_print(trait_name + trait_info[trait_id].get_text);
	return TRUE;
}

bool lose_mutative_trait(creature_type *creature_ptr, TRAIT_ID trait_id, bool message)
{
	remove_flag(creature_ptr->mutative_trait, trait_id);
	if(message) msg_print(trait_name + trait_info[trait_id].lost_text);
	return TRUE;
}
