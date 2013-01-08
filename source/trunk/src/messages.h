
// Keywords

#ifdef JP
#define KEYWORD_CURSED "����Ă���"
#else
#define KEYWORD_CURSED "cursed"
#endif


#ifdef JP

#define SYS_MESSAGE_FAILED_TEMPFILE "�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B"
#define SYS_MESSAGE_FAILED_FILEOPEN "�t�@�C�����J���܂���B"
#define SYS_MESSAGE_FAILED_FILEOPEN2 "%s ���J�����Ƃ��ł��܂���ł����B"
#define SYS_MESSAGE_REPLACE_FILE "��������t�@�C���ɏ㏑�����܂���? (%s)"
#define SYS_MESSAGE_HIT_ANY_KEY "�����L�[�������ĉ������B"
#define SYS_MESSAGE_OPTION_AUX "�����I�v�V����((*)�̓X�R�A�ɉe��)"
#define SYS_MESSAGE_OPTION_AUX2 "�����I�v�V����(�Q�Ƃ̂�)"
#define SYS_MESSAGE_SAVE_FAILED "�Z�[�u���s�I"
#define SYS_MESSAGE_SCORE_FAILED "�X�R�A�E�t�@�C�����g�p�ł��܂���B"
#define SYS_MESSAGE_PANIC_SAVE_SUCCEED "�ً}�Z�[�u�����I"
#define SYS_MESSAGE_PANIC_SAVE_FAILED "�ً}�Z�[�u���s�I"
#define SYS_MESSAGE_YOUR_TRAITS "���Ȃ��ɂ͈ȉ��̓���������"
#define SYS_MESSAGE_YOUR_NO_TRAIT "���Ȃ��ɂ͓��ʂȓ������Ȃ�"
#define SYS_MESSAGE_SERVANTS "%s�ɂ�%d��ށA%d�̂̏]�l������"
#define SYS_MESSAGE_NO_SERVANT "%s�ɂ͏]�l�����Ȃ�"
#define SYS_MESSAGE_UNKNOWN_SYMBOL "�����ȕ���"
#define SYS_MESSAGE_ERROR "�G���["
#define SYS_MESSAGE_TODAY "����"
#define SYS_MESSAGE_NORTH "�k"
#define SYS_MESSAGE_SOUTH "��"
#define SYS_MESSAGE_EAST "��"
#define SYS_MESSAGE_WEST "��"
#define SYS_MESSAGE_MONEY "������"
#define SYS_MESSAGE_WEIGHT "�d��"

#define SYS_MESSAGE_MODIFICATION "�\�͏C��"

#define SYS_MESSAGE_VISUAL_COMMAND "�R�}���h (n/N/^N/a/A/^A/c/C/^C/v/V/^V): "
#else

#define SYS_MESSAGE_FAILED_TEMPFILE "Failed to create temporary file %s."
#define SYS_MESSAGE_FAILED_FILEOPEN "Can not open file."
#define SYS_MESSAGE_FAILED_FILEOPEN2 "Failed to open %s."
#define SYS_MESSAGE_REPLACE_FILE "Replace existing file %s? "
#define SYS_MESSAGE_HIT_ANY_KEY "Hit Any Key�B"
#define SYS_MESSAGE_OPTION_AUX "Birth Option((*)s effect score)"
#define SYS_MESSAGE_OPTION_AUX2 "Birth Options(browse only)"
#define SYS_MESSAGE_SAVE_FAILED "death save failed!"
#define SYS_MESSAGE_SCORE_FAILED "Score file unavailable."
#define SYS_MESSAGE_PANIC_SAVE_SUCCEED "Panic save succeeded!"
#define SYS_MESSAGE_PANIC_SAVE_FAILED "panic save failed!"
#define SYS_MESSAGE_YOUR_TRAITS "You have these trait..."
#define SYS_MESSAGE_YOUR_NO_TRAIT "You have no trait"
#define SYS_MESSAGE_YOUR_SERVANT "%s have no servant"
#define SYS_MESSAGE_YOUR_NO_SERVANT "%s have %d species, %d servants"
#define SYS_MESSAGE_UNKNOWN_SYMBOL "unknown symbol"
#define SYS_MESSAGE_ERROR "Error"
#define SYS_MESSAGE_TODAY "Today"
#define SYS_MESSAGE_NORTH " North"
#define SYS_MESSAGE_SOUTH " South"
#define SYS_MESSAGE_EAST " East"
#define SYS_MESSAGE_WEST " West"
#define SYS_MESSAGE_MONEY "AU"
#define SYS_MESSAGE_WEIGHT "Weight"

#define SYS_MESSAGE_MODIFICATION "Modification"

#define SYS_MESSAGE_VISUAL_COMMAND "Command (n/N/^N/a/A/^A/c/C/^C/v/V/^V):"
#endif
 
#if JP
#define KW_NAME "���O"
#define KW_FAIL "����"
#define KW_UNKNOWN "���m"
#define KW_FORGOTTEN "�Y�p"
#define KW_UNTRIED "���o��"
#define KW_CARRYING_WEIGHT "�����d��"
#define KW_EQUIPMENT_WEIGHT "�����d��"
#define KW_CREATURE_PROFILE "�N���[�`���[�̃v���t�B�[��"
#else
#define KW_NAME "Name"
#define KW_FAIL "Fail"
#define KW_UNKNOWN "Unknown"
#define KW_FORGOTTEN "Forgotten"
#define KW_UNTRIED "Untried"
#define KW_CARRYING_WEIGHT "Carrying Weight"
#define KW_EQUIPMENT_WEIGHT "Equipment Weight"
#define KW_CREATURE_PROFILE "Creature Profile"
#endif

