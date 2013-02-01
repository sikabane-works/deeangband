
// Keywords

#ifdef JP
#define KEYWORD_CURSED "����Ă���"
#else
#define KEYWORD_CURSED "cursed"
#endif


#ifdef JP

#define SYS_MESSAGE_WIZARD_MODE_ON "�E�B�U�[�h���[�h�˓��B"
#define SYS_MESSAGE_WIZARD_MODE_OFF "�E�B�U�[�h���[�h�����B"
#define SYS_MESSAGE_STARTING_GAME "�Q�[�����J�n���܂��B���҂�������..."
#define SYS_MESSAGE_FAILED_TEMPFILE "�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B"
#define SYS_MESSAGE_FAILED_FILEOPEN "�t�@�C�����J���܂���B"
#define SYS_MESSAGE_FAILED_FILEOPEN2 "%s ���J�����Ƃ��ł��܂���ł����B"
#define SYS_MESSAGE_REPLACE_FILE "��������t�@�C���ɏ㏑�����܂���? (%s)"
#define SYS_MESSAGE_HIT_ANY_KEY "�����L�[�������ĉ������B"
#define SYS_MESSAGE_PLACE_SPACE "�ꏊ��I��ŃX�y�[�X�L�[�������ĉ������B"
#define SYS_MESSAGE_OPTION_AUX "�����I�v�V����((*)�̓X�R�A�ɉe��)"
#define SYS_MESSAGE_OPTION_AUX2 "�����I�v�V����(�Q�Ƃ̂�)"
#define SYS_MESSAGE_AUTO_SAVING "�����Z�[�u��"
#define SYS_MESSAGE_GAME_SAVING "�Q�[�����Z�[�u���Ă��܂�..."
#define SYS_MESSAGE_SAVE_FAILED "�Z�[�u���s�I"
#define SYS_MESSAGE_SCORE_FAILED "�X�R�A�E�t�@�C�����g�p�ł��܂���B"
#define SYS_MESSAGE_PANIC_SAVE_SUCCEED "�ً}�Z�[�u�����I"
#define SYS_MESSAGE_PANIC_SAVE_FAILED "�ً}�Z�[�u���s�I"
#define SYS_MESSAGE_YOUR_TRAITS "���Ȃ��ɂ͈ȉ��̓���������"
#define SYS_MESSAGE_YOUR_NO_TRAIT "���Ȃ��ɂ͓��ʂȓ������Ȃ�"
#define SYS_MESSAGE_SERVANTS "%s�ɂ�%d��ށA%d�̂̏]�l������"
#define SYS_MESSAGE_NO_SERVANT "%s�ɂ͏]�l�����Ȃ�"
#define SYS_MESSAGE_UNKNOWN_SYMBOL "�����ȕ���"
#define SYS_MESSAGE_INVALID_NUM "�l�����������ł��B"
#define SYS_MESSAGE_ERROR "�G���["
#define SYS_MESSAGE_OUT_OF_SWITCH "switch�\���͈͊O"

#define SYS_MESSAGE_TODAY "����"
#define SYS_MESSAGE_NORTH "�k"
#define SYS_MESSAGE_SOUTH "��"
#define SYS_MESSAGE_EAST "��"
#define SYS_MESSAGE_WEST "��"
#define SYS_MESSAGE_MONEY "������"
#define SYS_MESSAGE_WEIGHT "�d��"

#define SYS_MESSAGE_MODIFICATION "�\�͏C��"

#define SYS_MESSAGE_VISUAL_COMMAND "�R�}���h (n/N/^N/a/A/^A/c/C/^C/v/V/^V): "
#define SYS_MESSAGE_ASK_USE "%s���g���܂����H"
#define SYS_MESSAGE_ASK_SURE "��낵���ł����H"

#define SYS_MESSAGE_CHARGE_NUM(NUM) "��"
#else

#define SYS_MESSAGE_WIZARD_MODE_ON "Wizard mode on."
#define SYS_MESSAGE_WIZARD_MODE_OFF "Wizard mode off."
#define SYS_MESSAGE_STARTING_GAME "Starting Game, Please wait..."
#define SYS_MESSAGE_FAILED_TEMPFILE "Failed to create temporary file %s."
#define SYS_MESSAGE_FAILED_FILEOPEN "Can not open file."
#define SYS_MESSAGE_FAILED_FILEOPEN2 "Failed to open %s."
#define SYS_MESSAGE_REPLACE_FILE "Replace existing file %s? "
#define SYS_MESSAGE_HIT_ANY_KEY "Hit Any Key�B"
#define SYS_MESSAGE_PLACE_SPACE "Select a point and press space."

#define SYS_MESSAGE_OPTION_AUX "Birth Option((*)s effect score)"
#define SYS_MESSAGE_OPTION_AUX2 "Birth Options(browse only)"
#define SYS_MESSAGE_AUTO_SAVING "Autosaving the game..."
#define SYS_MESSAGE_GAME_SAVING "Saving game..."
#define SYS_MESSAGE_SAVE_FAILED "death save failed!"
#define SYS_MESSAGE_SCORE_FAILED "Score file unavailable."
#define SYS_MESSAGE_PANIC_SAVE_SUCCEED "Panic save succeeded!"
#define SYS_MESSAGE_PANIC_SAVE_FAILED "panic save failed!"
#define SYS_MESSAGE_YOUR_TRAITS "You have these trait..."
#define SYS_MESSAGE_YOUR_NO_TRAIT "You have no trait"
#define SYS_MESSAGE_YOUR_SERVANT "%s have no servant"
#define SYS_MESSAGE_YOUR_NO_SERVANT "%s have %d species, %d servants"

#define SYS_MESSAGE_UNKNOWN_SYMBOL "unknown symbol"
#define SYS_MESSAGE_INVALID_NUM "Invalid response."
#define SYS_MESSAGE_ERROR "Error"
#define SYS_MESSAGE_OUT_OF_SWITCH "Out of switch."
#define SYS_MESSAGE_TODAY "Today"
#define SYS_MESSAGE_NORTH " North"
#define SYS_MESSAGE_SOUTH " South"
#define SYS_MESSAGE_EAST " East"
#define SYS_MESSAGE_WEST " West"
#define SYS_MESSAGE_MONEY "AU"
#define SYS_MESSAGE_WEIGHT "Weight"

#define SYS_MESSAGE_MODIFICATION "Modification"

#define SYS_MESSAGE_VISUAL_COMMAND "Command (n/N/^N/a/A/^A/c/C/^C/v/V/^V):"
#define SYS_MESSAGE_ASK_USE "use %s?"
#define SYS_MESSAGE_ASK_SURE "Are you sure?"

#define SYS_MESSAGE_CHARGE_NUM(NUM) ((NUM) > 1 ? " charges" : "charge")

#endif
 
