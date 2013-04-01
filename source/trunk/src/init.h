/* File: init.h */

/*
 * Copyright (c) 2000 Robert Ruehlmann
 *
 * This software may be copied and distributed for educational, research,
 * and not for profit purposes provided that this copyright and statement
 * are included in all such copies.
 */

#ifndef INCLUDED_INIT_H
#define INCLUDED_INIT_H

#include "h-basic.h"



typedef struct header header;

typedef errr (*parse_info_txt_func)(char *buf, header *head);
typedef errr (*parse_reprocess_func)(header *head);

/*
 * Template file header information (see "init.c").  16 bytes.
 *
 * Note that the sizes of many of the "arrays" are between 32768 and
 * 65535, and so we must use "unsigned" values to hold the "sizes" of
 * these arrays below.  Normally, I try to avoid using unsigned values,
 * since they can cause all sorts of bizarre problems, but I have no
 * choice here, at least, until the "race" array is split into "normal"
 * and "unique" creatures, which may or may not actually help.
 *
 * Note that, on some machines, for example, the Macintosh, the standard
 * "read()" and "write()" functions cannot handle more than 32767 bytes
 * at one time, so we need replacement functions, see "util.c" for details.
 *
 * Note that, on some machines, for example, the Macintosh, the standard
 * "malloc()" function cannot handle more than 32767 bytes at one time,
 * but we may assume that the "ralloc()" function can handle up to 65535
 * butes at one time.  We should not, however, assume that the "ralloc()"
 * function can handle more than 65536 bytes at a time, since this might
 * result in segmentation problems on certain older machines, and in fact,
 * we should not assume that it can handle exactly 65536 bytes at a time,
 * since the internal functions may use an unsigned short to specify size.
 *
 * In general, these problems occur only on machines (such as most personal
 * computers) which use 2 byte "int" values, and which use "int" for the
 * arguments to the relevent functions.
 * 
 * 2012 Deskull added tmporary buffer
 */
struct header
{
	byte v_major;		// Version -- major
	byte v_minor;		// Version -- minor
	byte v_patch;		// Version -- patch
	byte v_extra;		// Version -- extra

	s32b info_num;		// Number of "info" records
	s32b info_len;		// Size of each "info" record
	s32b head_size;		// Size of the "header" in bytes
	s32b info_size;		// Size of the "info" array in bytes
	s32b name_size;		// Size of the "name" array in bytes
	s32b text_size;		// Size of the "text" array in bytes
	s32b tmp_size;		// Size of the "tmp" array in bytes
	s32b tag_size;		// Size of the "tag" array in bytes
	void *object_ptr;
	char *name_ptr;
	char *text_ptr;
	char *tmp_ptr;
	char *tag_ptr;

	parse_info_txt_func parse_info_txt;
	parse_reprocess_func parse_reprocess;

	void (*retouch)(header *head);
};

extern cptr err_str[PARSE_ERROR_MAX];

extern errr get_split_offset(int *split_offset, int *split_size, char *buf, int field_num, char delimiter, char enclosure);

extern errr init_info_txt(FILE *fp, char *buf, header *head, parse_info_txt_func parse_info_txt_line);
extern errr init_info_csv(FILE *fp, char *buf, header *head, parse_info_txt_func parse_info_txt_line);

extern errr parse_z_info(char *buf, header *head);
extern errr parse_vault_info(char *buf, header *head);
extern errr parse_feature_info(char *buf, header *head);
extern void retouch_feature_info(header *head);
extern errr parse_object_kind_info(char *buf, header *head);
extern errr parse_a_info(char *buf, header *head);
extern errr parse_e_info(char *buf, header *head);
extern errr parse_re_info(char *buf, header *head);
extern errr parse_dungeon_info(char *buf, header *head);
extern errr parse_dungeon_info_csv(char *buf, header *head);
extern errr parse_skill_info(char *buf, header *head);
extern errr parse_magic_info(char *buf, header *head);

extern errr parse_species_info_csv(char *buf, header *head);

extern errr parse_race_info_csv(char *buf, header *head);
extern errr parse_authority_info_csv(char *buf, header *head);
extern errr parse_class_info_csv(char *buf, header *head);
extern errr parse_chara_info_csv(char *buf, header *head);

extern errr parse_trait_csv(char *buf, header *head);
extern errr reprocess_trait(header *head);

extern errr parse_store_pre_info_csv(char *buf, header *head);

extern errr parse_object_kind_csv(char *buf, header *head);
extern errr parse_object_ego_csv(char *buf, header *head);
extern errr parse_artifact_csv(char *buf, header *head);

/*
 * Error tracking
 */
extern int error_idx;
extern int error_line;


/*
 * File headers
 */
extern header z_head;
extern header v_head;
extern header f_head;
extern header object_kind_head;
extern header artifact_head;
extern header object_ego_head;
extern header species_head;
extern header p_head;
extern header h_head;
extern header b_head;
extern header g_head;

#endif /* INCLUDED_INIT_H */