#if JP
#define GAME_MESSAGE_COMPLETE_QUEST "�N�G�X�g��B�������B"
#define GAME_MESSAGE_IMPROVEMENT_FAILED "���ǂɎ��s�����B"""
#define GAME_MESSAGE_RESIST_THE_EFFECT "���������͂𒵂˕Ԃ����B"
#define GAME_MESSAGE_IS_IMMUNE "�ɂ͊��S�ȑϐ�������B"
#define GAME_MESSAGE_RESISTED "�ɂ͑ϐ�������B"
#define GAME_MESSAGE_IS_UNAFFECTED "�ɂ͌��ʂ��Ȃ������B"
#define GAME_MESSAGE_IS_CONFUSED "�͖ڂ������Ȃ�"
#define GAME_MESSAGE_IS_BLIND "�͍������Ă���B"
#define GAME_MESSAGE_CREATURE_IN_THE_WAY "�N���[�`���[�������ӂ������Ă���B"
#define GAME_MESSAGE_MANA_RECOVERLY "�����n�b�L���Ƃ����B"
#define GAME_MESSAGE_SUCCESS_PICKING "�����͂������B"
#define GAME_MESSAGE_FAILED_PICKING "�����͂����Ȃ������B"
#define GAME_MESSAGE_FOUND_SOMETHING "�����𔭌������I"

#define GAME_MESSAGE_HEAR_NOISE "���������������B"

#define GAME_MESSAGE_NO_STUDY_SPELL "�ǂ߂�������Ȃ��B"
#define GAME_MESSAGE_NO_STUDY_TECH "�w�ׂ�Z���Ȃ��B"

#define GAME_MESSAGE_BLACK_AURA_TO_OBJECT "���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I"
#define GAME_MESSAGE_REMOVED_OBJECT_CURSE "�N���Ɍ�����Ă���悤�ȋC������B"
#define GAME_MESSAGE_COMBAT_TALK_STAIR "�u�Ȃ񂾂��̊K�i�́I�H�v"
#define GAME_MESSAGE_PERILOUS_SECRET "�΂𐧌�ł��Ȃ��B"
#define GAME_MESSAGE_RESISTED_FEAR "���������|�ɐN����Ȃ������B"
#define GAME_MESSAGE_NO_MONEY "����������܂���B"
#define GAME_MESSAGE_NO_MONEY_INSULT "�����I�����Ȃ�����Ȃ����I�o�Ă����I"
#define GAME_MESSAGE_BOUNTY_1 "%s ���������܂����H"

#define GAME_MESSAGE_TRAP_WARNING "* ����:���̐�̓g���b�v�̊��m�͈͊O�ł��I *"
#define GAME_MESSAGE_TRAP_DESTROY "�g���b�v�𕲍ӂ����B"

#define GAME_MESSAGE_TRAP_NEEDLE "�d�|�����Ă��������Ȑj�Ɏh����Ă��܂����I"
#define GAME_MESSAGE_TRAP_DARTS "�����ȃ_�[�c�����ł��Ďh�������I"
#define GAME_MESSAGE_TRAP_DARTS_MISSED "�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B"
#define GAME_MESSAGE_TRAP_FIRE "���ɕ�܂ꂽ�I"
#define GAME_MESSAGE_TRAP_ACID "�_������������ꂽ�I"
#define GAME_MESSAGE_TRAP_ACID_FLOW "�ˑR������_�����o�����I"
#define GAME_MESSAGE_TRAP_POISON_FLOW "�ˑR������ŉt�����o�����I"
#define GAME_MESSAGE_TRAP_PIRANHAS "�ˑR�ǂ��琅�����o�����I�s���j�A������I"

#ifdef JP
#define GAME_MESSAGE_TRAP_BLIND "�����K�X�ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_CONFUSE "����߂��K�X�ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_POISON "�h���I�ȗΐF�̃K�X�ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_SLEPT "��Ȕ������ɕ�܂ꂽ�I"
#else
#define GAME_MESSAGE_TRAP_BLIND "A black gas surrounds you!"
#define GAME_MESSAGE_TRAP_CONFUSE "A gas of scintillating colors surrounds you!"
#define GAME_MESSAGE_TRAP_POISON "A pungent green gas surrounds you!"
#define GAME_MESSAGE_TRAP_SLEPT "A strange white mist surrounds you!"
#endif

#define GAME_MESSAGE_TRAP_POISONOUS_GAS "�˔@�����o�����ΐF�̃K�X�ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_PARALYZE_GAS "�˔@�����o�������F���K�X�ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_SUMMONING "�˔@�����o�������ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_S_ELEMENTAL "�����邽�߂ɃG�������^�������ꂽ�I"
#define GAME_MESSAGE_TRAP_S_BIRD "���̌Q�ꂪ���Ȃ�����芪�����I"
#define GAME_MESSAGE_TRAP_S_H_DEMON "���Ɨ����̉_�̒��Ɉ������p���������I"
#define GAME_MESSAGE_TRAP_S_H_DRAGON "�ÈłɃh���S���̉e���ڂ���ƌ��ꂽ�I"
#define GAME_MESSAGE_TRAP_S_HYBRID "��Ȏp�̉������P���ė����I"
#define GAME_MESSAGE_TRAP_S_VORTEX "�Q�������̂��A�j�􂵂��I"
#define GAME_MESSAGE_TRAP_E_RUNE "���낵������������:  �u�Èł������܂�I�v"
#define GAME_MESSAGE_TRAP_EXPLOSIVE "�ˑR�A�������������I���̒��̕��͂��ׂĕ��X�ɍӂ��U�����I"
#define GAME_MESSAGE_TRAP_SCATTER "�󔠂̒��g�̓_���W�������イ�ɎU�������I"
#define GAME_MESSAGE_TRAP_TELEPORT "�e���|�[�g�E�g���b�v�ɂЂ����������I"
#define GAME_MESSAGE_TRAP_TY_CURSE "�������s�J�b�ƌ������I"
#define GAME_MESSAGE_TRAP_ARMAGEDDON "�ˑR�V�E�̐푈�Ɋ������܂ꂽ�I"
#define GAME_MESSAGE_TRAP_ALARM "�������܂��������苿�����I"