#if JP
#define KW_CREATURE "�N���[�`���["
#define KW_NAME "���O"
#define KW_RACE "�푰"
#define KW_CLASS "�E��"
#define KW_CHARA "���i"
#define KW_PATLON "��_"
#define KW_LEVEL "���x��"
#define KW_HP "HP"
#define KW_MP "MP"
#define KW_EXP "�\�E��"
#define KW_MONEY "��"
#define KW_FAIL "����"
#define KW_UNKNOWN "���m"
#define KW_FORGOTTEN "�Y�p"
#define KW_ILLEGIBLE "���Ǖs�\"
#define KW_UNTRIED "���o��"
#define KW_CARRYING_WEIGHT "�����d��"
#define KW_EQUIPMENT_WEIGHT "�����d��"
#define KW_STAFF "��"
#define KW_WAND "���@�_"
#define KW_ROD "���b�h"
#define KW_YOU "���Ȃ�"
#define KW_CREATURE_PROFILE "�N���[�`���[�̃v���t�B�[��"
#define KW_GOOD "�P"
#define KW_EVIL "��"
#define KW_LAW "����"
#define KW_CHAOS "����"
#define KW_BALANCE "�V��"
#define KW_DAM "����"
#define KW_RAN "�˒�"
#define KW_POW "����"
#define KW_RAD "���a"
#define KW_YES "�͂�"
#define KW_NO "������"
#define KW_DONE "����"
#define KW_FAILED "���s"
#define KW_NONE "�Ȃ�"
#define KW_SURFACE "�n��"
#define KW_QUEST "�N�G�X�g"
#define KW_SCORE_RANKING "�E�҂̓a��"
#define KW_WINNER "�̑�Ȃ��"
#define KW_MAX "�ō�"
#define KW_RANDOM "�����_��"
#define KW_BACK_TO_START "�ŏ��ɖ߂�"
#define KW_QUIT_GAME "�I������"
#define KW_ACCEPT "���肷��"
#define KW_IT "����"
#define KW_SOMETHING "����"
#define KW_CLONE "�N���[��"
#define KW_VALIABLE_RACE "�푰��"
#define KW_VALIABLE_CLASS "�푰��"
#define KW_VALIABLE_CHARA "���i��"
#define KW_MALE "�j��"
#define KW_FEMALE "����"
#define KW_INTERSEX "����"
#define KW_NOSEX "����"
#define KW_FAST "����"
#define KW_SLOW "����"
#define KW_RIDING "��n"
#define KW_ASKING_PRICE "�񎦉��i"
#define KW_FINAL_OFFER_PRICE "�ŏI�񎦉��i"
#else
#define KW_CREATURE "Creatures"
#define KW_NAME "Name"
#define KW_RACE "Race"
#define KW_CLASS "Class"
#define KW_CHARA "Chara"
#define KW_PATLON "Patron"
#define KW_LEVEL "Level"
#define KW_HP "HP"
#define KW_MP "MP"
#define KW_EXP "SOUL"
#define KW_MONEY "AU"
#define KW_FAIL "Fail"
#define KW_UNKNOWN "Unknown"
#define KW_FORGOTTEN "Forgotten"
#define KW_ILLEGIBLE "Illegible"
#define KW_UNTRIED "Untried"
#define KW_CARRYING_WEIGHT "Carrying Weight"
#define KW_EQUIPMENT_WEIGHT "Equipment Weight"
#define KW_STAFF "staff"
#define KW_WAND "wand"
#define KW_ROD "rod"
#define KW_YOU "You"
#define KW_CREATURE_PROFILE "Creature Profile"
#define KW_GOOD "Good"
#define KW_EVIL "Evil"
#define KW_LAW "Law"
#define KW_CHAOS "Chaos"
#define KW_BALANCE "Balance"
#define KW_DAM "Dam"
#define KW_RAN "Ran"
#define KW_POW "Pow"
#define KW_RAD "Rad"
#define KW_YES "Yes"
#define KW_NO "No"
#define KW_DONE "Done"
#define KW_FAILED "Failed"
#define KW_NONE "None"
#define KW_SURFACE "the surface"
#define KW_QUEST "quest"
#define KW_SCORE_RANKING "Hall of Fame"
#define KW_WINNER "Magnificent"
#define KW_MAX "Max"
#define KW_RANDOM "Random"
#define KW_BACK_TO_START "Back to start"
#define KW_QUIT_GAME "Quit game"
#define KW_ACCEPT "Accept"
#define KW_IT "It"
#define KW_SOMETHING "Something"
#define KW_CLONE "Clone"
#define KW_VALIABLE_RACE "VariableRace"
#define KW_VALIABLE_CLASS "VariableClass"
#define KW_VALIABLE_CHARA "VariableChara"
#define KW_MALE "Male"
#define KW_FEMALE "Female"
#define KW_INTERSEX "Intersex"
#define KW_NOSEX "Nosex"
#define KW_FAST "Fast"
#define KW_SLOW "Slow"
#define KW_RIDING "Riding"
#define KW_ASKING_PRICE "Asking Price"
#define KW_FINAL_OFFER_PRICE "Final Offer"
#endif

#ifdef JP
#define GAME_MESSAGE_TRAP_BLIND "�����K�X�ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_CONFUSE "����߂��K�X�ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_POISON "�h���I�ȗΐF�̃K�X�ɕ�ݍ��܂ꂽ�I"
#define GAME_MESSAGE_TRAP_SLEPT "��Ȕ������ɕ�܂ꂽ�I"

#define GAME_MESSAGE_TRAP_NEEDLE "�d�|�����Ă��������Ȑj�Ɏh����Ă��܂����I"
#define GAME_MESSAGE_TRAP_DARTS "�����ȃ_�[�c�����ł��Ďh�������I"
#define GAME_MESSAGE_TRAP_DARTS_MISSED "�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B"
#define GAME_MESSAGE_TRAP_FIRE "���ɕ�܂ꂽ�I"
#define GAME_MESSAGE_TRAP_ACID "�_������������ꂽ�I"
#define GAME_MESSAGE_TRAP_ACID_FLOW "�ˑR������_�����o�����I"
#define GAME_MESSAGE_TRAP_POISON_FLOW "�ˑR������ŉt���o�����I"
#define GAME_MESSAGE_TRAP_PIRANHAS "�ˑR�ǂ��琅�����o�����I�s���j�A������I"

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

#else
#define GAME_MESSAGE_TRAP_BLIND "A black gas surrounds you!"
#define GAME_MESSAGE_TRAP_CONFUSE "A gas of scintillating colors surrounds you!"
#define GAME_MESSAGE_TRAP_POISON "A pungent green gas surrounds you!"
#define GAME_MESSAGE_TRAP_SLEPT "A strange white mist surrounds you!"

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
#endif

#if JP
#define GAME_MESSAGE_COMPLETE_QUEST "�N�G�X�g��B�������B"
#define GAME_MESSAGE_CURRENT_QUEST "�s���s���̃N�G�X�g�t"
#define GAME_MESSAGE_COMPLETED_QUEST "�s�B�������N�G�X�g�t"
#define GAME_MESSAGE_FAILED_QUEST "�s���s�����N�G�X�g�t"
#define GAME_MESSAGE_REMAINING_QUEST "�s�c���Ă���N�G�X�g�t"

#define GAME_MESSAGE_FAILED "���s�����B"
#define GAME_MESSAGE_ENCHANTMENT_FAILED "�����Ɏ��s�����B"
#define GAME_MESSAGE_IMPROVEMENT_FAILED "���ǂɎ��s�����B"
#define GAME_MESSAGE_RESIST_THE_EFFECT "���������͂𒵂˕Ԃ����B"
#define GAME_MESSAGE_IS_IMMUNE "�ɂ͊��S�ȑϐ�������B"
#define GAME_MESSAGE_RESISTED "�ɂ͑ϐ�������B"
#define GAME_MESSAGE_IS_UNAFFECTED "�ɂ͌��ʂ��Ȃ������B"
#define GAME_MESSAGE_NO_EFFECT "���ʂ��Ȃ������B"
#define GAME_MESSAGE_IS_CONFUSED "�͖ڂ������Ȃ�"
#define GAME_MESSAGE_IS_BLIND "�͍������Ă���B"
#define GAME_MESSAGE_CREATURE_IN_THE_WAY "�N���[�`���[�������ӂ������Ă���B"
#define GAME_MESSAGE_MANA_RECOVERLY "�����n�b�L���Ƃ����B"
#define GAME_MESSAGE_SUCCESS_PICKING "�����͂������B"
#define GAME_MESSAGE_FAILED_PICKING "�����͂����Ȃ������B"
#define GAME_MESSAGE_FOUND_SOMETHING "�����𔭌������I"

#define GAME_MESSAGE_BREAK_P_RUNE "���̃��[������ꂽ�I"
#define GAME_MESSAGE_BREAK_E_RUNE "���[�������������I"
#define GAME_MESSAGE_DISARM_E_RUNE "�����̃��[���͉������ꂽ�B"

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

#define GAME_MESSAGE_LITE_FAINT "�����肪�����ɂȂ��Ă��Ă���B"
#define GAME_MESSAGE_LITE_OUT "�����肪�����Ă��܂����I"
#define GAME_MESSAGE_LITE_NONE "�����𑕔����Ă��Ȃ��B"
#define GAME_MESSAGE_LITE_NOREFILL "���̌����͎��������΂��Ȃ��B"

#define GAME_MESSAGE_FAILED_RUNAWAY "���܂��������Ȃ������B"

#define GAME_MESSAGE_DAMAGE_DENSITY "�̂̕��q�����������C������I"
#define GAME_MESSAGE_ROCK_CRUSHING "���ꂽ��ɉ����ׂ��ꂽ�I"

#define GAME_MESSAGE_SEEM_TO_CURSE "�Ӂ[�ށA�ǂ�������Ă���悤���B"
#define GAME_MESSAGE_PREVENT_BY_RIDING "��n���ɂ͏o���Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_CONFUSION "������Ԃŏ�肭�o���Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_BLIND "�Ⴊ�����Ȃ��ďo���Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_NO_LITE "�����肪�Ȃ��ďo���Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_HALLUCINATION "���o�ɑj�܂�Ăł��Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_STUNED "�����N�O�Ƃ��Ă��ďW���ł��Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_PET "���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB"
#define GAME_MESSAGE_PREVENT_BY_NO_MIRROR "���̏�łȂ��ƏW���ł��Ȃ��B"
#define GAME_MESSAGE_PREVENT_BY_NO_BAREBAND "�f�肶��Ȃ��Ƃł��܂���B"
#define GAME_MESSAGE_PREVENT_BY_HP "HP������܂���."
#define GAME_MESSAGE_PREVENT_BY_MP "MP������܂���B"
#define GAME_MESSAGE_PREVENT_BY_LEVEL "���̔\�͂��g�p����ɂ̓��x�� %d �ɒB���Ă��Ȃ���΂Ȃ�܂���B"
#define GAME_MESSAGE_PREVENT_BY_ANTIMAGIC "�����@�o���A�����@���ז������I"
#define GAME_MESSAGE_PREVENT_BY_TIMESTOP "�~�܂������̒��ł͂��܂������Ȃ��悤���B"
#define GAME_MESSAGE_PREVENT_BY_HUNGER "���̑O�ɐH�����Ƃ�Ȃ��ƁB"
#define GAME_MESSAGE_PREVENT_BY_POSTURE "���͍\���ɏW�����Ă���B"
#define GAME_MESSAGE_PREVENT_BY_CREATURE(NAME) "%s���ז����I", (NAME)
#define GAME_MESSAGE_FISHING_PREVENT_BY_FEATURE "�����͐��ӂł͂Ȃ��B"
#define GAME_MESSAGE_MAGIC_PREVENT_BY_DUNGEON "�_���W���������@���z�������I"
#define GAME_MESSAGE_HAYAGAKE_PREVENT "�����ł͑f���������Ȃ��B"
#define GAME_MESSAGE_ATTEMPT_ANYWAY "����ł����킵�܂���? "

#define GAME_MESSAGE_IMPROVE_LIMIT "���̃A�C�e���͂���ȏ���ǂł��Ȃ��B"

#define GAME_MESSAGE_NO_HAPPEN "�����N����Ȃ������B"
#define GAME_MESSAGE_RIDING_FAILED "���܂����Ȃ������B"
#define GAME_MESSAGE_CONVERT_FAILED "�ϊ��Ɏ��s�����B"
#define GAME_MESSAGE_NO_DICRECTION_CREATURE "���̕����ɂ̓N���[�`���[�͂��܂���B"
#define GAME_MESSAGE_FALL_RIDING "��R����n�ʂɗ��Ƃ��ꂽ�B"

#define GAME_MESSAGE_FIND_TRAP "�g���b�v�𔭌������B"
#define GAME_MESSAGE_FIND_DOOR "�B���h�A�𔭌������B"

#define GAME_MESSAGE_TELEPORT_PREVENTED "�s�v�c�ȗ͂��e���|�[�g��h�����I"

#define GAME_MESSAGE_ARENA_READY "�����J�n�I"
#define GAME_MESSAGE_ARENA_LIMIT "�A���[�i�����@���z�������I"
#define GAME_MESSAGE_ARENA_BERSERK "����m�����Ă��ē������Ȃ��I"

#define GAME_MESSAGE_SEE_OBJECT "%s������B"
#define GAME_MESSAGE_GET_MONEY " $%ld �̉��l������%s���������B"
#define GAME_MESSAGE_PACK_NO_ROOM "�U�b�N�ɂ�%s�����錄�Ԃ��Ȃ��B"
#define GAME_MESSAGE_PACK_NO_ROOM_FLOOR "�U�b�N�ɂ͏��ɂ���ǂ̃A�C�e��������Ȃ��B"
#define GAME_MESSAGE_PACK_OVERFLOW "�U�b�N����A�C�e�������ӂꂽ�I"
#define GAME_MESSAGE_PACK_REORDER "�U�b�N�̒��̃A�C�e������ג������B"
#define GAME_MESSAGE_PACK_COMBINE "�U�b�N�̒��̃A�C�e�����܂Ƃߒ������B"
#define GAME_MESSAGE_STAFF_UNSTACK "����܂Ƃ߂Ȃ������B"

#define GAME_MESSAGE_UMBUSHED "�P�����I"

#define GAME_MESSAGE_PROJECTILE_HITS "%s��%s�ɖ��������B"

#define GAME_MESSAGE_FIELD_MAGIC_FAILED1 "����̃A�C�e���������𒵂˕Ԃ����B"

#define GAME_MESSAGE_CALL_ARTIFACT "���̃A�[�e�B�t�@�N�g�����Ɩ��t���܂����H"
#define GAME_MESSAGE_SMITH_USE_ESSENCE "�G�b�Z���X��%d�g�p���܂��B"
#define GAME_MESSAGE_SMITH_NO_ESSENCE "�G�b�Z���X������Ȃ��B"

#define GAME_MESSAGE_TARGET_SET "�^�[�Q�b�g����B"
#define GAME_MESSAGE_TARGET_ABORT "�^�[�Q�b�g�����B"
#define GAME_MESSAGE_TARGET_CANCELED "���f�����B"

#define GAME_MESSAGE_GOODBYE(NAME) ("���悤�Ȃ�A%s!", (NAME))
#else

#define GAME_MESSAGE_COMPLETE_QUEST "You completed the quest."
#define GAME_MESSAGE_CURRENT_QUEST "< Current Quest >"
#define GAME_MESSAGE_COMPLETED_QUEST "< Completed Quest >"
#define GAME_MESSAGE_FAILED_QUEST "< Failed Quest >"
#define GAME_MESSAGE_REMAINING_QUEST "< Remaining Quest >"

#define GAME_MESSAGE_FAILED "Failed."
#define GAME_MESSAGE_ENCHANTMENT_FAILED "The enchantment failed."
#define GAME_MESSAGE_IMPROVEMENT_FAILED "The improvement failed."
#define GAME_MESSAGE_RESIST_THE_EFFECT "You resist the effects."
#define GAME_MESSAGE_IS_IMMUNE "is immune."
#define GAME_MESSAGE_RESISTED "resists."
#define GAME_MESSAGE_IS_UNAFFECTED "You are unaffected."
#define GAME_MESSAGE_NO_EFFECT "There is no effect."
#define GAME_MESSAGE_IS_CONFUSED "is confused"
#define GAME_MESSAGE_IS_BLIND "is blind!"
#define GAME_MESSAGE_CREATURE_IN_THE_WAY "There is a creature in the way."
#define GAME_MESSAGE_MANA_RECOVERLY "You feel your head clear."
#define GAME_MESSAGE_SUCCESS_PICKING "You have picked the lock."
#define GAME_MESSAGE_FAILED_PICKING "You failed to pick the lock."
#define GAME_MESSAGE_FOUND_SOMETHING "You have found something!"

#define GAME_MESSAGE_BREAK_P_RUNE "The rune of protection is broken!"
#define GAME_MESSAGE_BREAK_E_RUNE "The rune explodes!"
#define GAME_MESSAGE_DISARM_E_RUNE "An explosive rune was disarmed."
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

#define GAME_MESSAGE_LITE_FAINT "Your light is growing faint."
#define GAME_MESSAGE_LITE_OUT "Your light has gone out!"
#define GAME_MESSAGE_LITE_NONE "You are not wielding a light."
#define GAME_MESSAGE_LITE_NOREFILL "Your light cannot be refilled."

#define GAME_MESSAGE_FAILED_RUNAWAY "You are failed to run away."

#define GAME_MESSAGE_DAMAGE_DENSITY "Your molecules feel disrupted!"
#define GAME_MESSAGE_RCOK_CRUSHING "You are being crushed!"

#define GAME_MESSAGE_SEEM_TO_CURSE "Hmmm, it seems to be cursed."
#define GAME_MESSAGE_PREVENT_BY_RIDING "You cannot do it when riding."
#define GAME_MESSAGE_PREVENT_BY_CONFUSION "You are too confused."
#define GAME_MESSAGE_PREVENT_BY_BLIND "You are blind!"
#define GAME_MESSAGE_PREVENT_BY_NO_LITE "You have no light to read by."
#define GAME_MESSAGE_PREVENT_BY_HALLUCINATION "You are too hallucinated!"
#define GAME_MESSAGE_PREVENT_BY_STUNED "You are too stuned!"
#define GAME_MESSAGE_PREVENT_BY_PET "You need concentration on the pets now."
#define GAME_MESSAGE_PREVENT_BY_NO_MIRROR "Here are not any mirrors!"
#define GAME_MESSAGE_PREVENT_BY_NO_BAREBAND "You need to be bare hand."
#define GAME_MESSAGE_PREVENT_BY_HP "You do not have enough hit-point to use this power."
#define GAME_MESSAGE_PREVENT_BY_MP "You do not have enough mana to use this power."
#define GAME_MESSAGE_PREVENT_BY_LEVEL "You need to attain level %d to use this power."
#define GAME_MESSAGE_PREVENT_BY_ANTIMAGIC "An anti-magic shell disrupts your magic!"
#define GAME_MESSAGE_PREVENT_BY_TIMESTOP "It shows no reaction."
#define GAME_MESSAGE_PREVENT_BY_HUNGER "You must eat something here."
#define GAME_MESSAGE_PREVENT_BY_POSTURE "You need concentration on your form."
#define GAME_MESSAGE_PREVENT_BY_CREATURE(NAME) "%^s is stand in your way.", (NAME)
#define GAME_MESSAGE_FISHING_PREVENT_BY_FEATURE "There is no fishing place."
#define GAME_MESSAGE_MAGIC_PREVENT_BY_DUNGEON "The dungeon absorbs all attempted magic!"
#define GAME_MESSAGE_HAYAGAKE_PREVENT "You cannot run in here."
#define GAME_MESSAGE_ATTEMPT_ANYWAY "Attempt it anyway? "