#define GAME_MESSAGE_SEEM_TO_CURSE "�Ӂ[�ށA�ǂ�������Ă���悤���B"
#define GAME_MESSAGE_PREVENT_BY_RIDING "��n���ɂ͏o���Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_CONFUSION "������Ԃŏ�肭�o���Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_BLIND "�Ⴊ�����Ȃ��ďo���Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_NO_LITE "�����肪�Ȃ��ďo���Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_HALLUCINATION "���o�ɑj�܂�Ăł��Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_STUNED "�����N�O�Ƃ��Ă��ďW���ł��Ȃ��B"
#define GAME_MESSAGE_IMPROVE_LIMIT "���̃A�C�e���͂���ȏ���ǂł��Ȃ��B"
#define GAME_MESSAGE_NO_HAPPEN "�����N����Ȃ������B"

#define GAME_MESSAGE_FIND_TRAP "�g���b�v�𔭌������B"
#define GAME_MESSAGE_FIND_DOOR "�B���h�A�𔭌������B"

#define GAME_MESSAGE_TELEPORT_PREVENTED "�s�v�c�ȗ͂��e���|�[�g��h�����I"

#define GAME_MESSAGE_PATTERN_LIMIT1 "�p�^�[���̏������n�߂�ƁA�S�Ă�����Ȃ���΂Ȃ�܂���B�����ł����H"
#define GAME_MESSAGE_PATTERN_LIMIT2 "�p�^�[���̏������ɂ̓X�^�[�g�n�_��������n�߂Ȃ��Ă͂Ȃ�܂���B"
#define GAME_MESSAGE_PATTERN_LIMIT3 "�p�^�[���̏�͐����������ŕ����˂΂Ȃ�܂���B"
#define GAME_MESSAGE_PATTERN_LIMIT4 "�p�^�[���𓥂݊O���Ă͂����܂���B"

#define GAME_MESSAGE_ARENA_LIMIT "�A���[�i�����@���z�������I"
#define GAME_MESSAGE_ARENA_BERSERK "����m�����Ă��ē������Ȃ��I"

#define GAME_MESSAGE_SEE_OBJECT "%s������B"
#define GAME_MESSAGE_GET_MONEY " $%ld �̉��l������%s���������B"
#define GAME_MESSAGE_PACK_NO_ROOM "�U�b�N�ɂ�%s�����錄�Ԃ��Ȃ��B"
#define GAME_MESSAGE_PACK_NO_ROOM_FLOOR "�U�b�N�ɂ͏��ɂ���ǂ̃A�C�e��������Ȃ��B"
#define GAME_MESSAGE_PACK_OVERFLOW "�U�b�N����A�C�e�������ӂꂽ�I"
#define GAME_MESSAGE_PACK_REORDER "�U�b�N�̒��̃A�C�e������ג������B"
#define GAME_MESSAGE_PACK_COMBINE "�U�b�N�̒��̃A�C�e�����܂Ƃߒ������B"

#define GAME_MESSAGE_UMBUSHED "�P�����I"

#define GAME_MESSAGE_PROJECTILE_HITS "%s��%s�ɖ��������B"

#define GAME_MESSAGE_FIELD_MAGIC_FAILED1 "����̃A�C�e���������𒵂˕Ԃ����B"

#define GAME_MESSAGE_SMITH_USE_ESSENCE "�G�b�Z���X��%d�g�p���܂��B"
#define GAME_MESSAGE_SMITH_NO_ESSENCE "�G�b�Z���X������Ȃ��B"

#else

#define GAME_MESSAGE_COMPLETE_QUEST "You completed the quest."
#define GAME_MESSAGE_IMPROVEMENT_FAILED "The improvement failed."
#define GAME_MESSAGE_RESIST_THE_EFFECT "You resist the effects."
#define GAME_MESSAGE_IS_IMMUNE "is immune."
#define GAME_MESSAGE_RESISTED "resists."
#define GAME_MESSAGE_IS_UNAFFECTED "You are unaffected."
#define GAME_MESSAGE_IS_CONFUSED "is confused"
#define GAME_MESSAGE_IS_BLIND "is blind!"
#define GAME_MESSAGE_CREATURE_IN_THE_WAY "There is a creature in the way."
#define GAME_MESSAGE_MANA_RECOVERLY "You feel your head clear."
#define GAME_MESSAGE_SUCCESS_PICKING "You have picked the lock."
#define GAME_MESSAGE_FAILED_PICKING "You failed to pick the lock."
#define GAME_MESSAGE_FOUND_SOMETHING "You have found something!"

#define GAME_MESSAGE_HEAR_NOISE "You hear noise."

#define GAME_MESSAGE_NO_STUDY_SPELL "No spells to browse."
#define GAME_MESSAGE_NO_STUDY_TECH "No techniques to browse."


#define GAME_MESSAGE_BLACK_AURA_TO_OBJECT "A terrible black aura blasts your %s!"
#define GAME_MESSAGE_REMOVED_OBJECT_CURSE "You feel as if someone is watching over you."
#define GAME_MESSAGE_COMBAT_TALK_STAIR "'What's a fucking stair!?'"
#define GAME_MESSAGE_PERILOUS_SECRET "You are too weak to control the stone!"
#define GAME_MESSAGE_RESISTED_FEAR "You stand your ground!"
#define GAME_MESSAGE_NO_MONEY "You do not have the gold."
#define GAME_MESSAGE_NO_MONEY_INSULT "Hey! You don't have the gold - get out of here!"
#define GAME_MESSAGE_BOUNTY_1 "Convert %s into money?"

#define GAME_MESSAGE_TRAP_WARNING "*Leaving trap detect region!*"
#define GAME_MESSAGE_TRAP_DESTROY "You destroyed the trap."