#define GAME_MESSAGE_IMPROVE_LIMIT "This item is no more able to be improved."

#define GAME_MESSAGE_NO_HAPPEN "Nothing happens."
#define GAME_MESSAGE_CONVERT_FAILED "You failed to convert."
#define GAME_MESSAGE_RIDING_FAILED "You failed to ride."
#define GAME_MESSAGE_NO_DICRECTION_CREATURE "There is no creature."

#define GAME_MESSAGE_FALL_RIDING "You have fallen from your riding pet."

#define GAME_MESSAGE_FIND_TRAP "You have found a trap."
#define GAME_MESSAGE_FIND_DOOR "You have found a secret door."

#define GAME_MESSAGE_TELEPORT_PREVENTED "A mysterious force prevents you from teleporting!"

#define GAME_MESSAGE_ARENA_READY "Ready..Fight!"
#define GAME_MESSAGE_ARENA_LIMIT "The arena absorbs all attempted magic!"
#define GAME_MESSAGE_ARENA_BERSERK "You cannot think directly!"

#define GAME_MESSAGE_SEE_OBJECT "You see %s."
#define GAME_MESSAGE_GET_MONEY "You collect %ld gold pieces worth of %s."
#define GAME_MESSAGE_PACK_NO_ROOM "You have no room for %s."
#define GAME_MESSAGE_PACK_NO_ROOM_FLOOR "You have no room for any of the objects on the floor."
#define GAME_MESSAGE_PACK_OVERFLOW "Your pack overflows!"
#define GAME_MESSAGE_PACK_REORDER "You reorder some items in your pack."
#define GAME_MESSAGE_PACK_COMBINE "You combine some items in your pack."
#define GAME_MESSAGE_STAFF_UNSTACK "You unstack your staff."

#define GAME_MESSAGE_UMBUSHED "You are ambushed !"

#define GAME_MESSAGE_PROJECTILE_HITS "The %s hits %s."

#define GAME_MESSAGE_FIELD_MAGIC_FAILED1 "The object resists the spell."

#define GAME_MESSAGE_CALL_ARTIFACT "What do you want to call the artifact?"

#define GAME_MESSAGE_SMITH_USE_ESSENCE "It will take %d essences."
#define GAME_MESSAGE_SMITH_NO_ESSENCE "You don't have enough essences."

#define GAME_MESSAGE_TARGET_SET "Target selected."
#define GAME_MESSAGE_TARGET_ABORT "Target aborted."
#define GAME_MESSAGE_TARGET_CANCELED "Canceled."

#define GAME_MESSAGE_GOODBYE(NAME) ("Goodbye, %s!", (NAME))
#endif

#ifdef JP
#define GAME_MESSAGE_WEAPON_ATTACK "%s��%s��%s�ōU�������B"
#define GAME_MESSAGE_WEAPON_ATTACK_MISS "%s��%s�̍U�������킵���B"
#define GAME_MESSAGE_WEAPON_AMBUSH(ATTACKER, TARGET) ("%s�͗⍓�ɂ������Ă��閳�͂�%s��˂��h�����I", (ATTACKER), (TARGET))
#define GAME_MESSAGE_WEAPON_FATAL_SPOT(ATTACKER, TARGET) ("%s�͕s�ӂ�˂���%s�ɋ���Ȉꌂ�����킹���I", (ATTACKER), (TARGET))
#define GAME_MESSAGE_WEAPON_BACKSTUB(ATTACKER, TARGET) ("%s�͓�����%s��w������˂��h�����I", (ATTACKER), (TARGET))
#define GAME_MESSAGE_WEAPON_ZANTETSU_JELLY "����ȓ�炩�����̂͐؂��I"
#define GAME_MESSAGE_WEAPON_ZANTETSU_SERIF "�܂��܂�ʂ��̂��a���Ă��܂����D�D�D"
#define GAME_MESSAGE_WEAPON_EX_JR_SPIDER "�w偂͋�肾�I"
#define GAME_MESSAGE_WEAPON_VORPAL_BLADE_SERIF "�ڂɂ��~�܂�ʃ��H�[�p���u���[�h�A��B�̑��ƁI"
#define GAME_MESSAGE_WEAPON_ATTACK_DRAIN(WEAPON, TARGET) ("%s��%s���琶���͂��z��������I", (WEAPON), (TARGET))
#define GAME_MESSAGE_WEAPON_SUCK_BLOOD(WEAPON) ("%s�͌����z���ċ����Ȃ����I", WEAPON)
#define GAME_MESSAGE_WEAPON_RETURN(WEAPON, ATTACKER) ("�U��񂵂�%s��%s�ɕԂ��Ă����B", (WEAPON), (ATTACKER))
#define GAME_MESSAGE_FATAL_SPOT "%s�̋}���ɓ˂��h�������I"
#define GAME_MESSAGE_SUICIDE_BOMBER "�������ĕ��X�ɂȂ����B"
#define GAME_MESSAGE_CARSE_OF_BLOOD_DEAD "%^s�͎��̊ԍۂɋ��낵�����̎􂢂�%^s�ɂ������I"
#define GAME_MESSAGE_LOST_LIFE1 "�����͂������z�����ꂽ�C������I"
#define GAME_MESSAGE_LOST_LIFE2 "�����͂��̂���z�����ꂽ�C������I"
#define GAME_MESSAGE_BLOE_AWAY "%s�𐁂���΂����I"
#define GAME_MESSAGE_CAMP_GET_ANGRY "%s�͓{�����I"
#define GAME_MESSAGE_HOSTILE "%^s�͓G�ɂ܂�����I"
#define GAME_MESSAGE_BOUNTY_DEAD "%s�̎�ɂ͕񏧂��������Ă���B"
#else
#define GAME_MESSAGE_FATAL_SPOT "Your shot sticked on a fatal spot of %s!"
#define GAME_MESSAGE_WEAPON_ATTACK_MISS %^s misses %s.
#define GAME_MESSAGE_WEAPON_AMBUSH(ATTACKER, TARGET) "%s (cruelly stab the helpless, sleeping %s!", (ATTACKER), (TARGET))
#define GAME_MESSAGE_WEAPON_FATAL_SPOT(ATTACKER, TARGET) ("%s make surprise attack, and hit %s with a powerful blow!", (ATTACKER), (TARGET))
#define GAME_MESSAGE_WEAPON_BACKSTUB(ATTACKER, TARGET) ("%s ambush the fleeing %s.", (ATTACKER), (TARGET))
#define GAME_MESSAGE_WEAPON_ZANTETSU_JELLY "You cannot cut such a elastic thing!"
#define GAME_MESSAGE_WEAPON_ZANTETSU_SERIF "Sigh... Another trifling thing I've cut...."
#define GAME_MESSAGE_WEAPON_EX_JR_SPIDER "Spiders are difficult for you to deal with!"
#define GAME_MESSAGE_WEAPON_VORPAL_BLADE_SERIF "Your Vorpal Blade goes snicker-snack!"
#define GAME_MESSAGE_WEAPON_SUCK_BLOOD(WEAPON) ("%s sucked blood, and became more powerful!", (WEAPON))
#define GAME_MESSAGE_WEAPON_RETURN(WEAPON, ATTACKER) ("%s returns to %s.", ATTACKER, WEAPON)
#define GAME_MESSAGE_SUICIDE_BOMBER "%^s explodes into tiny shreds."
#define GAME_MESSAGE_CARSE_OF_BLOOD_DEAD "On death and dying, %^s puts a terrible blood curse on %^s!"
#define GAME_MESSAGE_LOST_LIFE1 "You feel your life slipping away!"
#define GAME_MESSAGE_LOST_LIFE2 "You feel your life draining away..."
#define GAME_MESSAGE_BLOE_AWAY "You blow %s away!"
#define GAME_MESSAGE_CAMP_GET_ANGRY "%^s gets angry!"
#define GAME_MESSAGE_HOSTILE "%^s suddenly becomes hostile!"
#define GAME_MESSAGE_BOUNTY_DEAD "There is a price on %s's head."
#endif

#ifdef JP
#define GAME_MESSAGE_RESTORE_SOME_MP "���������n�b�L�������B"
#else
#define GAME_MESSAGE_RESTORE_SOME_MP "You feel your head clear a little."
#endif

#ifdef JP
#define GAME_MESSAGE_EARTHQUAKE "�n�ʂ��h�ꂽ�B"
#else
#define GAME_MESSAGE_EARTHQUAKE "The ground trembles."
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
#define GAME_MESSAGE_DETECT_NOLIVING "���R�łȂ��N���[�`���[�̑��݂��������I"
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
#define GAME_MESSAGE_DETECT_NOLIVING "You sense the presence of unnatural beings!"
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

#ifdef JP
#define GAME_MESSAGE_FEATURE_NO_UP_STAIR "�����ɂ͏��K�i����������Ȃ��B"
#define GAME_MESSAGE_FEATURE_NO_DOWN_STAIR "�����ɂ͉���K�i����������Ȃ��B"
#define GAME_MESSAGE_FEATURE_UP_STAIR "��̊K�ɍ~�肽�B"
#define GAME_MESSAGE_FEATURE_DOWN_STAIR "���̊K�ɍ~�肽�B"
#define GAME_MESSAGE_FEATURE_CREATE_STAIR "���@�̊K�i�����ꂽ..."

#define GAME_MESSAGE_FEATURE_STUCK "%s�͂�������ƕ����Ă���悤���B"
#define GAME_MESSAGE_FEATURE_CLOSED_DUNGEON "�_���W�����̓����͍ǂ���Ă���I"
#define GAME_MESSAGE_FEATURE_CHECK_DUNGEON "�����ɂ�%s�̓����(%d�K����)������܂�"
#define GAME_MESSAGE_FEATURE_ENTER_DUNGEON "�{���ɂ��̃_���W�����ɓ���܂����H"

#define GAME_MESSAGE_PATTERN_LIMIT1 "�p�^�[���̏������n�߂�ƁA�S�Ă�����Ȃ���΂Ȃ�܂���B�����ł����H"
#define GAME_MESSAGE_PATTERN_LIMIT2 "�p�^�[���̏������ɂ̓X�^�[�g�n�_��������n�߂Ȃ��Ă͂Ȃ�܂���B"
#define GAME_MESSAGE_PATTERN_LIMIT3 "�p�^�[���̏�͐����������ŕ����˂΂Ȃ�܂���B"
#define GAME_MESSAGE_PATTERN_LIMIT4 "�p�^�[���𓥂݊O���Ă͂����܂���B"
#define GAME_MESSAGE_PATTERN_GOAL "�u�p�^�[���v�̂��̕����͑��̕�����苭�͂łȂ��悤���B"
#define GAME_MESSAGE_PATTERN_WRECKED "�p�^�[�������ŉ����Ă��܂����I�������낵�������N���肻�����I"

#else
#define GAME_MESSAGE_FEATURE_NO_UP_STAIR "I see no up staircase here."
#define GAME_MESSAGE_FEATURE_NO_DOWN_STAIR "I see no down staircase here."
#define GAME_MESSAGE_FEATURE_UP_STAIR "You enter the up staircase."
#define GAME_MESSAGE_FEATURE_DOWN_STAIR "You enter the down staircase."
#define GAME_MESSAGE_FEATURE_CREATE_STAIR "A magical staircase appears..."

#define GAME_MESSAGE_FEATURE_STUCK "The %s appears to be stuck."
#define GAME_MESSAGE_FEATURE_CLOSED_DUNGEON "The entrance of this dungeon is closed!"
#define GAME_MESSAGE_FEATURE_CHECK_DUNGEON "There is the entrance of %s (Danger level: %d)"
#define GAME_MESSAGE_FEATURE_ENTER_DUNGEON "Do you really get in this dungeon? "

#define GAME_MESSAGE_PATTERN_LIMIT1 "If you start walking the Pattern, you must walk the whole way. Ok? "
#define GAME_MESSAGE_PATTERN_LIMIT2 "You must start walking the Pattern from the startpoint."
#define GAME_MESSAGE_PATTERN_LIMIT3 "You must walk the Pattern in correct order."
#define GAME_MESSAGE_PATTERN_LIMIT4 "You may not step off from the Pattern."
#define GAME_MESSAGE_PATTERN_GOAL "This section of the Pattern looks less powerful."
#define GAME_MESSAGE_PATTERN_WRECKED "You bleed on the Pattern! Something terrible happens!"
#endif

#ifdef JP
#define GAME_MESSAGE_SUMMON_SERVANT "��p�ł������܂����H�䂪��B"
#else
#define GAME_MESSAGE_SUMMON_SERVANT "'What is thy bidding... Master?'"
#endif

#ifdef JP
#define GAME_MESSAGE_OBJECT_KILLED "���Ă��܂����B"
#define GAME_MESSAGE_OBJECT_DUSTED "�D�ɂȂ����B"
#else
#define GAME_MESSAGE_OBJECT_KILLED (plural ? " are destroyed!" : " is destroyed!")
#define GAME_MESSAGE_OBJECT_DUSTED (plural ? " become dust." : " becomes dust.")
#endif

#ifdef JP
#define GAME_MESSAGE_DIGGING_PERMANENT1 "���̊�͍d�����Č@��Ȃ��悤���B"
#define GAME_MESSAGE_DIGGING_PERMANENT2 "�����͌@��Ȃ�!"
#define GAME_MESSAGE_DIGGED1(NAME) ("%s�����������B", (NAME))
#define GAME_MESSAGE_DIGGED2(NAME) ("%s���������Ă���B", (NAME))
#else
#define GAME_MESSAGE_DIGGING_PERMANENT1 "This seems to be permanent rock."
#define GAME_MESSAGE_DIGGING_PERMANENT2 "You can't tunnel through that!"
#define GAME_MESSAGE_DIGGED1(NAME) ("You have removed the %s.", (NAME))
#define GAME_MESSAGE_DIGGED2(NAME) ("You dig into the %s.", (NAME))
#endif



// Cause of Death
#if JP

#define COD_QUITTING "�r���I��"
#define COD_INTERRUPTING "�����I��"
#define COD_CHEATING_DEATH "���̋\��"
#define COD_SAVE "(�Z�[�u)"
#define COD_PANIC_SAVE "(�ً}�Z�[�u)"
#define COD_ALIVE_AND_WELL "(���C�ɐ����Ă���)"
#define COD_RETIRE "����"
#define COD_SEPPUKU "�ؕ�"
#define COD_SUICIDE_BOMBER "����"
#define COD_TRAP "�g���b�v"
#define COD_POISON "��"
#define COD_WOUND "�v����"
#define COD_STARVATION "�Q��"
#define COD_DART_TRAP "�_�[�c���"
#define COD_PERILOUS_IDENTIFY "�댯�Ȕ閧"
#define COD_TRIPPING "�]�|"
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
#define COD_GENOCIDE_CREATURE "�N���[�`���[����"
#define COD_GENOCIDE_SYMBOL "���E"
#define COD_GENOCIDE_MASS "���Ӗ��E"
#define COD_GENOCIDE_UNDEAD "�A���f�b�h����"
#define COD_HP_TO_MP "HP����MP�ւ̖��d�ȕϊ�"
#define COD_HP_TO_MP_PASSIVE "���ɏ�������"
#define COD_HARD_CONCENTRATION "�ߓx�̏W��"
#define COD_BLOOD_CURSE "���̎􂢂̎���"
#define COD_ABSTINENCE "�֗~����������J"
#define COD_BUNP_WALL "�ǂւ̏Փ�"
#define COD_DENSITY "���x"
#define COD_SOLID_ROCK "�d����"
#define COD_DROWNING "�M��"
#define COD_FIRE_AURA "���̃I�[��"
#define COD_COLD_AURA "��C�̃I�[��"
#define COD_ELEC_AURA "�d���̃I�[��"
#define COD_LETHAL_MUTATION "�v���I�ȓˑR�ψ�"
#define COD_POLYMORPHED_WOUND "�ω�������"
#define COD_TY_CURSE_ENERGY "�G�l���M�[�̂��˂�"
#define COD_UNC_FORCE "�C�̖\��" 
#define COD_CHAOS_TAINT "���ׂɐI�܂ꂽ�_���[�W"
#define COD_PATRON_WRATH(PATRON) "%s�̓{��", (PATRON)
#else