#define GAME_MESSAGE_TRAP_NEEDLE "A small needle has pricked you!"
#define GAME_MESSAGE_TRAP_DARTS "A small dart hits you!"
#define GAME_MESSAGE_TRAP_DARTS_MISSED "A small dart barely misses you."
#define GAME_MESSAGE_TRAP_FIRE "You are enveloped in flames."
#define GAME_MESSAGE_TRAP_ACID "You are splashed with acid."
#define GAME_MESSAGE_TRAP_ACID_FLOW "Suddenly, the room is filled with acid."
#define GAME_MESSAGE_TRAP_POISON_FLOW "Suddenly, the room is filled with poisonous liquid."
#define GAME_MESSAGE_TRAP_PIRANHAS "Suddenly, the room is filled with water with piranhas!"

#define GAME_MESSAGE_TRAP_POISONOUS_GAS "A puff of green gas surrounds you!"
#define GAME_MESSAGE_TRAP_PARALYZE_GAS "A puff of yellow gas surrounds you!"
#define GAME_MESSAGE_TRAP_SUMMONING "You are enveloped in a cloud of smoke!"
#define GAME_MESSAGE_TRAP_S_ELEMENTAL "Elemental beings appear to protect their treasures!"
#define GAME_MESSAGE_TRAP_S_BIRD "A storm of birds swirls around you!"
#define GAME_MESSAGE_TRAP_S_H_DEMON "Demons materialize in clouds of fire and brimstone!"
#define GAME_MESSAGE_TRAP_S_H_DRAGON "Draconic forms loom out of the darkness!"
#define GAME_MESSAGE_TRAP_S_HYBRID "Creatures strange and twisted assault you!"
#define GAME_MESSAGE_TRAP_S_VORTEX "Vortices coalesce and wreak destruction!"
#define GAME_MESSAGE_TRAP_E_RUNE "Hideous voices bid:  'Let the darkness have thee!'"
#define GAME_MESSAGE_TRAP_EXPLOSIVE "There is a sudden explosion! Everything inside the chest is destroyed!"
#define GAME_MESSAGE_TRAP_SCATTER  "The contents of the chest scatter all over the dungeon!"
#define GAME_MESSAGE_TRAP_TELEPORT "You hit a teleport trap!"
#define GAME_MESSAGE_TRAP_TY_CURSE "There is a flash of shimmering light!"
#define GAME_MESSAGE_TRAP_ARMAGEDDON "Suddenly, you are surrounded by immotal beings!"
#define GAME_MESSAGE_TRAP_ALARM "An alarm sounds!"

#define GAME_MESSAGE_SEEM_TO_CURSE "Hmmm, it seems to be cursed."
#define GAME_MESSAGE_PREVENT_BY_RIDING "You cannot do it when riding."
#define GAME_MESSAGE_PREVENT_BY_CONFUSION "You are too confused.";
#define GAME_MESSAGE_PREVENT_BY_BLIND "You are blind!"
#define GAME_MESSAGE_PREVENT_BY_NO_LITE "You have no light to read by."
#define GAME_MESSAGE_PREVENT_BY_HALLUCINATION "You are too hallucinated!"
#define GAME_MESSAGE_PREVENT_BY_STUNED "You are too stuned!"
#define GAME_MESSAGE_IMPROVE_LIMIT "This item is no more able to be improved."
#define GAME_MESSAGE_NO_HAPPEN "Nothing happens."

#define GAME_MESSAGE_FIND_TRAP "You have found a trap."
#define GAME_MESSAGE_FIND_DOOR "You have found a secret door."

#define GAME_MESSAGE_TELEPORT_PREVENTED "A mysterious force prevents you from teleporting!"

#define GAME_MESSAGE_PATTERN_LIMIT1 "If you start walking the Pattern, you must walk the whole way. Ok? "
#define GAME_MESSAGE_PATTERN_LIMIT2 "You must start walking the Pattern from the startpoint."
#define GAME_MESSAGE_PATTERN_LIMIT3 "You must walk the Pattern in correct order."
#define GAME_MESSAGE_PATTERN_LIMIT4 "You may not step off from the Pattern."

#define GAME_MESSAGE_ARENA_LIMIT "The arena absorbs all attempted magic!"
#define GAME_MESSAGE_ARENA_BERSERK "You cannot think directly!"

#define GAME_MESSAGE_SEE_OBJECT "You see %s."
#define GAME_MESSAGE_GET_MONEY "You collect %ld gold pieces worth of %s."
#define GAME_MESSAGE_PACK_NO_ROOM "You have no room for %s."
#define GAME_MESSAGE_PACK_NO_ROOM_FLOOR "You have no room for any of the objects on the floor."
#define GAME_MESSAGE_PACK_OVERFLOW "Your pack overflows!"
#define GAME_MESSAGE_PACK_REORDER "You reorder some items in your pack."
#define GAME_MESSAGE_PACK_COMBINE "You combine some items in your pack."

#define GAME_MESSAGE_UMBUSHED "You are ambushed !"

#define GAME_MESSAGE_PROJECTILE_HITS "The %s hits %s."

#define GAME_MESSAGE_FIELD_MAGIC_FAILED1 "The object resists the spell."

#define GAME_MESSAGE_SMITH_USE_ESSENCE "It will take %d essences."
#define GAME_MESSAGE_SMITH_NO_ESSENCE "You don't have enough essences."

#endif


// messages of detection

#ifdef JP

#define GAME_MESSAGE_DETECT_TRAPS "�g���b�v�̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_DOORS "�h�A�̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_STAIRS "�K�i�̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_B_TREASURE "�������ꂽ����̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_TREASURE "����̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_OBJECTS "�A�C�e���̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_M_OBJECTS "���@�̃A�C�e���̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_CREATURES "�N���[�`���[�̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_I_CREATURES "�����Ȑ����̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_E_CREATURES "�׈��Ȃ鐶���̑��݂������Ƃ����I"
#define GAME_MESSAGE_DETECT_S_CREATURES "�E�C�������Ƃ����I"
#define GAME_MESSAGE_DETECT_SOMEONE "%s�̑��݂������Ƃ����I"

#else

#define GAME_MESSAGE_DETECT_TRAPS "You sense the presence of traps!"
#define GAME_MESSAGE_DETECT_DOORS "You sense the presence of doors!"
#define GAME_MESSAGE_DETECT_STAIRS "You sense the presence of stairs!"
#define GAME_MESSAGE_DETECT_B_TREASURE "You sense the presence of buried treasure!"
#define GAME_MESSAGE_DETECT_TREASURE "You sense the presence of treasure!"
#define GAME_MESSAGE_DETECT_OBJECTS "You sense the presence of objects!"
#define GAME_MESSAGE_DETECT_M_OBJECTS "You sense the presence of magic objects!"
#define GAME_MESSAGE_DETECT_CREATURES "You sense the presence of creatures!"
#define GAME_MESSAGE_DETECT_I_CREATURES "You sense the presence of invisible creatures!"
#define GAME_MESSAGE_DETECT_E_CREATURES "You sense the presence of evil creatures!"
#define GAME_MESSAGE_DETECT_S_CREATURES "You sense the presence of someone's mind!"
#define GAME_MESSAGE_DETECT_SOMEONE "You sense the presence of %s!"

#endif


#ifdef JP
#define GAME_MESSAGE_FEELING_QUESTDUN "�T�^�I�ȃN�G�X�g�̃_���W�����̂悤���B"
#define GAME_MESSAGE_FEELING_WILD_POINT "�������肻���ȍr��̂悤���B"
#define GAME_MESSAGE_FEELING_TOWN "�T�^�I�Ȓ��̂悤���B"
#define GAME_MESSAGE_FEELING_WILD "�T�^�I�ȍr��̂悤���B"
#else
#define GAME_MESSAGE_FEELING_QUESTDUN "Looks like a typical quest level.";
#define GAME_MESSAGE_FEELING_WILD_POINT "Looks like a strange wilderness."
#define GAME_MESSAGE_FEELING_TOWN "Looks like a typical town."
#define GAME_MESSAGE_FEELING_WILD "Looks like a typical wilderness."
#endif

#ifdef JP
#define GAME_MESSAGE_CANNOT_GO_MORE "����ȏ��ɂ͐i�߂Ȃ��B"
#else
#define GAME_MESSAGE_CANNOT_GO_MORE "You cannot go any more."
#endif

#ifdef JP
#define GAME_MESSAGE_BUILDING_NO_BLDG "�����ɂ͌����͂Ȃ��B"
#define GAME_MESSAGE_BUILDING_LOSER "�u�s�҂ɗp�͂Ȃ��B�v"
#define GAME_MESSAGE_BUILDING_ARENA_FIGHT "�Q�[�g�͕܂��Ă���B���肪���Ȃ���҂��Ă���I"
#define GAME_MESSAGE_BUILDING_NO_RIGHT "�����I�����錠���͂���܂���I"

#define GAME_MESSAGE_GET_PRICE "�܋� %ld������ɓ��ꂽ�B"

#else
#define GAME_MESSAGE_BUILDING_NO_BLDG "You see no building here."
#define GAME_MESSAGE_BUILDING_LOSER "'There's no place here for a LOSER like you!'"
#define GAME_MESSAGE_BUILDING_ARENA_FIGHT "The gates are closed.  The enemy awaits!"
#define GAME_MESSAGE_BUILDING_NO_RIGHT "You have no right to choose that!"

#define GAME_MESSAGE_GET_PRICE "You get %ldgp."

#endif


// Cause of Death
#if JP

#define COD_QUITTING "�r���I��"
#define COD_INTERRUPTING "�����I��"
#define COD_CHEATING_DEATH "���̋\��"
#define COD_PANIC_SAVE "(�ً}�Z�[�u)"
#define COD_TRAP "�g���b�v"
#define COD_DART_TRAP "�_�[�c���"
#define COD_PERILOUS_IDENTIFY "�댯�Ȕ閧"
#define COD_FALL_RIDING "���n"
#define COD_POTION_OF_RUIN "�j�ł̖�"
#define COD_POISON_NEEDLE "�Őj"
#define COD_SCROLL_OF_FIRE "���̊���"
#define COD_SCROLL_OF_ICE "�X�̊���"
#define COD_SCROLL_OF_LOGRUS "���O���X�̊���"
#define COD_POISONOUS_FOOD "�œ���H��"
#define COD_CHEST_DP_TRAP "�j�ł̃g���b�v�̕�"
#define COD_SUNLIGHT "����"
#define COD_PATTERN_DAMAGE_1 "�u�p�^�[���v��������_���[�W"
#define COD_PATTERN_DAMAGE_2 "��ꂽ�u�p�^�[���v��������_���[�W"
#define COD_PATTERN_DAMAGE_3 "�p�^�[������"
#define COD_MISCAST_DEATH_SPELL "�Í����@�̋t��"
#define COD_PIT_TRAP "���Ƃ���"
#define COD_SPIKED_PIT_TRAP "�g�Q�̂��闎�Ƃ���"
#define COD_EXPLOADING_CHEST "�������锠"
#define COD_POTION_OF_DEATH "���̖�"
#define COD_CALL_OF_VOLD "���E�I�ȋ�������"
#define COD_TRAP_DOOR "���Ƃ���"
#define COD_GENOCIDE_COST "%^s�̎�������������J"
#else