#define COD_QUITTING "Quitting"
#define COD_INTERRUPTING "Interrupting"
#define COD_CHEATING_DEATH "cheating death"
#define COD_SAVE "(saved)"
#define COD_PANIC_SAVE "(panic save)"
#define COD_ALIVE_AND_WELL "(alive and well)"
#define COD_RETIRE "retired"
#define COD_SEPPUKU "seppuku"
#define COD_SUICIDE_BOMBER "suicide bomber"
#define COD_TRAP "a trap"
#define COD_POISON "poison"
#define COD_WOUND "a fatal wound"
#define COD_STARVATION "starvation"
#define COD_DART_TRAP "a dart trap"
#define COD_PERILOUS_IDENTIFY "perilous secrets"
#define COD_FALL_RIDING "Falling from riding"
#define COD_TRIPPING "Tripping"
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
#define COD_GENOCIDE_CREATURE "Genocide one"
#define COD_GENOCIDE_SYMBOL "Genocide"
#define COD_GENOCIDE_MASS "Mass Gecocide"
#define COD_GENOCIDE_UNDEAD "Annihilate Undead"
#define COD_HP_TO_MP "Thoughtless convertion from HP to MP"
#define COD_HP_TO_MP_PASSIVE "blood rushing to the head"
#define COD_HARD_CONCENTRATION "Concentrating too hard"
#define COD_ABSTINENCE "the strain of forcing abstinence"
#define COD_BLOOD_CURSE "self wound for blood curse"
#define COD_BUNP_WALL "bumping into wall"
#define COD_DENSITY "density"
#define COD_SOLID_ROCK "solid rock"
#define COD_DROWNING "drowning"
#define COD_FIRE_AURA "fire aura"
#define COD_COLD_AURA "cold aura"
#define COD_ELEC_AURA "elec aura"
#define COD_LETHAL_MUTATION "a lethal mutation"
#define COD_POLYMORPHED_WOUND "a polymorphed wound"
#define COD_TY_CURSE_ENERGY "surge of energy"
#define COD_UNC_FORCE "Explosion of the Force"
#define COD_CHAOS_TAINT "Damage of tainted by chaos"
#define COD_PATRON_WRATH(PATRON) "the Wrath of %s", (PATRON)
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
	#define PROMPT_COUNT "��: "
	#define PROMPT_REST "�x�e (0-9999, '*' �� HP/MP�S��, '&' �ŕK�v�Ȃ���): "
	#define PROMPT_PICK "%s���E���܂���? " 
	#define PROMPT_DIRECTION "������(ESC�Œ��f)? "
#else
	#define PROMPT_COMMAND "Command: "
	#define PROMPT_KEYPRESS "Keypress: "
	#define PROMPT_FILE "File: "
	#define PROMPT_MEMO "Memo: "
	#define PROMPT_TRIGGER_KEY "Trigger: "
	#define PROMPT_INVENTORY "Inven: "
	#define PROMPT_EQUIPMENT "Equip: "
	#define PROMPT_COUNT "Count: "
	#define PROMPT_REST "Rest (0-9999, '*' for HP/SP, '&' as needed): "
	#define PROMPT_PICK "Pick up %s? " 
	#define PROMPT_DIRECTION "Direction (Escape to cancel)? "
#endif

#ifdef JP
#define MES_GET_CHECK_AHEAD "�{���ɂ��̂܂ܐi�ނ��H"
#define MES_GET_CHECK_SUICIDE "�{���Ɏ��E���܂����H"
#define MES_GET_CHECK_LEVEL_TELEPORT "�{���ɑ��̊K�Ƀe���|�[�g���܂����H"
#else
#define MES_GET_CHECK_AHEAD "Really want to go ahead? "
#define MES_GET_CHECK_SUICIDE "Do you really want to commit suicide? "
#define MES_GET_CHECK_LEVEL_TELEPORT "Are you sure? (Teleport Level)"
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
	#define SKILL_NAME_FORCE "�B�C�p"
	#define SKILL_NAME_BRUTAL "���z"
#else
	#define SKILL_NAME_MAGIC "magic"
	#define SKILL_NAME_PSIONIC "psionic powers"
	#define SKILL_NAME_IMITATION "imitation"
	#define SKILL_NAME_HISSATSU "hissatsu"
	#define SKILL_NAME_MIRROR_M "mirror magic"
	#define SKILL_NAME_NINJUTSU "ninjutsu"
	#define SKILL_NAME_PRAYER "prayer"
	#define SKILL_NAME_FORCE "force"
	#define SKILL_NAME_BRUTAL "brutal power"
#endif

#ifdef JP

#define DIARY_CREATE_PLAYER "-------- �v���C���[�쐬 --------"
#define DIARY_GAMEOVER "-------- �Q�[���I�[�o�[ --------"
#define DIARY_SAVEQUIT "----�Q�[�����f----"
#define DIARY_GAMERESTART "----�Q�[���ĊJ----"
#define DIARY_GAMEABORT "----�Q�[���ُ�I��----"
#define DIARY_SELECTMODE "�L�����y�[���w%s�x��I������"
#define DIARY_RETIRED "�_���W�����̒T��������ނ����B"
#define DIARY_SUICIDE "��]���Ď��E�����B"
#define DIARY_WINNING_SEPPUKU "�����̌�ؕ������B"
#define DIARY_WIZARD "�E�B�U�[�h���[�h�ɓ˓����ăX�R�A���c���Ȃ��Ȃ����B"
#define DIARY_UP_STAIR "�K�i�������"
#define DIARY_DOWN_STAIR "�K�i�����肽"
#define DIARY_TRAP_DOOR "���Ƃ��˂ɗ�����"
#define DIARY_PINCH "%s�ɂ���ăs���`�Ɋׂ����B"
#define DIARY_PATRON(REWARD) "�p�g�����̕�V��%s", (REWARD)
#else

#define DIARY_CREATE_PLAYER "-------- Create Player --------"
#define DIARY_GAMEOVER "--------   Game  Over   --------"
#define DIARY_SAVEQUIT "---- Save and Exit Game ----"
#define DIARY_GAMERESTART "---- Restart Game ----"
#define DIARY_GAMEABORT "---- Panic Save and Abort Game ----"
#define DIARY_SELECTMODE "Select Campaign '%s'"
#define DIARY_RETIRED "retired exploring dungeons."
#define DIARY_SUICIDE "give up all hope to commit suicide."
#define DIARY_WINNING_SEPPUKU "did Seppuku after the winning."
#define DIARY_WIZARD "give up sending score to use wizard mode."
#define DIARY_UP_STAIR 1"climbed up the stairs to"
#define DIARY_DOWN_STAIR "climbed down the stairs to"
#define DIARY_TRAP_DOOR "You have fallen through a trap door."
#define DIARY_PINCH "A critical situation because of %s".
#define DIARY_PATRON(REWARD) "The patron rewards you with %s.", (REWARD)
#endif

#ifdef JP
#define SHUFFLE_START "���Ȃ��̓J�[�h��؂��Ĉꖇ������..."
#define SHUFFLE_TELEPORT "�e���|�[�g�E�J�[�h���B"
#define SHUFFLE_FRIEND "�F�D�I�ȃN���[�`���[�̊G���B"
#define SHUFFLE_DEATH "�Ȃ�Ă������I�s���t���I"
#define SHUFFLE_DEVIL "�Ȃ�Ă������I�s�����t���I"
#define SHUFFLE_HANGEDMAN "�Ȃ�Ă������I�s�݂�ꂽ�j�t���I"
#define SHUFFLE_DISCODE "�s�s���a�̌��t���B"
#define SHUFFLE_FOOL "�s���ҁt���B"
#define SHUFFLE_STRANGE "��ȃN���[�`���[�̊G���B"
#define SHUFFLE_MOON "�s���t���B"
#define SHUFFLE_WHEEL "�s�^���̗ցt���B"
#define SHUFFLE_JUSTICE "�s���`�t���B"
#define SHUFFLE_TOWER "�s���t���B"
#define SHUFFLE_TEMPERANCE "�s�ߐ��t���B"
#define SHUFFLE_LOVER "�s���l�t���B"
#define SHUFFLE_HERMIT "�s�B�ҁt���B"
#define SHUFFLE_JUDGEMENT "�s�R���t���B"
#define SHUFFLE_SUN "�s���z�t���B"
#define SHUFFLE_WORLD "�s���E�t���B"
#else
#define SHUFFLE_START "You shuffle the deck and draw a card..."
#define SHUFFLE_TELEPORT "It's a teleport trump card."
#define SHUFFLE_FRIEND "It's the picture of a friendly creature."
#define SHUFFLE_DEATH "Oh no! It's Death!"
#define SHUFFLE_DEVIL "Oh no! It's the Devil!"
#define SHUFFLE_HANGEDMAN "Oh no! It's the Hanged Man."
#define SHUFFLE_DISCODE "It's the swords of discord."
#define SHUFFLE_FOOL "It's the Fool."
#define SHUFFLE_STRANGE "It's the picture of a strange creature."
#define SHUFFLE_MOON "It's the Moon."
#define SHUFFLE_WHEEL "It's the Wheel of Fortune."
#define SHUFFLE_JUSTICE "It's Justice."
#define SHUFFLE_TOWER "It's the Tower."
#define SHUFFLE_TEMPERANCE "It's Temperance."
#define SHUFFLE_LOVER "It's the Lovers."
#define SHUFFLE_HERMIT "It's the Hermit."
#define SHUFFLE_JUDGEMENT "It's the Judgement."
#define SHUFFLE_SUN "It's the Sun."
#define SHUFFLE_WORLD "It's the World."
#endif