#define COD_QUITTING "Quitting"
#define COD_INTERRUPTING "Interrupting"
#define COD_CHEATING_DEATH "cheating death"
#define COD_PANIC_SAVE "(panic save)"
#define COD_TRAP "a trap"
#define COD_DART_TRAP "a dart trap"
#define COD_PERILOUS_IDENTIFY "perilous secrets"
#define COD_FALL_RIDING "Falling from riding"
#define COD_POTION_OF_RUIN "a potion of ruin"
#define COD_POISON_NEEDLE "a potion needle"
#define COD_SCROLL_OF_FIRE "a Scroll of Fire"
#define COD_SCROLL_OF_ICE "a Scroll of Ice"
#define COD_SCROLL_OF_LOGRUS "a Scroll of Logrus"
#define COD_POISONOUS_FOOD "poisonous food"
#define COD_CHEST_DP_TRAP "a chest dispel-player trap"
#define COD_SUNLIGHT "sunlight"
#define COD_PATTERN_DAMAGE_1 "walking the Pattern"
#define COD_PATTERN_DAMAGE_2 "walking the corrupted Pattern"
#define COD_PATTERN_DAMAGE_3 "corrupting the Pattern"
#define COD_MISCAST_DEATH_SPELL "a miscast Death spell"
#define COD_PIT_TRAP "a pit trap"
#define COD_SPIKED_PIT_TRAP "a spiked pit"
#define COD_EXPLOADING_CHEST "an exploding chest"
#define COD_POTION_OF_DEATH "a potion of Death"
#define COD_CALL_OF_VOLD "a suicidal Call the Void"
#define COD_TRAP_DOOR "a trap door"
#define COD_GENOCIDE_COST "the strain of casting %^s"
#endif

// Hack -- possible "insult" messages
static const cptr desc_insult[] =
{
#ifdef JP
	"�����Ȃ��𕎐J�����I",
	"�����Ȃ��̕�𕎐J�����I",
	"�����Ȃ����y�̂����I",
	"�����Ȃ���J�߂��I",
	"�����Ȃ����������I",
	"�����Ȃ��̉��ŗx�����I",
	"�������Ȑg�Ԃ�������I",
	"�����Ȃ����ڂ���ƌ����I�I�I",
	"�����Ȃ����p���T�C�g�Ă΂�肵���I",
	"�����Ȃ����T�C�{�[�O���������I"
#else
	"insults you!",
	"insults your mother!",
	"gives you the finger!",
	"humiliates you!",
	"defiles you!",
	"dances around you!",
	"makes obscene gestures!",
	"moons you!!!"
	"calls you a parasite!",
	"calls you a cyborg!"
#endif
};

// Hack -- possible "insult" messages
static const cptr desc_moan[] =
{
#ifdef JP
	"�͉�����߂���ł���悤���B",
	"���ނ̎����������Ȃ��������Ɛq�˂Ă���B",
	"���꒣�肩��o�čs���ƌ����Ă���B",
	"�̓L�m�R���ǂ��Ƃ��ꂢ�Ă���B"
#else
	"seems sad about something.",
	"asks if you have seen his dogs.",
	"tells you to get off his land.",
	"mumbles something about mushrooms."
#endif
};

#ifdef JP
#define TEXT_FILES_GAME_NEWS "news_j.txt"
#define TEXT_FILES_ARMOUR_CURSED_ARTIFACT "a_cursed_j.txt"
#define TEXT_FILES_ARMOUR_LOW_ARTIFACT "a_low_j.txt"
#define TEXT_FILES_ARMOUR_MED_ARTIFACT "a_med_j.txt"
#define TEXT_FILES_ARMOUR_HIGH_ARTIFACT "a_high_j.txt"

#define TEXT_FILES_WEAPON_CURSED_ARTIFACT "w_cursed_j.txt"
#define TEXT_FILES_WEAPON_LOW_ARTIFACT "w_low_j.txt"
#define TEXT_FILES_WEAPON_MED_ARTIFACT "w_med_j.txt"
#define TEXT_FILES_WEAPON_HIGH_ARTIFACT "w_high_j.txt"
#define TEXT_FILES_CHAINSWORD "chainswd_j.txt"

#define TEXT_FILES_ERROR "error_j.txt"
#define TEXT_FILES_DEATH "death_j.txt"
#define TEXT_FILES_SEPPUKU "seppuku_j.txt"
#define TEXT_FILES_CREATURE_FEAR "monfear_j.txt"
#define TEXT_FILES_CREATURE_PET "monpet_j.txt"

#define TEXT_FILES_CREATURE_FRIENDLY "monfrien_j.txt"
#define TEXT_FILES_CREATURE_SPEAK "monspeak_j.txt"
#define TEXT_FILES_RUMOR "rumors_j.txt"
#define TEXT_FILES_SILLY "silly_j.txt"
#define TEXT_FILES_NEWS "news_j.txt"

#define TEXT_FILES_OPTION "joption.txt"
#define TEXT_FILES_OPTION_AUTOSAVE "joption.txt#Autosave"
#define TEXT_FILES_OPTION_WINDOW "joption.txt#Window"
#define TEXT_FILES_OPTION_BASEDELAY "joption.txt#BaseDelay"
#define TEXT_FILES_OPTION_HITPOINT "joption.txt#Hitpoint"

#define TEXT_FILES_OPTION_MANAPOINT "joption.txt#Manapoint"
#define TEXT_FILES_EDITOR "jeditor.txt"
#define TEXT_FILES_BIRTH_AUTO_ROLLER "jbirth.txt#AutoRoller"
#define TEXT_FILES_GAMBLING "jgambling.txt"
#define TEXT_FILES_ARENA "jarena.txt"

#define TEXT_FILES_BUILDING "jbldg.txt"
#define TEXT_FILES_TIME_NORMAL "timenorm_j.txt"
#define TEXT_FILES_TIME_FUN "timefun_j.txt"
#define TEXT_FILES_HELPINFO "jhelpinfo.txt"
#define TEXT_FILES_DEAD "dead_j.txt"

#define TEXT_FILES_ELVISH "elvish.txt"
#define TEXT_FILES_HISTEDIT_N "histedit-%s.prf"
#define TEXT_FILES_HISTEDIT "histedit.prf"

#else

#define TEXT_FILES_GAME_NEWS "news.txt"
#define TEXT_FILES_ARMOUR_CURSED_ARTIFACT "a_cursed.txt"
#define TEXT_FILES_ARMOUR_LOW_ARTIFACT "a_low.txt"
#define TEXT_FILES_ARMOUR_MED_ARTIFACT "a_med.txt"
#define TEXT_FILES_ARMOUR_HIGH_ARTIFACT "a_high.txt"

#define TEXT_FILES_WEAPON_CURSED_ARTIFACT "w_cursed.txt"
#define TEXT_FILES_WEAPON_LOW_ARTIFACT "w_low.txt"
#define TEXT_FILES_WEAPON_MED_ARTIFACT "w_med.txt"
#define TEXT_FILES_WEAPON_HIGH_ARTIFACT "w_high.txt"
#define TEXT_FILES_CHAINSWORD "chainswd.txt"

#define TEXT_FILES_ERROR "error.txt"
#define TEXT_FILES_DEATH "death.txt"
#define TEXT_FILES_SEPPUKU "seppuku.txt"
#define TEXT_FILES_CREATURE_FEAR "monfear.txt"
#define TEXT_FILES_CREATURE_PET "monpet.txt"

#define TEXT_FILES_CREATURE_FRIENDLY "monfrien.txt"
#define TEXT_FILES_CREATURE_SPEAK "monspeak.txt"
#define TEXT_FILES_RUMOR "rumors.txt"
#define TEXT_FILES_SILLY "silly.txt"
#define TEXT_FILES_NEWS "news.txt"

#define TEXT_FILES_OPTION "option.txt"
#define TEXT_FILES_OPTION_AUTOSAVE "option.txt#Autosave"
#define TEXT_FILES_OPTION_WINDOW "option.txt#Window"
#define TEXT_FILES_OPTION_BASEDELAY "option.txt#BaseDelay"
#define TEXT_FILES_OPTION_HITPOINT "option.txt#Hitpoint"

#define TEXT_FILES_OPTION_MANAPOINT "option.txt#Manapoint"
#define TEXT_FILES_EDITOR "editor.txt"
#define TEXT_FILES_BIRTH_AUTO_ROLLER "birth.txt#AutoRoller"
#define TEXT_FILES_GAMBLING "gambling.txt"
#define TEXT_FILES_ARENA "arena.txt"

#define TEXT_FILES_BUILDING "bldg.txt"
#define TEXT_FILES_TIME_NORMAL "timenorm.txt"
#define TEXT_FILES_TIME_FUN "timefun.txt"
#define TEXT_FILES_HELPINFO "helpinfo.txt"
#define TEXT_FILES_DEAD "dead.txt"

#define TEXT_FILES_ELVISH "elvish.txt"
#define TEXT_FILES_HISTEDIT_N "histpref-%s.prf"
#define TEXT_FILES_HISTEDIT "histpref.prf"

#endif


static const cptr effect_message_in_blind[MAX_DO_EFFECT] =
{
#if JP
	"�����ɍU�����ꂽ�B",
	"�d���ōU�����ꂽ�B",
	"�łōU�����ꂽ�B",
	"�_�ōU�����ꂽ�B",
	"��C�ōU�����ꂽ�B",

	"�Ή��ōU�����ꂽ�B",
	"�G�l���M�[�̉�ōU�����ꂽ�B",
	"�����ōU�����ꂽ�B",
	"�����s�����̂ōU�����ꂽ�B",
	"�����ƂĂ��M�����̂ōU�����ꂽ�B",
//10
	"�������������̂ōU�����ꂽ�B",
	"�����ōU�����ꂽ�B",
	"�����ōU�����ꂽ�B",
	"�����ōU�����ꂽ�B",
	"",

	"�����s�����̂ōU�����ꂽ�B",
	"�����ōU�����ꂽ�B",
	"��������������̂ōU�����ꂽ�B",
	"�^���G�l���M�[�ōU�����ꂽ�B",
	"�����x�����̂ōU�����ꂽ�B",
//20
	"",
	"�������󂩂炠�Ȃ��̓���ɗ����Ă����B",
	"�����s���₽�����̂ōU�����ꂽ�B",
	"�������̔g���ōU�����ꂽ�B",
	"�n���̗͂ōU�����ꂽ�B",

	"���������Ȃ����̂ōU�����ꂽ�B",
	"������Ȃ��̂ōU�����ꂽ�B",
	"�ߋ�����̏Ռ��ɍU�����ꂽ�I",
	"�����d�����̂ōU�����ꂽ�I",
	"",
//30
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"���炩�̍U���ɂ���ċC�����悭�Ȃ����B",
	"�����ōU�����ꂽ�I",
//40
	"�����x�����̂ōU�����ꂽ�I",
	"",
	"�����Ă��܂����I",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//50
	"",
	"",
	"",
	"",
	"",

	"",
	"���˔\�ōU�����ꂽ�I",
	"�������������I",
	"",
	"",
//60
	"�������ɗ₽�����̂ōU�����ꂽ�I",
	"�����ōU�����ꂽ�I",
	"�����ōU�����ꂽ�I",
	"�����ȃG�l���M�[�ōU�����ꂽ�I",
	"",

	"",
	"",
	"",
	"",
	"",
//70
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//80
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//90
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//100
	"",
	"",
	"",

#else
	"You are hit by something.",
	"You are hit by lightning.",
	"You are hit by poison.",
	"You are hit by acid.",
	"You are hit by cold.",

	"You are hit by fire.",
	"You are hit by an energy.",
	"You are hit by something.",
	"You are hit by something sharp.",
	"You are hit by something *HOT*.",
//10
	"You are hit by something wet.",
	"You are hit by something.",
	"You are hit by something.",
	"You are hit by something.",
	"",

	"You are hit by something sharp.",
	"You are hit by a loud noise.",
	"You are hit by something puzzling.",
	"You are hit by kinetic force.",
	"You are hit by something slow.",
//20
	"",
	"Something falls from the sky on you.",
	"You are hit by something sharp and cold.",
	"You are hit by a wave of anarchy."
	"You are hit by nether forces.",

	"You are hit by something.",
	"You are hit by something strange.",
	"You are hit by a blast from the past!",
	"You are hit by something heavy!",
	"",
//30
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"You are hit by something invigorating!",
	"You are hit by something!",
//40
	"You are hit by something slow!",
	"",
	"You fall asleep!",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//50
	"",
	"",
	"",
	"",
	"",

	"",
	"You are hit by radiation!",
	"There is an explosion!",
	"",
	"",
//60
	"You are hit by something extremely cold!",
	"You are hit by something!",
	"You are hit by something!",
	"You are hit by pure energy!",
	"",

	"",
	"",
	"",
	"",
	"",
//70
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//80
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//90
	"",
	"",
	"",
	"",
	"",

	"",
	"",
	"",
	"",
	"",
//100
	"",
	"",
	"",

#endif
};