#ifdef JP
#define INVOKE_SPIRIT_START "���Ȃ��͎��҂����̗͂����W����..."
#define INVOKE_SPIRIT_HIGH_POWER "���Ȃ��͂��ǂ남�ǂ낵���͂̂��˂���������I"
#define INVOKE_SPIRIT_LOW1 "�Ȃ�Ă������I���Ȃ��̎���̒n�ʂ��狀�����l�e�������オ���Ă����I"
#define INVOKE_SPIRIT_LOW2 "���󂵓�׈��ȑ��݂����Ȃ��̐S��ʂ�߂��čs����..."
#define INVOKE_SPIRIT_LOW3 "���Ȃ��̓��ɑ�ʂ̗H�삽���̑��X�������������񂹂Ă���..."
#define INVOKE_SPIRIT_FUNBLE_AFTER "�A�T�Ȑ����N�X�N�X�΂��B�u�����������܂��͉�X�̒��ԂɂȂ邾�낤�B�カ�҂�B�v"
#else
#define INVOKE_SPIRIT_START "You call on the power of the dead..."
#define INVOKE_SPIRIT_HIGH_POWER "You feel a surge of eldritch force!"
#define INVOKE_SPIRIT_LOW1 "Oh no! Mouldering forms rise from the earth around you!"
#define INVOKE_SPIRIT_LOW2 "An unnamable evil brushes against your mind..."
#define INVOKE_SPIRIT_LOW3 "Your head is invaded by a horde of gibbering spectral voices..."
#define INVOKE_SPIRIT_FUNBLE_AFTER "Sepulchral voices chuckle. 'Soon you will join us, mortal.'"
#endif

#ifdef JP
#define DEBUG_MESSAGE_ROOM_NUM "������: %d"
#define DEBUG_MESSAGE_GLASS_ROOM "[�K���X�̕���]"
#define DEBUG_MESSAGE_ROOM_OF "[%s�̕���]"
#define DEBUG_MESSAGE_CRYPT_ROOM "[����]"
#define DEBUG_MESSAGE_TRAP_PIT_ROOM "[%s%s��㩃s�b�g]"
#define DEBUG_MESSAGE_BIAS_ARTIFACT "�^�̕΂����A�[�e�B�t�@�N�g: %d�B"
#define DEBUG_MESSAGE_NO_BIAS_ARTIFACT "�΂��Ă��Ȃ��A�[�e�B�t�@�N�g"
#define DEBUG_MESSAGE_DIE_CHOICE "���ɂ܂���?"
#define DEBUG_MESSAGE_LAKE "�΂𐶐��B"
#define DEBUG_MESSAGE_CAVE "���A�𐶐��B"
#define DEBUG_MESSAGE_ARENA "�A���[�i���x��"
#define DEBUG_MESSAGE_DEC_CREATURE(FROM, TO) "�N���[�`���[����{�l�� %d ���� %d �Ɍ��炵�܂�", (FROM), (TO)
#define DEBUG_MESSAGE_SMALL_LEVEL "�����ȃt���A"
#define DEBUG_MESSAGE_FLOOR_SIZE(WIDTH, HEIGHT) "[W:%d, H:%d]"
#define DEBUG_MESSAGE_VAULT(NAME) "�n����(%s)", (NAME)
#define DEBUG_MESSAGE_GREATER_VAULT(NAME) "����Ȓn����(%s)", (NAME)
#define DEBUG_MESSAGE_FAILED_VAULT "����Ȓn������z�u�ł��܂���B"
#define DEBUG_MESSAGE_ARTIFACT_CANCEL(OBJECT) "�`���̃A�C�e�� (%s) �͐�������*�j��*���ꂽ�B", (OBJECT)
#define DEBUG_MESSAGE_RANDOM_ARTIFACT_CANCEL "�����_���E�A�[�e�B�t�@�N�g��1�͐�������*�j��*���ꂽ�B"
#else
#define DEBUG_MESSAGE_ROOM_NUM "Number of Rooms: %d"
#define DEBUG_MESSAGE_GLASS_ROOM "[Glass Room]"
#define DEBUG_MESSAGE_ROOM_OF "[Room of %s]"
#define DEBUG_MESSAGE_CRYPT_ROOM "[Crypt]"
#define DEBUG_MESSAGE_TRAP_PIT_ROOM "[Trapped creature pit (%s%s)]"
#define DEBUG_MESSAGE_BIAS_ARTIFACT "Biased artifact: %d."
#define DEBUG_MESSAGE_NO_BIAS_ARTIFACT "No biased artifact."
#define DEBUG_MESSAGE_DIE_CHOICE "Die?"
#define DEBUG_MESSAGE_LAKE "Lake on the level."
#define DEBUG_MESSAGE_CAVE "Cavern on level."
#define DEBUG_MESSAGE_ARENA "Arena level."
#define DEBUG_MESSAGE_DEC_CREATURE(FROM, TO) "Reduced creatures base from %d to %d", (FROM), (TO)
#define DEBUG_MESSAGE_SMALL_LEVEL "A small dungeon level."
#define DEBUG_MESSAGE_FLOOR_SIZE(WIDTH, HEIGHT) "[W:%d, H:%d]"
#define DEBUG_MESSAGE_VAULT(NAME) "Vault(%s)", (NAME)
#define DEBUG_MESSAGE_GREATER_VAULT(NAME) "Greater vault (%s)", (NAME)
#define DEBUG_MESSAGE_FAILED_VAULT "Could not place greater vault."
#define DEBUG_MESSAGE_ARTIFACT_CANCEL(OBJECT) "Artifact (%s) was *destroyed* during generation.", (OBJECT)
#define DEBUG_MESSAGE_RANDOM_ARTIFACT_CANCEL "One of the random artifacts was *destroyed* during generation."
#endif

#ifdef JP
#define EFFECT_MES_MIRROR_CRUSH "�������ꂽ�I"
#define EFFECT_MES_MIND_BACKFIRE "%^s�̑��������_�͍U���𒵂˕Ԃ����I"
#define EFFECT_MES_DISARM_TRAP "�J�`�b�Ɖ��������B"
#else
#define EFFECT_MES_MIRROR_CRUSH "The mirror was crashed!"
#define EFFECT_MES_MIND_BACKFIRE "%^s corrupted mind backlashes your attack!"
#define EFFECT_MES_DISARM_TRAP "Click!"
#endif

#ifdef JP
#define MES_BIRTH_SELECT_CAMPAIGN "�L�����y�[����I�����ĉ�����"
#else
#define MES_BIRTH_SELECT_CAMPAIGN "Select Campaign"
#endif