#ifdef JP
	#define PROMPT_COMMAND "�R�}���h: "
	#define PROMPT_KEYPRESS "�����L�[: "
	#define PROMPT_FILE "�t�@�C��: "
	#define PROMPT_MEMO "����: "
	#define PROMPT_TRIGGER_KEY "�g���K�[�L�[: "
	#define PROMPT_INVENTORY "������: "
	#define PROMPT_EQUIPMENT "�����i: "
	#define PROMPT_REST "�x�e (0-9999, '*' �� HP/MP�S��, '&' �ŕK�v�Ȃ���): "
	#define PROMPT_PICK "%s���E���܂���? " 
#else
	#define PROMPT_COMMAND "Command: "
	#define PROMPT_KEYPRESS "Keypress: "
	#define PROMPT_FILE "File: "
	#define PROMPT_MEMO "Memo: "
	#define PROMPT_TRIGGER_KEY "Trigger: "
	#define PROMPT_INVENTORY "Inven: "
	#define PROMPT_EQUIPMENT "Equip: "
	#define PROMPT_REST "Rest (0-9999, '*' for HP/SP, '&' as needed): "
	#define PROMPT_PICK "Pick up %s? " 
#endif

#ifdef JP
	#define ST_VIEW_HUNGER_WEAK		"����  "
	#define ST_VIEW_HUNGER_HUNGRY	"��  "
	#define ST_VIEW_HUNGER_NORMAL	"      "
	#define ST_VIEW_HUNGER_FULL		"����  "
	#define ST_VIEW_HUNGER_GORGED	"�H�߂�"
#else
	#define ST_VIEW_HUNGER_WEAK		"Weak  "
	#define ST_VIEW_HUNGER_HUNGRY	"Hungry"
	#define ST_VIEW_HUNGER_NORMAL	"      "
	#define ST_VIEW_HUNGER_FULL		"Full  "
	#define ST_VIEW_HUNGER_Gorged	"Gorges"
#endif


#ifdef JP
	#define SKILL_NAME_MAGIC "���@"
	#define SKILL_NAME_PSIONIC "���\��"
	#define SKILL_NAME_IMITATION "���̂܂�"
	#define SKILL_NAME_HISSATSU "�K�E��"
	#define SKILL_NAME_MIRROR_M "�����@"
	#define SKILL_NAME_NINJUTSU "�E�p"
	#define SKILL_NAME_PRAYER "�F��"
#else
	#define SKILL_NAME_MAGIC "magic"
	#define SKILL_NAME_PSIONIC "psionic powers"
	#define SKILL_NAME_IMITATION "imitation"
	#define SKILL_NAME_HISSATSU "hissatsu"
	#define SKILL_NAME_MIRROR_M "mirror magic"
	#define SKILL_NAME_NINJUTSU "ninjutsu"
	#define SKILL_NAME_PRAYER "prayer"
#endif

#ifdef JP

#define DIARY_CREATE_PLAYER "-------- �v���C���[�쐬 --------"
#define DIARY_GAMEOVER "-------- �Q�[���I�[�o�[ --------"
#define DIARY_SAVEQUIT "----�Q�[�����f----"
#define DIARY_GAMERESTART "----�Q�[���ĊJ----"
#define DIARY_GAMEABORT "----�Q�[���ُ�I��----"
#define DIARY_RETIRED "�_���W�����̒T��������ނ����B"
#define DIARY_SUICIDE "��]���Ď��E�����B"
#define DIARY_WIZARD "�E�B�U�[�h���[�h�ɓ˓����ăX�R�A���c���Ȃ��Ȃ����B"
#define DIARY_UP_STAIR "�K�i�������"
#define DIARY_DOWN_STAIR "�K�i�����肽"

#else

#define DIARY_CREATE_PLAYER "-------- Create Player --------"
#define DIARY_GAMEOVER "--------   Game  Over   --------"
#define DIARY_SAVEQUIT "---- Save and Exit Game ----"
#define DIARY_GAMERESTART "---- Restart Game ----"
#define DIARY_GAMEABORT "---- Panic Save and Abort Game ----"
#define DIARY_RETIRED "retired exploring dungeons."
#define DIARY_SUICIDE "give up all hope to commit suicide."
#define DIARY_WIZARD "give up sending score to use wizard mode."
#define DIARY_UP_STAIR 1"climbed up the stairs to"
#define DIARY_DOWN_STAIR "climbed down the stairs to"

#endif