#ifdef JP
#define MES_QUEST_INFO(LEVEL) "�N�G�X�g��� (�댯�x: %d �K����)", (LEVEL)
#define MES_QUEST_NO_QUEST "���̂Ƃ���N�G�X�g�͂���܂���B"
#define MES_QUEST_DEFAULT_KILLING "�N�G�X�g: %s�� %d�̓|��"
#define MES_QUEST_WARN_UNCOMPLETE "���Ȃ��͌��݂̃N�G�X�g���I�������Ă��܂���I\nCTRL-Q���g���΃N�G�X�g�̏�Ԃ��`�F�b�N�ł��܂��B\n�N�G�X�g���I��点����߂��ė��ĉ������B\n"
#define MES_QUEST_ASK_ACCEPT "���̃N�G�X�g��������܂����H"
#define MES_QUEST_ACCEPT "�N�G�X�g�w%s�x��������܂����B"
#define MES_QUEST_NO_ENTRANCE "�����ɂ̓N�G�X�g�̓����͂Ȃ�"
#define MES_QUEST_ENTRANCE "�����ɂ̓N�G�X�g�ւ̓���������܂��B"
#define MES_QUEST_ASK_ENTER "�N�G�X�g�ɓ���܂����H"
#define MES_QUEST_TARGET_INFO(TARGET) "���ӂ���I���̊K��%s�ɂ���Ď���Ă���I", (TARGET)
#define MES_QUEST_TARGETS_INFO(TARGET, NUM) "���ӂ���I���̊K��%d�̂�%s�ɂ���Ď���Ă���I", (NUM), (TARGET)
#define MES_QUEST_EXIT_WARNING "���̊K����x����Ɠ�x�Ɩ߂��ė����܂���B"
#define MES_QUEST_ASK_EXIT "�{���ɂ��̊K������܂����H"
#else
#define MES_QUEST_INFO(LEVEL) ("Quest Information (Danger level: %d)", (LEVEL))
#define MES_QUEST_NO_QUEST "I don't have a quest for you at the moment."
#define MES_QUEST_DEFAULT_KILLING "Your quest: kill %d %s"
#define MES_QUEST_WARN_UNCOMPLETE "You have not completed your current quest yet!\nUse CTRL-Q to check the status of your quest.\nReturn when you have completed your quest."
#define MES_QURST_ASK_ACCEPT "Accept this quest? "
#define MES_QUEST_ACCEPT "You accepted the quest, '%s'. "
#define MES_QUEST_NO_ENTRANCE "You see no quest level here."
#define MES_QUEST_ENTRANCE "There is an entry of a quest."
#define MES_QUEST_ASK_ENTER "Do you enter? "
#define MES_QUEST_TARGET_INFO(TARGET) "Beware, this level is protected by %s!", (TARGET)
#define MES_QUEST_TARGETS_INFO(TARGET, NUM) "Be warned, this level is guarded by %d %s!", (NUM), (TARGET)
#define MES_QUEST_EXIT_WARNING "You can't come back here once you leave this floor."
#define MES_QUEST_ASK_EXIT "Really leave this floor? "
#endif

static cptr desc_stat_pos[] =
{
#ifdef JP
	"����",
	"�m�I��",
	"����",
	"��p��",
	"���N��",
	"������"
#else
	"strong",
	"smart",
	"wise",
	"dextrous",
	"healthy",
	"cute"
#endif
};


static cptr desc_stat_neg[] =
{
#ifdef JP
	"�キ",
	"���m��",
	"������",
	"�s��p��",
	"�s���N��",
	"�X��"
#else
	"weak",
	"stupid",
	"naive",
	"clumsy",
	"sickly",
	"ugly"
#endif
};

#ifdef JP
#define MES_CREATURE_LEVELUP(LEV) "���x�� %d �ɂ悤�����B", (LEV)
#define MES_CREATURE_DEC_STATUS(CREATURE, DEC) "%s�͂Ђǂ�%s�Ȃ����B", (CREATURE)->name, (DEC)
#define MES_CREATURE_DEC_STATUS_CANCEL(CREATURE, DEC) "%s�͂Ђǂ�%s�Ȃ肩�������A���ɖ߂���", (CREATURE)->name, (DEC)
#define MES_CREATURE_LIFE_RATE(NUM) "���݂̗̑̓����N : %d/100", (NUM)
#else
#define MES_CREATURE_LEVELUP(LEV) "Welcome to level %d.", (LEV)
#define MES_CREATURE_DEC_STATUS(CREATURE, DEC) "%s become very %s.", (CREATURE)->name, (DEC)
#define MES_CREATURE_DEC_STATUS_CANCEL(CREATURE, DEC) "%s become %s for a moment, but the feeling passes.", (CREATURE)->name, (DEC)
#define MES_CREATURE_LIFE_RATE(NUM) "Your current Life Rating is %d/100.", (NUM)
#endif

#ifdef JP
#define MES_POLYSELF_FEELING "���Ȃ��͕ω��̖K���������..."
#define MES_POLYSELF_ORDINARY "��Ȃ��炢���ʂɂȂ����C������B"
#define MES_POLYSELF_DIFFICULT_FORM "���݂̎p�Ő����Ă����͍̂���Ȃ悤���I"
#else
#define MES_POLYSELF_FEELING "You feel a change coming over you..."
#define MES_POLYSELF_ORDINARY "You feel oddly normal."
#define MES_POLYSELF_DIFFICULT_FORM "You find living difficult in your present form!"
#endif

#ifdef JP
#define MES_STORE_ENTIRE "����őS���ł��B"
#define MES_STORE_ITEM_LIMIT "����ȂɃA�C�e�������ĂȂ��B"
#define MES_STORE_ITEM_LIMIT2 "�U�b�N�ɂ��̃A�C�e�������錄�Ԃ��Ȃ��B"
#define MES_STORE_FIXED_PRICE(PRICE) "��ɂ� $%ld�ł��B", (PRICE)
#define MES_STORE_BUYING(NAME, SYM) "%s(%c)���w������B", (NAME), (SYM)
#define MES_STORE_BOUGHT(NAME, PRICE) "%s�� $%ld�ōw�����܂����B", (NAME), (PRICE)
#define MES_STORE_SELL(NAME, PRICE) "%s�� $%ld�Ŕ��p���܂����B", (NAME), (PRICE)
#define MES_STORE_NO_FIT "���Ȃ��̑̊i�ɍ���Ȃ��悤�����A��낵�����H"
#define MES_STORE_NEW_STOCK "�X��͐V���ȍ݌ɂ����o�����B"
#define MES_STORE_DROP(NAME, SYM) "%s��u�����B(%c)", (NAME), (SYM)
#else
#define MES_STORE_ENTIRE "That's all."
#define MES_STORE_ITEM_LIMIT "You cannot carry that many different items."
#define MES_STORE_ITEM_LIMIT2 "You cannot carry that many items."
#define MES_STORE_FIXED_PRICE(PRICE) "That costs %ld gold per item.", (PRICE)
#define MES_STORE_BUYING(NAME, SYM) "Buying %s(%c).", (NAME), (SYM)
#define MES_STORE_BOUGHT(NAME, PRICE) "You bought %s for %ld gold.", (NAME), (PRICE)
#define MES_STORE_SELL(NAME, PRICE) "You sold %s for %ld gold.", (NAME), (PRICE)
#define MES_STORE_NO_FIT "It may be not your size, Are you sure?"
#define MES_STORE_NEW_STOCK "The shopkeeper brings out some new stock."
#define MES_STORE_DROP(NAME, SYM) "You drop %s (%c).", (NAME), (SYM)
#endif

#ifdef JP
#define MES_STATUS_CUMBER_GROVE "�肪�����Ď����������ɂ�������������B"
#define MES_STATUS_NO_CUMBER_GROVE "���̎�̏�ԂȂ�A�����Ǝ����������₷���������B"
#define MES_STATUS_CUMBER_ARMOR "�����̏d���œ������݂��Ȃ��Ă��܂��Ă���B"
#define MES_STATUS_NO_CUMBER_ARMOR "�����Ɗy�ɑ̂𓮂�����悤�ɂȂ����B"
#else
#define MES_STATUS_CUMBER_GROVE "Your covered hands feel unsuitable for spellcasting."
#define MES_STATUS_NO_CUMBER_GROVE "Your hands feel more suitable for spellcasting."
#define MES_STATUS_CUMBER_ARMOR "The weight of your equipment encumbers your movement."
#define MES_STATUS_NO_CUMBER_ARMOR "You feel able to move more freely."
#endif

#ifdef JP
#define MES_PATRON_BOOM_OUT(PATRON) "%s�̐��������n����:", (PATRON)
#else
#define MES_PATRON_BOOM_OUT(PATRON) "The voice of %s booms out:", (PATRON)
#endif

#ifdef JP
#define MES_INTERFACE_DUMP "['h'�Ń��[�h�ύX, ESC�ŏI��]"
#else
#define MES_INTERFACE_DUMP "['h' to change mode, or ESC]"
#endif

#ifdef JP
#define MES_MELEE_DISABLE(ATTACKER) "%s�͍U���ł��Ȃ��B", (ATTACKER)
#define MES_MELEE_NO_METHOD(ATTACKER) "%s�͍U�������i�������Ă��Ȃ��B", (ATTACKER)
#else
#define MES_MELEE_DISABLE(ATTACKER) "%s don't have attack method.", (ATTACKER)
#define MES_MELEE_NO_METHOD(ATTACKER) "%s don't have attack method.", (ATTACKER)
#endif