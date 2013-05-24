
#ifdef JP
#define BRACKET_ARTIFACT "�w%s�x"
#define BRACKET_DAMMY_ARTIFACT "�s%s�t"
#else
#define BRACKET_ARTIFACT "'%s'"
#define BRACKET_DAMMY_ARTIFACT "'%s'"
#endif

#ifdef JP
#define BRACK_D_ANGLE "�s"
#define KET_D_ANGLE "�t"
#else
#define BRACK_D_ANGLE "<<"
#define KET_D_ANGLE ">>"
#endif

// Keywords

#ifdef JP
#define KEYWORD_CURSED "����Ă���"
#else
#define KEYWORD_CURSED "cursed"
#endif

#ifdef JP
#define MES_INITIALIZE_USER "[���[�U�[�ݒ�t�@�C�������������Ă��܂�]"
#define MES_INITIALIZE_COMPLETE "[�������I��]"
#else
#define MES_INITIALIZE_USER "[Initializing user pref files]"
#define MES_INITIALIZE_COMPLETE "[Initialization complete]"
#endif

#ifdef JP

#define MES_SYS_YOU_DIED(CREATURE) (has_trait((CREATURE), TRAIT_ANDROID) ? "���Ȃ��͉��܂���" : "���Ȃ��͎��ɂ܂���")
#define MES_SYS_MORE "-- ���� --"
#define MES_SYS_WIZARD_MODE_ON "�E�B�U�[�h���[�h�˓��B"
#define MES_SYS_WIZARD_MODE_OFF "�E�B�U�[�h���[�h�����B"
#define MES_SYS_STARTING_GAME "�Q�[�����J�n���܂��B���҂�������..."
#define MES_SYS_PLEASE_WAIT "���҂�������..."
#define MES_SYS_FAILED_TEMPFILE "�ꎞ�t�@�C�� %s ���쐬�ł��܂���ł����B"
#define MES_SYS_FAILED_FILEOPEN "�t�@�C�����J���܂���B"
#define MES_SYS_FAILED_FILEOPEN2 "%s ���J�����Ƃ��ł��܂���ł����B"
#define MES_SYS_FAILED_FILEREAD "�t�@�C����ǂ߂܂���B"
#define MES_SYS_REPLACE_FILE "��������t�@�C���ɏ㏑�����܂���? (%s)"
#define MES_SYS_HIT_ANY_KEY "�����L�[�������ĉ������B"
#define MES_SYS_HIT_SPACE_KEY "�X�y�[�X�L�[�ő��s"
#define MES_SYS_HIT_VERIFY_KEY "�m�F�̂��� '@' �������ĉ������B"
#define MES_SYS_PLACE_SPACE "�ꏊ��I��ŃX�y�[�X�L�[�������ĉ������B"
#define MES_SYS_OPTION_AUX "�����I�v�V����((*)�̓X�R�A�ɉe��)"
#define MES_SYS_OPTION_AUX2 "�����I�v�V����(�Q�Ƃ̂�)"
#define MES_SYS_AUTO_SAVING "�����Z�[�u��"
#define MES_SYS_GAME_SAVING "�Q�[�����Z�[�u���Ă��܂�..."
#define MES_SYS_QUITING_KEY "���^�[���L�[�� ESC �L�[�������ĉ������B"
#define MES_SYS_ANY_KEY "[�����L�[�������ƃQ�[���ɖ߂�܂�]"
#define MES_SYS_SAVEFILE_ERROR(NUM, MAJ, MIN, EXT) "�G���[(%s)���o�[�W����%d.%d.%d �p�Z�[�u�t�@�C���ǂݍ����ɔ����B",(NUM), (MAJ), (MIN), (EXT)
#define MES_SYS_SAVEFILE_ERROR2 "�Z�[�u�t�@�C������͏o���܂���B"
#define MES_SYS_SAVEFILE_ERROR3 "�Z�[�u�t�@�C�������Ă��܂�"
#define MES_SYS_SAVEFILE_ERROR4(ERROR_NUM) "�Z�[�u�t�@�C�������Ă��܂�: ERR:%d", (ERROR_NUM)
#define MES_SYS_SAVEFILE_CONVERTED(MAJ, MIN, EXT) "�o�[�W���� %d.%d.%d �p�̃Z�[�u�E�t�@�C����ϊ����܂����B", (MAJ), (MIN), (EXT)
#define MES_SYS_SAVE_FAILED "�Z�[�u���s�I"
#define MES_SYS_SCORE_FAILED "�X�R�A�E�t�@�C�����g�p�ł��܂���B"
#define MES_SYS_PANIC_SAVE_SUCCEED "�ً}�Z�[�u�����I"
#define MES_SYS_PANIC_SAVE_FAILED "�ً}�Z�[�u���s�I"
#define MES_SYS_TRAITS(CREATURE) "%s�ɂ͈ȉ��̓���������", (CREATURE)->name
#define MES_SYS_YOUR_NO_TRAIT "���Ȃ��ɂ͓��ʂȓ������Ȃ�"
#define MES_SYS_SERVANTS "%s�ɂ�%d��ށA%d�̂̏]�l������"
#define MES_SYS_NO_SERVANT "%s�ɂ͏]�l�����Ȃ�"
#define MES_SYS_UNKNOWN_SYMBOL "�����ȕ���"
#define MES_SYS_INVALID_NUM "�l�����������ł��B"
#define MES_SYS_ERROR "�G���["
#define MES_SYS_PANIC "�ً}����"
#define MES_SYS_INTERUPTING "�����I����"
#define MES_SYS_CONNECTING "�ڑ���..."
#define MES_SYS_SENDING_SCORE "�X�R�A���M��..."

#define MES_SYS_OUT_OF_SWITCH "switch�\���͈͊O"

#define MES_SYS_TODAY "����"
#define MES_SYS_HERE "�^��"
#define MES_SYS_NORTH "�k"
#define MES_SYS_SOUTH "��"
#define MES_SYS_EAST "��"
#define MES_SYS_WEST "��"
#define MES_SYS_MONEY "������"
#define MES_SYS_PRICE "���i"
#define MES_SYS_WEIGHT "�d��"

#define MES_SYS_MODIFICATION "�\�͏C��"

#define MES_SYS_VISUAL_COMMAND "�R�}���h (n/N/^N/a/A/^A/c/C/^C/v/V/^V): "
#define MES_SYS_ASK_USE "%s���g���܂����H"
#define MES_SYS_ASK_SURE "��낵���ł����H"

#define MES_SYS_CHARGE_NUM(NUM) "��"

#define MES_SYS_SKILL "�Z�\�l"

#define MES_SYS_AUTODUMP_WARNING1 "# *�x��!!* �ȍ~�̍s�͎����������ꂽ���̂ł��B\n"
#define MES_SYS_AUTODUMP_WARNING2 "# *�x��!!* ��Ŏ����I�ɍ폜�����̂ŕҏW���Ȃ��ł��������B\n"
#define MES_SYS_AUTODUMP_WARNING3 "# *�x��!!* �ȏ�̍s�͎����������ꂽ���̂ł��B\n"

#define MES_SYS_NO_TOWN_ITEM "���̃A�C�e�����Ȃ��I"
#define MES_SYS_NO_VALUT_TRAP "�n�����̃g���b�v��z�u�ł��܂���"

#define MES_SYS_ASK_SCREEN_DUMP "�L�O�B�e���܂����H [(y)es/(h)tml/(n)o] "
#define MES_SYS_SCREEN_DUMPED "���(�L�O�B�e)���t�@�C���ɏ����o���܂����B"
#define MES_SYS_SCREEN_LOADED "�t�@�C���ɏ����o���ꂽ���(�L�O�B�e)�����[�h���܂����B"
#define MES_SYS_HP_WARNING "*** �x��:��q�b�g�E�|�C���g�I ***"
#define MES_SYS_FIND "����: "

#else

#define MES_SYS_YOU_DIED(CREATURE) (has_trait((CREATURE), TRAIT_ANDROID) ? "You were broken." : "You died.")
#define MES_SYS_MORE "-- MORE --"
#define MES_SYS_WIZARD_MODE_ON "Wizard mode on."
#define MES_SYS_WIZARD_MODE_OFF "Wizard mode off."
#define MES_SYS_STARTING_GAME "Starting Game, Please wait..."
#define MES_SYS_PLEASE_WAIT "Please wait..."
#define MES_SYS_FAILED_TEMPFILE "Failed to create temporary file %s."
#define MES_SYS_FAILED_FILEOPEN "Can not open file."
#define MES_SYS_FAILED_FILEOPEN2 "Failed to open %s."
#define MES_SYS_FAILED_FILEREAD "Cannot read file."
#define MES_SYS_REPLACE_FILE "Replace existing file %s? "
#define MES_SYS_HIT_ANY_KEY "Hit Any Key�B"
#define MES_SYS_HIT_VERIFY_KEY "Please verify by typing the '@' sign: "
#define MES_SYS_PLACE_SPACE "Select a point and press space."

#define MES_SYS_OPTION_AUX "Birth Option((*)s effect score)"
#define MES_SYS_OPTION_AUX2 "Birth Options(browse only)"
#define MES_SYS_AUTO_SAVING "Autosaving the game..."
#define MES_SYS_GAME_SAVING "Saving game..."
#define MES_SYS_QUITING_KEY "Press Return (or Escape)."
#define MES_SYS_ANY_KEY "[Press any key to continue]"
#define MES_SYS_HIT_SPACE_KEY "Press the space bar to continue"
#define MES_SYS_SAVEFILE_ERROR(NUM, MAJ, MIN, EXT) "Error (%s) reading %d.%d.%d savefile.", (NUM), (MAJ), (MIN), (EXT)
#define MES_SYS_SAVEFILE_ERROR2 "Cannot parse savefile."
#define MES_SYS_SAVEFILE_ERROR3 "Broken savefile"
#define MES_SYS_SAVEFILE_ERROR4(ERROR_NUM) "broken savefile: ERR:%d", (ERROR_NUM)
#define MES_SYS_SAVEFILE_CONVERTED(MAJ, MIN, EXT) "Converted a %d.%d.%d savefile.", (MAJ), (MIN), (EXT)
#define MES_SYS_SAVE_FAILED "death save failed!"
#define MES_SYS_SCORE_FAILED "Score file unavailable."
#define MES_SYS_PANIC_SAVE_SUCCEED "Panic save succeeded!"
#define MES_SYS_PANIC_SAVE_FAILED "panic save failed!"
//TODO
#define MES_SYS_TRAITS(CREATURE) "%s have these trait...", (CREATURE)->name
#define MES_SYS_YOUR_NO_TRAIT "You have no trait"
#define MES_SYS_YOUR_SERVANT "%s have no servant"
#define MES_SYS_YOUR_NO_SERVANT "%s have %d species, %d servants"

#define MES_SYS_UNKNOWN_SYMBOL "unknown symbol"
#define MES_SYS_INVALID_NUM "Invalid response."
#define MES_SYS_ERROR "Error"
#define MES_SYS_PANIC "Panic"
#define MES_SYS_INTERUPTING "Interrupting"

#define MES_SYS_CONNECTING "Connecting..."
#define MES_SYS_SENDING_SCORE "Sending the score..."

#define MES_SYS_OUT_OF_SWITCH "Out of switch."
#define MES_SYS_TODAY "Today"
#define MES_SYS_HERE ""
#define MES_SYS_NORTH " North"
#define MES_SYS_SOUTH " South"
#define MES_SYS_EAST " East"
#define MES_SYS_WEST " West"
#define MES_SYS_MONEY "AU"
#define MES_SYS_PRICE "Price"
#define MES_SYS_WEIGHT "Weight"

#define MES_SYS_MODIFICATION "Modification"

#define MES_SYS_VISUAL_COMMAND "Command (n/N/^N/a/A/^A/c/C/^C/v/V/^V):"
#define MES_SYS_ASK_USE "use %s?"
#define MES_SYS_ASK_SURE "Are you sure?"

#define MES_SYS_CHARGE_NUM(NUM) ((NUM) > 1 ? " charges" : "charge")

#define MES_SYS_SKILL "Proficiency"

#define MES_SYS_AUTODUMP_WARNING1 "# *Warning!*  The lines below are an automatic dump.\n"
#define MES_SYS_AUTODUMP_WARNING2 "# Don't edit them; changes will be deleted and replaced automatically.\n"
#define MES_SYS_AUTODUMP_WARNING3 "# *Warning!*  The lines above are an automatic dump.\n"

#define MES_SYS_NO_TOWN_ITEM "No town objects!"
#define MES_SYS_NO_VALUT_TRAP "Could not place vault trap."

#define MES_SYS_ASK_SCREEN_DUMP "Save screen dump? [(y)es/(h)tml/(n)o] "
#define MES_SYS_SCREEN_DUMPED "Screen dump saved."
#define MES_SYS_SCREEN_LOADED "Screen dump loaded."
#define MES_SYS_HP_WARNING "*** LOW HITPOINT WARNING! ***"
#define MES_SYS_FIND "Find: "

#endif

#if JP
#define MES_SYS_WIN_CANNOT_SAVE "���̓Z�[�u���邱�Ƃ͏o���܂���B"
#define MES_SYS_WIN_CANNOT_EXIT "���͏I���ł��܂���B"
#define MES_SYS_WIN_INITIALIZE_NOW "�܂����������ł�..."
#define MES_SYS_WIN_CANNOT_OPEN_MOVIE "�v���C���̓��[�r�[�����[�h���邱�Ƃ��ł��܂���I"
#define MES_SYS_WIN_MAIN_MUST_ENABLE "���C���E�B���h�E�͔�\���ɂł��܂���I"
#else
#define MES_SYS_WIN_CANNOT_SAVE "You may not do that right now."
#define MES_SYS_WIN_CANNOT_EXIT "You may not do that right now."
#define MES_SYS_WIN_INITIALIZE_NOW "You cannot do that yet..."
#define MES_SYS_WIN_CANNOT_OPEN_MOVIE "You can't open a movie while you're playing!"
#define MES_SYS_WIN_MAIN_MUST_ENABLE "You are not allowed to do that!"
#endif

#ifdef JP
#define MES_LOAD_START(VER1, VER2, VER3) "�o�[�W���� %d.%d.%d �̃Z�[�u�E�t�@�C�������[�h��...", (VER1), (VER2), (VER3)
#define MES_LOAD_RANDOM_SEED "�����������[�h���܂���"
#define MES_LOAD_OPTION "�I�v�V���������[�h���܂���"
#define MES_LOAD_MESSAGE "���b�Z�[�W�����[�h���܂���"
#define MES_LOAD_WORLD "���E�������[�h���܂���"
#define MES_LOAD_ERROR_WILDERNESS(SAVE_X, MAX_X, SAVE_Y, MAX_Y) "�r�삪�傫������(X:%u/%u) (Y:%u/%u)", (SAVE_X), (MAX_X), (SAVE_Y), (MAX_Y)
#define MES_LOAD_FLOOR(NUM) "�_���W����������...�t���A��:%d", (NUM)
#else
#define MES_LOAD_START(VER1, VER2, VER3) "Loading a %d.%d.%d savefile...", (VER1), (VER2), (VER3)
#define MES_LOAD_RANDOM_SEED "Loaded Randomizer Info"
#define MES_LOAD_OPTION "Loaded Option Flags"
#define MES_LOAD_MESSAGE "Loaded Messages"
#define MES_LOAD_WORLD "Loaded world information"
#define MES_LOAD_ERROR_WILDERNESS(SAVE_X, MAX_X, SAVE_Y, MAX_Y) "Wilderness is too big (X:%u/%u) (Y:%u/%u)", (SAVE_X), (MAX_X), (SAVE_Y), (MAX_Y)
#define MES_LOAD_FLOOR(NUM) "Restoring Dungeon... NUM:%d", (NUM)
#endif

#if JP
#define KW_MORE "-����-"
#define KW_CREATURE "�N���[�`���["
#define KW_NAME "���O"
#define KW_RACE "�푰"
#define KW_PURE_BLOOD "������"
#define KW_CLASS "�E��"
#define KW_REALM "���@"
#define KW_CHARA "���i"
#define KW_PATRON "��_"
#define KW_LEVEL "���x��"
#define KW_LEVEL_DEC "���x��x"
#define KW_HP "HP"
#define KW_MP "MP"
#define KW_AC "AC"
#define KW_EV "EV"
#define KW_VO "VO"
#define KW_EXP "�\�E��"
#define KW_EXP_DEC "�\�E��x"
#define KW_EXP_NEXT "�����x��"
#define KW_EXP_DEC "�\�E��x"
#define KW_MONEY "������"
#define KW_KARMA "��"
#define KW_ALIGNMENT "����"
#define KW_PROFICIENCY "�Z�\�l"
#define KW_DURING "����"
#define KW_RANGE "�͈�"
#define KW_HEAL "��"
#define KW_FAIL "����"
#define KW_STRANGE "���m"
#define KW_FORGOTTEN "�Y�p"
#define KW_ILLEGIBLE "���Ǖs�\"
#define KW_UNTRIED "���o��"
#define KW_INVENTORY "�����i"
#define KW_EQUIPMENT "�����i"
#define KW_CARRYING_WEIGHT "�����d��"
#define KW_EQUIPMENT_WEIGHT "�����d��"
#define KW_STAFF "��"
#define KW_WAND "���@�_"
#define KW_ROD "���b�h"
#define KW_YOU "���Ȃ�"
#define KW_CREATURE_PROFILE "�N���[�`���[�̃v���t�B�[��"
#define KW_CREATURE_PROFILE_EDITING "�N���[�`���[�̃v���t�B�[�� - �ҏW��"
#define KW_CREATURE_PROFILE_EDITED "�N���[�`���[�̃v���t�B�[�� - �ҏW�ς�"
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
#define KW_SURE "����"
#define KW_CANCEL "�L�����Z��"
#define KW_FAILED "���s"
#define KW_NONE "�Ȃ�"
#define KW_NOTHING "�Ȃ�"
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
#define KW_OFFER "�񎦋��z"
#define KW_ASKING_PRICE "�񎦉��i"
#define KW_FINAL_OFFER_PRICE "�ŏI�񎦉��i"
#define KW_FLOOR_NUM(NUM) "%d�K", (NUM)
#define KW_FLOOR_NUM2(DUNGEON, NUM) "%d�K(%s)", (NUM), (DUNGEON)
#define KW_DEAD "���S"
#define KW_ALIVE "����"
#define KW_UNIDENTIFIED "���Ӓ�"
#define KW_EMPTY "��"
#define KW_TRIED "������"
#define KW_PRICE_OFF "%����"
#define KW_HISSATSU "�K�E�Z"
#define KW_SONG "��"
#define KW_MIRACLE "���"
#define KW_SPELL "���@"
#define KW_STUDY "�w��"
#define KW_SOMETHING_STRANGE "������ȕ�"
#define KW_NOTHING_TARGET "�w��Ȃ�"
#define KW_UNEXPLORED_ZONE "�����j�n��"
#define KW_ROAD "��"
#define KW_MESSAGE "���b�Z�[�W"
#define KW_WINNING_MESSAGE "*����*���b�Z�[�W"
#define KW_LEAVE "���u"
#define KW_DESTROY "�j��"
#define KW_PICKUP "�E��"
#define KW_QUERY "�m�F"
#define KW_TRY "�{����"
#define KW_FEATURE "�n�`"
#define KW_CAMOUFLAGE "�U���n�`"
#define KW_QUANTITY "��"
#define KW_KILLS "�E�Q��"
#define KW_SYM "����"
#define KW_PET "�y�b�g"
#define KW_FRIEND "�F�R"
#define KW_CLONE "�N���[��"
#define KW_WANTED "�܋���"
#define KW_CHARGING "(�[�U��)"
#define KW_FLOORITEM "����"
#define KW_WITH_THOHANDED "(����𗼎莝��)"
#define KW_CHARA_INVENTORY "�L�����N�^�̎�����"
#define KW_CHARA_EQUIPMENT "�L�����N�^�̑���"
#define KW_MUSIUM_LIST "�����ق̃A�C�e��"
#define KW_JUST_LIFTING "�^����"
#define KW_SHOOTING "�ˌ��p"
#define KW_UNKNOWN "�s��"
#define KW_BODY "��"
#define KW_WORLD_MAP "���ׂ̒n��"
#define KW_ARENA "���Z��"
#define KW_AUTOSAVE "�����Z�[�u"
#else
#define KW_MORE "-more-"
#define KW_CREATURE "Creatures"
#define KW_NAME "Name"
#define KW_RACE "Race"
#define KW_PURE_BLOOD "Pure Blood"
#define KW_CLASS "Class"
#define KW_REALM "Realm"
#define KW_CHARA "Chara"
#define KW_PATRON "Patron"
#define KW_LEVEL "LEVEL"
#define KW_LEVEL_DEC "Level"
#define KW_HP "HP"
#define KW_MP "MP"
#define KW_AC "AC"
#define KW_EV "EV"
#define KW_VO "VO"
#define KW_EXP "SOUL"
#define KW_EXP_DEC "Soul"
#define KW_EXP_NEXT "NEXT"
#define KW_MONEY "Money"
#define KW_KARMA "Karma"
#define KW_ALIGNMENT "Alignment"
#define KW_PROFICIENCY "Proficiency"
#define KW_DURING "Dur"
#define KW_RANGE "Range"
#define KW_HEAL "Heal"
#define KW_FAIL "Fail"
#define KW_STRANGE "Strange"
#define KW_FORGOTTEN "Forgotten"
#define KW_ILLEGIBLE "Illegible"
#define KW_UNTRIED "Untried"
#define KW_INVENTORY "Inventory"
#define KW_EQUIPMENT "Equipment"
#define KW_CARRYING_WEIGHT "Carrying Weight"
#define KW_EQUIPMENT_WEIGHT "Equipment Weight"
#define KW_STAFF "staff"
#define KW_WAND "wand"
#define KW_ROD "rod"
#define KW_YOU "You"
#define KW_CREATURE_PROFILE "Creature Profile"
#define KW_CREATURE_PROFILE_EDITING "Creature Profile - Edit Mode"
#define KW_CREATURE_PROFILE_EDITED "Creature Profile - Edited"
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
#define KW_SURE "Sure"
#define KW_NOTHING "Nothing"
#define KW_CANCEL "Cancel"
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
#define KW_OFFER "Offer"
#define KW_ASKING_PRICE "Asking Price"
#define KW_FINAL_OFFER_PRICE "Final Offer"
#define KW_FLOOR_NUM(NUM) "Floor %d", (NUM)
#define KW_FLOOR_NUM2(DUNGEON, NUM) "%s L%d:", (DUNGEON), (NUM) 
#define KW_DEAD "dead"
#define KW_ALIVE "alive"
#define KW_UNIDENTIFIED "unidentified"
#define KW_EMPTY "empty"
#define KW_TRIED "tried"
#define KW_PRICE_OFF "%off"
#define KW_HISSATSU "Art"
#define KW_SONG "Song"
#define KW_MIRACLE "Miracle"
#define KW_SPELL "Spell"
#define KW_STUDY "Study"
#define KW_SOMETHING_STRANGE "something strange"
#define KW_NOTHING_TARGET "nothing"
#define KW_UNEXPLORED_ZONE "unexplored zone"
#define KW_ROAD "road"
#define KW_MESSAGE "message"
#define KW_WINNING_MESSAGE "*Winning* message"
#define KW_LEAVE "Leave"
#define KW_DESTROY "Destroy"
#define KW_PICKUP "Pickup"
#define KW_QUERY "Query"
#define KW_TRY "Try"
#define KW_FEATURE "Feature"
#define KW_CAMOUFLAGE "Camouflage"
#define KW_QUANTITY "Quantity"
#define KW_KILLS "Kills"
#define KW_SYM "Sym"
#define KW_PET "Pet"
#define KW_FRIEND "Friend"
#define KW_CLONE "Clone"
#define KW_WANTED "Wanted"
#define KW_CHARGING "(charging)""
#define KW_FLOORITEM "On a floor"
#define KW_WITH_THOHANDED "(wielding with two-hands)"
#define KW_CHARA_INVENTORY "Character inventory"
#define KW_CHARA_EQUIPMENT "Character Equipment"
#define KW_MUSIUM_LIST "Museum"
#define KW_JUST_LIFTING "Just lifting"
#define KW_SHOOTING "Shooting"
#define KW_UNKNOWN "UNKNOWN"
#define KW_BODY "body"
#define KW_WORLD_MAP "The Surface of Chaos"
#define KW_ARENA "Arena"
#define KW_AUTOSAVE "Autosave"
#endif

#ifdef JP
#define MES_FEATURE_KILLED "�܂΂䂢�M�����������I"
#else
#define MES_FEATURE_KILLED "There is a bright flash of light!"
#endif

#ifdef JP
#define MES_TRAP_FOUND "�g���b�v���B"
#define MES_TRAP_FOUND_CHEST "���Ɏd�|����ꂽ�g���b�v�𔭌������I"

#define MES_TRAP_DOOR "���Ƃ��˂ɗ������I"
#define MES_TRAP_DOOR_AVOID "���Ƃ��˂��щz�����B"

#define MES_TRAP_PIT "���Ƃ����ɗ����Ă��܂����I"
#define MES_TRAP_PIT_AVOID "���Ƃ������щz�����B"
#define MES_TRAP_SPIKED_PIT "�X�p�C�N���~���ꂽ���Ƃ����ɗ����Ă��܂����I"
#define MES_TRAP_SPIKED_PIT_AVOID "�g�Q�̂��闎�Ƃ������щz�����B"
#define MES_TRAP_INPILED "�X�p�C�N���h�������I"
#define MES_TRAP_POISON_INPILED "�ł�h��ꂽ�X�p�C�N���h�������I"

#define MES_TRAP_BLIND "�����K�X�ɕ�ݍ��܂ꂽ�I"
#define MES_TRAP_CONFUSE "����߂��K�X�ɕ�ݍ��܂ꂽ�I"
#define MES_TRAP_POISON "�h���I�ȗΐF�̃K�X�ɕ�ݍ��܂ꂽ�I"
#define MES_TRAP_SLEPT "��Ȕ������ɕ�܂ꂽ�I"

#define MES_TRAP_NEEDLE "�d�|�����Ă��������Ȑj�Ɏh����Ă��܂����I"
#define MES_TRAP_DARTS "�����ȃ_�[�c�����ł��Ďh�������I"
#define MES_TRAP_DARTS_MISSED "�����ȃ_�[�c�����ł����I���A�^�ǂ�������Ȃ������B"
#define MES_TRAP_FIRE "���ɕ�܂ꂽ�I"
#define MES_TRAP_ACID "�_������������ꂽ�I"
#define MES_TRAP_ACID_FLOW "�ˑR������_�����o�����I"
#define MES_TRAP_POISON_FLOW "�ˑR������ŉt���o�����I"
#define MES_TRAP_PIRANHAS "�ˑR�ǂ��琅�����o�����I�s���j�A������I"

#define MES_TRAP_POISONOUS_GAS "�˔@�����o�����ΐF�̃K�X�ɕ�ݍ��܂ꂽ�I"
#define MES_TRAP_PARALYZE_GAS "�˔@�����o�������F���K�X�ɕ�ݍ��܂ꂽ�I"
#define MES_TRAP_SUMMONING "�˔@�����o�������ɕ�ݍ��܂ꂽ�I"
#define MES_TRAP_S_ELEMENTAL "�����邽�߂ɃG�������^�������ꂽ�I"
#define MES_TRAP_S_BIRD "���̌Q�ꂪ���Ȃ�����芪�����I"
#define MES_TRAP_S_H_DEMON "���Ɨ����̉_�̒��Ɉ������p���������I"
#define MES_TRAP_S_H_DRAGON "�ÈłɃh���S���̉e���ڂ���ƌ��ꂽ�I"
#define MES_TRAP_S_HYBRID "��Ȏp�̉������P���ė����I"
#define MES_TRAP_S_VORTEX "�Q�������̂��A�j�􂵂��I"
#define MES_TRAP_E_RUNE "���낵������������:  �u�Èł������܂�I�v"
#define MES_TRAP_EXPLOSIVE "�ˑR�A�������������I���̒��̕��͂��ׂĕ��X�ɍӂ��U�����I"
#define MES_TRAP_SCATTER "�󔠂̒��g�̓_���W�������イ�ɎU�������I"
#define MES_TRAP_TELEPORT "�e���|�[�g�E�g���b�v�ɂЂ����������I"
#define MES_TRAP_TY_CURSE "�������s�J�b�ƌ������I"
#define MES_TRAP_ARMAGEDDON "�ˑR�V�E�̐푈�Ɋ������܂ꂽ�I"
#define MES_TRAP_ALARM "�������܂��������苿�����I"
#define MES_TRAP_OPEN "�剹���Ƌ��ɂ܂��̕ǂ����ꂽ�I"
#else

#define MES_TRAP_FOUND "You found a trap."
#define MES_TRAP_FOUND_CHEST "You have discovered a trap on the chest!"

#define MES_TRAP_DOOR "You have fallen through a trap door!"
#define MES_TRAP_DOOR_AVOID "You fly over a trap door."

#define MES_TRAP_PIT "You fly over a pit trap."
#define MES_TRAP_PIT_AVOID "You have fallen into a pit!"
#define MES_TRAP_SPIKED_PIT "You have fallen into a spiked pit!"
#define MES_TRAP_SPIKED_PIT_AVOID "You fly over a spiked pit."
#define MES_TRAP_INPILED "You are impaled!"
#define MES_TRAP_POISON_INPILED "You are impaled on poisonous spikes!"

#define MES_TRAP_BLIND "A black gas surrounds you!"
#define MES_TRAP_CONFUSE "A gas of scintillating colors surrounds you!"
#define MES_TRAP_POISON "A pungent green gas surrounds you!"
#define MES_TRAP_SLEPT "A strange white mist surrounds you!"

#define MES_TRAP_NEEDLE "A small needle has pricked you!"
#define MES_TRAP_DARTS "A small dart hits you!"
#define MES_TRAP_DARTS_MISSED "A small dart barely misses you."
#define MES_TRAP_FIRE "You are enveloped in flames."
#define MES_TRAP_ACID "You are splashed with acid."
#define MES_TRAP_ACID_FLOW "Suddenly, the room is filled with acid."
#define MES_TRAP_POISON_FLOW "Suddenly, the room is filled with poisonous liquid."
#define MES_TRAP_PIRANHAS "Suddenly, the room is filled with water with piranhas!"

#define MES_TRAP_POISONOUS_GAS "A puff of green gas surrounds you!"
#define MES_TRAP_PARALYZE_GAS "A puff of yellow gas surrounds you!"
#define MES_TRAP_SUMMONING "You are enveloped in a cloud of smoke!"
#define MES_TRAP_S_ELEMENTAL "Elemental beings appear to protect their treasures!"
#define MES_TRAP_S_BIRD "A storm of birds swirls around you!"
#define MES_TRAP_S_H_DEMON "Demons materialize in clouds of fire and brimstone!"
#define MES_TRAP_S_H_DRAGON "Draconic forms loom out of the darkness!"
#define MES_TRAP_S_HYBRID "Creatures strange and twisted assault you!"
#define MES_TRAP_S_VORTEX "Vortices coalesce and wreak destruction!"
#define MES_TRAP_E_RUNE "Hideous voices bid:  'Let the darkness have thee!'"
#define MES_TRAP_EXPLOSIVE "There is a sudden explosion! Everything inside the chest is destroyed!"
#define MES_TRAP_SCATTER  "The contents of the chest scatter all over the dungeon!"
#define MES_TRAP_TELEPORT "You hit a teleport trap!"
#define MES_TRAP_TY_CURSE "There is a flash of shimmering light!"
#define MES_TRAP_ARMAGEDDON "Suddenly, you are surrounded by immotal beings!"
#define MES_TRAP_ALARM "An alarm sounds!"
#define MES_TRAP_OPEN "Suddenly, surrounding walls are opened!"
#endif

#if JP
#define MES_COMPLETE_QUEST "�N�G�X�g��B�������B"
#define MES_CURRENT_QUEST "�s���s���̃N�G�X�g�t"
#define MES_COMPLETED_QUEST "�s�B�������N�G�X�g�t"
#define MES_FAILED_QUEST "�s���s�����N�G�X�g�t"
#define MES_REMAINING_QUEST "�s�c���Ă���N�G�X�g�t"

#define MES_FAILED "���s�����B"
#define MES_ENCHANTMENT_FAILED "�����Ɏ��s�����B"
#define MES_IMPROVEMENT_FAILED "���ǂɎ��s�����B"
#define MES_RESIST_THE_EFFECT "���������͂𒵂˕Ԃ����B"
#define MES_IS_IMMUNE "�ɂ͊��S�ȑϐ�������B"
#define MES_RESISTED "�ɂ͑ϐ�������B"
#define MES_IS_UNAFFECTED "�ɂ͌��ʂ��Ȃ������B"
#define MES_NO_EFFECT "���ʂ��Ȃ������B"
#define MES_IS_CONFUSED "�͖ڂ������Ȃ�"
#define MES_IS_BLIND "�͍������Ă���B"
#define MES_CREATURE_IN_THE_WAY "�N���[�`���[�������ӂ������Ă���B"
#define MES_MANA_RECOVERLY "�����n�b�L���Ƃ����B"
#define MES_SUCCESS_PICKING "�����͂������B"
#define MES_FAILED_PICKING "�����͂����Ȃ������B"
#define MES_FOUND_SOMETHING "�����𔭌������I"

#define MES_BREAK_P_RUNE "���̃��[������ꂽ�I"
#define MES_BREAK_E_RUNE "���[�������������I"
#define MES_DISARM_E_RUNE "�����̃��[���͉������ꂽ�B"

#define MES_HEAR_NOISE "���������������B"

#define MES_NO_STUDY_SPELL "�ǂ߂�������Ȃ��B"
#define MES_NO_STUDY_TECH "�w�ׂ�Z���Ȃ��B"

#define MES_BLACK_AURA_TO_OBJECT "���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I"
#define MES_REMOVED_OBJECT_CURSE "�N���Ɍ�����Ă���悤�ȋC������B"
#define MES_COMBAT_TALK_STAIR "�u�Ȃ񂾂��̊K�i�́I�H�v"
#define MES_PERILOUS_SECRET "�΂𐧌�ł��Ȃ��B"
#define MES_RESISTED_FEAR "���������|�ɐN����Ȃ������B"
#define MES_NO_MONEY "����������܂���B"
#define MES_NO_MONEY_INSULT "�����I�����Ȃ�����Ȃ����I�o�Ă����I"
#define MES_BOUNTY_1 "%s ���������܂����H"

#define MES_TRAP_WARNING "* ����:���̐�̓g���b�v�̊��m�͈͊O�ł��I *"
#define MES_TRAP_DESTROY "�g���b�v�𕲍ӂ����B"

#define MES_LITE_AREA "�����ӂ�𕢂����B"
#define MES_UNLITE_AREA "�Èł��ӂ�𕢂����B"

#define MES_DESTROY "�R����悤�ȑM�������������I"

#define MES_FAILED_RUNAWAY "���܂��������Ȃ������B"

#define MES_DAMAGE_DENSITY "�̂̕��q�����������C������I"
#define MES_ROCK_CRUSHING(CREATURE) "%s�͕��ꂽ��ɉ����ׂ��ꂽ�I", (CREATURE)->name

#define MES_HAYAGAKE_PREVENT "�����ł͑f���������Ȃ��B"
#define MES_ATTEMPT_ANYWAY "����ł����킵�܂���? "

#define MES_IMPROVE_LIMIT "���̃A�C�e���͂���ȏ���ǂł��Ȃ��B"

#define MES_NO_HAPPEN "�����N����Ȃ������B"
#define MES_RIDING_FAILED "���܂����Ȃ������B"
#define MES_CONVERT_FAILED "�ϊ��Ɏ��s�����B"
#define MES_NO_DICRECTION_CREATURE "���̕����ɂ̓N���[�`���[�͂��܂���B"
#define MES_FALL_RIDING "��R����n�ʂɗ��Ƃ��ꂽ�B"

#define MES_FIND_TRAP "�g���b�v�𔭌������B"
#define MES_FIND_DOOR "�B���h�A�𔭌������B"


#define MES_SEE_OBJECT "%s������B"
#define MES_GET_MONEY " $%ld �̉��l������%s���������B"
#define MES_PACK_WHICH_PICK "�ǂ���E���܂����H"
#define MES_PACK_NO_ROOM "�U�b�N�ɂ�%s�����錄�Ԃ��Ȃ��B"
#define MES_PACK_NO_ROOM_FLOOR "�U�b�N�ɂ͏��ɂ���ǂ̃A�C�e��������Ȃ��B"
#define MES_PACK_OVERFLOW "�U�b�N����A�C�e�������ӂꂽ�I"
#define MES_PACK_REORDER "�U�b�N�̒��̃A�C�e������ג������B"
#define MES_PACK_COMBINE "�U�b�N�̒��̃A�C�e�����܂Ƃߒ������B"
#define MES_STAFF_UNSTACK "����܂Ƃ߂Ȃ������B"

#define MES_PROJECTILE_HITS "%s��%s�ɖ��������B"

#define MES_FIELD_MAGIC_FAILED1 "����̃A�C�e���������𒵂˕Ԃ����B"

#define MES_CALL_ARTIFACT "���̃A�[�e�B�t�@�N�g�����Ɩ��t���܂����H"

#define MES_TARGET_SET "�^�[�Q�b�g����B"
#define MES_TARGET_ABORT "�^�[�Q�b�g�����B"
#define MES_TARGET_CANCELED "���f�����B"

#define MES_GOODBYE(NAME) ("���悤�Ȃ�A%s!", (NAME))
#else

#define MES_COMPLETE_QUEST "You completed the quest."
#define MES_CURRENT_QUEST "< Current Quest >"
#define MES_COMPLETED_QUEST "< Completed Quest >"
#define MES_FAILED_QUEST "< Failed Quest >"
#define MES_REMAINING_QUEST "< Remaining Quest >"

#define MES_FAILED "Failed."
#define MES_ENCHANTMENT_FAILED "The enchantment failed."
#define MES_IMPROVEMENT_FAILED "The improvement failed."
#define MES_RESIST_THE_EFFECT "You resist the effects."
#define MES_IS_IMMUNE "is immune."
#define MES_RESISTED "resists."
#define MES_IS_UNAFFECTED "You are unaffected."
#define MES_NO_EFFECT "There is no effect."
#define MES_IS_CONFUSED "is confused"
#define MES_IS_BLIND "is blind!"
#define MES_CREATURE_IN_THE_WAY "There is a creature in the way."
#define MES_MANA_RECOVERLY "You feel your head clear."
#define MES_SUCCESS_PICKING "You have picked the lock."
#define MES_FAILED_PICKING "You failed to pick the lock."
#define MES_FOUND_SOMETHING "You have found something!"

#define MES_BREAK_P_RUNE "The rune of protection is broken!"
#define MES_BREAK_E_RUNE "The rune explodes!"
#define MES_DISARM_E_RUNE "An explosive rune was disarmed."
#define MES_HEAR_NOISE "You hear noise."

#define MES_NO_STUDY_SPELL "No spells to browse."
#define MES_NO_STUDY_TECH "No techniques to browse."

#define MES_BLACK_AURA_TO_OBJECT "A terrible black aura blasts your %s!"
#define MES_REMOVED_OBJECT_CURSE "You feel as if someone is watching over you."
#define MES_COMBAT_TALK_STAIR "'What's a fucking stair!?'"
#define MES_PERILOUS_SECRET "You are too weak to control the stone!"
#define MES_RESISTED_FEAR "You stand your ground!"
#define MES_NO_MONEY "You do not have the gold."
#define MES_NO_MONEY_INSULT "Hey! You don't have the gold - get out of here!"
#define MES_BOUNTY_1 "Convert %s into money?"

#define MES_TRAP_WARNING "*Leaving trap detect region!*"
#define MES_TRAP_DESTROY "You destroyed the trap."

#define MES_LITE_AREA "You are surrounded by a light."
#define MES_UNLITE_AREA "Darkness surrounds you."

#define MES_DESTROY "There is a searing blast of light!"

#define MES_FAILED_RUNAWAY "You are failed to run away."

#define MES_DAMAGE_DENSITY "Your molecules feel disrupted!"
#define MES_ROCK_CRUSHING "%s %s being crushed!", (CREATURE)->name, is_player(CREATURE) ? "are" : "is"

#define MES_HAYAGAKE_PREVENT "You cannot run in here."
#define MES_ATTEMPT_ANYWAY "Attempt it anyway? "

#define MES_IMPROVE_LIMIT "This item is no more able to be improved."

#define MES_NO_HAPPEN "Nothing happens."
#define MES_CONVERT_FAILED "You failed to convert."
#define MES_RIDING_FAILED "You failed to ride."
#define MES_NO_DICRECTION_CREATURE "There is no creature."

#define MES_FALL_RIDING "You have fallen from your riding pet."

#define MES_FIND_TRAP "You have found a trap."
#define MES_FIND_DOOR "You have found a secret door."

#define MES_SEE_OBJECT "You see %s."
#define MES_GET_MONEY "You collect %ld gold pieces worth of %s."
#define MES_PACK_WHICH_PICK "Get which item? "
#define MES_PACK_NO_ROOM "You have no room for %s."
#define MES_PACK_NO_ROOM_FLOOR "You have no room for any of the objects on the floor."
#define MES_PACK_OVERFLOW "Your pack overflows!"
#define MES_PACK_REORDER "You reorder some items in your pack."
#define MES_PACK_COMBINE "You combine some items in your pack."
#define MES_STAFF_UNSTACK "You unstack your staff."

#define MES_PROJECTILE_HITS "The %s hits %s."

#define MES_FIELD_MAGIC_FAILED1 "The object resists the spell."

#define MES_CALL_ARTIFACT "What do you want to call the artifact?"

#define MES_TARGET_SET "Target selected."
#define MES_TARGET_ABORT "Target aborted."
#define MES_TARGET_CANCELED "Canceled."

#define MES_GOODBYE(NAME) ("Goodbye, %s!", (NAME))
#endif

#ifdef JP
#define MES_ARENA_READY "�����J�n�I"
#define MES_ARENA_LIMIT "�A���[�i�����@���z�������I"
#define MES_ARENA_ITEM_LIMIT "�A���[�i�ł̓A�C�e�����g���Ȃ��I"
#define MES_ARENA_BERSERK "����m�����Ă��ē������Ȃ��I"
#define MES_ARENA_WON "�����I�`�����s�I���ւ̓���i��ł���B"
#define MES_ARENA_BECAME_CHAMP "�f���炵���I�N�����^�̏����҂��B"
#define MES_ARENA_EXTRA "�N�̂��߂ɍŋ��̒���҂�p�ӂ��Ă������B"
#define MES_ARENA_EXTRA_ASK "���킷�邩�ˁH"
#define MES_ARENA_EXTRA_CANCELED "�c�O���B"
#define MES_ARENA_CHALLENGE(NAME) "%s �ɒ��킷����̂͂��Ȃ����H", (NAME)
#define MES_ARENA_LOST(TARGET) "���Ȃ���%s�̑O�ɔs�ꋎ�����B", (TARGET)->name
#define MES_ARENA_FEEL_GLORY "���Ȃ��̓A���[�i�ɓ���A���΂炭�̊ԉh���ɂЂ������B"
#define MES_ARENA_NO_PET "�y�b�g�ɏ�����܂܂ł̓A���[�i�֓��ꂳ���Ă��炦�Ȃ������B"
#define MES_ARENA_YOU_ARE_WINNER "���Ȃ��͏����҂��B �A���[�i�ł̃Z�����j�[�ɎQ�����Ȃ����B"
#define MES_ARENA_CLEARED "���Ȃ��͂��ׂĂ̓G�ɏ��������B"
#define MES_ARENA_WINNER "�A���[�i�̗D���ҁI\n���߂łƂ��I���Ȃ��͑S�Ă̓G��|���܂����B\n�܋��Ƃ��� $1,000,000 ���^�����܂��B"
#else
#define MES_ARENA_READY "Ready..Fight!"
#define MES_ARENA_LIMIT "The arena absorbs all attempted magic!"
#define MES_ARENA_ITEM_LIMIT "You're in the arena now. This is hand-to-hand!"
#define MES_ARENA_WON "Victorious! You're on your way to becoming Champion."
#define MES_ARENA_BECAME_CHAMP "You are a Genuine Champion!"
#define MES_ARENA_BERSERK "You cannot think directly!"
#define MES_ARENA_EXTRA "The strongest challenger is waiting for you."
#define MES_ARENA_EXTRA_ASK "Do you fight? "
#define MES_ARENA_EXTRA_CANCELED "We are disappointed."
#define MES_ARENA_CHALLENGE(NAME) "Do I hear any challenges against: %s", (NAME)
#define MES_ARENA_LOST(TARGET) "You are beaten by %s.", (TARGET)->name;
#define MES_ARENA_FEEL_GLORY "You enter the arena briefly and bask in your glory."
#define MES_ARENA_NO_PET "You don't have permission to enter with pet."
#define MES_ARENA_YOU_ARE_WINNER "You are victorious. Enter the arena for the ceremony."
#define MES_ARENA_CLEARED "You have won against all foes."
#define MES_ARENA_WINNER "Arena Victor!\nCongratulations!  You have defeated all before you.\nFor that, receive the prize: 1,000,000 gold pieces"
#endif

#ifdef JP
#define MES_FIELD_UMBUSHED "�P�����I"
#define MES_FIELD_ASK_CHAOS "�{���Ɂq���ׁr�̗̈�ɓ���܂����H"
#define MES_FIELD_START "���Ȃ��͍��ׂ̒n������ݎn�߂��c"
#define MES_FIELD_TOO_NEAR_ENEMY "�G�������߂��ɂ���Ƃ��͍��ׂ̒n���ɓ���Ȃ��I"
#define MES_FIELD_LEAVE_SERVANT "�y�b�g��u���č��ׂ̒n���ɓ���܂����H"
#else
#define MES_FIELD_UMBUSHED "You are ambushed !"
#define MES_FIELD_ASK_CHAOS "Really want to enter territory of chaos? "
#define MES_FIELD_START "You begin to walk the horizon of chaos..."
#define MES_FIELD_TOO_NEAR_ENEMY "You cannot enter global map, since there is some creatures nearby!"
#define MES_FIELD_LEAVE_SERVANT "Do you leave your pets behind? "
#endif

#ifdef JP
#define MES_SMITH_WHICH_STUFF "�ǂ̃A�C�e��������܂����H "
#define MES_SMITH_NO_STUFF "�ޗ��������Ă��Ȃ��B"
#define MES_SMITH_WHICH_AMMO "[S]�e, [A]��, [B]�N���X�{�E�̖� :"
#define MES_SMITH_WHICH_DRAIN "�ǂ̃A�C�e�����璊�o���܂����H"
#define MES_SMITH_NO_DRAIN "���o�ł���A�C�e��������܂���B"
#define MES_SMITH_DRAIN_ASK(OBJECT) "�{����%s���璊�o���Ă�낵���ł����H", (OBJECT)
#define MES_SMITH_DRAIN_NOTHING "�G�b�Z���X�͒��o�ł��܂���ł����B"
#define MES_SMITH_DRAINED_ESSENCE "���o�����G�b�Z���X:"
#define MES_SMITH_WHICH_ENCHANT "����t�����܂���:"
#define MES_SMITH_WHICH_ABILITY "('*'�ňꗗ, ESC�Œ��f) �ǂ̔\�͂�t�����܂����H"
#define MES_SMITH_WHICH_ERASE "�ǂ̃A�C�e���̃G�b�Z���X���������܂����H"
#define MES_SMITH_ERASED "�G�b�Z���X����苎�����B"
#define MES_SMITH_NO_ERASE "�G�b�Z���X��t�������A�C�e��������܂���B"
#define MES_SMITH_MADE(OBJECT) "%s��������B", (OBJECT)->name
#define MES_SMITH_USE_ESSENCE "�G�b�Z���X��%d�g�p���܂��B"
#define MES_SMITH_ASK_ADD(ESSENCE) "%s��t�����܂����H ", (ESSENCE)
#define MES_SMITH_NO_ESSENCE "�G�b�Z���X������Ȃ��B"
#define MES_SMITH_NEED_ESSENCE(NUM) "�G�b�Z���X��%d�K�v�ł��B", (NUM)
#define MES_SMITH_CANNOT_INCREASE "���̃A�C�e���̔\�͏C�����������邱�Ƃ͂ł��Ȃ��B"
#define MES_SMITH_PVAL_BECOME_ONE "�C���l��1�ɂȂ�܂��B��낵���ł����H"
#define MES_SMITH_HOW_MANY_ENCHANT(NUM) "�����t�����܂����H (1-%d)", (NUM)
#define MES_SMITH_ADDED_ESSENCE(OBJECT, TYPE) "%s��%s�̔\�͂�t�����܂����B", (OBJECT)->name, (TYPE)
#else
#define MES_SMITH_WHICH_STUFF "Convert which item? "
#define MES_SMITH_NO_STUFF "You have no item to convert."
#define MES_SMITH_WHICH_AMMO "Create [S]hots, Create [A]rrow or Create [B]olt ?"
#define MES_SMITH_WHICH_DRAIN "Extract from which item? "
#define MES_SMITH_NO_DRAIN "You have nothing you can extract from."
#define MES_SMITH_DRAIN_ASK(OBJECT) "Really extract from %s? ", (OBJECT)
#define MES_SMITH_DRAIN_NOTHING "You were not able to extract any essence."
#define MES_SMITH_DRAINED_ESSENCE "Extracted essences:"
#define MES_SMITH_WHICH_ENCHANT "Command :"
#define MES_SMITH_WHICH_ABILITY "(*=List, ESC=exit) Add which ability? "
#define MES_SMITH_WHICH_ERASE "Remove from which item? "
#define MES_SMITH_ERASED "You removed all essence you have added."
#define MES_SMITH_NO_ERASE "You have nothing to remove essence."
#define MES_SMITH_MADE(OBJECT) "You made %s", (OBJECT)->name
#define MES_SMITH_USE_ESSENCE "It will take %d essences."
#define MES_SMITH_ASK_ADD(ESSENCE) "Add the abilitiy of %s? ", (ESSENCE)
#define MES_SMITH_NO_ESSENCE "You don't have enough essences."
#define MES_SMITH_NEED_ESSENCE(NUM) "It will take %d essences.", (NUM)
#define MES_SMITH_CANNOT_INCREASE "You cannot increase magic number of this item."
#define MES_SMITH_PVAL_BECOME_ONE "The magic number of this weapon will become 1. Are you sure? "
#define MES_SMITH_HOW_MANY_ENCHANT(NUM) "Enchant how many? (1-%d)", (NUM)
#define MES_SMITH_ADDED_ESSENCE(OBJECT, TYPE) "You have added ability of %s to %s.", (OBJECT)->name, (TYPE)
#endif


#ifdef JP
#define MES_PROBING_START "������..."
#define MES_PROBING_FORMAT "%s ����:%s ����:%s HP:%d/%d AC:%d ���x:%s%d �r:%d �m:%d ��:%d ��:%d ��:%d ��:%d �o��:"
#else
#define MES_PROBING_START "Probing..."
#define MES_PROBING_FORMAT "%s align:%s sex:%s HP:%d/%d AC:%d speed:%s%d STR:%d INT:%d WIS:%d DEX:%d CON:%d CHA:%d exp:"
#endif

#ifdef JP
#define MES_FATAL_SPOT "%s�̋}���ɓ˂��h�������I"
#define MES_SUICIDE_BOMBER "�������ĕ��X�ɂȂ����B"
#define MES_CARSE_OF_BLOOD_DEAD "%^s�͎��̊ԍۂɋ��낵�����̎􂢂�%^s�ɂ������I"
#define MES_LOST_LIFE1 "�����͂������z�����ꂽ�C������I"
#define MES_LOST_LIFE2 "�����͂��̂���z�����ꂽ�C������I"
#define MES_GUARD_LIFE "���������Ȃ̐����͂���肫�����I"
#define MES_BLOE_AWAY "%s�𐁂���΂����I"
#define MES_CAMP_GET_ANGRY "%s�͓{�����I"
#define MES_HOSTILE "%^s�͓G�ɂ܂�����I"
#define MES_BOUNTY_DEAD "%s�̎�ɂ͕񏧂��������Ă���B"
#else
#define MES_FATAL_SPOT "Your shot sticked on a fatal spot of %s!"
#define MES_SUICIDE_BOMBER "%^s explodes into tiny shreds."
#define MES_CARSE_OF_BLOOD_DEAD "On death and dying, %^s puts a terrible blood curse on %^s!"
#define MES_LOST_LIFE1 "You feel your life slipping away!"
#define MES_LOST_LIFE2 "You feel your life draining away..."
#define MES_GUARD_LIFE "You keep hold of your life force!"
#define MES_BLOE_AWAY "You blow %s away!"
#define MES_CAMP_GET_ANGRY "%^s gets angry!"
#define MES_HOSTILE "%^s suddenly becomes hostile!"
#define MES_BOUNTY_DEAD "There is a price on %s's head."
#endif

#ifdef JP
#define MES_RESTORE_SOME_MP "���������n�b�L�������B"
#else
#define MES_RESTORE_SOME_MP "You feel your head clear a little."
#endif

#ifdef JP
#define MES_EARTHQUAKE "�n�ʂ��h�ꂽ�B"
#else
#define MES_EARTHQUAKE "The ground trembles."
#endif


// messages of detection

#ifdef JP

#define MES_DETECT_TRAPS "�g���b�v�̑��݂������Ƃ����I"
#define MES_DETECT_DOORS "�h�A�̑��݂������Ƃ����I"
#define MES_DETECT_STAIRS "�K�i�̑��݂������Ƃ����I"
#define MES_DETECT_B_TREASURE "�������ꂽ����̑��݂������Ƃ����I"
#define MES_DETECT_TREASURE "����̑��݂������Ƃ����I"
#define MES_DETECT_OBJECTS "�A�C�e���̑��݂������Ƃ����I"
#define MES_DETECT_M_OBJECTS "���@�̃A�C�e���̑��݂������Ƃ����I"
#define MES_DETECT_CREATURES "�N���[�`���[�̑��݂������Ƃ����I"
#define MES_DETECT_I_CREATURES "�����Ȑ����̑��݂������Ƃ����I"
#define MES_DETECT_E_CREATURES "�׈��Ȃ鐶���̑��݂������Ƃ����I"
#define MES_DETECT_S_CREATURES "�E�C�������Ƃ����I"
#define MES_DETECT_SOMEONE "%s�̑��݂������Ƃ����I"
#define MES_DETECT_NOLIVING "���R�łȂ��N���[�`���[�̑��݂��������I"
#define MES_DETECT_ENLIGHTMENT "�����̒u����Ă���󋵂��]���ɕ�����ł���..."
#define MES_DETECT_MORE_ENLIGHTMENT "�X�Ȃ�[�ւ�������..."
#else

#define MES_DETECT_TRAPS "You sense the presence of traps!"
#define MES_DETECT_DOORS "You sense the presence of doors!"
#define MES_DETECT_STAIRS "You sense the presence of stairs!"
#define MES_DETECT_B_TREASURE "You sense the presence of buried treasure!"
#define MES_DETECT_TREASURE "You sense the presence of treasure!"
#define MES_DETECT_OBJECTS "You sense the presence of objects!"
#define MES_DETECT_M_OBJECTS "You sense the presence of magic objects!"
#define MES_DETECT_CREATURES "You sense the presence of creatures!"
#define MES_DETECT_I_CREATURES "You sense the presence of invisible creatures!"
#define MES_DETECT_E_CREATURES "You sense the presence of evil creatures!"
#define MES_DETECT_S_CREATURES "You sense the presence of someone's mind!"
#define MES_DETECT_SOMEONE "You sense the presence of %s!"
#define MES_DETECT_NOLIVING "You sense the presence of unnatural beings!"
#define MES_DETECT_ENLIGHTMENT "An image of your surroundings forms in your mind..."
#define MES_DETECT_MORE_ENLIGHTMENT "You begin to feel more enlightened..."
#endif


#ifdef JP
#define MES_FEELING_QUESTDUN "�T�^�I�ȃN�G�X�g�̃_���W�����̂悤���B"
#define MES_FEELING_WILD_POINT "�������肻���ȍr��̂悤���B"
#define MES_FEELING_TOWN "�T�^�I�Ȓ��̂悤���B"
#define MES_FEELING_WILD "�T�^�I�ȍr��̂悤���B"
#else
#define MES_FEELING_QUESTDUN "Looks like a typical quest level.";
#define MES_FEELING_WILD_POINT "Looks like a strange wilderness."
#define MES_FEELING_TOWN "Looks like a typical town."
#define MES_FEELING_WILD "Looks like a typical wilderness."
#endif

#ifdef JP
#define MES_CANNOT_GO_MORE "����ȏ��ɂ͐i�߂Ȃ��B"
#else
#define MES_CANNOT_GO_MORE "You cannot go any more."
#endif

#ifdef JP
#define MES_BLDG_CLOSED "(�X)"
#define MES_BLDG_NO_BLDG "�����ɂ͌����͂Ȃ��B"
#define MES_BLDG_LOSER "�u�s�҂ɗp�͂Ȃ��B�v"
#define MES_BLDG_ARENA_FIGHT "�Q�[�g�͕܂��Ă���B���肪���Ȃ���҂��Ă���I"
#define MES_BLDG_NO_RIGHT "�����I�����錠���͂���܂���I"
#define MES_BULD_EXIT "�������o��"

#define MES_BLDG_GET_PRICE "�܋� %ld������ɓ��ꂽ�B"
#define MES_BLDG_TODAY_TARGET1(TARGET) "�^�[�Q�b�g�F %s", (TARGET)
#define MES_BLDG_TODAY_TARGET2(PRICE) "���� ---- $%d", (PRICE)
#define MES_BLDG_TODAY_TARGET3(PRICE) "��   ---- $%d", (PRICE)
#define MES_BLDG_TSUCHINOKO1 "��l����̑�`�����X�I�I�I\n�^�[�Q�b�g�F���̒��b�u�c�`�m�R�v"
#define MES_BLDG_TSUCHINOKO2 "�����߂� ---- $1,000,000\n����     ----   $200,000\n��       ----   $100,000"
#define MES_BOUNTY_HAND_OVER(CORPSE) "%s��n���܂����H", (CORPSE)
#define MES_BOUNTY_EARN_POINT(NUM) "����ō��v %d �|�C���g�l�����܂����B", (NUM)
#define MES_BOUNTY_GET_REWARD(OBJECT, SYM) "%s(%c)�������B", (OBJECT), (SYM)
#define MES_BOUNTY_NOTHING "�܋��𓾂�ꂻ���Ȃ��͎̂����Ă��Ȃ������B"

#else
#define MES_BLDG_CLOSED "(closed)"
#define MES_BLDG_NO_BLDG "You see no building here."
#define MES_BLDG_LOSER "'There's no place here for a LOSER like you!'"
#define MES_BLDG_ARENA_FIGHT "The gates are closed.  The enemy awaits!"
#define MES_BLDG_NO_RIGHT "You have no right to choose that!"
#define MES_BULD_EXIT "Exit building"

#define MES_BLDG_GET_PRICE "You get %ldgp."
#define MES_BLDG_TODAY_TARGET1(TARGET) "target: %s", (TARGET)
#define MES_BLDG_TODAY_TARGET2(PRICE) "corpse   ---- $%d", (PRICE)
#define MES_BLDG_TODAY_TARGET3(PRICE) "skeleton ---- $%d", (PRICE)
#define MES_BLDG_TSUCHINOKO1 "Big chance to quick money!!!\ntarget: the rarest animal 'Tsuchinoko'"
#define MES_BLDG_TSUCHINOKO2 "catch alive ---- $1,000,000\ncorpse      ----   $200,000\nbones       ----   $100,000"
#define MES_BOUNTY_HAND_OVER(CORPSE) "Hand %s over? ", (CORPSE)
#define MES_BOUNTY_EARN_POINT(NUM) "You earned %d point%s total.", (NUM), ((NUM) > 1 ? "s" : "") 
#define MES_BOUNTY_GET_REWARD(OBJECT, SYM) "You get %s (%c). ", (OBJECT), (SYM)
#define MES_BOUNTY_NOTHING "You have nothing."

#endif

#ifdef JP
#define MES_FEATURE_NO_UP_STAIR "�����ɂ͏��K�i����������Ȃ��B"
#define MES_FEATURE_NO_DOWN_STAIR "�����ɂ͉���K�i����������Ȃ��B"
#define MES_FEATURE_UP_STAIR "��̊K�ɍ~�肽�B"
#define MES_FEATURE_DOWN_STAIR "���̊K�ɍ~�肽�B"
#define MES_FEATURE_CREATE_STAIR "���@�̊K�i�����ꂽ..."
#define MES_FEATURE_TRAP_DOOR_SELF "�킴�Ɨ��Ƃ��˂ɗ������B"
#define MES_FEATURE_ENTER_DUNGEON(MES) "%s�֓������B", (MES)

#define MES_FEATURE_STUCK "%s�͂�������ƕ����Ă���悤���B"
#define MES_FEATURE_CLOSED_DUNGEON "�_���W�����̓����͍ǂ���Ă���I"
#define MES_FEATURE_CHECK_DUNGEON "�����ɂ�%s�̓����(%d�K����)������܂�"
#define MES_FEATURE_ASK_ENTER_DUNGEON "�{���ɂ��̃_���W�����ɓ���܂����H"

#define MES_PATTERN_LIMIT1 "�p�^�[���̏������n�߂�ƁA�S�Ă�����Ȃ���΂Ȃ�܂���B�����ł����H"
#define MES_PATTERN_LIMIT2 "�p�^�[���̏������ɂ̓X�^�[�g�n�_��������n�߂Ȃ��Ă͂Ȃ�܂���B"
#define MES_PATTERN_LIMIT3 "�p�^�[���̏�͐����������ŕ����˂΂Ȃ�܂���B"
#define MES_PATTERN_LIMIT4 "�p�^�[���𓥂݊O���Ă͂����܂���B"
#define MES_PATTERN_GOAL "�u�p�^�[���v�̂��̕����͑��̕�����苭�͂łȂ��悤���B"
#define MES_PATTERN_WRECKED "�p�^�[�������ŉ����Ă��܂����I�������낵�������N���肻�����I"

#else
#define MES_FEATURE_NO_UP_STAIR "I see no up staircase here."
#define MES_FEATURE_NO_DOWN_STAIR "I see no down staircase here."
#define MES_FEATURE_UP_STAIR "You enter the up staircase."
#define MES_FEATURE_DOWN_STAIR "You enter the down staircase."
#define MES_FEATURE_CREATE_STAIR "A magical staircase appears..."
#define MES_FEATURE_TRAP_DOOR_SELF "You deliberately jump through the trap door."
#define MES_FEATURE_ENTER_DUNGEON(MES) "You entered %s.", (MES)

#define MES_FEATURE_STUCK "The %s appears to be stuck."
#define MES_FEATURE_CLOSED_DUNGEON "The entrance of this dungeon is closed!"
#define MES_FEATURE_CHECK_DUNGEON "There is the entrance of %s (Danger level: %d)"
#define MES_FEATURE_ASK_ENTER_DUNGEON "Do you really get in this dungeon? "

#define MES_PATTERN_LIMIT1 "If you start walking the Pattern, you must walk the whole way. Ok? "
#define MES_PATTERN_LIMIT2 "You must start walking the Pattern from the startpoint."
#define MES_PATTERN_LIMIT3 "You must walk the Pattern in correct order."
#define MES_PATTERN_LIMIT4 "You may not step off from the Pattern."
#define MES_PATTERN_GOAL "This section of the Pattern looks less powerful."
#define MES_PATTERN_WRECKED "You bleed on the Pattern! Something terrible happens!"
#endif

#ifdef JP
#define MES_SUMMON_NOTHING "��������Ȃ������B"
#define MES_SUMMON_SERVANT "��p�ł������܂����H�䂪��B"
#define MES_SUMMON_FEELING "�������ԋ߂Ɍ��ꂽ��������B"
#define MES_SUMMON_FUMBLE(HOSTILE) "�������ꂽ%s�͏����҂ɓG�ӂ������Ă���B", (HOSTILE)
#define MES_SUMMON_INROU "�u�҂ǂ��A�Ђ������낤�I�I�I���̂������ǂȂ��Ƃ����낦��B�v"
#else
#define MES_SUMMON_NOTHING "Nobody ever turns up..."
#define MES_SUMMON_SERVANT "'What is thy bidding... Master?'"
#define MES_SUMMON_FEELING "You hear something appear nearby."
#define MES_SUMMON_FUMBLE(HOSTILE) "The summoned %s are angry.", (HOSTILE)
#define MES_SUMMON_INROU "'WHO do you think this person is! Bow your head, down your knees!'"
#endif

#ifdef JP
#define MES_OBJECT_DISENCHANTED(OBJECT) "%s�͗򉻂��Ă��܂����B", (OBJECT)->name
#define MES_OBJECT_RESISTED_CURSE(OBJECT) "%s�͋��|�̈Í��I�[���𒵂˕Ԃ����B", (OBJECT)->name
#define MES_OBJECT_KILLED "���Ă��܂����B"
#define MES_OBJECT_DUSTED "�D�ɂȂ����B"
#else
#define MES_OBJECT_DISENCHANTED(OBJECT) "Your %s was disenchanted.", (OBJECT)
#define MES_OBJECT_RESISTED_CURSE(OBJECT) "Your %s resists the terrible black aura.", (OBJECT)->name
#define MES_OBJECT_KILLED (plural ? " are destroyed!" : " is destroyed!")
#define MES_OBJECT_DUSTED (plural ? " become dust." : " becomes dust.")
#endif

#ifdef JP
#define MES_DIGGING_PERMANENT1 "���̊�͍d�����Č@��Ȃ��悤���B"
#define MES_DIGGING_PERMANENT2 "�����͌@��Ȃ�!"
#define MES_DIGGING_DONE1(NAME) ("%s�����������B", (NAME))
#define MES_DIGGING_DONE2(NAME) ("%s���������Ă���B", (NAME))
#define MES_DIGGING_CUT(TARGET) "%s��؂��Ă���B", (TARGET)
#define MES_DIGGING_CUT_DONE(TARGET) "%s��؂蕥�����B", (TARGET)
#define MES_DIGGING_TUNNEL(TARGET) "%s�Ɍ����@���Ă���B", (TARGET)
#define MES_DIGGING_TUNNEL_DONE "�����@��I�����B"
#define MES_DIGGING_TUNNEL_DOOR "�h�A�͌@��Ȃ��B"
#define MES_DIGGING_TUNNEL_DISABLE "�����͌@��Ȃ��B"
#else
#define MES_DIGGING_PERMANENT1 "This seems to be permanent rock."
#define MES_DIGGING_PERMANENT2 "You can't tunnel through that!"
#define MES_DIGGING_DONE1(NAME) ("You have removed the %s.", (NAME))
#define MES_DIGGING_DONE2(NAME) ("You dig into the %s.", (NAME))
#define MES_DIGGING_CUT(TARGET) "You chop away at the %s.", (TARGET)
#define MES_DIGGING_CUT_DONE(TARGET) "You have cleared away the %s.", (TARGET)
#define MES_DIGGING_TUNNEL(TARGET) "You tunnel into the %s.", (TARGET)
#define MES_DIGGING_TUNNEL_DONE "You have finished the tunnel."
#define MES_DIGGING_TUNNEL_DOOR "You cannot tunnel through doors."
#define MES_DIGGING_TUNNEL_DISABLE "You can't tunnel through that."
#endif

#ifdef JP
#define MES_TY_CURSE_TELEPORT "���͂̋�Ԃ��c�񂾁I"
#define MES_TY_CURSE_PURE_MANA "�����Ȗ��͂̎����ւ̔����J�����I"
#define MES_TY_CURSE_SURGE_ENERGY "�G�l���M�[�̂��˂���������I"
#define MES_TY_CURSE_STATUE "�����ɂȂ����C�����I"
#define MES_TY_CURSE_AMNESIA "�ق��H���͒N�H�����ŉ����Ă�H"
#else
#define MES_TY_CURSE_TELEPORT "Space warps about you!"
#define MES_TY_CURSE_PURE_MANA "A portal opens to a plane of raw mana!"
#define MES_TY_CURSE_SURGE_ENERGY "A surge of energy happens!"
#define MES_TY_CURSE_STATUE "You feel like a statue!"
#define MES_TY_CURSE_AMNESIA "Huh? Who am I? What am I doing here?"
#endif

#if JP
#define MES_TELEPORT_LEVEL_ASK "���̊K������܂����H"
#define MES_TELEPORT_NO_TOWN "�܂��s����Ƃ��낪�Ȃ��B"
#define MES_TELEPORT_WHICH_TOWN "�ǂ��ɍs���܂���:"
#define MES_TELEPORT_LEVEL_UP(TARGET) "%^s�͓V���˂��j���Ē��֕����Ă����B", (TARGET)
#define MES_TELEPORT_LEVEL_DOWN(TARGET) "%^s�͏���˂��j���Ē���ł����B", (TARGET)
#define MES_RECALL_ASK "�A�҂̗͂��g���܂����H"
#define MES_RECALL_STARTING "���̑�C������߂Ă���..."
#define MES_RECALL_CANCEL "����߂���C�����ꋎ����..."
#define MES_RECALL_TRUMP_TOWER "�g�����v�^���[��"
#else
#define MES_TELEPORT_LEVEL_ASK "Leave this level? "
#define MES_TELEPORT_NO_TOWN "You have not yet visited any town."
#define MES_TELEPORT_WHICH_TOWN "Which town you go: "
#define MES_TELEPORT_LEVEL_UP(TARGET) "%^s rise%s up through the ceiling.", (TARGET), (m_idx <= 0) ? "" : "s"
#define MES_TELEPORT_LEVEL_DOWN(TARGET) "%^s sink%s through the floor.", m_name, (m_idx <= 0) ? "" : "s"
#define MES_RECALL_ASK "Activate recall? "
#define MES_RECALL_STARTING "The air about you becomes charged..."
#define MES_RECALL_CANCEL "A tension leaves the air around you..."
#define MES_RECALL_TRUMP_TOWER "at Trump Tower"
#endif

#if JP
#define MES_ALCHEMY_WHICH_OBJECT "�ǂ̃A�C�e�������ɕς��܂����H"
#define MES_ALCHEMY_NO_OBJECT "���ɕς����镨������܂���B"
#define MES_ALCHEMY_ASK(OBJECT) "�{����%s�����ɕς��܂����H", (OBJECT)
#define MES_ALCHEMY_FAILED(OBJECT) "%s�����ɕς��邱�ƂɎ��s�����B", (OBJECT)
#define MES_ALCHEMY_NO_PRICE(OBJECT) "%s���j�Z�̋��ɕς����B", (OBJECT)
#define MES_ALCHEMY_SUCCESS(OBJECT, PRICE) "%s����%d �̋��ɕς����B", (OBJECT), (PRICE)
#else
#define MES_ALCHEMY_WHICH_OBJECT "Turn which item to gold? "
#define MES_ALCHEMY_NO_OBJECT "You have nothing to turn to gold."
#define MES_ALCHEMY_ASK(OBJECT) "Really turn %s to gold? ", (OBJECT)
#define MES_ALCHEMY_FAILED(OBJECT) "You fail to turn %s to gold!", (OBJECT)
#define MES_ALCHEMY_NO_PRICE(OBJECT) "You turn %s to fool's gold.", (OBJECT)
#define MES_ALCHEMY_SUCCESS(OBJECT, PRICE) "You turn %s to %ld coins worth of gold.", (OBJECT), (PRICE)
#endif

#ifdef JP
#define MES_ENCHANT_CURSE_BROKEN "�������Ă����􂢂��ł��j��ꂽ�I"
#else
#define MES_ENCHANT_CURSE_BROKEN "The curse is broken!"
#endif

#ifdef JP
#define MES_INDENTIFY_RESULT_EQUIPMENT(EQUIP_POS, NAME, LABEL) "%^s: %s(%c)�B", (EQUIP_POS), (NAME), (LABEL)
#define MES_INDENTIFY_RESULT_IN_ZACK(NAME, LABEL) "�U�b�N��: %s(%c)�B", (NAME), (LABEL)
#define MES_INDENTIFY_RESULT_ON_GROUND(NAME) "����: %s�B", (NAME)
#else
#define MES_INDENTIFY_RESULT_EQUIPMENT(EQUIP_POS, NAME, LABEL) "%^s: %s(%c)�B", (EQUIP_POS), (NAME), (LABEL)
#define MES_INDENTIFY_RESULT_IN_ZACK(NAME, LABEL) "In your pack: %s (%c).", (NAME), (LABEL)
#define MES_INDENTIFY_RESULT_ON_GROUND(NAME) "On the ground: %s.", (NAME)
#endif

// Cause of Death
#ifdef JP

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
#define COD_POTION_OF_DETONATION "�����̖�"
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
#define COD_EARTHQUAKE_CASTER(CASTER) "%s�̋N�������n�k", (CASTER)
#define COD_EARTHQUAKE "�n�k"
#define COD_PURE_MANA "�����Ȗ��͂̉��"
#define COD_EQUIPED_DAMAGE(OBJECT_NAME) "%s�𑕔������_���[�W", OBJECT_NAME
#define COD_HELLFIRE_RISK "�n���̍��΂̎�������������J"
#define COD_JOJ_RISK "�R���̕��"
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
#define COD_POTION_OF_DETONATION "a potion of Detonation"
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
#define COD_EARTHQUAKE_CASTER(CASTER) "an earthquake caused by %s", (CASTER)
#define COD_EARTHQUAKE "an earthquake"
#define COD_PURE_MANA "released pure mana"
#define COD_EQUIPED_DAMAGE(OBJECT_NAME) "wielding %s", OBJECT_NAME
#define COD_HELLFIRE_RISK "the strain of casting Hellfire"
#define COD_JOJ_RISK "the Jewel of Judgement"
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
#define BINARY_FILES_RAW(NAME) "%s_j.raw", (NAME)
#else
#define BINARY_FILES_RAW(NAME) "%s.raw", (NAME)
#endif

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

#define TEXT_FILES_HELP "jhelp.hlp"
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

#define TEXT_FILES_HELP "help.hlp"
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
	#define PROMPT_LABEL(N1, N2) " %c-%c,'(',')',", (N1), (N2)
	#define PROMPT_FIND "����(^I:������ ^L:�j�󂳂ꂽ��): "
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
	#define PROMPT_LABEL(N1, N2) " %c-%c,'(',')',", (N1), (N2)
	#define PROMPT_FIND "Search key(^I:inven ^L:destroyed): "
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
	#define SKILL_NAME_PSYONIC "���\��"
	#define SKILL_NAME_IMITATION "���̂܂�"
	#define SKILL_NAME_HISSATSU "�K�E��"
	#define SKILL_NAME_MIRROR_M "�����@"
	#define SKILL_NAME_NINJUTSU "�E�p"
	#define SKILL_NAME_PRAYER "�F��"
	#define SKILL_NAME_FORCE "�B�C�p"
	#define SKILL_NAME_BRUTAL "���z"
	#define SKILL_NAME_SNIPING "�ˌ��p"
#else
	#define SKILL_NAME_MAGIC "magic"
	#define SKILL_NAME_PSYONIC "psionic powers"
	#define SKILL_NAME_IMITATION "imitation"
	#define SKILL_NAME_HISSATSU "hissatsu"
	#define SKILL_NAME_MIRROR_M "mirror magic"
	#define SKILL_NAME_NINJUTSU "ninjutsu"
	#define SKILL_NAME_PRAYER "prayer"
	#define SKILL_NAME_FORCE "force"
	#define SKILL_NAME_BRUTAL "brutal power"
	#define SKILL_NAME_SNIPING "Sniping"
#endif

#ifdef JP

#define DIATY_DATE(DAY) "%d����\n", (DAY)
#define DIATY_COUNTSTOP_DATE "*****����\n"
#define DIARY_PLACE_ARENA "�A���[�i:"
#define DIARY_PLACE_SURFACE "�n��:"
#define DIARY_PLACE_QUEST "�N�G�X�g:"
#define DIARY_TITLE(CREATURE) "�u%s%s�̓`���v", chara_info[(CREATURE)->chara_idx].title, (CREATURE)->name
#define DIARY_NOTE "���e"

#define DIARY_CREATE_PLAYER "-------- �v���C���[�쐬 --------"
#define DIARY_GAMEOVER "-------- �Q�[���I�[�o�[ --------"
#define DIARY_SAVEQUIT "----�Q�[�����f----"
#define DIARY_GAMERESTART "----�Q�[���ĊJ----"
#define DIARY_GAMEABORT "----�Q�[���ُ�I��----"
#define DIARY_SELECTMODE "�L�����y�[���w%s�x��I������"
#define DIARY_SELECTSEX(SEX) "���ʂ�%s��I�������B", (SEX)
#define DIARY_SELECT_PURERACE(RACE) "�푰��%s��I�������B", (RACE)
#define DIARY_SELECT_HALFRACE(RACE1, RACE2) "�푰��%s��%s�̍�����I�������B", (RACE1), (RACE2)
#define DIARY_SELECT_CLASS(CLASS) "�E�Ƃ�%s��I�������B", (CLASS)
#define DIARY_SELECT_REALM(REALM) "���@�̗̈��%s��I�������B", (REALM)
#define DIARY_SELECT_CHALA(CHALA) "���i��%s��I�������B", (CHALA)
#define DIARY_RETIRED "�_���W�����̒T��������ނ����B"
#define DIARY_SUICIDE "��]���Ď��E�����B"
#define DIARY_WINNING_SEPPUKU "�����̌�ؕ������B"
#define DIARY_WIZARD "�E�B�U�[�h���[�h�ɓ˓����ăX�R�A���c���Ȃ��Ȃ����B"
#define DIARY_UP_STAIR "�K�i�������"
#define DIARY_DOWN_STAIR "�K�i�����肽"
#define DIARY_TRAP_DOOR "���Ƃ��˂ɗ�����"
#define DIARY_PINCH "%s�ɂ���ăs���`�Ɋׂ����B"
#define DIARY_PATRON(REWARD) "�p�g�����̕�V��%s", (REWARD)
#define DIARY_INN_OVER_DAY "�h���Œ��Ԃ��߂������B"
#define DIARY_INN_MORNING "���Ȃ��̓��t���b�V�����Ėڊo�߁A�V���ȓ����}�����B"
#define DIARY_INN_MORNING2 "�����������������}�����B"
#define DIARY_INN_EVENING "���Ȃ��̓��t���b�V�����Ėڊo�߁A�[�����}�����B"
#define DIARY_INN_EVENING2 "�[�����}�����B"
#define DIARY_INN_STAY_NIGHT "�h���ň��𖾂������B"
#define DIARY_INN_NIGHTMARE "�����ɂ��Ȃ���Ă悭����Ȃ������B"
#else

#define DIATY_DATE(DAY) "Day %d\n", (DAY)
#define DIATY_COUNTSTOP_DATE "Day *****\n"
#define DIARY_PLACE_ARENA "Arena:"
#define DIARY_PLACE_SURFACE "Surface:"
#define DIARY_PLACE_QUEST "Quest:"
#define DIARY_TITLE(CREATURE) "Legend of %s %s", chara_info[(CREATURE)->chara_idx].title, (CREATURE)->name
#define DIARY_NOTE "Diary Note"

#define DIARY_CREATE_PLAYER "-------- Create Player --------"
#define DIARY_GAMEOVER "--------   Game  Over   --------"
#define DIARY_SAVEQUIT "---- Save and Exit Game ----"
#define DIARY_GAMERESTART "---- Restart Game ----"
#define DIARY_GAMEABORT "---- Panic Save and Abort Game ----"
#define DIARY_SELECTMODE "Select Campaign '%s'"
#define DIARY_SELECTSEX(SEX) "choose %s personality.", (SEX)
#define DIARY_SELECT_PURERACE(RACE) "choose %s race.", (RACE)
#define DIARY_SELECT_HALFRACE(RACE1, RACE2) "choose %s and %s races.", (RACE1), (RACE2)
#define DIARY_SELECT_CLASS(CLASS) "choose %s class.", (CLASS)
#define DIARY_SELECT_REALM(REALM) "choose %s realm.", (REALM)
#define DIARY_SELECT_CHALA(CHALA) "choose %s.", (CHALA)
#define DIARY_RETIRED "retired exploring dungeons."
#define DIARY_SUICIDE "give up all hope to commit suicide."
#define DIARY_WINNING_SEPPUKU "did Seppuku after the winning."
#define DIARY_WIZARD "give up sending score to use wizard mode."
#define DIARY_UP_STAIR 1"climbed up the stairs to"
#define DIARY_DOWN_STAIR "climbed down the stairs to"
#define DIARY_TRAP_DOOR "You have fallen through a trap door."
#define DIARY_PINCH "A critical situation because of %s".
#define DIARY_PATRON(REWARD) "The patron rewards you with %s.", (REWARD)
#define DIARY_INN_MORNING "You awake refreshed for the new day."
#define DIARY_INN_MORNING2 "awake refreshed."
#define DIARY_INN_EVENING "You awake refreshed for the evening."
#define DIARY_INN_EVENING2 "awake refreshed."
#define DIARY_INN_OVER_DAY "stay over daytime at the inn."
#define DIARY_INN_STAY_NIGHT "stay over night at the inn."
#define DIARY_INN_NIGHTMARE "be troubled by a nightmare."
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
#define MES_DEBUG_ROOM_NUM "������: %d"
#define MES_DEBUG_GLASS_ROOM "[�K���X�̕���]"
#define MES_DEBUG_ROOM_OF "[%s�̕���]"
#define MES_DEBUG_CRYPT_ROOM "[����]"
#define MES_DEBUG_TRAP_PIT_ROOM "[%s%s��㩃s�b�g]"
#define MES_DEBUG_BIAS_ARTIFACT "�^�̕΂����A�[�e�B�t�@�N�g: %d�B"
#define MES_DEBUG_NO_BIAS_ARTIFACT "�΂��Ă��Ȃ��A�[�e�B�t�@�N�g"
#define MES_DEBUG_FIXED_ARTIFACT(OBJECT) "�Œ�A�[�e�B�t�@�N�g(%s)", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT(OBJECT) "�����_���A�[�e�B�t�@�N�g(%s)", (OBJECT)
#define MES_DEBUG_EGO_ITEM(OBJECT) "�G�S�A�C�e��(%s)", (OBJECT)
#define MES_DEBUG_NORMAL_ITEM(OBJECT) "�A�C�e��(%s)", (OBJECT)
#define MES_DEBUG_DIE_CHOICE "���ɂ܂���?"
#define MES_DEBUG_LAKE "�΂𐶐��B"
#define MES_DEBUG_CAVE "���A�𐶐��B"
#define MES_DEBUG_ARENA "�A���[�i���x��"
#define MES_DEBUG_DEC_CREATURE(FROM, TO) "�N���[�`���[����{�l�� %d ���� %d �Ɍ��炵�܂�", (FROM), (TO)
#define MES_DEBUG_SMALL_LEVEL "�����ȃt���A"
#define MES_DEBUG_FLOOR_SIZE(WIDTH, HEIGHT) "[W:%d, H:%d]"
#define MES_DEBUG_VAULT(NAME) "�n����(%s)", (NAME)
#define MES_DEBUG_GREATER_VAULT(NAME) "����Ȓn����(%s)", (NAME)
#define MES_DEBUG_FAILED_VAULT "�n������z�u�ł��܂���B"
#define MES_DEBUG_ARTIFACT_CANCEL(OBJECT) "�`���̃A�C�e�� (%s) �͐�������*�j��*���ꂽ�B", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT_CANCEL "�����_���E�A�[�e�B�t�@�N�g��1�͐�������*�j��*���ꂽ�B"
#define MES_DEBUG_ARTIFACT_CANCEL2(OBJECT) "�`���̃A�C�e�� (%s) �̓X�g���[�}�[�ɂ��폜���ꂽ�B", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT_CANCEL2 "�����_���E�A�[�e�B�t�@�N�g��1�̓X�g���[�}�[�ɂ��폜���ꂽ�B"
#define MES_DEBUG_FAILED_STREAMER "�X�g���[�}�[�̐����Ɏ��s�����B"
#define MES_DEBUG_DESTROYED_FLOOR "�j�󂳂ꂽ�K"
#define MES_DEBUG_FUNNY_PATTERN(FEATURE) "�������ȃp�^�[�����s�A%d�B", (FEATURE)
#define MES_DEBUG_FAILED_FLOOR "�_���W���������Ɏ��s"
#define MES_DEBUG_TOO_ITEM "�A�C�e�������߂���"
#define MES_DEBUG_TOO_CREATURE "�N���[�`���[�����߂���"
#define MES_DEBUG_FLOOR_RETAKE(WHY) "������蒼��(%s)"
#else
#define MES_DEBUG_ROOM_NUM "Number of Rooms: %d"
#define MES_DEBUG_GLASS_ROOM "[Glass Room]"
#define MES_DEBUG_ROOM_OF "[Room of %s]"
#define MES_DEBUG_CRYPT_ROOM "[Crypt]"
#define MES_DEBUG_TRAP_PIT_ROOM "[Trapped creature pit (%s%s)]"
#define MES_DEBUG_BIAS_ARTIFACT "Biased artifact: %d."
#define MES_DEBUG_NO_BIAS_ARTIFACT "No biased artifact."
#define MES_DEBUG_FIXED_ARTIFACT(OBJECT) "Fixed Artifact (%s)", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT(OBJECT) "Random Artifact (%s)", (OBJECT)
#define MES_DEBUG_EGO_ITEM(OBJECT) "Ego Item (%s)", (OBJECT)
#define MES_DEBUG_NORMAL_ITEM(OBJECT) "Item (%s)", (OBJECT)
#define MES_DEBUG_DIE_CHOICE "Die?"
#define MES_DEBUG_LAKE "Lake on the level."
#define MES_DEBUG_CAVE "Cavern on level."
#define MES_DEBUG_ARENA "Arena level."
#define MES_DEBUG_DEC_CREATURE(FROM, TO) "Reduced creatures base from %d to %d", (FROM), (TO)
#define MES_DEBUG_SMALL_LEVEL "A small dungeon level."
#define MES_DEBUG_FLOOR_SIZE(WIDTH, HEIGHT) "[W:%d, H:%d]"
#define MES_DEBUG_VAULT(NAME) "Vault(%s)", (NAME)
#define MES_DEBUG_GREATER_VAULT(NAME) "Greater vault (%s)", (NAME)
#define MES_DEBUG_FAILED_VAULT "Could not place vault."
#define MES_DEBUG_ARTIFACT_CANCEL(OBJECT) "Artifact (%s) was *destroyed* during generation.", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT_CANCEL "One of the random artifacts was *destroyed* during generation."
#define MES_DEBUG_ARTIFACT_CANCEL2(OBJECT) "Artifact (%s) was deleted by streamer.", (OBJECT)
#define MES_DEBUG_RANDOM_ARTIFACT_CANCEL2 "One of the random artifacts was deleted by streamer."
#define MES_DEBUG_FAILED_STREAMER "Could not place streamer!"
#define MES_DEBUG_DESTROYED_FLOOR "Destroyed Floor"
#define MES_DEBUG_FUNNY_PATTERN(FEATURE) "Funny Pattern walking, %d.", (FEATURE)
#define MES_DEBUG_FAILED_FLOOR "could not place player"
#define MES_DEBUG_TOO_ITEM "too many objects"
#define MES_DEBUG_TOO_CREATURE "too many creatures"
#define MES_DEBUG_FLOOR_RETAKE(WHY) "Generation restarted (%s)"
#endif

#ifdef JP
#define MES_EFFECT_KILL_ACID "�Z���Ă��܂����I"
#define MES_EFFECT_KILL_FIRE "�R���Ă��܂����I"
#define MES_EFFECT_KILL_COLD "�ӂ��U���Ă��܂����I"
#define MES_EFFECT_KILL_DISINTEGRATE "�������Ă��܂����I"
#define MES_EFFECT_REVIVED "�����Ԃ����B"
#define MES_EFFECT_KILL_OBJECT(OBJECT_NAME, KILL) "%s��%s", (OBJECT_NAME), (KILL)
#define MES_EFFECT_UNEFFECTED_OBJECT "%s�͉e�����󂯂Ȃ������B", object_name
#define MES_EFFECT_CHAOS_DONE "���Ȃ��̐g�̂̓J�I�X�̗͂ŔP���Ȃ���ꂽ�I"
#define MES_EFFECT_TIME_DONE(TARGET) "%s�̎��̗��ꂪ�t�߂肵���B", (TARGET)
#define MES_EFFECT_GRAVITY_DONE "���ӂ̏d�͂��䂪�񂾁B"
#define MES_EFFECT_MIRROR_CRUSH "�������ꂽ�I"
#define MES_EFFECT_MIND_BACKFIRE "%^s�̑��������_�͍U���𒵂˕Ԃ����I"
#define MES_EFFECT_DISARM_TRAP "�J�`�b�Ɖ��������B"
#define MES_EFFECT_CUT_ARROW "����a��̂Ă��I"
#define MES_EFFECT_BLASTED "�͂ꂽ"
#define MES_EFFECT_SHRANK "�k��"
#define MES_EFFECT_MELTED "�n����"
#define MES_EFFECT_FROZEN "����A�ӂ��U����"
#define MES_EFFECT_BURN_UP "�R����"
#define MES_EFFECT_CRUSHED "���ӂ��ꂽ"
#define MES_EFFECT_A_TREE_(MES) "�؂�%s�B", (MES)
#define MES_EFFECT_CRUSHED2(MES) "%s�����ꂽ�I", (MES)
#define MES_EFFECT_PSY_DEAD "�̐��_�͕��󂵁A���͔̂����k�ƂȂ����B"
#define MES_EFFECT_ANTIPATHY "�͂��Ȃ��ɓG�ӂ�����Ă���B"
#define MES_EFFECT_TAMED "���x�z�����B"
#define MES_EFFECT_ALREADY_THRALL "�͊��ɂ��Ȃ��̓z�ꂾ�B"
#define MES_EFFECT_STASIS "�͓����Ȃ��Ȃ����I"
#define MES_EFFECT_FRIENDLY "�͓ˑR�F�D�I�ɂȂ����悤���I"
#define MES_EFFECT_OTHER_HEALTHIER(CREATURE) "%^s�͋C�����ǂ��������B", (CREATURE)->name
#define MES_EFFECT_HAND_OF_DOOM "���Ȃ��͖������܂��Ă����悤�Ɋ������I"
#define MES_EFFECT_PHOTO(TARGET) "%s���ʐ^�ɎB�����B", (TARGET)->name
#define MES_EFFECT_HEAVY_REDUCE_STAT(TARGET, STAT) "%s��%s���傫�����Ȃ�ꂽ�B", (TARGET)->name, (STAT)
#define MES_EFFECT_HEAVY_REDUCE_STAT_ALL(TARGET) "%s�̔\�͂��傫���������B", (TARGET)->name
#define MES_EFFECT_GENOCIDE(TARGET) "%s�͏��ł����I", (TARGET)->name
#else
#define MES_EFFECT_KILL_ACID (plural ? " melt!" : " melts!")
#define MES_EFFECT_KILL_FIRE (plural ? " burn up!" : " burns up!")
#define MES_EFFECT_KILL_COLD (plural ? " shatter!" : " shatters!")
#define MES_EFFECT_KILL_DISINTEGRATE (plural ? " evaporate!" : " evaporates!")
#define MES_EFFECT_REVIVED " revived."
#define MES_EFFECT_UNEFFECTED_OBJECT "The %s %s unaffected.", object_name, (plural ? "are" : "is")
#define MES_EFFECT_KILL_OBJECT(OBJECT_NAME, KILL) "The %s%s", (OBJECT_NAME), (KILL)
#define MES_EFFECT_CHAOS_DONE "Your body is twisted by chaos!"
#define MES_EFFECT_TIME_DONE(TARGET) "You saw %s' life has clocked back.", (TARGET)
#define MES_EFFECT_GRAVITY_DONE "Gravity warps around you."
#define MES_EFFECT_MIRROR_CRUSH "The mirror was crashed!"
#define MES_EFFECT_MIND_BACKFIRE "%^s corrupted mind backlashes your attack!"
#define MES_EFFECT_DISARM_TRAP "Click!"
#define MES_EFFECT_CUT_ARROW "You cut down the arrow!"
#define MES_EFFECT_BLASTED "was blasted."
#define MES_EFFECT_SHRANK "shrank."
#define MES_EFFECT_MELTED "melted."
#define MES_EFFECT_FROZEN "was frozen and smashed."
#define MES_EFFECT_BURN_UP "burns up!"
#define MES_EFFECT_CRUSHED "was crushed."
#define MES_EFFECT_A_TREE_(MES) "A tree %s", (MES)
#define MES_EFFECT_CRUSHED2(MES) "The %s was crashed!", (MES)
#define MES_EFFECT_PSY_DEAD " collapses, a mindless husk."
#define MES_EFFECT_ANTIPATHY " hates you too much!"
#define MES_EFFECT_TAMED " is tamed!"
#define MES_EFFECT_ALREADY_THRALL " is in your thrall!"
#define MES_EFFECT_STASIS " is suspended!"
#define MES_EFFECT_FRIENDLY " suddenly seems friendly!"
#define MES_EFFECT_OTHER_HEALTHIER(CREATURE) "%^s appears healthier.", (CREATURE)->name
#define MES_EFFECT_HAND_OF_DOOM "You feel your life fade away!"
#define MES_EFFECT_PHOTO(TARGET) "You take a photograph of %s.", (TARGET)->name
#define MES_EFFECT_HEAVY_REDUCE_STAT(TARGET, STAT) "%s is not as %s as you used to be.", (TARGET)->name, (STAT)
#define MES_EFFECT_HEAVY_REDUCE_STAT_ALL(TARGET) "%s is not as powerful as you used to be...", (TARGET)->name, (STAT)
#define MES_EFFECT_GENOCIDE(TARGET) "%^s disappered!", (TARGET)->name
#endif

#ifdef JP
#define MES_BIRTH_SELECT_CAMPAIGN "�L�����y�[����I�����ĉ�����"
#define MES_BIRTH_PROFILE_LOAD_ASK "�v���t�B�[���t�@�C�������[�h���܂���? "
#define MES_BIRTH_PROFILE_LOAD_FAILED "�v���t�B�[���t�@�C���̓ǂݍ��݂Ɏ��s���܂����B"
#define MES_BIRTH_PROFILE_LOAD_NO_DATA "�L���ȃv���t�B�[���͂��̃t�@�C���ɂ���܂���B"
#define MES_BIRTH_REALM "�̈��I�����ĉ�����:"
#define MES_BIRTH_PATRON "��_��I�����ĉ�����:"
#define MES_BIRTH_PERSONALITY "���i��I�����ĉ�����:"
#define MES_BIRTH_START_POINT "�J�n�n�_��I�����ĉ�����:"
#else
#define MES_BIRTH_SELECT_CAMPAIGN "Select Campaign"
#define MES_BIRTH_PROFILE_LOAD_ASK "Load profile preference file? "
#define MES_BIRTH_PROFILE_LOAD_FAILED "Failed to load profile preference."
#define MES_BIRTH_PROFILE_LOAD_NO_DATA "There does not exist valid profile preference."
#define MES_BIRTH_REALM "Select a realm:"
#define MES_BIRTH_PATRON "Select a patron:"
#define MES_BIRTH_PERSONALITY "Select a personality:"
#define MES_BIRTH_START_POINT "Select a starting point:"
#endif

#ifdef JP
#define MES_DUNGEON_INFO(NAME, LEVEL) "%s(%d�K����)", (NAME), (LEVEL)
#else
#define MES_DUNGEON_INFO(NAME, LEVEL) "%s(level %d)", (NAME), (LEVEL)
#endif

#ifdef JP
#define MES_QUEST_INFO(LEVEL) "�N�G�X�g��� (�댯�x: %d �K����)", (LEVEL)
#define MES_QUEST_INFO2(NAME, LEVEL) "�N�G�X�g�u%s�v(%d�K����)", (NAME), (LEVEL)
#define MES_QUEST_NO_QUEST "���̂Ƃ���N�G�X�g�͂���܂���B"
#define MES_QUEST_DEFAULT_KILLING "�N�G�X�g: %s�� %d�̓|��"
#define MES_QUEST_WARN_UNCOMPLETE "���Ȃ��͌��݂̃N�G�X�g���I�������Ă��܂���I\nCTRL-Q���g���΃N�G�X�g�̏�Ԃ��`�F�b�N�ł��܂��B\n�N�G�X�g���I��点����߂��ė��ĉ������B\n"
#define MES_QUEST_ASK_ACCEPT "���̃N�G�X�g��������܂����H"
#define MES_QUEST_ACCEPT "�N�G�X�g�w%s�x��������܂����B"
#define MES_QUEST_NO_ENTRANCE "�����ɂ̓N�G�X�g�̓����͂Ȃ�"
#define MES_QUEST_ENTRANCE "�����ɂ̓N�G�X�g�ւ̓���������܂��B"
#define MES_QUEST_ASK_ENTER "�N�G�X�g�ɓ���܂����H"
#define MES_QUEST_TARGET_INFO(TARGET) "���ӂ���I���̊K��%s�ɂ���Ď���Ă���I", (TARGET)
#define MES_QUEST_LORD(FLOOR, TARGET) "���̊K�ɂ�%s�̎�ł���%s������ł���B", (FLOOR), (TARGET)
#define MES_QUEST_TARGETS_INFO(TARGET, NUM) "���ӂ���I���̊K��%d�̂�%s�ɂ���Ď���Ă���I", (NUM), (TARGET)
#define MES_QUEST_TARGET_GONE "���̊K�͈ȑO�͒N���ɂ���Ď���Ă����悤���c�B"
#define MES_QUEST_EXIT_WARNING "���̊K����x����Ɠ�x�Ɩ߂��ė����܂���B"
#define MES_QUEST_ASK_EXIT "�{���ɂ��̊K������܂����H"
#define MES_QUEST_TYPE_KILL_ONE(FLOOR, TARGET) "�u%s�v�ɂ���u%s�v��|���B", (FLOOR), (TARGET)
#define MES_QUEST_TYPE_FIND_OBJECT(FLOOR, TARGET) "�u%s�v�ɂ���u%s�v�������o���B", (FLOOR), (TARGET)
#define MES_QUEST_TYPE_SEARCH "�T������B"
#define MES_QUEST_TYPE_KILL_ALL "�S�ẴN���[�`���[��|���B"
#define MES_QUEST_TYPE_KILL_NUMBER(MAX, CUR) "%d �̂̃N���[�`���[��|���B(%d �̎E�Q)", (MAX), (CUR)
#else
#define MES_QUEST_INFO2(NAME, LEVEL) "the entrance to the quest '%s'(level %d)", (NAME), (LEVEL)
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
#define MES_QUEST_LORD(FLOOR, TARGET) "%^s lives in this level as the keeper of %s.", (TARGET), (FLOOR)
#define MES_QUEST_TARGETS_INFO(TARGET, NUM) "Be warned, this level is guarded by %d %s!", (NUM), (TARGET)
#define MES_QUEST_TARGET_GONE "It seems that this level was protected by someone before..."
#define MES_QUEST_EXIT_WARNING "You can't come back here once you leave this floor."
#define MES_QUEST_ASK_EXIT "Really leave this floor? "
#define MES_QUEST_TYPE_KILL_ONE(FLOOR, TARGET) "kill %s in %s.", (TARGET), (FLOOR)
#define MES_QUEST_TYPE_FIND_OBJECT(FLOOR, TARGET) "Find out %s in %s.", (FLOOR), (TARGET)
#define MES_QUEST_TYPE_SEARCH "Search."
#define MES_QUEST_TYPE_KILL_ALL "Kill all creatures."
#define MES_QUEST_TYPE_KILL_NUMBER(MAX, CUR) "Kill %d creatures, have killed %d.", (MAX), (CUR)
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
#define MES_CREATURE_HP_GAIN(POINT) "�ő�q�b�g�E�|�C���g�� %d ���������I", (POINT)
#define MES_CREATURE_MP_GAIN(POINT) "�ő�}�W�b�N�E�|�C���g�� %d ���������I", (POINT)
#define MES_CREATURE_INC_STATUS(CREATURE, INC) "���[�I�I%s�͂ƂĂ�%s�Ȃ����I", (CREATURE)->name, (INC)
#define MES_CREATURE_DEC_STATUS(CREATURE, DEC) "%s�͂Ђǂ�%s�Ȃ����B", (CREATURE)->name, (DEC)
#define MES_CREATURE_DEC_STATUS_CANCEL(CREATURE, DEC) "%s�͂Ђǂ�%s�Ȃ肩�������A���ɖ߂���", (CREATURE)->name, (DEC)
#define MES_CREATURE_RES_STATUS(CREATURE, RES) "%s�͌��ʂ��%s�Ȃ����B", (CREATURE)->name, (RES)
#define MES_CREATURE_LIFE_RATE(NUM) "���݂̗̑̓����N : %d/100", (NUM)
#define MES_CREATURE_RES_EXP "�\�E�����񕜂����悤�ȋC������B"
#define MES_CREATURE_GAIN_EXP "�X�Ƀ\�E���𓾂��悤�ȋC������B"
#define MES_CREATURE_WHICH_GAIN "�ǂ̔\�͒l���グ�܂����H"
#define MES_CREATURE_KARMA(TYPE, NUM) "[%s]�̋�: %d\n", (TYPE), (NUM)
#else
#define MES_CREATURE_LEVELUP(LEV) "Welcome to level %d.", (LEV)
#define MES_CREATURE_HP_GAIN(POINT) "Max-HitPoint increased in %d.", (POINT)
#define MES_CREATURE_MP_GAIN(POINT) "Max-ManaPoint increased in %d.", (POINT)
#define MES_CREATURE_INC_STATUS(CREATURE, INC) "Wow! %s became very %s!", (CREATURE)->name, (INC)
#define MES_CREATURE_DEC_STATUS(CREATURE, DEC) "%s beaome very %s.", (CREATURE)->name, (DEC)
#define MES_CREATURE_DEC_STATUS_CANCEL(CREATURE, DEC) "%s become %s for a moment, but the feeling passes.", (CREATURE)->name, (DEC)
#define MES_CREATURE_RES_STATUS(CREATURE, RES) "%s became %s as before.", (CREATURE)->name, (RES)
#define MES_CREATURE_LIFE_RATE(NUM) "Your current Life Rating is %d/100.", (NUM)
#define MES_CREATURE_RES_EXP "You feel your soul energies returning."
#define MES_CREATURE_GAIN_EXP "You feel more souls."
#define MES_CREATURE_WHICH_GAIN "Which stat do you want to raise?"
#define MES_CREATURE_KARMA(TYPE, NUM) "Your karma of %s is %d.", (TYPE), (NUM)
#endif

#ifdef JP
#define MES_POLYSELF_ASK "�ϐg���܂��B��낵���ł����H"
#define MES_POLYSELF_FEELING "���Ȃ��͕ω��̖K���������..."
#define MES_POLYSELF_ORDINARY "��Ȃ��炢���ʂɂȂ����C������B"
#define MES_POLYSELF_DIFFICULT_FORM "���݂̎p�Ő����Ă����͍̂���Ȃ悤���I"
#define MES_POLYMORPH_DONE(CREATURE) "�ˑR%s���ϐg�����B", (CREATURE)
#define MES_POLYSELF_LESS_WOUND "�����Ă����������y���Ȃ����B"
#define MES_POLYSELF_MORE_WOUND "�V���ȏ����ł����I"
#else
#define MES_POLYSELF_ASK "You will polymorph your self. Are you sure? "
#define MES_POLYSELF_FEELING "You feel a change coming over you..."
#define MES_POLYSELF_ORDINARY "You feel oddly normal."
#define MES_POLYSELF_DIFFICULT_FORM "You find living difficult in your present form!"
#define MES_POLYMORPH_DONE(CREATURE) "Suddenly, %s transforms!", (CREATURE)
#define MES_POLYSELF_LESS_WOUND "Your wounds are polymorphed into less serious ones."
#define MES_POLYSELF_MORE_WOUND "A new wound was created!"
#endif

#ifdef JP
#define MES_STORE_RUMOR "�X��͎���������:"
#define MES_STORE_DECIDE_INSTANTLY "�����ɂ��̋��z�ɂ܂Ƃ܂����B"
#define MES_STORE_DECIDE_FINALY "���ǂ��̋��z�ɂ܂Ƃ܂����B"
#define MES_STORE_DECIDE_QUICKLY "����Ȃ�Ƃ��̋��z�ɂ܂Ƃ܂����B"
#define MES_STORE_LAST_BID(PRICE) "�O��̒񎦉��i $%ld", (PRICE)
#define MES_STORE_ASK_PRICE "�񎦂��鉿�i? "
#define MES_STORE_ENTIRE "����őS���ł��B"
#define MES_STORE_ITEM_LIMIT "����ȂɃA�C�e�������ĂȂ��B"
#define MES_STORE_ITEM_LIMIT2 "�U�b�N�ɂ��̃A�C�e�������錄�Ԃ��Ȃ��B"
#define MES_STORE_MUSEUM "�����ق�����o�����Ƃ͂ł��܂���B"
#define MES_STORE_NO_ITEM_HOME "�䂪�Ƃɂ͉����u���Ă���܂���B"
#define MES_STORE_NO_ITEM_MUSEUM "�����قɂ͉����u���Ă���܂���B"
#define MES_STORE_NO_ITEM_STORE "���ݏ��i�̍݌ɂ�؂炵�Ă��܂��B"
#define MES_STORE_WHICH_EXAMINE "�ǂ�𒲂ׂ܂����H"
#define MES_STORE_FULL_ITEM_HOME "�䂪�Ƃɂ͂����u���ꏊ���Ȃ��B"
#define MES_STORE_FULL_ITEM_MUSEUM "�����ق͂������t���B"
#define MES_STORE_FULL_ITEM_STORE "�����܂��񂪁A�X�ɂ͂����u���ꏊ������܂���B"
#define MES_STORE_WHICH_TAKING "�ǂ̃A�C�e�������o���܂����H"
#define MES_STORE_WHICH_PURCHASE "�ǂ̕i�����~�����񂾂��H"
#define MES_STORE_WHICH_DROP "�ǂ̃A�C�e����u���܂���? "
#define MES_STORE_WHICH_GIVE "�ǂ̃A�C�e�����񑡂��܂���? "
#define MES_STORE_WHICH_SELL "�ǂ̃A�C�e���𔄂�܂���? "
#define MES_STORE_WHICH_REMOVE "�ǂ̃A�C�e���̓W������߂����܂����H"
#define MES_STORE_NO_DROP "�u����A�C�e���������Ă��܂���B"
#define MES_STORE_NO_GIVE "�񑡂ł���A�C�e���������Ă��܂���B"
#define MES_STORE_NO_SELL "�~���������Ȃ��ł��˂��B"
#define MES_STORE_MUSEUM_SAME "����Ɠ����i���͊��ɔ����قɂ���悤�ł��B"
#define MES_STORE_MUSEUM_WARN "�����قɊ񑡂������͎̂��o�����Ƃ��ł��܂���I�I"
#define MES_STORE_MUSEUM_WARN2 "�W������߂������A�C�e���͓�x�ƌ��邱�Ƃ͂ł��܂���I"
#define MES_STORE_MUSEUM_ASK(OBJECT) "�{����%s���񑡂��܂����H", (OBJECT)
#define MES_STORE_MUSEUM_ASK2(OBJECT) "�{����%s�̓W������߂����܂����H", (OBJECT)
#define MES_STORE_FIXED_PRICE(PRICE) "��ɂ� $%ld�ł��B", (PRICE)
#define MES_STORE_BUYING(NAME, SYM) "%s(%c)���w������B", (NAME), (SYM)
#define MES_STORE_SELLING(NAME, SYM) "%s(%c)�𔄋p����B", (NAME), (SYM)
#define MES_STORE_BOUGHT(NAME, PRICE) "%s�� $%ld�ōw�����܂����B", (NAME), (PRICE)
#define MES_STORE_SELL(NAME, PRICE) "%s�� $%ld�Ŕ��p���܂����B", (NAME), (PRICE)
#define MES_STORE_NO_FIT "���Ȃ��̑̊i�ɍ���Ȃ��悤�����A��낵�����H"
#define MES_STORE_NEW_STOCK "�X��͐V���ȍ݌ɂ����o�����B"
#define MES_STORE_PURCHASED(NAME, SYM) "%s(%c)����ɓ��ꂽ�B", (NAME), (SYM)
#define MES_STORE_TOOK(NAME, SYM) "%s(%c)�����o�����B", (NAME), (SYM)
#define MES_STORE_DROP(NAME, SYM) "%s��u�����B(%c)", (NAME), (SYM)
#define MES_STORE_REMOVE(NAME) "%s�̓W������߂������B", (NAME)
#define MES_STORE_COMMAND_LIMIT "���̃R�}���h�͓X�̒��ł͎g���܂���B"
#define MES_STORE_COMMAND_LIST1 " -)�O�y�[�W"
#define MES_STORE_COMMAND_LIST2 " �X�y�[�X) ���y�[�W"
#define MES_STORE_COMMAND_LIST3 " g) �A�C�e�������"
#define MES_STORE_COMMAND_LIST4 " d) �A�C�e����u��"
#define MES_STORE_COMMAND_LIST5 " x) �Ƃ̃A�C�e���𒲂ׂ�"
#define MES_STORE_COMMAND_LIST6 " r) �A�C�e���̓W������߂�"
#define MES_STORE_COMMAND_LIST7 " p) ���i�𔃂�"
#define MES_STORE_COMMAND_LIST8 " s) �A�C�e���𔄂�"
#define MES_STORE_COMMAND_LIST9 " x) ���i�𒲂ׂ�"
#define MES_STORE_COMMAND_LIST10 " l) �X��̗l�q���f��"
#define MES_STORE_COMMAND_LIST11 " i/e) ������/�����̈ꗗ"
#define MES_STORE_COMMAND_LIST12 " w/T) ��������/�͂���"
#define MES_STORE_COMMAND_LIST13 " w/t) ��������/�͂���"
#define MES_STORE_COMMAND_LIST14 " ESC) �X���o��"
#define MES_STORE_OVERFLOW1 "�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂĔ����ق���o��..."
#define MES_STORE_OVERFLOW2 "�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂēX����o��..."
#define MES_STORE_OVERFLOW3 "�U�b�N����A�C�e�������ӂꂻ���Ȃ̂ŁA����ĂĉƂ���o��..."
#define MES_STORE_DROP_ITEM(NAME, SYM) "%s���������B(%c)", (NAME), (SYM)
#define MES_STORE_PAGE(NUM) "(%d�y�[�W)  ", (NUM)
#define MES_STORE_NUM(CUR, MAX) "�A�C�e����:  %4d/%4d", (CUR), (MAX)
#define MES_STORE_ITEM_DESCRIPTION "�A�C�e���̈ꗗ"
#define MES_STORE_ITEM_DESCRIPTION "�A�C�e���̈ꗗ"
#else
#define MES_STORE_RUMOR "The shopkeeper whispers something into your ear:"
#define MES_STORE_DECIDE_INSTANTLY "You instantly agree upon the price."
#define MES_STORE_DECIDE_FINALY "You eventually agree upon the price."
#define MES_STORE_DECIDE_QUICKLY "You quickly agree upon the price."
#define MES_STORE_LAST_BID(PRICE) "Your last bid %ld", (PRICE)
#define MES_STORE_ASK_PRICE "What price do you ask? "
#define MES_STORE_ENTIRE "That's all."
#define MES_STORE_ITEM_LIMIT "You cannot carry that many different items."
#define MES_STORE_ITEM_LIMIT2 "You cannot carry that many items."
#define MES_STORE_MUSEUM "Museum."
#define MES_STORE_NO_ITEM_HOME "Your home is empty."
#define MES_STORE_NO_ITEM_MUSEUM "Museum is empty."
#define MES_STORE_NO_ITEM_STORE "I am currently out of stock."
#define MES_STORE_WHICH_EXAMINE "Which item do you want to examine? "
#define MES_STORE_FULL_ITEM_HOME "Your home is full."
#define MES_STORE_FULL_ITEM_MUSEUM "Museum is full."
#define MES_STORE_FULL_ITEM_STORE "I have not the room in my store to keep it."
#define MES_STORE_WHICH_TAKING "Which item do you want to take? "
#define MES_STORE_WHICH_PURCHASE "Which item are you interested in? "
#define MES_STORE_WHICH_DROP "Drop which item? "
#define MES_STORE_WHICH_GIVE "Give which item? "
#define MES_STORE_WHICH_SELL "Sell which item? "
#define MES_STORE_WHICH_REMOVE "Which item do you want to order to remove? "
#define MES_STORE_NO_DROP "You don't have any item to drop."
#define MES_STORE_NO_GIVE "You don't have any item to give."
#define MES_STORE_NO_SELL "You have nothing that I want."
#define MES_STORE_MUSEUM_SAME "The same object as it is already in the Museum."
#define MES_STORE_MUSEUM_WARN "You cannot take items which is given to the Museum back!!"
#define MES_STORE_MUSEUM_WARN2 "You cannot see items which is removed from the Museum!"
#define MES_STORE_MUSEUM_ASK(OBJECT) "Really give %s to the Museum? ", (OBJECT)
#define MES_STORE_MUSEUM_ASK2(OBJECT) "Really order to remove %s from the Museum? ", (OBJECT)
#define MES_STORE_FIXED_PRICE(PRICE) "That costs %ld gold per item.", (PRICE)
#define MES_STORE_BUYING(NAME, SYM) "Buying %s(%c).", (NAME), (SYM)
#define MES_STORE_SELLING(NAME, SYM) "Selling %s (%c).", (NAME), (SYM)
#define MES_STORE_BOUGHT(NAME, PRICE) "You bought %s for %ld gold.", (NAME), (PRICE)
#define MES_STORE_SELL(NAME, PRICE) "You sold %s for %ld gold.", (NAME), (PRICE)
#define MES_STORE_NO_FIT "It may be not your size, Are you sure?"
#define MES_STORE_NEW_STOCK "The shopkeeper brings out some new stock."
#define MES_STORE_PURCHASED(NAME, SYM) "You have %s (%c).", (NAME), (SYM)
#define MES_STORE_TOOK(NAME, SYM) "You have %s (%c).", (NAME), (SYM)
#define MES_STORE_DROP(NAME, SYM) "You drop %s (%c).", (NAME), (SYM)
#define MES_STORE_REMOVE(NAME) "You ordered to remove %s.", (NAME)
#define MES_STORE_COMMAND_LIMIT "That command does not work in stores."
#define MES_STORE_COMMAND_LIST1 " -) Previous page"
#define MES_STORE_COMMAND_LIST2 " SPACE) Next page"
#define MES_STORE_COMMAND_LIST3 " g) Get an item."
#define MES_STORE_COMMAND_LIST4 " d) Drop an item."
#define MES_STORE_COMMAND_LIST5 " x) eXamine an item in the home."
#define MES_STORE_COMMAND_LIST6 " r) order to Remove an item."
#define MES_STORE_COMMAND_LIST7 " p) Purchase an item."
#define MES_STORE_COMMAND_LIST8 " s) Sell an item."
#define MES_STORE_COMMAND_LIST9 " x) eXamine an item in the shop"
#define MES_STORE_COMMAND_LIST10 " l) Lock at the owner"
#define MES_STORE_COMMAND_LIST11 " i/e) Inventry/Equipment list"
#define MES_STORE_COMMAND_LIST12 " w/T) Wear/Take off equipment"
#define MES_STORE_COMMAND_LIST13 " w/t) Wear/Take off equipment"
#define MES_STORE_COMMAND_LIST14 " ESC) Exit from Store."
#define MES_STORE_OVERFLOW1 "Your pack is so full that you flee the Museum..."
#define MES_STORE_OVERFLOW2 "Your pack is so full that you flee the store..."
#define MES_STORE_OVERFLOW3 "Your pack is so full that you flee your home..."
#define MES_STORE_DROP_ITEM(NAME, SYM) "You drop %s (%c).", (NAME), (SYM)
#define MES_STORE_PAGE(NUM) "(Page%d)  ", (NUM)
#define MES_STORE_NUM(CUR, MAX) "Objects:  %4d/%4d", (CUR), (MAX)
#define MES_STORE_ITEM_DESCRIPTION "Item Description"
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
#define MES_INTERFACE_QUANTITY(NUM) "�����ł��� (1-%d): ", (NUM)
#define MES_INTERFACE_DUMP "['h'�Ń��[�h�ύX, ESC�ŏI��]"
#define MES_INTERFACE_TO_SEE " '*'�ꗗ,"
#define MES_INTERFACE_EQUIP_AND_INVEN "'4' �����i, '6' ������,"
#define MES_INTERFACE_INVEN " '4'or'6' ������,"
#define MES_INTERFACE_EQUIP " '4'or'6' �����i,"
#define MES_INTERFACE_ENTER_SCROLL_DOWN " Enter ��,"
#define MES_INTERFACE_CTRL "CTRL: "
#define MES_INTERFACE_OPTION "<����>�ňړ�, Enter�Ō���, ESC�ŃL�����Z��, ?�Ńw���v: "
#define MES_INTERFACE_FLOOR_OBJECT " '-'����,"
#define MES_INTERFACE_TARGET_1 "q�~ t�� p�� o�� +�� -�O"
#define MES_INTERFACE_TARGET_2 "q�~ p�� o�� +�� -�O"
#define MES_INTERFACE_TARGET_3 "q�~ t�� p�� m�� +�� -�O"
#define MES_INTERFACE_TARGET_4 "���� ('*'�Ń^�[�Q�b�g�I��, ESC�Œ��f)? "
#define MES_INTERFACE_TARGET_5 "���� ('5'�Ń^�[�Q�b�g��, '*'�Ń^�[�Q�b�g�đI��, ESC�Œ��f)? "
#define MES_INTERFACE_RESERACH " ['r'�v���o, ' '�ő��s, ESC]"
#define MES_INTERFACE_RESERACH2 "�N���[�`���[�̕�������͂��ĉ�����(�L�� or ^A�S,^U��,^N��,^M���O):"
#define MES_INTERFACE_VISUAL "�R�}���h (n/N/^N/a/A/^A/c/C/^C/l/L/^L/d/D/^D/v/V/^V): "
#define MES_INTERFACE_SKILL_LIST "Lv   MP ���� ����"
#define MES_INTERFACE_SKILL_LIST2 "  Lv   MP"
#define MES_INTERFACE_SKILL_LIST3 "�n���x Lv   MP ���� ����"
#define MES_INTERFACE_SNIPE_LIST "Lv   �W���x"
#define MES_INTERFACE_RACIAL_LIST "                            Lv   MP ����"
#define MES_INTERFACE_FORCE "('w'���C�p, ESC) 'w'��ESC�������Ă��������B "
#define MES_INTERFACE_FORCE2 " 'w'���C�p,"
#define MES_INTERFACE_FOR_INVEN " '/' ������,"
#define MES_INTERFACE_FOR_EQUIP " '/' �����i,"
#define MES_INTERFACE_LOCATE(Y, X, DIR) "�}�b�v�ʒu [%d,%d] (�v���C���[��%s)  ����?"
#define MES_INTERFACE_EDIT_HISTORY "[ �J�[�\���L�[�ňړ��AEnter�ŏI���ACtrl-A�Ńt�@�C���ǂݍ��� ]"
#define MES_INTERFACE_CREATURE_DUMP1 ", 'h' ���̑��̏��"
#define MES_INTERFACE_CREATURE_DUMP2 ", Enter ���̐��l�Ɍ���"
#define MES_INTERFACE_CREATURE_DUMP3 "[ 'Q' ���f, 'S' ���߂���, Enter �Q�[���J�n ]"
#define MES_INTERFACE_VIEW_MAP "�����L�[�������Ă�������('M':�E�� 'N':���u 'D':M+N 'K':�󂷃A�C�e����\��)"
#define MES_INTERFACE_DUMP_VERSION(VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH) "[%s %d.%d.%d �L�����N�^���]\n\n", (VERSION_NAME), (VER_MAJOR), (VER_MINOR), (VER_PATCH)
#define MES_INTERFACE_LEARNED_BLUEMAGIC "\n\n  [�w�K�ς݂̐��@]\n"
#define MES_INTERFACE_EATEN_MAGIC "\n\n  [��荞�񂾖��@����]\n"
#define MES_INTERFACE_QUEST_INFO "\n\n  [�N�G�X�g���]\n"
#define MES_INTERFACE_LAST_MESSAGE "\n  [���ʒ��O�̃��b�Z�[�W]\n\n"
#define MES_INTERFACE_WINNING_MESSAGE "\n  [*����*���b�Z�[�W]\n\n"
#define MES_INTERFACE_RECALL_DEPTH "\n  [�A�ҏꏊ]\n\n"
#define MES_INTERFACE_RECALL_DEPTH_LIST(CONQUESTED, NAME, DEPTH) "   %c%-12s: %3d �K\n", (CONQUESTED) ? '!' : ' ', (NAME), (DEPTH)
#define MES_INTERFACE_DEFEATED_CREATURE "\n  [�|�����N���[�`���[]\n\n"
#define MES_INTERFACE_DEFEATED_SCORE(UNIQUE, TOTAL) "%ld�̂̃��j�[�N�E�N���[�`���[���܂ށA���v%ld�̂̓G��|���Ă��܂��B\n", (UNIQUE), (TOTAL)
#define MES_INTERFACE_DEFEATED_UNIQUE_TOP(NUM) "\n�s���%ld�̂̃��j�[�N�E�N���[�`���[�t\n", (NUM)
#define MES_INTERFACE_FIRST_RACE(CREATURE) "\n\n ���Ȃ���%s�Ƃ��Đ��܂ꂽ�B", desc_race_name((CREATURE), CD_SEX)
#define MES_INTERFACE_MUTATION "\n\n  [�ˑR�ψ�]\n\n"
#define MES_INTERFACE_CHECKSUM(SUM) "  [�`�F�b�N�T��: \"%s\"]\n\n", (SUM)
#else
#define MES_INTERFACE_QUANTITY(NUM) "Quantity (1-%d): ", (NUM)
#define MES_INTERFACE_DUMP "['h' to change mode, or ESC]"
#define MES_INTERFACE_TO_SEE " * to see,"
#define MES_INTERFACE_EQUIP_AND_INVEN " 4 for Equip, 6 for Inven,"
#define MES_INTERFACE_INVEN " 4 or 6 for Inven,"
#define MES_INTERFACE_EQUIP " 4 or 6 for Equip,"
#define MES_INTERFACE_ENTER_SCROLL_DOWN " Enter for scroll down,"
#define MES_INTERFACE_CTRL "Control: "
#define MES_INTERFACE_OPTION "Move to <dir>, Select to Enter, Cancel to ESC, ? to help: "
#define MES_INTERFACE_FLOOR_OBJECT " - for floor,"
#define MES_INTERFACE_TARGET_1 "q,t,p,o,+,-,<dir>"
#define MES_INTERFACE_TARGET_2 "q,p,o,+,-,<dir>"
#define MES_INTERFACE_TARGET_3 "q,t,p,m,+,-,<dir>"
#define MES_INTERFACE_TARGET_4 "Direction ('*' to choose a target, Escape to cancel)? "
#define MES_INTERFACE_TARGET_5 "Direction ('5' for target, '*' to re-target, Escape to cancel)? "
#define MES_INTERFACE_RESERACH " [(r)ecall, ESC, space to continue]"
#define MES_INTERFACE_RESERACH2 "Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): "
#define MES_INTERFACE_VISUAL "Command (n/N/^N/a/A/^A/c/C/^C/l/L/^L/d/D/^D/v/V/^V): "
#define MES_INTERFACE_SKILL_LIST "Lv Mana Fail Info"
#define MES_INTERFACE_SKILL_LIST2 "  Lv Mana"
#define MES_INTERFACE_SKILL_LIST3 "Profic Lv   SP Fail Effect"
#define MES_INTERFACE_SNIPE_LIST "Lv Pow"
#define MES_INTERFACE_RACIAL_LIST "                            Lv Cost Fail"
#define MES_INTERFACE_FORCE "(w for the Force, ESC) Hit 'w' or ESC. "
#define MES_INTERFACE_FORCE2 " w for the Force,"
#define MES_INTERFACE_FOR_INVEN " / for Inven,"
#define MES_INTERFACE_FOR_EQUIP " / for Equip,"
#define MES_INTERFACE_LOCATE(Y, X, DIR) "Map sector [%d,%d], which is %s your sector.  Direction?"
#define MES_INTERFACE_EDIT_HISTORY "[ Cursor key for Move, Enter for End, Ctrl-A for Read pref ]"
#define MES_INTERFACE_CREATURE_DUMP1 ", 'h' Misc."
#define MES_INTERFACE_CREATURE_DUMP2 ", or Enter to accept"
#define MES_INTERFACE_CREATURE_DUMP3 "['Q'uit, 'S'tart over, or Enter to continue]"
#define MES_INTERFACE_VIEW_MAP " Hit M, N(for ~), K(for !), or D(same as M+N) to display auto-picker items."
#define MES_INTERFACE_DUMP_VERSION(VERSION_NAME, VER_MAJOR, VER_MINOR, VER_PATCH) "  [%s %d.%d.%d Character Dump]\n\n", (VERSION_NAME), (VER_MAJOR), (VER_MINOR), (VER_PATCH)
#define MES_INTERFACE_LEARNED_BLUEMAGIC "\n\n  [Learned Blue Magic]\n"
#define MES_INTERFACE_EATEN_MAGIC "\n\n  [Magic devices eaten]\n"
#define MES_INTERFACE_QUEST_INFO "\n\n  [Quest Information]\n"
#define MES_INTERFACE_LAST_MESSAGE "\n  [Last Messages]\n\n"
#define MES_INTERFACE_WINNING_MESSAGE "\n  [*Winning* Message]\n\n"
#define MES_INTERFACE_RECALL_DEPTH "\n  [Recall Depth]\n\n"
#define MES_INTERFACE_RECALL_DEPTH_LIST(CONQUESTED, NAME, DEPTH) "   %c%-16s: level %3d\n", (CONQUESTED) ? '!' : ' ', (NAME), (DEPTH)
#define MES_INTERFACE_DEFEATED_CREATURE "\n  [Defeated Creatures]\n\n"
#define MES_INTERFACE_DEFEATED_SCORE(UNIQUE, TOTAL) "You have defeated %ld %s including %ld unique creature%s in total.\n", (TOTAL), (TOTAL) == 1 ? "enemy" : "enemies",(UNIQUE), ((UNIQUE) == 1 ? "" : "s"))
#define MES_INTERFACE_DEFEATED_UNIQUE_TOP(NUM) "\n< Unique creatures top %ld >\n", (NUM)
#define MES_INTERFACE_FIRST_RACE(CREATURE) "\n\n You were born as %s.", desc_race_name((CREATURE), CD_SEX)
#define MES_INTERFACE_MUTATION "\n\n  [Mutation]\n\n"
#define MES_INTERFACE_CHECKSUM(SUM) "  [Check Sum: \"%s\"]\n\n", (SUM)
#endif

#ifdef JP
#define MES_MELEE_ATTACK "%s��%s��%s�ōU�������B"
#define MES_MELEE_ATTACK_MISS "%s��%s�̍U�������킵���B"
#define MES_MELEE_AMBUSH(ATTACKER, TARGET) ("%s�͗⍓�ɂ������Ă��閳�͂�%s��˂��h�����I", (ATTACKER), (TARGET))
#define MES_MELEE_FATAL_SPOT(ATTACKER, TARGET) ("%s�͕s�ӂ�˂���%s�ɋ���Ȉꌂ�����킹���I", (ATTACKER), (TARGET))
#define MES_MELEE_BACKSTUB(ATTACKER, TARGET) ("%s�͓�����%s��w������˂��h�����I", (ATTACKER), (TARGET))
#define MES_MELEE_ZANTETSU_JELLY "����ȓ�炩�����̂͐؂��I"
#define MES_MELEE_ZANTETSU_SERIF "�܂��܂�ʂ��̂��a���Ă��܂����D�D�D"
#define MES_MELEE_EX_JR_SPIDER "�w偂͋�肾�I"
#define MES_MELEE_VORPAL_BLADE_SERIF "�ڂɂ��~�܂�ʃ��H�[�p���u���[�h�A��B�̑��ƁI"
#define MES_MELEE_ATTACK_DRAIN(WEAPON, TARGET) ("%s��%s���琶���͂��z��������I", (WEAPON), (TARGET))
#define MES_MELEE_SUCK_BLOOD(WEAPON) ("%s�͌����z���ċ����Ȃ����I", WEAPON)
#define MES_MELEE_RETURN(WEAPON, ATTACKER) ("�U��񂵂�%s��%s�ɕԂ��Ă����B", (WEAPON), (ATTACKER))
#define MES_MELEE_TO_EMPTY "�����Ȃ��󒆂��U�������B"
#define MES_MELEE_DISABLE(ATTACKER) "%s�͍U���ł��Ȃ��B", (ATTACKER)
#define MES_MELEE_NO_METHOD(ATTACKER) "%s�͍U�������i�������Ă��Ȃ��B", (ATTACKER)
#define MES_MELEE_PROTECTED(ATTACKER) "%^s�͌��ނ��ꂽ�B", (ATTACKER)
#define MES_MELEE_COUNTER(ATTACKER) "%^s�ɔ��������I", (ATTACKER)
#define MES_MELEE_ZANTETSU_CANCEL(ATTACKER) "%s�͎v�킸���񂾁B�u�َҁA���Ȃ��͎a��ʁI�v", (ATTACKER)
#define MES_MELEE_THIEF "�D�_�͏΂��ē������I"
#define MES_MELEE_THIEF_FAILED "�D�_�͏΂��ē���...�悤�Ƃ������o���A�ɖh���ꂽ�B"
#define MES_MELEE_NO_MELEE_DUNGEON "�Ȃ����U�����邱�Ƃ��ł��Ȃ��B"
#define MES_MELEE_FORCED_STORMBRINGER(WEAPON, TARGET) "%s�͋��~��%s���U�������I", (WEAPON), (TARGET)
#define MES_MELEE_ASK "�{���ɍU�����܂����H"
#define MES_MELEE_CEASE(TARGET) "%s���U������̂��~�߂��B", (TARGET)
#define MES_MELEE_FIRE_AURA "�ˑR�ƂĂ��M���Ȃ����I"
#define MES_MELEE_COLD_AURA "�ˑR�ƂĂ������Ȃ����I"
#define MES_MELEE_ELEC_AURA "�d������������I"
#define MES_MELEE_RESELECT "�U�����đI�����܂����B"
#define MES_MELEE_EYE_EYE(ATTACKER) "�U����%s���g���������I", (ATTACKER)->name
#define MES_MELEE_JOJ_DONE "�R���̕�΂͐Ԃ����邭�������I"
#define MES_MELEE_JOJ_STRAIN "��΂͂��Ȃ��̗̑͂�D����..."
#else
#define MES_MELEE_ATTACK_MISS %^s misses %s.
#define MES_MELEE_AMBUSH(ATTACKER, TARGET) "%s (cruelly stab the helpless, sleeping %s!", (ATTACKER), (TARGET))
#define MES_MELEE_FATAL_SPOT(ATTACKER, TARGET) ("%s make surprise attack, and hit %s with a powerful blow!", (ATTACKER), (TARGET))
#define MES_MELEE_BACKSTUB(ATTACKER, TARGET) ("%s ambush the fleeing %s.", (ATTACKER), (TARGET))
#define MES_MELEE_ZANTETSU_JELLY "You cannot cut such a elastic thing!"
#define MES_MELEE_ZANTETSU_SERIF "Sigh... Another trifling thing I've cut...."
#define MES_MELEE_EX_JR_SPIDER "Spiders are difficult for you to deal with!"
#define MES_MELEE_VORPAL_BLADE_SERIF "Your Vorpal Blade goes snicker-snack!"
#define MES_MELEE_SUCK_BLOOD(WEAPON) ("%s sucked blood, and became more powerful!", (WEAPON))
#define MES_MELEE_RETURN(WEAPON, ATTACKER) ("%s returns to %s.", ATTACKER, WEAPON)
#define MES_MELEE_TO_EMPTY "You attack the empty air."
#define MES_MELEE_DISABLE(ATTACKER) "%s don't have attack method.", (ATTACKER)
#define MES_MELEE_NO_METHOD(ATTACKER) "%s don't have attack method.", (ATTACKER)
#define MES_MELEE_PROTECTED(ATTACKER) "%^s is repelled.", (ATTACKER)
#define MES_MELEE_COUNTER(ATTACKER) "Your counterattack to %s!", (ATTACKER)
#define MES_MELEE_ZANTETSU_CANCEL(ATTACKER) "%s shouted, \"I can not attack women!\"", (ATTACKER)
#define MES_MELEE_THIEF "The thief flees laughing!"
#define MES_MELEE_THIEF_FAILED "The thief flees laughing...? But magic barrier obstructs it."
#define MES_MELEE_NO_MELEE_DUNGEON "Something prevent you from attacking."
#define MES_MELEE_FORCED_STORMBRINGER(WEAPON, TARGET) "%s greedily attacks %s!", (WEAPON), (TARGET)
#define MES_MELEE_ASK "Really hit it? "
#define MES_MELEE_CEASE(TARGET) "You stop to avoid hitting %s.", (TARGET)
#define MES_MELEE_FIRE_AURA "You are suddenly very hot!"
#define MES_MELEE_COLD_AURA "You are suddenly very cold!"
#define MES_MELEE_ELEC_AURA "You get zapped!"
#define MES_MELEE_RESELECT "Attack re-selected."
#define MES_MELEE_EYE_EYE(ATTACKER) "The attack of %s has wounded %s!", (ATTACKER)->name, (ATTACKER)->name /*TODO*/
#define MES_MELEE_JOJ_DONE "The Jewel flashes bright red!"
#define MES_MELEE_JOJ_STRAIN "The Jewel drains your vitality..."
#endif

#ifdef JP
#define MES_SHOOT_ARTIFACT_ARROW(ARROW, TARGET) "%s��%s�ɓ˂��h�������I", (ARROW)->name, (TARGET)->name
#define MES_SHOOT_MARK(ARROW) "%s���G��ߑ������B", (ARROW)
#define MES_SHOOT_ROCK_SHATTER "�₪�ӂ��U�����B"
#else
#define MES_SHOOT_ARTIFACT_ARROW(ARROW, TARGET) "%^s have stuck into %s!", (ARROW)->name, (TARGET)->name
#define MES_SHOOT_MARK(ARROW) "The %s finds a mark.", (ARROW)
#define MES_SHOOT_ROCK_SHATTER "Wall rocks were shattered."
#endif

#ifdef JP
#define MES_THROW_FIGURE_FAILED "�l�`�͔P���Ȃ���ӂ��U���Ă��܂����I"
#define MES_THROW_FIGURE_CURSED "����͂��܂�ǂ��Ȃ��C������B"
#define MES_THROW_SHATTER(OBJECT) "%s�͍ӂ��U�����I", (OBJECT)->name
#define MES_THROW_BACK(OBJECT) "%s���茳�ɕԂ��Ă����B", (OBJECT)->name
#define MES_THROW_NO_BACK(OBJECT) "%s���Ԃ��Ă��Ȃ������I", (OBJECT)->name
#define MES_THROW_BACK_FAILED(OBJECT) "%s���󂯑��˂��I", (OBJECT)->name
#define MES_THROW_BACK_GONE(OBJECT) "%s�͂ǂ����֍s�����B", (OBJECT)->name
#else
#define MES_THROW_FIGURE_FAILED "The Figurine writhes and then shatters."
#define MES_THROW_FIGURE_CURSED "You have a bad feeling about this."
#define MES_THROW_SHATTER(OBJECT) "The %s shatters!", (OBJECT)->name
#define MES_THROW_BACK(OBJECT) "%s comes back to you.", (OBJECT)->name
#define MES_THROW_NO_BACK(OBJECT) , (OBJECT)->name
#define MES_THROW_BACK_FAILED(OBJECT) "%s backs, but you can't catch!", (OBJECT)->name
#define MES_THROW_BACK_GONE(OBJECT) "The %s have gone to somewhere.", (OBJECT)->name
#endif

#ifdef JP
#define MES_OPEN_NO_TARGET "�����ɂ͊J������̂���������Ȃ��B"
#else
#define MES_OPEN_NO_TARGET "You see nothing there to open."
#endif

#ifdef JP
#define MES_STUDY_WHICH_BOOK "�ǂ̖{����w�т܂���? "
#define MES_STUDY_NO_BOOK "�ǂ߂�{���Ȃ��B"
#define MES_STUDY_NO_READING "�{��ǂނ��Ƃ��ł��Ȃ��I"
#define MES_STUDY_NO_SLOT(REALM) "�V����%s���o���邱�Ƃ͂ł��Ȃ��I", (REALM)
#define MES_STUDY_SLOT(NUM, REALM) "���� %d ���%s���w�ׂ�B", (NUM), (REALM)
#define MES_STUDY_NO_SPELL(REALM) "���̖{�ɂ͊w�Ԃׂ�%s���Ȃ��B", (REALM)
#define MES_STUDY_PERFECT_LEARNED(REALM) "����%s�͊��S�Ɏg�����Ȃ���̂Ŋw�ԕK�v�͂Ȃ��B", (REALM)
#define MES_SPELL_CHANGE_REALM_ASK "�{���ɖ��@�̗̈��ύX���܂����H"
#define MES_SPELL_FAILED "���������܂��������Ȃ������I"
#define MES_SPELL_CANNOT "�������������Ȃ��I"
#define MES_SPELL_NO_MORE_SPELLING "����ȏ�V�����������r�����邱�Ƃ͂ł��Ȃ��B"
#define MES_SPELL_ALREADY "���̎����͂��łɉr�������B"
#define MES_SPELL_FORGET(REALM, SPELL) "%s��%s��Y��Ă��܂����B", (REALM), (SPELL)
#define MES_SPELL_REMEMBER(REALM, SPELL) "%s��%s���v���o�����B", (REALM), (SPELL)
#else
#define MES_STUDY_NO_READING "You cannot read books!"
#define MES_STUDY_NO_SLOT(REALM) "You cannot learn any new %ss!", (REALM)
#define MES_STUDY_SLOT(REALM, NUM) "You can learn %d new %s%s.", (REALM), (NUM), ((NUM) == 1 ? "":"s")
#define MES_STUDY_NO_SPELL(REALM) "You cannot learn any %ss in that book.", (REALM)
#define MES_STUDY_PERFECT_LEARNED(REALM) "You don't need to study this %s anymore.", (REALM)
#define MES_SPELL_CHANGE_REALM_ASK "Really, change magic realm? "
#define MES_SPELL_FAILED "You failed to get the magic off!"
#define MES_SPELL_CANNOT "You cannot cast spells!"
#define MES_SPELL_NO_MORE_SPELLING "Can not spell new spells more."
#define MES_SPELL_ALREADY "You are already casting it."
#define MES_SPELL_FORGET(REALM, SPELL) "You have forgotten the %s of %s.", (SPELL), (REALM)
#define MES_SPELL_REMEMBER(REALM, SPELL) "You have remembered the %s of %s.", (SPELL), (REALM)
#endif

#ifdef JP
#define MES_RIDING_NO_DIRECTION "������ɂ͍~����܂���B"
#define MES_RIDING_NO_CREATURE "���̏ꏊ�ɂ̓N���[�`���[�͂��܂���B"
#define MES_RIDING_NOT_PET "���̃N���[�`���[�̓y�b�g�ł͂���܂���B"
#define MES_RIDING_DISABLE "���̃N���[�`���[�ɂ͏��Ȃ��������B"
#else
#define MES_RIDING_NO_DIRECTION "You cannot go to that direction."
#define MES_RIDING_NO_CREATURE "Here is no creature."
#define MES_RIDING_NOT_PET "That creature is not a pet."
#define MES_RIDING_DISABLE "This creature doesn't seem suitable for riding."
#endif

#ifdef JP
#define MES_AUTOPICK_COMMANDKEY "C:%d:<�R�}���h�L�[>: "
#define MES_AUTOPICK_WHICH_ENTRY "�ǂ̃A�C�e����o�^���܂���"
#define MES_AUTOPICK_NO_ENTRY "�A�C�e���������Ă��Ȃ��B"
#define MES_AUTOPICK_WHICH_SEARCH "�ǂ̃A�C�e�����������܂���? "
#define MES_AUTOPICK_NO_SEARCH "�A�C�e���������Ă��Ȃ��B"
#define MES_AUTOPICK_LOAD_FILE "�����E���ݒ�t�@�C�������[�h���܂���? "
#define MES_AUTOPICK_AUTODUMP "\n# �����}�N���Z�[�u\n\n"
#define MES_AUTOPICK_PATTERN_NOT_FOUND(STR) "�p�^�[����������܂���: %s", (STR)
#define MES_AUTOPICK_SKIP_INACTIVE_LINE(STR) "������Ԃ̍s���X�L�b�v���܂����B(%s��������)", (STR)
#define MES_AUTOPICK_FOUND_ONLY_INACTIVE(STR) "������Ԃ̍s���������t����܂����B(%s��������)", (STR)
#define MES_AUTOPICK_NO_PATTERN_SEARCH "��������p�^�[��������܂���(^S �Ō���)�B"
#define MES_AUTOPICK_COMMENT "���̍s�̓R�����g�ł��B"
#define MES_AUTOPICK_CONDITIONAL_EXPRESSION "���̍s�͏������򎮂ł��B"
#define MES_AUTOPICK_DEFINE_MACRO_ACTION "���̍s�̓}�N���̎��s���e���`���܂��B"
#define MES_AUTOPICK_DEFINE_TRIGER_KEY "���̍s�̓}�N���̃g���K�[�E�L�[���`���܂��B"
#define MES_AUTOPICK_DEFINE_KEY_MAP "���̍s�̓L�[�z�u���`���܂��B"
#define MES_AUTOPICK_CUR_FALSE "���݂̎��̒l�́u�U(=0)�v�ł��B"
#define MES_AUTOPICK_CUR_TRUE "���݂̎��̒l�́u�^(=1)�v�ł��B"
#define MES_AUTOPICK_CUR_DELETE "���̍s�͌�ō폜����܂��B"
#define MES_AUTOPICK_CUR_BYPASS "���̍s�͌��݂͖����ȏ�Ԃł��B"
#define MES_AUTOPICK_DISCARD_QUIT "�S�Ă̕ύX��j�����Ă���I�����܂��B��낵���ł����H "
#define MES_AUTOPICK_DISCARD_REVERT "�S�Ă̕ύX��j�����Č��̏�Ԃɖ߂��܂��B��낵���ł����H "
#define MES_AUTOPICK_EDITOR "(^Q:�I�� ^W:�Z�[�u���ďI��, ESC:���j���[, ���̑�:����)"
#define MES_AUTOPICK_TRIGGER_KEY "P:<�g���K�[�L�[>: "
#else
#define MES_AUTOPICK_COMMANDKEY "<Keypress>"
#define MES_AUTOPICK_WHICH_ENTRY "Enter which item? "
#define MES_AUTOPICK_NO_ENTRY "You have nothing to enter."
#define MES_AUTOPICK_WHICH_SEARCH "Search which item? "
#define MES_AUTOPICK_NO_SEARCH "You have nothing to search."
#define MES_AUTOPICK_LOAD_FILE "Reload auto-pick preference file? "
#define MES_AUTOPICK_AUTODUMP "\n# Automatic macro dump\n\n"
#define MES_AUTOPICK_PATTERN_NOT_FOUND(STR) "Pattern not found: %s", (STR)
#define MES_AUTOPICK_SKIP_INACTIVE_LINE(STR) "Some inactive lines are skipped. (Searching %s)", (STR)
#define MES_AUTOPICK_FOUND_ONLY_INACTIVE(STR) "Found only an inactive line. (Searching %s)", (STR)
#define MES_AUTOPICK_NO_PATTERN_SEARCH "No pattern to search. (Press ^S to search.)"
#define MES_AUTOPICK_COMMENT "This line is a comment."
#define MES_AUTOPICK_CONDITIONAL_EXPRESSION "This line is a Conditional Expression."
#define MES_AUTOPICK_DEFINE_MACRO_ACTION "This line defines a Macro action."
#define MES_AUTOPICK_DEFINE_TRIGER_KEY "This line defines a Macro trigger key."
#define MES_AUTOPICK_DEFINE_KEY_MAP "This line defines a Keymap."
#define MES_AUTOPICK_CUR_FALSE "The expression is 'False'(=0) currently."
#define MES_AUTOPICK_CUR_TRUE "The expression is 'True'(=1) currently."
#define MES_AUTOPICK_CUR_DELETE "This line will be delete later."
#define MES_AUTOPICK_CUR_BYPASS "This line is bypassed currently."
#define MES_AUTOPICK_DISCARD_QUIT "Discard all changes and quit. Are you sure? "
#define MES_AUTOPICK_DISCARD_REVERT "Discard all changes and revert to original file. Are you sure? "
#define MES_AUTOPICK_EDITOR "(^Q:Quit, ^W:Save&Quit, ESC:Menu, Other:Input text)"
#define MES_AUTOPICK_TRIGGER_KEY "P:<Trigger key>: "
#endif

#ifdef JP
#define MES_CRITICAL_LEVEL1 "�育�������������I"
#define MES_CRITICAL_LEVEL2 "���Ȃ�̎育�������������I"
#define MES_CRITICAL_LEVEL3 "��S�̈ꌂ���I"
#define MES_CRITICAL_LEVEL4 "�ō��̉�S�̈ꌂ���I"
#define MES_CRITICAL_LEVEL5 "��ނȂ��ō��̉�S�̈ꌂ���I"
#else
#define MES_CRITICAL_LEVEL1 "It was a good hit!"
#define MES_CRITICAL_LEVEL2 "It was a great hit!"
#define MES_CRITICAL_LEVEL3 "It was a superb hit!"
#define MES_CRITICAL_LEVEL4 "It was a *GREAT* hit!"
#define MES_CRITICAL_LEVEL5 "It was a *SUPERB* hit!"
#endif

#ifdef JP
#define MES_RECHARGE_WHICH_ITEM "�ǂ̃A�C�e���ɖ��͂��[�U���܂���? "
#define MES_RECHARGE_NO_ITEM "���͂��[�U���ׂ��A�C�e�����Ȃ��B"
#define MES_RECHARGE_NO_NEED "����͍ď[�U����K�v�͂���܂���B"
#define MES_RECHARGE_NO_MONEY(NAME, MONEY) "%s���ď[�U����ɂ́�%d �K�v�ł��I", (NAME), (MONEY)
#define MES_RECHARGE_NO_MONEY_ALL(MONEY) "���ׂẴA�C�e�����ď[�U����ɂ́�%d �K�v�ł��I", (MONEY)
#define MES_RECHARGE_ALL_ASK(MONEY) "���ׂẴA�C�e���� ��%d �ōď[�U���܂����H", (MONEY)
#define MES_RECHARGED(NAME, MONEY, NUM) "%s����%d �ōď[�U���܂����B", (NAME), (MONEY), (NUM)
#define MES_RECHARGED_ALL(MONEY) "��%d �ōď[�U���܂����B", (MONEY)
#define MES_RECHARGE_FAILED(NAME) "���͂��t�������I%s�͊��S�ɖ��͂��������B", (NAME)
#define MES_RECHARGE_FAILED2 "���͂��t���˂��āA���b�h���炳��ɖ��͂��z������Ă��܂����I"
#define MES_RECHARGE_FAILED3(NAME) "%s�͔j����Ƃꂽ���A���͂��S�Ď���ꂽ�B", (NAME)
#define MES_RECHARGE_BROKEN1(NAME) "���\�Ȗ��@�̂��߂�%s����ꂽ�I", (NAME)
#define MES_RECHARGE_BROKEN2(NAME) "���\�Ȗ��@�̂��߂�%s����{��ꂽ�I", (NAME)
#define MES_RECHARGE_BROKEN3(NAME) "���\�Ȗ��@�̂��߂�%s���S�ĉ�ꂽ�I", (NAME)
#define MES_RECHARGE_COMMENT "�ď[�U�̔�p�̓A�C�e���̎�ނɂ��܂��B"
#define MES_RECHARGE_COUNT(MONEY) "��񕪁�%d �ŉ��񕪏[�U���܂����H", (MONEY)
#define MES_RECHARGE_ROD_PRICE(NUM, MONEY) "���b�h%d�{����%d �ōď[�U���܂����H", (NUM), (MONEY)
#define MES_RECHARGE_NEED_IDENTIFY "�[�U����O�ɊӒ肳��Ă���K�v������܂��B"
#define MES_RECHARGE_ASK_IDENTIFY(MONEY) "��%d�ŊӒ肵�܂����H ", (MONEY)
#define MES_RECHARGE_INDENTIFIED(OBJECT) "%s �ł��B", (OBJECT)->name
#else
#define MES_RECHARGE_WHICH_ITEM "Recharge which item? "
#define MES_RECHARGE_NO_ITEM "You have nothing to recharge."
#define MES_RECHARGE_NO_NEED "That doesn't need to be recharged."
#define MES_RECHARGE_NO_MONEY(NAME, MONEY) "You need %d gold to recharge %s!", (MONEY), (NAME)
#define MES_RECHARGE_NO_MONEY_ALL(MONEY) "You need %d gold to recharge all items!", (MONEY)
#define MES_RECHARGE_ALL_ASK(MONEY) "Recharge all items for %d gold? ", (MONEY)
#define MES_RECHARGED(NAME, MONEY, NUM) "%^s %s recharged for %d gold.", (NAME), ((NUM)> 1) ? "were" : "was"), (MONEY) 
#define MES_RECHARGED_ALL(MONEY) "You pay %d gold.", (MONEY)
#define MES_RECHARGE_FAILED(NAME) "The recharging backfires - %s is completely drained!", (NAME)
#define MES_RECHARGE_FAILED2 "The recharge backfires, draining the rod further!"
#define MES_RECHARGE_FAILED3(NAME) "You save your %s from destruction, but all charges are lost.", (NAME)
#define MES_RECHARGE_BROKEN1(NAME) "Wild magic consumes your %s!", (NAME)
#define MES_RECHARGE_BROKEN2(NAME) "Wild magic consumes one of your %s!", (NAME)
#define MES_RECHARGE_BROKEN3(NAME) "Wild magic consumes all your %s!", (NAME)
#define MES_RECHARGE_COMMENT "The prices of recharge depend on the type."
#define MES_RECHARGE_COUNT(MONEY) "Add how many charges for %d gold? ", (MONEY)
#define MES_RECHARGE_ROD_PRICE(NUM, MONEY) "Recharge the %s for %d gold? ", (((NUM) > 1) ? "rods" : "rod"), (MONEY)), 
#define MES_RECHARGE_NEED_IDENTIFY "The item must be identified first�B"
#define MES_RECHARGE_ASK_IDENTIFY(MONEY) "Identify for %d gold? ", (MONEY)
#define MES_RECHARGE_INDENTIFIED(OBJECT) "You have: %s.", (OBJECT)->name
#endif

#ifdef JP
#define MES_WEP_BLESS_WHICH_OBJECT "�ǂ̃A�C�e�����j�����܂����H"
#define MES_WEP_BLESS_NO_OBJECT "�j���ł��镐�킪����܂���B"
#define MES_WEP_BLESS_CURSED_CANCEL(NAME, ID) "%s�𕢂������I�[���͏j���𒵂˕Ԃ����I", (NAME)
#define MES_WEP_BLESS_VANISH_CURSE(NAME, ID) "%s ����׈��ȃI�[�����������B", (NAME)
#define MES_WEP_BLESS_RESIST "���̕���͏j���������Ă���I"
#define MES_WEP_BLESS_FAILED "���͂��}�f�ȕ��͋C�Ŗ�����..."
#else
#define MES_WEP_BLESS_WHICH_OBJECT "Bless which weapon? "
#define MES_WEP_BLESS_NO_OBJECT "You have weapon to bless."
#define MES_WEP_BLESS_CURSED_CANCEL(NAME, ID) "The black aura on %s %s disrupts the blessing!", (((ID) >= 0) ? "your" : "the"), (NAME)
#define MES_WEP_BLESS_VANISH_CURSE(NAME, ID) "A malignant aura leaves %s %s.", (((ID) >= 0) ? "your" : "the"), (NAME)
#define MES_WEP_BLESS_RESIST "The weapon resists your blessing!"
#define MES_WEP_BLESS_FAILED "There is a static feeling in the air..."
#endif

#ifdef JP
#define MES_INSANITY_NIGHTMARE_PRE "���낵�����i���S���悬�����B"
#define MES_INSANITY_NIGHTMARE(ELDRITCH) "���̒���%s�ɒǂ�������ꂽ�B", (ELDRITCH)
#define MES_INSANITY_FACED(ADJ, ELDRITCH) "%s%s�̊�����Ă��܂����I", (ADJ), (ELDRITCH)
#define MES_INSANITY_GLANCE(ADJ, ELDRITCH) "%s%s�̊炪�_�Ԍ������B", (ADJ), (ELDRITCH)
#define MES_INSANITY_AMNESIA "���܂�̋��|�ɑS�Ă̂��Ƃ�Y��Ă��܂����I"

#else
#define MES_INSANITY_NIGHTMARE_PRE "Horrible visions flit through your mind."
#define MES_INSANITY_NIGHTMARE(ELDRITCH) "%^s chases you through your dreams.", (ELDRITCH)
#define MES_INSANITY_FACED(ADJ, ELDRITCH) "You behold the %s visage of %s.", (ADJ), (ELDRITCH)
#define MES_INSANITY_GLANCE(ADJ, ELDRITCH) "You glance at the %s visage of %s.", (ADJ), (ELDRITCH)
#define MES_INSANITY_AMNESIA "You forget everything in your utmost terror!"
#endif

#ifdef JP
#define MES_WALK_NEVER_MOVE "�����Ȃ��B"
#define MES_WALK_NEVER_SWIM "�j���Ȃ��B"
#define MES_WALK_NEVER_LAND "���ɏオ��Ȃ��B"
#define MES_WALK_STEED_FEAR(STEED) "%s�����|���Ă��Đ���ł��Ȃ��B", (STEED)
#define MES_WALK_STEED_STUNED(STEED) "%s���N�O�Ƃ��Ă��Ă��܂������Ȃ��I", (STEED)
#define MES_WALK_TOO_HOT(FEATURE) "�M����%s�̏�ɍs���Ȃ��B", (FEATURE)
#define MES_WALK_MUST_FLY(FEATURE) "����΂Ȃ���%s�̏�ɂ͍s���Ȃ��B", (FEATURE)
#define MES_WALK_BLOCK(FEATURE) "%s���s������͂΂�ł���B", (FEATURE)
#define MES_WALK_ASK_ENTERING_CHAOS "�{���Ɂq���ׁr�̗̈�ɓ���܂����H"
#define MES_WALK_PUSH_PAST(TARGET) "%s�������ނ����B", (TARGET)
#define MES_WALK_CANNOT_RUN "���̕����ɂ͑���܂���B"
#define MES_WALK_NO_ROUTE "���؂�������܂���I"
#else
#define MES_WALK_NEVER_MOVE "Can't move."
#define MES_WALK_NEVER_SWIM "Can't swim"
#define MES_WALK_NEVER_LAND "Can't land"
#define MES_WALK_STEED_FEAR(STEED) "%^s is too scared to control.", (STEED)
#define MES_WALK_STEED_STUNED(STEED) "You cannot control stunned %s!", (STEED)
#define MES_WALK_TOO_HOT(FEATURE) "Too hot to go through %s.", (FEATURE)
#define MES_WALK_MUST_FLY(FEATURE) "You need to fly to go through the %s.", (FEATURE)
#define MES_WALK_BLOCK(FEATURE) "There is %s %s blocking your way.", is_a_vowel((FEATURE)[0]) ? "an" : "a", (FEATURE)
#define MES_WALK_ASK_ENTERING_CHAOS "Really want to enter territory of chaos? "
#define MES_WALK_PUSH_PAST(TARGET) "You push past %s.", (TARGET)
#define MES_WALK_CANNOT_RUN "You cannot run in that direction."
#define MES_WALK_NO_ROUTE "No route is found!"
#endif

#ifdef JP
#define MES_OBJECT_WHICH_IMPROVE "�ǂ̃A�C�e�������ǂ��܂����H"
#define MES_OBJECT_NO_IMPROVE "���ǂł�����̂�����܂���B"
#define MES_OBJECT_FOUND(NUM) "%d��̃A�C�e��������B", (NUM)
#define MES_OBJECT_WHICH_USE "�ǂ���g���܂����H"
#define MES_OBJECT_NO_USE "�g������̂�����܂���B"
#define MES_OBJECT_WHICH_DROP "�ǂ̃A�C�e���𗎂Ƃ��܂���? "
#define MES_OBJECT_NO_DROP "���Ƃ���A�C�e���������Ă��Ȃ��B"
#define MES_OBJECT_WHICH_OBSERVE "�ǂ̃A�C�e���𒲂ׂ܂���? "
#define MES_OBJECT_NO_OBSERVE "���ׂ���A�C�e�����Ȃ��B"
#define MES_OBJECT_WHICH_INDEN "�ǂ̃A�C�e�����Ӓ肵�܂���? "
#define MES_OBJECT_WHICH_ALL_INDEN "���ׂĊӒ�ς݂ł��B "
#define MES_OBJECT_NO_INDEN "�Ӓ肷��ׂ��A�C�e�����Ȃ��B"
#define MES_OBJECT_WHICH_INDEN_FULLY "�ǂ̃A�C�e����*�Ӓ�*���܂���? "
#define MES_OBJECT_WHICH_ALL_INDEN_FULLY "���ׂ�*�Ӓ�*�ς݂ł��B "
#define MES_OBJECT_NO_INDEN_FULLY "*�Ӓ�*����ׂ��A�C�e�����Ȃ��B"
#define MES_OBJECT_NO_INDENTIFED "���̃A�C�e���ɂ��ē��ɒm���Ă��邱�Ƃ͂Ȃ��B"
#define MES_OBJECT_EXAMING(OBJECT) "%s�𒲂ׂĂ���..." ,(OBJECT)
#define MES_OBJECT_NO_SPECIAL "���ɕς�����Ƃ���͂Ȃ��悤���B"
#define MES_OBJECT_DROP_FEET "�����������ɓ]�����Ă����B"
#define MES_OBJECT_CHARGE_LEFT(NUM) "���� %d �񕪂̖��͂��c���Ă���B", (NUM)
#define MES_OBJECT_CHARGE_LEFT2(NUM) "���̏���̃A�C�e���́A���� %d �񕪂̖��͂��c���Ă���B", (NUM)
#define MES_OBJECT_NO_CHARGE_LEFT "�������͂��c���Ă��Ȃ��B"
#define MES_OBJECT_WHICH_REFILL_LAMP "�ǂ̖��ڂ��璍���܂���? "
#define MES_OBJECT_NO_REFILL_LAMP "���ڂ��Ȃ��B"
#define MES_OBJECT_WHICH_TORCH "�ǂ̏����Ŗ���������߂܂���? "
#define MES_OBJECT_NO_TORCH "���ɏ������Ȃ��B"
#define MES_OBJECT_DROPPED(NAME, SYM) "%s(%c)�𗎂Ƃ����B", (NAME), (SYM)
#define MES_OBJECT_INDETIFY_ALL_ASK "��������S�ĊӒ肵�Ă�낵���ł����H"
#define MES_OBJECT_INDETIFIED_ALL "�������S�Ă��Ӓ肳��܂����B"
#define MES_OBJECT_PICKED(CREATURE, OBJECT) "%^s��%s���E�����B", (CREATURE), (OBJECT)
#define MES_OBJECT_PICK_FAILED(CREATURE, OBJECT) "%^s��%s���E�����Ƃ������A���߂������B", (CREATURE), (OBJECT)
#define MES_OBJECT_DESTORY(CREATURE, OBJECT) "%^s��%s��j�󂵂��B", (CREATURE), (OBJECT)
#define MES_OBJECT_WHICH_PULISH "�ǂ̏��𖁂��܂����H"
#define MES_OBJECT_NO_PULISH "������������܂���B"
#define MES_OBJECT_PULISHED(OBJECT) "%s�͋P�����I", (OBJECT)->name
#else
#define MES_OBJECT_WHICH_IMPROVE "Improve which item? "
#define MES_OBJECT_NO_IMPROVE "You have nothing to improve."
#define MES_OBJECT_FOUND(NUM) "You see %d items.", (NUM)
#define MES_OBJECT_WHICH_USE "Use which item? "
#define MES_OBJECT_NO_USE "You have nothing to use." 
#define MES_OBJECT_WHICH_DROP "Drop which item? "
#define MES_OBJECT_NO_DROP "You have nothing to drop."
#define MES_OBJECT_WHICH_OBSERVE "Examine which item? "
#define MES_OBJECT_NO_OBSERVE "You have nothing to examine."
#define MES_OBJECT_WHICH_INDEN "Identify which item? "
#define MES_OBJECT_WHICH_ALL_INDEN "All items are identified. "
#define MES_OBJECT_NO_INDEN "You have nothing to identify."
#define MES_OBJECT_WHICH_INDEN_FULLY "*Identify* which item? "
#define MES_OBJECT_WHICH_ALL_INDEN_FULLY "All items are *identified*. "
#define MES_OBJECT_NO_INDEN_FULLY "You have nothing to *identify*."
#define MES_OBJECT_NO_INDENTIFED "You have no special knowledge about that item."
#define MES_OBJECT_EXAMING(OBJECT) "Examining %s...", (OBJECT)
#define MES_OBJECT_NO_SPECIAL "You see nothing special."
#define MES_OBJECT_DROP_FEET "You feel something roll beneath your feet."
#define MES_OBJECT_CHARGE_LEFT(NUM) "You have %d charges remaining.", (NUM)
#define MES_OBJECT_CHARGE_LEFT2(NUM) "There is %d charge remaining.", (NUM)
#define MES_OBJECT_NO_CHARGE_LEFT "No charges left."
#define MES_OBJECT_WHICH_REFILL_LAMP "Refill with which flask? "
#define MES_OBJECT_NO_REFILL_LAMP "You have no flasks of oil."
#define MES_OBJECT_WHICH_TORCH "Refuel with which torch? "
#define MES_OBJECT_NO_TORCH "You have no extra torches."
#define MES_OBJECT_DROPPED(NAME, SYM) "You drop %s (%c).", (NAME), (SYM)
#define MES_OBJECT_INDETIFY_ALL_ASK "Do you pay for identify all your possession? "
#define MES_OBJECT_INDETIFIED_ALL "Your possessions have been identified."
#define MES_OBJECT_PICK_FAILED(CREATURE, OBJECT) "%^s tries to pick up %s, but fails.", (CREATURE), (OBJECT)
#define MES_OBJECT_PICKED(CREATURE, OBJECT) "%^s picks up %s.", (CREATURE), (OBJECT)
#define MES_OBJECT_DESTORY(CREATURE, OBJECT) "%^s destroys %s.", (CREATURE), (OBJECT)
#define MES_OBJECT_WHICH_PULISH "Pulish which weapon? "
#define MES_OBJECT_NO_PULISH "You have weapon to pulish."
#define MES_OBJECT_PULISHED(OBJECT) "The %s shine%s!", (OBJECT)->name, ((OBJECT)->number > 1) ? "" : "s")
#endif

#ifdef JP
#define MES_OBJECT_WHICH_STAFF "�ǂ̏���g���܂���? "
#define MES_OBJECT_NO_STAFF "�g����񂪂Ȃ��B"
#define MES_OBJECT_WHICH_WAND "�ǂ̖��@�_�ő_���܂���? "
#define MES_OBJECT_NO_WAND "�g���閂�@�_���Ȃ��B" 
#define MES_OBJECT_WHICH_ROD "�ǂ̃��b�h��U��܂���? "
#define MES_OBJECT_NO_ROD "�g���郍�b�h���Ȃ��B"
#define MES_OBJECT_WHICH_ACTIVATE "�ǂ̃A�C�e�����n�������܂���? "
#define MES_OBJECT_NO_ACTIVATE "�n���ł���A�C�e���𑕔����Ă��Ȃ��B"
#define MES_OBJECT_WAND_NEED_PICKUP "�܂��͖��@�_���E��Ȃ���΁B"
#define MES_OBJECT_ROD_NEED_PICKUP "�܂��̓��b�h���E��Ȃ���΁B"
#define MES_OBJECT_STAFF_FAILED "���܂�����g���Ȃ������B"
#define MES_OBJECT_WAND_FAILED "���܂����@�_���g���Ȃ������B"
#define MES_OBJECT_ROD_FAILED "���܂����b�h���g���Ȃ������B"
#define MES_OBJECT_STILL_CHARGING1 "���̃��b�h�͂܂����͂��[�U���Ă���Œ����B"
#define MES_OBJECT_STILL_CHARGING2 "���̃��b�h�͂܂��[�U���ł��B"
#define MES_OBJECT_MBALL_FAILED "����Ɏ��s�����B"
#define MES_OBJECT_ACTIVATE_NONE "���̃A�C�e���͎n���ł��Ȃ��B"
#define MES_OBJECT_WHICH_THROW "���𓊂��܂���? "
#define MES_OBJECT_NO_THROW "������A�C�e�����Ȃ��B"
#define MES_OBJECT_WHICH_FIRE "�ǂ�������܂���? "
#define MES_OBJECT_NO_FIRE "���˂����A�C�e��������܂���B"
#else
#define MES_OBJECT_WHICH_STAFF "Use which staff? "
#define MES_OBJECT_NO_STAFF "You have no staff to use."
#define MES_OBJECT_WHICH_WAND "Aim which wand? "
#define MES_OBJECT_NO_WAND "You have no wand to aim."
#define MES_OBJECT_WHICH_ROD "Zap which rod? "
#define MES_OBJECT_NO_ROD "You have no rod to zap."
#define MES_OBJECT_WHICH_ACTIVATE "Activate which item? "
#define MES_OBJECT_NO_ACTIVATE "You have nothing to activate."
#define MES_OBJECT_WAND_NEED_PICKUP "You must first pick up the wands."
#define MES_OBJECT_ROD_NEED_PICKUP "You must first pick up the rods."
#define MES_OBJECT_STAFF_FAILED "You failed to use the staff properly."
#define MES_OBJECT_WAND_FAILED "You failed to use the wand properly."
#define MES_OBJECT_ROD_FAILED "You failed to use the rod properly."
#define MES_OBJECT_STILL_CHARGING1 "The rod is still charging."
#define MES_OBJECT_STILL_CHARGING2 "The rods are still charging."
#define MES_OBJECT_MBALL_FAILED "You failed to release your pet."
#define MES_OBJECT_ACTIVATE_NONE "That object cannot be activated."
#define MES_OBJECT_WHICH_THROW "Throw which item? "
#define MES_OBJECT_NO_THROW "You have nothing to throw."
#define MES_OBJECT_WHICH_FIRE "Fire which item? "
#define MES_OBJECT_NO_FIRE "You have nothing to fire."
#endif

#ifdef JP
#define MES_OBJECT_DESTROY(OBJECT) "%s���󂵂��B", (OBJECT)
#define MES_OBJECT_CANNOT_DESTROY(OBJECT) "%s�͔j��s�\���B", (OBJECT)
#define MES_OBJECT_WHICH_DESTROY "�ǂ̃A�C�e�����󂵂܂���? "
#define MES_OBJECT_NO_DESTROY "�󂹂�A�C�e���������Ă��Ȃ��B"
#define MES_OBJECT_DESTROY_VERIFY(OBJECT) "�{����%s���󂵂܂���? [y/n/Auto]", (OBJECT)
#else
#define MES_OBJECT_DESTROY(OBJECT) "You destroy %s.", (OBJECT)
#define MES_OBJECT_CANNOT_DESTROY(OBJECT) "You cannot destroy %s.", (OBJECT)
#define MES_OBJECT_WHICH_DESTROY "Destroy which item? "
#define MES_OBJECT_NO_DESTROY "You have nothing to destroy."
#define MES_OBJECT_DESTROY_VERIFY(OBJECT) "Really destroy %s? [y/n/Auto]", (OBJECT)
#endif

#ifdef JP
#define MES_OBJECT_WHICH_INSCRIBE "�ǂ̃A�C�e���ɖ������݂܂���? "
#define MES_OBJECT_NO_INSCRIBE "�������߂�A�C�e�����Ȃ��B"
#define MES_OBJECT_WHICH_UNINSCRIBE "�ǂ̃A�C�e���̖��������܂���? "
#define MES_OBJECT_NO_UNINSCRIBE "����������A�C�e�����Ȃ��B"
#define MES_OBJECT_INSCRIBING(OBJECT) "%s�ɖ������ށB", (OBJECT)
#define MES_OBJECT_NO_INSCRIPTION "���̃A�C�e���ɂ͏����ׂ������Ȃ��B"
#define MES_OBJECT_REMOVE_INSCRIPTION "�����������B"
#define MES_OBJECT_INSCRIBING_PROMPT "��"
#else
#define MES_OBJECT_WHICH_INSCRIBE "Inscribe which item? "
#define MES_OBJECT_NO_INSCRIBE "You have nothing to inscribe."
#define MES_OBJECT_WHICH_UNINSCRIBE "Un-inscribe which item? "
#define MES_OBJECT_NO_UNINSCRIBE "You have nothing to un-inscribe."
#define MES_OBJECT_INSCRIBING(OBJECT) "Inscribing %s.", (OBJECT)
#define MES_OBJECT_NO_INSCRIPTION "That item had no inscription to remove."
#define MES_OBJECT_REMOVE_INSCRIPTION "Inscription removed."
#define MES_OBJECT_INSCRIBING_PROMPT "Inscription"
#endif

#ifdef JP
#define MES_BOUNTY_DETAIL "���̂������A��Ε�V�������グ�܂��B"
#define MES_BOUNTY_TODAY "�{���̏܋���"
#define MES_BOUNTY_CURRENT "���݂̏܋���"
#define MES_BOUNTY_DONE "(��)"
#else
#define MES_BOUNTY_DETAIL "Offer a prize when you bring a wanted creature's corpse"
#define MES_BOUNTY_TODAY "Wanted creature that changes from day to day"
#define MES_BOUNTY_CURRENT "Wanted creatures"
#define MES_BOUNTY_DONE "(done)"
#endif

#ifdef JP
#define MES_EQUIP_WHICH_WIELD "�ǂ�𑕔����܂���? "
#define MES_EQUIP_NO_WIELD "�����\�ȃA�C�e�����Ȃ��B"
#define MES_EQUIP_WHICH_SLOT "�ǂ��ɑ������܂���?"
#define MES_EQUIP_NO_SLOT "����͑����ł��Ȃ��B"
#define MES_EQUIP_WHICH_TAKEOFF "�ǂ�𑕔�����͂����܂���? "
#define MES_EQUIP_NO_TAKEOFF "�͂����鑕�����Ȃ��B"
#define MES_EQUIP_ASK_CURSED_WIELD(NAME, CURSED) "�{����%s{%s}���g���܂����H", (NAME), (CURSED)
#define MES_EQUIP_ASK_BECOME_VAMPIRE(NAME) "%s�𑕔�����Ɛ^���̋z���S�ɂȂ�܂��B��낵���ł����H", (NAME)
#define MES_EQUIP_DONE(NAME, SYM) "%s(%c)�𑕔������B", (NAME), (SYM)
#define MES_EQUIP_CURSED "����I �����܂����₽���I"
#define MES_EQUIP_HEAVY_BOW "����ȏd���|�𑕔����Ă���̂͑�ς��B"
#define MES_EQUIP_FITTING_BOW "���̋|�Ȃ瑕�����Ă��Ă��h���Ȃ��B"
#define MES_EQUIP_RELIEVE_HEAVY_BOW "�d���|�𑕔�����͂����đ̂��y�ɂȂ����B"
#define MES_EQUIP_HEAVY_WEAPON "����ȏd������𑕔����Ă���̂͑�ς��B"
#define MES_EQUIP_FITTING_WEAPON "����Ȃ瑕�����Ă��Ă��h���Ȃ��B"
#define MES_EQUIP_HEAVY_WEAPON_STILL "�܂����킪�d���B"
#define MES_EQUIP_RELIEVE_HEAVY_WEAPON "�d������𑕔�����͂����đ̂��y�ɂȂ����B"
#define MES_EQUIP_NO_RIDING_WEAPON "���̕���͏�n���Ɏg���ɂ͂ނ��Ȃ��悤���B"
#define MES_EQUIP_SUITABLE_WALKING_WEAPON "���̕���͓k���Ŏg���₷���B"
#define MES_EQUIP_RIDING_WEAPON "����Ȃ��n���ɂ҂����肾�B"
#define MES_EQUIP_HEAVY_ARMOR "�������d���ăo�����X�����Ȃ��B"
#define MES_EQUIP_LIGHT_ARMOR "�o�����X���Ƃ��悤�ɂȂ����B"
#define MES_EQUIP_UNCONTROL_STEED "�n�𑀂�Ȃ��B"
#define MES_EQUIP_CONTROL_STEED "�n�𑀂��悤�ɂȂ����B"
#define MES_EQUIP_BOTH_HAND(OBJECT) "%s�𗼎�ō\�����B", (OBJECT)->name
#else
#define MES_EQUIP_WHICH_WIELD "Wear/Wield which item? "
#define MES_EQUIP_NO_WIELD "You have nothing you can wear or wield."
#define MES_EQUIP_WHICH_SLOT "Equip which hand? "
#define MES_EQUIP_NO_SLOT "You can't equip it"
#define MES_EQUIP_WHICH_TAKEOFF "Take off which item? "
#define MES_EQUIP_NO_TAKEOFF "You are not wearing anything to take off."
#define MES_EQUIP_ASK_CURSED_WIELD(NAME, CURSED) "Really use the %s {%s}? ", (NAME), (CURSED)
#define MES_EQUIP_ASK_BECOME_VAMPIRE(NAME) "%s will transforms you into a true vampire permanently when equiped. Do you become a vampire?", (NAME)
#define MES_EQUIP_DONE(NAME, SYM) "You are wearing %s (%c).", (NAME), (SYM)
#define MES_EQUIP_CURSED "Oops! It feels deathly cold!"
#define MES_EQUIP_HEAVY_BOW "You have trouble wielding such a heavy bow."
#define MES_EQUIP_FITTING_BOW "You have no trouble wielding your bow."
#define MES_EQUIP_RELIEVE_HEAVY_BOW "You feel relieved to put down your heavy bow."
#define MES_EQUIP_HEAVY_WEAPON "You have trouble wielding such a heavy weapon."
#define MES_EQUIP_FITTING_WEAPON "You have no trouble wielding your weapon."
#define MES_EQUIP_HEAVY_WEAPON_STILL "You have still trouble wielding a heavy weapon."
#define MES_EQUIP_RELIEVE_HEAVY_WEAPON "You feel relieved to put down your heavy weapon."
#define MES_EQUIP_NO_RIDING_WEAPON "This weapon is not suitable for use while riding."
#define MES_EQUIP_SUITABLE_WALKING_WEAPON "This weapon was not suitable for use while riding."
#define MES_EQUIP_RIDING_WEAPON "This weapon is suitable for use while riding."
#define MES_EQUIP_HEAVY_ARMOR "The weight of your armor disrupts your balance."
#define MES_EQUIP_LIGHT_ARMOR "You regain your balance."
#define MES_EQUIP_UNCONTROL_STEED "You are using both hand for fighting, and you can't control a riding pet."
#define MES_EQUIP_CONTROL_STEED "You began to control riding pet with one hand."
#define MES_EQUIP_BOTH_HAND(OBJECT) "You are wielding %s with both hands.", (OBJECT)->name
#endif

#ifdef JP
#define MES_DAMAGE_RES1 "�ɂ͂��Ȃ�̑ϐ�������B"
#define MES_DAMAGE_RES2 "�ɂ͑ϐ�������B"
#define MES_DAMAGE_RES3 "�ɂ͂킸���ɑϐ�������B"
#define MES_DAMAGE_RES4 "�͂Ђǂ��Ɏ���󂯂��B"
#define MES_DAMAGE_HURT_LITE "���œ��̂��ł����ꂽ�I"
#define MES_DAMAGE_RES_TIME(CREATURE) "%s�͎��Ԃ��ʂ�߂��Ă����l�q���������B", (CREATURE)
#define MES_DAMAGE_VOID_INVULN "�o���A��؂�􂢂��I"
#define MES_DAMAGE_MULTI_SHADOW_SUCCESS "�U���͌��e�ɖ������A�͂��Ȃ������B"
#define MES_DAMAGE_MULTI_SHADOW_FAILED "���e����Ƃ��̂��؂�􂩂ꂽ�I"
#define MES_DAMAGE_VOID_WRAITH_FORM "�������̑̂��؂�􂩂ꂽ�I"
#define MES_DAMAGE_PSYONIC "��I�G�l���M�[�Ő��_���U�����ꂽ�B"
#else
#define MES_DAMAGE_RES1 " resists strongly."
#define MES_DAMAGE_RES2 " resists."
#define MES_DAMAGE_RES3 " resists a little."
#define MES_DAMAGE_RES4 " is hit hard."
#define MES_DAMAGE_HURT_LITE "The light scorches your flesh!"
#define MES_DAMAGE_RES_TIME(CREATURE) "%s felt as if time is passing %s by.", (CREATURE), (CREATURE)
#define MES_DAMAGE_VOID_INVULN "The barrier is penetrated!"
#define MES_DAMAGE_MULTI_SHADOW_SUCCESS "The attack hits Shadow, unharmed."
#define MES_DAMAGE_MULTI_SHADOW_FAILED "The attack hits Shadow together with you!"
#define MES_DAMAGE_VOID_WRAITH_FORM "The attack cuts through your ethereal body!"
#define MES_DAMAGE_PSYONIC "Your mind is blasted by psyonic energy."
#endif

#ifdef JP
#define MES_DISARM_DONE(TRAP) "%s�����������B", (TRAP)
#define MES_DISARM_CHEST "���Ɏd�|�����Ă����g���b�v�����������B"
#define MES_DISARM_NONE "�����ɂ͉���������̂���������Ȃ��B"
#define MES_DISARM_NO_TRAP "�g���b�v����������Ȃ��B"
#define MES_DISARM_NO_TRAP_CHEST "���ɂ̓g���b�v���d�|�����Ă��Ȃ��B"
#define MES_DISARM_FAILED(TARGET) "%s�̉����Ɏ��s�����B", (TARGET)
#define MES_DISARM_FUMBLE "�g���b�v���쓮�����Ă��܂����I"
#else
#define MES_DISARM_DONE(TRAP) "You have disarmed the %s.", (TRAP)
#define MES_DISARM_CHEST "You have disarmed the chest."
#define MES_DISARM_NONE "You see nothing there to disarm."
#define MES_DISARM_NO_TRAP "You don't see any traps."
#define MES_DISARM_NO_TRAP_CHEST "The chest is not trapped."
#define MES_DISARM_FAILED(TARGET) "You failed to disarm the %s.", (TARGET)
#define MES_DISARM_FUMBLE "You set off a trap!"
#endif

#ifdef JP
#define MES_TRAVEL_ALREADY "���łɂ����ɂ��܂��B"
#define MES_TRAVEL_CANNOT "�����ɂ͍s�����Ƃ��ł��܂���B"
#else
#define MES_TRAVEL_ALREADY "You are already there."
#define MES_TRAVEL_CANNOT "You cannot travel there!"
#endif

#ifdef JP
#define MES_BASH_NO_TARGET "�����ɂ͑̓����肷����̂���������Ȃ��B"
#define MES_BASH_DO(TARGET) "%s�ɑ̓�����������I", (TARGET)
#define MES_BASH_CRUSHED(TARGET) "%s���󂵂��I", (TARGET)
#define MES_BASH_HOLD(TARGET) "����%s�͊�䂾�B", (TARGET)
#define MES_BASH_OFF_BALANCE "�̂̃o�����X���������Ă��܂����B"
#else
#define MES_BASH_NO_TARGET "You see nothing there to bash."
#define MES_BASH_DO(TARGET) "You smash into the %s!", (TARGET)
#define MES_BASH_CRUSHED(TARGET) "The %s crashes open!", (TARGET)
#define MES_BASH_HOLD(TARGET) "The %s holds firm.", (TARGET)
#define MES_BASH_OFF_BALANCE "You are off-balance."
#endif

#ifdef JP
#define MES_SPIKE_NO_TARGET "�����ɂ͂����т�łĂ���̂���������Ȃ��B"
#define MES_SPIKE_NO_SPIKE "�����т������Ă��Ȃ��B"
#define MES_SPIKE_JAM(TARGET) "%s�ɂ����т�ł����񂾁B", (TARGET)
#else
#define MES_SPIKE_NO_TARGET "You see nothing there to spike."
#define MES_SPIKE_NO_SPIKE "You have no spikes!"
#define MES_SPIKE_JAM(TARGET) "You jam the %s with a spike.", (TARGET)
#endif

#ifdef JP
#define MES_TIME_STRANGE "�ςȎ������B"
#define MES_TIME_DETAIL(DAY, HH, MM) "%s����, ������%d:%02d %s�ł��B", DAY, (HH % 12 == 0) ? 12 : (HH % 12), MM, (HH < 12) ? "AM" : "PM"
#else
#define MES_TIME_STRANGE "It is a strange time."
#define MES_TIME_DETAIL(DAY, HH, MM) "This is day %s. The time is %d:%02d %s.", DAY, (HH % 12 == 0) ? 12 : (HH % 12), MM, (HH < 12) ? "AM" : "PM"
#endif

#ifdef JP
#define MES_KNOW_PET "���݂̃y�b�g"
#define MES_KNOW_ALIVE_UNIQUES "�܂������Ă��郆�j�[�N�E�N���[�`���["
#define MES_KNOW_NO_ALIVE_UNIQUES "���m�̐������j�[�N�͂��܂���B\n"
#define MES_KNOW_NO_KILLED "���Ȃ��͂܂��N���E���Ă��Ȃ��B\n\n"
#define MES_KNOW_KILLED(NUMBER) "���Ȃ���%ld�̂̃N���[�`���[���E���Ă���B\n\n", (NUMBER)
#define MES_KNOW_ALIVE_UNIQUE_LIST1(NUM) "     �n��  ����: %3d��\n", (NUM)
#define MES_KNOW_ALIVE_UNIQUE_LIST2(FROM, TO, NUM) "%3d-%3d�K  ����: %3d��\n", (FROM), (TO), (NUM)
#define MES_KNOW_ALIVE_UNIQUE_LIST3(TO, NUM) "%s-   �K  ����: %3d��\n", (TO), (NUM)
#else
#define MES_KNOW_PET "Current Pets"
#define MES_KNOW_ALIVE_UNIQUES "Alive Uniques"
#define MES_KNOW_NO_ALIVE_UNIQUES "No known uniques alive.\n"
#define MES_KNOW_NO_KILLED "You have killed no creatures yet.\n\n"
#define MES_KNOW_KILLED(NUMBER) "You have killed %ld %s.\n\n", (NUMBER), ((NUMBER) == 1) ? "creature" : "creatures"
#define MES_KNOW_ALIVE_UNIQUE_LIST1(NUM) "      Surface  alive: %3d\n", (NUM)
#define MES_KNOW_ALIVE_UNIQUE_LIST2(FROM, TO, NUM) "Level %3d-%3d  alive: %3d\n", (FROM), (TO), (NUM)
#define MES_KNOW_ALIVE_UNIQUE_LIST3(TO, NUM) "Level %3d-     alive: %3d\n", (TO), (NUM)
#endif



#ifdef JP
#define MES_RUSH_NO_ENTER "�����ɂ͓��g�ł͓���Ȃ��B"
#define MES_RUSH_DONE(TARGET) "�f����%s�̉��ɓ��荞�񂾁I", (TARGET)
#else
#define MES_RUSH_NO_ENTER "You can't move to that place."
#define MES_RUSH_DONE(TARGET) "You quickly jump in and attack %s!", (TARGET)
#endif

#ifdef JP
#define MES_EARTHQUAKE_DONE1 "�_���W�����̕ǂ����ꂽ�I"
#define MES_EARTHQUAKE_DONE2 "�_���W�����̏����s���R�ɂ˂��Ȃ������I"
#define MES_EARTHQUAKE_DONE3 "�_���W�������h�ꂽ�I���ꂽ�₪���ɍ~���Ă����I"
#define MES_EARTHQUAKE_HEAVY_CRUSHED "���Ȃ��͂Ђǂ�����𕉂����I"
#define MES_EARTHQUAKE_EVADE "�~�蒍��������܂��������I"
#define MES_EARTHQUAKE_CRUSHED1 "��΂����Ȃ��ɒ�������!"
#define MES_EARTHQUAKE_CRUSHED2 "���Ȃ��͏��ƕǂƂ̊Ԃɋ��܂�Ă��܂����I"
#define MES_EARTHQUAKE_CANCELED "�_���W�������h�ꂽ�B"
#define MES_CALL_OF_VOID_EXPRO1 "���Ȃ��͕ǂɋ߂�����ꏊ�Ŕ������Ă��܂����I"
#define MES_CALL_OF_VOID_EXPRO2 "�傫�Ȕ��������������I"
#define MES_VANISH_DUNGEON_CANCELED "�_���W�����͈�u�Â܂�Ԃ����B" 
#else
#define MES_EARTHQUAKE_DONE1 "The cave ceiling collapses!"
#define MES_EARTHQUAKE_DONE2 "The cave floor twists in an unnatural way!"
#define MES_EARTHQUAKE_DONE3 "The cave quakes!  You are pummeled with debris!"
#define MES_EARTHQUAKE_HEAVY_CRUSHED "You are severely crushed!"
#define MES_EARTHQUAKE_EVADE "You nimbly dodge the blast!"
#define MES_EARTHQUAKE_CRUSHED1 "You are bashed by rubble!"
#define MES_EARTHQUAKE_CRUSHED2 "You are crushed between the floor and ceiling!"
#define MES_EARTHQUAKE_CANCELED "The dungeon trembles."
#define MES_CALL_OF_VOID_EXPRO1 "You evoked the void too close to a wall!"
#define MES_CALL_OF_VOID_EXPRO2 "There is a loud explosion!"
#define MES_VANISH_DUNGEON_CANCELED "The dungeon silences a moment."
#endif

#ifdef JP
#define MES_DIS_MINION_ASK "�{���ɑS�y�b�g�𔚔j���܂����H"
#define MES_DIS_MINION_CANCEL(MINION) "%s�͔��j�����̂�������A����Ɏ����̐��E�ւƋA�����B", (MINION)
#else
#define MES_DIS_MINION_ASK "You will blast all pets. Are you sure? "
#define MES_DIS_MINION_CANCEL(MINION)  "%^s resists to be blasted, and run away.", (MINION)
#endif

#ifdef JP
#define MES_PATRON_BOOM_OUT(PATRON) "%s�̐��������n����:", (PATRON)
#define MES_PATRON_MUTATION(PATRON) "%^s�͖J���Ƃ��Ă��Ȃ���ˑR�ψق������B"
#define MES_PATRON_POLY_SELF "�u���A�V���Ȃ�p��K�v�Ƃ���I�v"
#define MES_PATRON_GAIN_EXP "�u���͗ǂ��s������I������I�v"
#define MES_PATRON_LOSE_EXP "�u���l��A������ɒl�����B�v"
#define MES_PATRON_GOOD_OBJ "�u�䂪�^�������������Ɏg���ׂ��B�v"
#define MES_PATRON_GOOD_WEP "�u���̍s���͋M�����ɒl����B�v"
#define MES_PATRON_GOOD_OBS "�u���̍s���͋M���񂢂ɒl����v"
#define MES_PATRON_GREAT_OBS "�u���l��A���̌��g�ւ̉䂪�ɂ��ݖ����񂢂����邪�悢�B�v"
#define MES_PATRON_TY_CURSE "�u���l��A������Ȃ�B�v"
#define MES_PATRON_S_ENEMY "�u�䂪���l������A���̘����Ȃ�҂�|���ׂ��I�v"
#define MES_PATRON_H_SUMMON "�u���A��苭���G��K�v�Ƃ���I�v"
#define MES_PATRON_HAVOC "�u���Ɣj�󂱂��䂪��тȂ�I�v"
#define MES_PATRON_GAIN_ABL "�u���܂�̂��A���l��B�]�����̓��̂�b����B�v"
#define MES_PATRON_LOSE_ABL "�u���l��A�]�͓��ɖO�݂���B�v"
#define MES_PATRON_RUIN_ABL "�u���A�������邱�Ƃ��w�Ԃׂ��I�v"
#define MES_PATRON_POLY_WND(PATRON) "%s�̗͂��G���̂��������B", (PATRON)
#define MES_PATRON_AUGM_ABL "�u�䂪�����₩�Ȃ鎒�����󂯂Ƃ邪�悢�I�v"
#define MES_PATRON_HURT_LOT "�u�ꂵ�ނ��悢�A���\�ȋ����҂�I�v"
#define MES_PATRON_HEAL_FUL "�u�S�邪�悢�A�䂪���l��I�v"
#define MES_PATRON_CURSE_WEAPON "�u���A����ɗ��邱�ƂȂ���B�v"
#define MES_PATRON_CURSE_ARMOR "�u���A�h��ɗ��邱�ƂȂ���B�v"
#define MES_PATRON_PISS_OFF "�u���{�肵�߂��߂������ׂ��B�v"
#define MES_PATRON_GENOCIDE "�u��A���̓G�𖕎E����I�v"
#define MES_PATRON_ASSULT(PATRON) "%s�̗͂��G���U������̂��������I", (PATRON)
#define MES_PATRON_WRATH "�u���ʂ��悢�A���l��I�v"
#define MES_PATRON_IGNORE(PATRON) "%s�͂��Ȃ��𖳎������B", (PATRON)
#define MES_PATRON_DEMON_SERVANT(PATRON) "%s�͖J���Ƃ��Ĉ����̎g�����悱�����I", (PATRON)
#define MES_PATRON_UNDEAD_SERVANT(PATRON) "%s�͖J���Ƃ��ăA���f�b�h�̎g�����悱�����I", (PATRON)
#define MES_PATRON_SERVANT(PATRON) "%s�͖J���Ƃ��Ďg�����悱�����I", (PATRON)
#define MES_PATRON_PRAISE_WINNER "�w�悭������A�䂪�����ׂ�I�x"
#define MES_PATRON_ERROR_REWARD1(PATRON) "%s�̐����ǂ�����:", (PATRON)
#define MES_PATRON_ERROR_REWARD2(TYPE, EFFECT) "�u���[�A���[�A������ %d/%d�B����͂��邩���H�v", (TYPE), (EFFECT)
#define MES_DIARY_PATRON_DESTROYED(OBJECT) "%s���j�󂳂ꂽ�B", (OBJECT)
#define MES_DIARY_PATRON_MUTATION "�ψق����B"
#define MES_DIARY_PATRON_GAIN_EXP "�o���l�𓾂�"
#define MES_DIARY_PATRON_LOST_EXP "�o���l���������B"
#define MES_DIARY_PATRON_GOOD_ITEM "�㎿�ȃA�C�e������ɓ��ꂽ�B"
#define MES_DIARY_PATRON_GREAT_ITEM "�����i�̃A�C�e������ɓ��ꂽ�B"
#define MES_DIARY_PATRON_CHAOS_WP "(����)�̕������ɓ��ꂽ�B"
#define MES_DIARY_PATRON_S_ENEMY "�N���[�`���[���������ꂽ�B"
#define MES_DIARY_PATRON_TY_CURSE "�ЁX�����􂢂�������ꂽ�B"
#define MES_DIARY_PATRON_HAVOC "�J�I�X�̗͂��Q�������B"
#define MES_DIARY_PATRON_GAIN_ABL "�\�͒l���オ�����B"
#define MES_DIARY_PATRON_LOST_ABL "�\�͒l�����������B"
#define MES_DIARY_PATRON_AUGM_ABL "�S�\�͒l���オ�����B"
#define MES_DIARY_PATRON_RUIN_ABL "�S�\�͒l�����������B"
#define MES_DIARY_PATRON_POLY_WND "�����ω������B"
#define MES_DIARY_PATRON_HURT_LOT "�����̋������������B"
#define MES_DIARY_PATRON_DESTRUCTION "�_���W������*�j��*���ꂽ�B"
#define MES_DIARY_PATRON_GENOCIDE "�N���[�`���[�����E���ꂽ�B"
#define MES_DIARY_PATRON_SERVANT "�N���[�`���[���y�b�g�ɂȂ����B"
#define MES_DIARY_PATRON_DEMONIC_SERVANT "�������y�b�g�ɂȂ����B"
#define MES_DIARY_PATRON_UNDEAD_SERVANT "�A���f�b�h���y�b�g�ɂȂ����B"
#define MES_DIARY_STAND_IN(PLACE) "%s�ɍ~�藧�����B", (PLACE)
#define MES_DIARY_PATRON_HEALING "�̗͂��񕜂����B"
#define MES_DIARY_CHEAT_DEATH "                            �������A�����Ԃ����B"
#define MES_DIARY_WINNER "������D\'angband�̏����҂ƂȂ����I"
#define MES_DIARY_DISCOVER_ART(ART_NAME) "%s�𔭌������B\n", (ART_NAME)
#define MES_DIARY_DEFEAT_UNIQUE(UNIQUE_NAME) "%s��|�����B\n", (UNIQUE_NAME)
#define MES_DIARY_FIX_QUEST(QUEST_NAME) "�N�G�X�g�u%s�v��B�������B\n", (QUEST_NAME)
#define MES_DIARY_AWAY_QUEST(QUEST_NAME) " �N�G�X�g�u%s�v���瓦���A�����B\n", (QUEST_NAME)
#define MES_DIARY_FIX_R_QUEST(QUEST_NAME) " �����_���N�G�X�g(%s)��B�������B\n", (QUEST_NAME)
#define MES_DIARY_AWAY_R_QUEST(QUEST_NAME) " �����_���N�G�X�g(%s)���瓦���o�����B\n", (QUEST_NAME)
#define MES_DIARY_MAXDEPTH(DUNGEON_NAME, DEPTH) " %s�̍Ő[�K%d�K�ɓ��B�����B\n", (DUNGEON_NAME), (DEPTH)
#define MES_DIARY_TRUMP_TOWER_SET(NOTE, DUNGEON_NAME, DEPTH) " %s%s�̍Ő[�K��%d�K�ɃZ�b�g�����B\n", (NOTE), (DUNGEON_NAME), (DEPTH)
#define MES_DIARY_STAIR(TO, NOTE) " %s��%s�B\n", (TO), (NOTE)
#define MES_DIARY_RECALL_DEPTH(DUNGEON_NAME, DEPTH) " �A�҂��g����%s��%d�K�։��肽�B\n", (DUNGEON_NAME), (DEPTH)
#define MES_DIARY_RECALL_SURFACE " �A�҂��g���Ēn��ւƖ߂����B\n"
#define MES_DIARY_TO_QUEST(QUEST_NAME) " �N�G�X�g�u%s�v�ւƓ˓������B\n", (QUEST_NAME)
#define MES_DIARY_LEV_TELE " ���x���E�e���|�[�g�ŒE�o�����B\n"
#define MES_DIARY_BUY(OBJECT_NAME) " %s���w�������B\n", (OBJECT_NAME)
#define MES_DIARY_SELL(OBJECT_NAME) " %s�𔄋p�����B\n", (OBJECT_NAME)
#define MES_DIARY_ARENA_WON(NUM, ENEMY_NAME) " ���Z���%d���(%s)�ɏ��������B\n", (NUM), (ENEMY_NAME)
#define MES_DIARY_ARENA_LOST(NUM, ENEMY_NAME) " ���Z���%d���ŁA%s�̑O�ɔs�ꋎ�����B\n", (NUM), (ENEMY_NAME)
#define MES_DIARY_ARENA_CHAMPION "                 ���Z��̂��ׂĂ̓G�ɏ������A�`�����s�I���ƂȂ����B\n"
#define MES_DIARY_IDEN(OBJECT_NAME) " %s�����ʂ����B\n", (OBJECT_NAME)
#define MES_DIARY_WIZ_TELE(TO) " %s�ւƃE�B�U�[�h�E�e���|�[�g�ňړ������B\n", (TO)
#define MES_DIARY_PAT_TELE(TO) " %s�ւƃp�^�[���̗͂ňړ������B\n", (TO)
#define MES_DIARY_LEVELUP(LEVEL) " ���x����%d�ɏオ�����B\n", (LEVEL)
#else
#define MES_PATRON_BOOM_OUT(PATRON) "The voice of %s booms out:", (PATRON)
#define MES_PATRON_MUTATION(PATRON) "%^s rewards you with a mutation!"
#define MES_PATRON_POLY_SELF "'Thou needst a new form!'"
#define MES_PATRON_GAIN_EXP "'Well done, Lead on!'"
#define MES_PATRON_LOSE_EXP "'Thou didst not deserve that, slave.'"
#define MES_PATRON_GOOD_OBJ "'Use my gift wisely.'"
#define MES_PATRON_GOOD_WEP "'Thy deed hath earned thee a worthy blade.'"
#define MES_PATRON_GOOD_OBS "'Thy deed hath earned thee a worthy reward.'"
#define MES_PATRON_GREAT_OBS "'Behold, mortal, how generously I reward thy loyalty.'"
#define MES_PATRON_TY_CURSE "'Thou art growing arrogant, mortal.'"
#define MES_PATRON_S_ENEMY "'My pets, destroy the arrogant mortal!'"
#define MES_PATRON_H_SUMMON "'Thou needst worthier opponents!'"
#define MES_PATRON_HAVOC "'Death and destruction! This pleaseth me!'"
#define MES_PATRON_GAIN_ABL "'Stay, mortal, and let me mold thee.'"
#define MES_PATRON_LOSE_ABL "'I grow tired of thee, mortal.'"
#define MES_PATRON_RUIN_ABL "'Thou needst a lesson in humility, mortal!'"
#define MES_PATRON_POLY_WND(PATRON) "You feel the power of %s touch you.", (PATRON)
#define MES_PATRON_AUGM_ABL "'Receive this modest gift from me!'"
#define MES_PATRON_HURT_LOT "'Suffer, pathetic fool!'"
#define MES_PATRON_HEAL_FUL "'Rise, my servant!'"
#define MES_PATRON_CURSE_WEAPON "'Thou reliest too much on thy weapon.'"
#define MES_PATRON_CURSE_ARMOR "'Thou reliest too much on thine equipment.'"
#define MES_PATRON_PISS_OFF "'Now thou shalt pay for annoying me.'"
#define MES_PATRON_GENOCIDE "'Let me relieve thee of thine oppressors!'"
#define MES_PATRON_ASSULT(PATRON) "You can feel the power of %s assault your enemies!", (PATRON)
#define MES_PATRON_WRATH "'Die, mortal!'"
#define MES_PATRON_IGNORE(PATRON) "%s ignores you.", (PATRON)
#define MES_PATRON_DEMON_SERVANT(PATRON) "%s rewards you with a demonic servant!", (PATRON)
#define MES_PATRON_UNDEAD_SERVANT(PATRON) "%s rewards you with an undead servant!", (PATRON)
#define MES_PATRON_SERVANT(PATRON) "%s rewards you with a servant!", (PATRON)
#define MES_PATRON_PRAISE_WINNER "'Thou art donst well, my devotee!'"
#define MES_PATRON_ERROR_REWARD1(PATRON) "The voice of %s stammers:", (PATRON)
#define MES_PATRON_ERROR_REWARD2(TYPE, EFFECT) "'Uh... uh... the answer's %d/%d, what's the question?'", (TYPE), (EFFECT)
#define MES_DIARY_PATRON_DESTROYED(OBJECT) "destroying %s", (OBJECT)
#define MES_DIARY_PATRON_MUTATION "mutation"
#define MES_DIARY_PATRON_GAIN_EXP "experience"
#define MES_DIARY_PATRON_LOST_EXP "losing experience"
#define MES_DIARY_PATRON_GOOD_ITEM "a good item"
#define MES_DIARY_PATRON_GREAT_ITEM "an excellent item"
#define MES_DIARY_PATRON_CHAOS_WP "chaos weapon"
#define MES_DIARY_PATRON_S_ENEMY "summoning hostile creatures"
#define MES_DIARY_PATRON_TY_CURSE "cursing"
#define MES_DIARY_PATRON_HAVOC "calling chaos"
#define MES_DIARY_PATRON_GAIN_ABL "increasing a stat"
#define MES_DIARY_PATRON_LOST_ABL "decreasing a stat"
#define MES_DIARY_PATRON_AUGM_ABL "increasing all stats"
#define MES_DIARY_PATRON_RUIN_ABL "decreasing all stats"
#define MES_DIARY_PATRON_POLY_WND "polymorphing wounds"
#define MES_DIARY_PATRON_HURT_LOT "generating disintegration ball"
#define MES_DIARY_PATRON_DESTRUCTION "*destruct*ing dungeon"
#define MES_DIARY_PATRON_GENOCIDE "genociding creatures"
#define MES_DIARY_PATRON_SERVANT "a servant"
#define MES_DIARY_PATRON_DEMONIC_SERVANT "a demonic servant"
#define MES_DIARY_PATRON_UNDEAD_SERVANT "an undead servant"
#define MES_DIARY_PATRON_HEALING "healing"
#define MES_DIARY_STAND_IN(PLACE) "You are standing in the %s.", (PLACE)
#define MES_DIARY_CHEAT_DEATH "                            but revived."
#define MES_DIARY_WINNER "become *WINNER* of D\'angband finely!"
#define MES_DIARY_DISCOVER_ART(ART_NAME) "discovered %s.\n", (ART_NAME)
#define MES_DIARY_DEFEAT_UNIQUE(UNIQUE_NAME) "defeated %s.\n", (UNIQUE_NAME)
#define MES_DIARY_FIX_QUEST(QUEST_NAME) "completed quest '%s'.\n", (QUEST_NAME)
#define MES_DIARY_AWAY_QUEST(QUEST_NAME) " run away from quest '%s'.\n", (QUEST_NAME)
#define MES_DIARY_FIX_R_QUEST(QUEST_NAME) " completed random quest '%s'\n", (QUEST_NAME)
#define MES_DIARY_AWAY_R_QUEST(QUEST_NAME) " ran away from quest '%s'.\n", (QUEST_NAME)
#define MES_DIARY_MAXDEPTH(DUNGEON_NAME, DEPTH) " reached level %d of %s for the first time.\n", (DUNGEON_NAME), (DEPTH)
#define MES_DIARY_TRUMP_TOWER_SET(NOTE, DUNGEON_NAME, DEPTH) " reset recall level of %s to %d %s.\n", (DUNGEON_NAME), (DEPTH1), (NOTE)
#define MES_DIARY_STAIR(TO, NOTE) " %s %s.\n", (NOTE), (TO)
#define MES_DIARY_RECALL_DEPTH(DUNGEON_NAME, DEPTH) " recalled to dungeon level %d of %s.\n", (DEPTH), (DUNGEON_NAME)
#define MES_DIARY_RECALL_SURFACE " recalled from dungeon to surface.\n"
#define MES_DIARY_TO_QUEST(QUEST_NAME) " entered the quest '%s'.\n", (QUEST_NAME)
#define MES_DIARY_LEV_TELE " Got out using teleport level.\n"
#define MES_DIARY_BUY(OBJECT_NAME) " bought %s.\n", (OBJECT_NAME)
#define MES_DIARY_SELL(OBJECT_NAME) " sold %s.\n", (OBJECT_NAME)
#define MES_DIARY_ARENA_WON(NUM, ENEMY_NAME) " " won the %d%s fight (%s).\n", (NUM),  get_ordinal_number_suffix(n), (ENEMY_NAME)
#define MES_DIARY_ARENA_LOST(NUM, ENEMY_NAME) " beaten by %s in the %d%s fight.\n", (ENEMY_NAME), (NUM),  get_ordinal_number_suffix(n)
#define MES_DIARY_ARENA_CHAMPION "                 won all fight to become a Champion.\n"
#define MES_DIARY_IDEN(OBJECT_NAME) " identified %s.\n", (OBJECT_NAME)
#define MES_DIARY_WIZ_TELE(TO) " wizard-teleport to %s.\n", (TO)
#define MES_DIARY_PAT_TELE(TO) " used Pattern to teleport to %s.\n", (TO)
#define MES_DIARY_LEVELUP(LEVEL) " reached player level %d.\n", (LEVEL)
#endif

#ifdef JP
#define MES_OPTION_CHANGE_CONFIG "�ݒ�ύX�R�}���h"
#define MES_OPTION_WARN_CHEAT "���\�t�͌����ď����ł��Ȃ��B"
#define MES_OPTION_WARN_CHEAT2 "<<  ����  >>\n���\�I�v�V��������x�ł��ݒ肷��ƁA�X�R�A�L�^���c��Ȃ��Ȃ�܂��I\n��ɉ������Ă��_���ł��̂ŁA�����҂�ڎw�����͂����̃I�v�V�����͂�\n����Ȃ��悤�ɂ��ĉ������B"
#define MES_OPTION_UNUSED "(���g�p)"
#define MES_OPTION_MAIN_TITLE "[ �I�v�V�����̐ݒ� ]"
#define MES_OPTION_CMD_HP_WARNING "�R�}���h: ��q�b�g�|�C���g�x��"
#define MES_OPTION_CMD_MP_CHECK "�R�}���h: �ᖂ�͐F臒l"
#define MES_OPTION_CMD_MP_PROMPT "�ᖂ��臒l (0-9) ESC�Ō���: "
#define MES_OPTION_CMD_HP_CURRENT(PERCENT) "���݂̒�q�b�g�|�C���g�x��: %d0%%", (PERCENT)
#define MES_OPTION_CMD_MP_CURRENT(PERCENT) "���݂̒ᖂ�͐F臒l: %d0%%", (PERCENT)
#define MES_OPTION_CMD_HP_PROMPT "��q�b�g�|�C���g�x�� (0-9) ESC�Ō���: "
#define MES_OPTION_CMD_AUTOSAVE_PROMPT(INFO) "%s ( ���^�[���Ŏ���, y/n �ŃZ�b�g, F �ŕp�x�����, ESC �Ō��� ) ", (INFO)
#define MES_OPTION_CMD_AUTOSAVE_FREQ(TURN) "�����Z�[�u�̕p�x�F %d �^�[����", (TURN)
#define MES_OPTION_CMD_WAIT "�R�}���h: ��{�E�F�C�g��"
#define MES_OPTION_CMD_VALUE(VALUE) "���݂̃E�F�C�g: %d (%d�~���b)", (VALUE), (VALUE)*(VALUE)*(VALUE)
#define MES_OPTION_CMD_WAIT_PROMPT "�E�F�C�g (0-9) ESC�Ō���: "
#define MES_OPTION_MACRO_PROMPT "�}�N���s��: "
#define MES_OPTION_MACRO_DESC "�J�[�\���L�[�̍��E�ŃJ�[�\���ʒu���ړ��BBackspace��Delete�ňꕶ���폜�B"
#define MES_OPTION_APPEND_KEYMAP "�L�[�z�u��ǉ����܂����B"
#define MES_OPTION_FOUND_KEYMAP "�L�[�z�u���m�F���܂����B"
#define MES_OPTION_FOUND_NO_KEYMAP "�L�[�z�u�͒�`����Ă��܂���B"
#define MES_OPTION_REMOVE_KEYMAP "�L�[�z�u���폜���܂����B"
#else
#define MES_OPTION_CHANGE_CONFIG "Pref"
#define MES_OPTION_WARN_CHEAT "Cheaters never win."
#define MES_OPTION_WARN_CHEAT2 ""
#define MES_OPTION_UNUSED "(Unused option)"
#define MES_OPTION_MAIN_TITLE [ Game Options ]"
#define MES_OPTION_CMD_HP_WARNING "Command: Hitpoint Warning"
#define MES_OPTION_CMD_MP_CHECK "Command: Mana Color Threshold"
#define MES_OPTION_CMD_MP_PROMPT "Mana color Threshold (0-9 or ESC to accept): "
#define MES_OPTION_CMD_HP_CURRENT(PERCENT) "Current hitpoint warning: %d0%%", (PERCENT)
#define MES_OPTION_CMD_MP_CURRENT(PERCENT) "Current mana color threshold: %d0%%", (PERCENT)
#define MES_OPTION_CMD_HP_PROMPT "Hitpoint Warning (0-9 or ESC to accept): "
#define MES_OPTION_CMD_AUTOSAVE_PROMPT(INFO) "%s (RET to advance, y/n to set, 'F' for frequency, ESC to accept) ", (INFO)
#define MES_OPTION_CMD_AUTOSAVE_FREQ(TURN) "Timed autosave frequency: every %d turns", (TURN)
#define MES_OPTION_CMD_WAIT "Command: Base Delay Factor"
#define MES_OPTION_CMD_VALUE(VALUE) "���݂̃E�F�C�g: %d (%d�~���b)", (VALUE), (VALUE)*(VALUE)*(VALUE)
#define MES_OPTION_CMD_WAIT_PROMPT "Delay Factor (0-9 or ESC to accept): "
#define MES_OPTION_MACRO_PROMPT "Action: "
#define MES_OPTION_MACRO_DESC "Press Left/Right arrow keys to move cursor. Backspace/Delete to delete a char."
#define MES_OPTION_APPEND_KEYMAP "Appended keymaps."
#define MES_OPTION_FOUND_KEYMAP "Found a keymap."
#define MES_OPTION_FOUND_NO_KEYMAP "Found no keymap."
#define MES_OPTION_REMOVE_KEYMAP "Removed a keymap."
#endif

#ifdef JP
#define MES_STATUS_SELECTION "['c'�Ŗ��O�ύX, 'f'�Ńt�@�C���֏��o, 'h'�Ń��[�h�ύX, ESC�ŏI��]"
#else
#define MES_STATUS_SELECTION "['c' to change name, 'f' to file, 'h' to change mode, or ESC]"
#endif

#ifdef JP
#define MES_DIARY_MENU "�L�^�̐ݒ�"
#define MES_DIARY_MENU1 "�L�^������"
#define MES_DIARY_MENU2 "���͂��L�^����"
#define MES_DIARY_MENU3 "���O�ɓ��薔�͊Ӓ肵�����̂��L�^����"
#define MES_DIARY_MENU4 "�L�^����������"
#define MES_DIARY_MENU5 "�v���C������L�^����/���~����"
#define MES_DIARY_ASK_ERASE "�{���ɋL�^���������܂����H"
#define MES_DIARY_DELETED "�L�^���������܂����B"
#define MES_DIARY_DELETE_FAIL(FILE) "%s �̏����Ɏ��s���܂����B", (FILE)
#else
#define MES_DIARY_MENU "Play Record"
#define MES_DIARY_MENU1 "Display your record"
#define MES_DIARY_MENU2 "Add record"
#define MES_DIARY_MENU3 "Record item you last get/identify"
#define MES_DIARY_MENU4 "Delete your record"
#define MES_DIARY_MENU5 "Record playing movie / or stop it"
#define MES_DIARY_ASK_ERASE "Do you really want to delete all your record? "
#define MES_DIARY_DELETED "deleted record."
#define MES_DIARY_DELETE_FAIL(FILE) "failed to delete %s.", (FILE)
#endif

#ifdef JP
#define MES_MUTATION_DONE "�̂��˂���n�߂�..."
#define MES_MUTATION_SOMETHING "�����ς�����C������I"
#else
#define MES_MUTATION_DONE "Your body starts to scramble..."
#define MES_MUTATION_SOMETHING "You feel different!"
#endif

#ifdef JP
#define MES_CAST_BROWSE "�ǂ�"
#define MES_CAST_DISABLE(REALM, CAST) "����%s��%s���Ƃ͂ł��܂���B", (REALM), (CAST)
#define MES_CAST_WHICH_BOOK "�ǂ̎��������g���܂���? "
#define MES_CAST_NO_BOOK "���������Ȃ��I"
#define MES_CAST_NONE "�������������Ȃ��I"
#define MES_CAST_NO_NEED_LEARN "�������w�K����K�v�͂Ȃ��I"
#define MES_CAST_FAILED(SPELL) "%s�����܂��������Ȃ������I", (SPELL)
#define MES_CAST_CHAOS_PENALTY "�J�I�X�I�Ȍ��ʂ𔭐������I"
#define MES_CAST_NECRO_PENALTY1 "�l�N���m�~�R����ǂ�Ő��C���������I"
#define MES_CAST_NECRO_PENALTY2 "�ɂ��I"
#define MES_CAST_MUSIC_PENALTY "����ȉ���������"
#define MES_CAST_FAINT "���_���W���������ċC�������Ă��܂����I"
#define MES_CAST_DAMAGE_HEALTH "�̂��������Ă��܂����I"
#define MES_CAST_WHICH_KNOW(SKILL, FROM, TO) "(%^s %c-%c, '*'�ňꗗ, ESC) �ǂ�%s�ɂ��Ēm��܂����H", (SKILL), (FROM), (TO), (SKILL)
#define MES_CAST_WHICH_USE(SKILL, FROM, TO) "(%^s %c-%c, '*'�ňꗗ, ESC) �ǂ�%s���g���܂����H", (SKILL), (FROM), (TO), (SKILL)
#define MES_CAST_ASK(SPELL) "%s�̖��@�������܂����H", (SPELL)

#else
#define MES_CAST_BROWSE "browse"
#define MES_CAST_DISABLE(REALM, CAST) "You may not %s that %s.", (CAST), (REALM)
#define MES_CAST_WHICH_BOOK "Use which book? "
#define MES_CAST_NO_BOOK "You have no spell books!"
#define MES_CAST_NONE "You cannot cast spells!"
#define MES_CAST_NO_NEED_LEARN "You don't have to learn spells!"
#define MES_CAST_FAILED(SPELL) "You failed to get the %s off!", (SPELL)
#define MES_CAST_CHAOS_PENALTY "You produce a chaotic effect!"
#define MES_CAST_NECRO_PENALTY1 "Your sanity is shaken by reading the Necronomicon!"
#define MES_CAST_NECRO_PENALTY2 "It hurts!"
#define MES_CAST_MUSIC_PENALTY "An infernal sound echoed."
#define MES_CAST_FAINT "You faint from the effort!"
#define MES_CAST_DAMAGE_HEALTH "You have damaged your health!"
#define MES_CAST_WHICH_KNOW(SKILL, FROM, TO) "(%^ss %c-%c, *=List, ESC=exit) Browse which %s? ", (SKILL), (FROM), (TO), (SKILL)
#define MES_CAST_WHICH_USE(SKILL, FROM, TO) "(%^ss %c-%c, *=List, ESC=exit) Use which %s? ", (SKILL), (FROM), (TO), (SKILL)
#define MES_CAST_ASK(SPELL) "Use %s? ", (SPELL)
#endif

#ifdef JP 
#define MES_DEBUG_FORBID "�f�o�b�O�R�}���h�͋�����Ă��܂���B "
#define MES_DEBUG_CHECK1 "�f�o�b�O�E�R�}���h�̓f�o�b�O�Ǝ����̂��߂̃R�}���h�ł��B"
#define MES_DEBUG_CHECK2 "�f�o�b�O�E�R�}���h���g���ƃX�R�A�͋L�^����܂���B"
#define MES_DEBUG_ASK "�{���Ƀf�o�b�O�E�R�}���h���g���܂���? "
#define MES_WIZARD_FORBID "�f�o�b�O�R�}���h�͋�����Ă��܂���B "
#define MES_WIZARD_CHECK1 "�E�B�U�[�h���[�h�̓f�o�b�O�Ǝ����̂��߂̃��[�h�ł��B "
#define MES_WIZARD_CHECK2 "��x�E�B�U�[�h���[�h�ɓ���ƃX�R�A�͋L�^����܂���B"
#define MES_WIZARD_ASK "�{���ɃE�B�U�[�h���[�h�ɓ��肽���̂ł���? "
#else
#define MES_DEBUG_FORBID "Use of debug command is not permitted."
#define MES_DEBUG_CHECK1 "The debug commands are for debugging and experimenting."
#define MES_DEBUG_CHECK2 "The game will not be scored if you use debug commands."
#define MES_DEBUG_ASK "Are you sure you want to use debug commands? "
#define MES_WIZARD_FORBID "Wizard mode is not permitted."
#define MES_WIZARD_CHECK1 "Wizard mode is for debugging and experimenting."
#define MES_WIZARD_CHECK2 "The game will not be scored if you enter wizard mode."
#define MES_WIZARD_ASK "Are you sure you want to enter wizard mode? "
#endif

#ifdef JP 
#define MES_STEED_RIDE_ON(STEED) "%s�ɏ�����B", (STEED)->name
#define MES_STEED_TAMED(STEED) "%s����Ȃ������B", (STEED)->name
#define MES_STEED_TAME_FAILED(STEED) "%s�ɐU�藎�Ƃ��ꂽ�I", (STEED)->name
#define MES_STEED_GOT_OFF(STEED) "%s����~�肽�B", (STEED)
#define MES_PET_DISMISS_ASK(PET, REMAIN) "%s������܂����H [Yes/No/Unnamed (%d��)]", (PET), (REMAIN)
#define MES_PET_RELEASE(PET) "%s ��������B", (PET)
#define MES_PET_DISMISS(NUM) "%d �̂̃y�b�g������܂����B", (NUM)
#define MES_PET_NOTHING "�y�b�g�����Ȃ��I"
#define MES_PET_NO_PET "���̃N���[�`���[�̓y�b�g�ł͂Ȃ��B"
#define MES_PET_CANNOT_CHANGE_NAME "���̃N���[�`���[�̖��O�͕ς����Ȃ��B"
#define MES_PET_CHANGE_NAME(NAME) "%s�ɖ��O������B", (NAME)
#define MES_PER_UNNAMED_DESC "'U'nnamed �́A��n�ȊO�̖��O�̂Ȃ��y�b�g������S�ĉ�����܂��B"
#define MES_STEED_BUMP_WALL(STEED) "%s����U�藎�Ƃ��ꂻ���ɂȂ��āA�ǂɂԂ������B", (STEED)
#define MES_STEED_GOOD_RANDING(STEED) "%s���痎�������A�󒆂ł��܂��̐��𗧂Ē����Ē��n�����B", (STEED)
#define MES_STEED_WAKE_UP(STEED) "%s���N�������B", (STEED)
#define MES_PET_OVER_UPKEEP "����Ȃɑ����̃y�b�g�𐧌�ł��Ȃ��I"
#define MES_PET_DISP_COST(UPKEEP_FACTOR) "�ێ��l�o�� %d%%", (UPKEEP_FACTOR)
#else
#define MES_STEED_GOT_OFF(STEED) "You have got off %s. ", (STEED)
#define MES_STEED_TAMED(STEED) "You tame %s.", (STEED)->name;
#define MES_STEED_TAME_FAILED(STEED) "You have thrown off by %s.", (STEED)->name
#define MES_PET_DISMISS_ASK(PET, REMAIN) "Dismiss %s? [Yes/No/Unnamed (%d remain)]", (PET), (REMAIN)
#define MES_PET_RELEASE(PET) "Dismissed %s.", (PET)
#define MES_PET_DISMISS(NUM) "You have dismissed %d pet%s.", (NUM), ((NUM) == 1 ? "" : "s")
#define MES_PET_NOTHING "You have no pets."
#define MES_PET_NO_PET "This creature is not a pet."
#define MES_PET_CANNOT_CHANGE_NAME "You cannot change name of this creature."
#define MES_PET_CHANGE_NAME(NAME) "Name %s.", (NAME)
#define MES_PER_UNNAMED_DESC "'U'nnamed means all your pets except named pets and your mount."
#define MES_STEED_BUMP_WALL(STEED) "You have nearly fallen from %s, but bumped into wall.", (STEED)
#define MES_STEED_GOOD_RANDING(STEED) "You are thrown from %s, but make a good landing.", (STEED)
#define MES_STEED_WAKE_UP(STEED) "You have waked %s up.", (STEED)
#define MES_PET_OVER_UPKEEP "Too many pets to control at once!"
#define MES_PET_DISP_COST(UPKEEP_FACTOR) "Upkeep: %d%% mana.", (UPKEEP_FACTOR)
#endif

#ifdef JP
#define MES_PETCOM_DISMISS_PET "�y�b�g�����"
#define MES_PETCOM_TARGETING(CURRENT) "�y�b�g�̃^�[�Q�b�g���w�� (���݁F%s)", (CURRENT)
#define MES_PETCOM_STAY_CLOSE "�߂��ɂ���"
#define MES_PETCOM_FOLLOW_ME "���ė���"
#define MES_PETCOM_SEEK_AND_DESTROY "�G�������ē|��"
#define MES_PETCOM_GIVE_ME_SPACE "��������Ă���"
#define MES_PETCOM_STAY_AWAY "����Ă���"
#define MES_PETCOM_OPEN_DOORS_ON "�h�A���J���� (����:ON)"
#define MES_PETCOM_OPEN_DOORS_OFF "�h�A���J���� (����:OFF)"
#define MES_PETCOM_OPEN_PICKUP_ITEM_ON "�A�C�e�����E�� (����:ON)"
#define MES_PETCOM_OPEN_PICKUP_ITEM_OFF "�A�C�e�����E�� (����:OFF)"
#define MES_PETCOM_ALLOW_TELEPORT_ON "�e���|�[�g�n���@���g�� (����:ON)"
#define MES_PETCOM_ALLOW_TELEPORT_OFF "�e���|�[�g�n���@���g�� (����:OFF)"
#define MES_PETCOM_ATTACK_SPELL_ON "�U�����@���g�� (����:ON)"
#define MES_PETCOM_ATTACK_SPELL_OFF "�U�����@���g�� (����:OFF)"
#define MES_PETCOM_SUMMON_SPELL_ON "�������@���g�� (����:ON)"
#define MES_PETCOM_SUMMON_SPELL_OFF "�������@���g�� (����:OFF)"
#define MES_PETCOM_INVOLVE_ON "�v���C���[���������ޔ͈͖��@���g�� (����:ON)"
#define MES_PETCOM_INVOLVE_OFF "�v���C���[���������ޔ͈͖��@���g�� (����:OFF)"
#define MES_PETCOM_GET_OFF "�y�b�g����~���"
#define MES_PETCOM_RIDE "�y�b�g�ɏ��"
#define MES_PETCOM_GIVE_PET_NAME "�y�b�g�ɖ��O������"
#else
#define MES_PETCOM_DISMISS_PET "dismiss pets"
#define MES_PETCOM_TARGETING(CURRENT) "specify a target of pet (now:%s)", (CURRENT)
#define MES_PETCOM_STAY_CLOSE "stay close"
#define MES_PETCOM_FOLLOW_ME "follow me"
#define MES_PETCOM_SEEK_AND_DESTROY "seek and destroy"
#define MES_PETCOM_GIVE_ME_SPACE "give me space"
#define MES_PETCOM_STAY_AWAY "stay away"
#define MES_PETCOM_OPEN_DOORS_ON "pets open doors (now On)"
#define MES_PETCOM_OPEN_DOORS_OFF "pets open doors (now Off)"
#define MES_PETCOM_OPEN_PICKUP_ITEM_ON "pets pick up items (now On)"
#define MES_PETCOM_OPEN_PICKUP_ITEM_OFF "pets pick up items (now Off)"
#define MES_PETCOM_ALLOW_TELEPORT_ON "allow teleport (now On)"
#define MES_PETCOM_ALLOW_TELEPORT_OFF "allow teleport (now Off)"
#define MES_PETCOM_ATTACK_SPELL_ON "allow cast attack spell (now On)"
#define MES_PETCOM_ATTACK_SPELL_OFF "allow cast attack spell (now Off)"
#define MES_PETCOM_SUMMON_SPELL_ON "allow cast summon spell (now On)"
#define MES_PETCOM_SUMMON_SPELL_OFF "allow cast summon spell (now Off)"
#define MES_PETCOM_INVOLVE_ON "allow involve player in area spell (now On)"
#define MES_PETCOM_INVOLVE_OFF "allow involve player in area spell (now Off)"
#define MES_PETCOM_GET_OFF "get off a pet"
#define MES_PETCOM_RIDE "ride a pet"
#define MES_PETCOM_GIVE_PET_NAME "name pets"
#endif

#ifdef JP
#define MES_TRAIT_DEFAULT_ACTIVATE(CASTER, TITLE) "%s��%s�𔭓������B", (CASTER), (TITLE)
#define MES_TRAIT_CRITICAL_STUN "�����悤�ȓ��ɂ�����B"
#define MES_TRAIT_CUT_PENALTY(CREATURE, PLAYER) "%s�ɂЂǂ����Ղ��c�����B", (CREATURE)
#define MES_TRAIT_RUINATION "�g���S������Ă��āA���C�������Ă����悤���B"
#define MES_TRAIT_KNOWLEDGE "�������g�̂��Ƃ������͕��������C������..."
#define MES_TRAIT_OKURE_NIISAN "�u�I�N���Z����I�v"
#define MES_TRAIT_SELF_DETONATION "�̂̒��Ō������������N�����I"
#define MES_TRAIT_CLOUDY_MIND "���_�ɂ��₪���������I"
#define MES_TRAIT_EXPAND_MIND "���_���L�������I"
#define MES_TRAIT_WASTING "���������サ�Ă����̂�������I"
#define MES_TRAIT_ATT_DRAGON "�h���S���������񂹂��I"
#define MES_TRAIT_ATT_ANIMAL "�����������񂹂��I"
#define MES_TRAIT_NAUSEA "�݂��z�����A�H�����������I"
#define MES_TRAIT_ADD_CURSED(OBJECT) "���ӂɖ����������I�[����%s���Ƃ�܂���...", (OBJECT)->name
#define MES_TRAIT_BERS_RAGE1 "�E�K�@�@�A�I"
#define MES_TRAIT_BERS_RAGE2 "���{�̔���ɏP��ꂽ�I"
#define MES_TRAIT_COWARDICE "�ƂĂ��Â�... �ƂĂ������I"
#define MES_TRAIT_CURSED_TELEPORT "���Ȃ��̈ʒu�͓ˑR�Ђ��傤�ɕs�m��ɂȂ���..."
#define MES_TRAIT_BANISH_NO_TARGET "�׈��ȑ��݂������Ƃ�܂���I"
#define MES_TRAIT_BANISH_DONE "�׈��ȃN���[�`���[�͗����L�����ƂƂ��ɏ����������I"
#define MES_TRAIT_BANISH_UNAFFECTED "�F��͌��ʂ��Ȃ������I"
#define MES_TRAIT_FLATULENT "�u�D�[�[�b�I�����ƁB"
#define MES_TRAIT_PROD_MANA "���@�̃G�l���M�[���ˑR���Ȃ��̒��ɗ��ꍞ��ł����I�G�l���M�[��������Ȃ���΂Ȃ�Ȃ��I"
#define MES_TRAIT_VAMPIRISM_NO_TARGET "�����Ȃ��ꏊ�Ɋ��݂����I"
#define MES_TRAIT_VAMPIRISM_DONE "���Ȃ��̓j�����Ƃ��ĉ���ނ���..."
#define MES_TRAIT_VAMPIRISM_FAILED "�����B�Ђǂ������B"
#define MES_TRAIT_VAMPIRISM_NO_HUNGER "���Ȃ��͋󕠂ł͂���܂���B"
#define MES_TRAIT_EAT_ROCK_CANNOT "���̒n�`�͐H�ׂ��Ȃ��B"
#define MES_TRAIT_EAT_ROCK_PERMANENT(FEATURE_NAME) "���Ă��I����%s�͂��Ȃ��̎����d���I", (FEATURE_NAME)
#define MES_TRAIT_ACROBAT(CREATURE) "%^s�͓ˑR���E���������!", (CREATURE)->name
#define MES_TRAIT_ACROBAT_EFFECT(CREATURE) "%^s�����Ȃ���͂�ŋ󒆂��瓊�����Ƃ����B", (CREATURE)->name
#define MES_TRAIT_ACROBAT_FLOAT_GENTLY(CREATURE) "%^s�͐Â��ɒ��n�����B", (CREATURE)->name
#define MES_TRAIT_ACROBAT_CRUSH(CREATURE) "%^s�͒n�ʂɒ@������ꂽ�B", (CREATURE)->name
#define MES_TRAIT_INROU_SUKE "�w������x�����ꂽ�B"
#define MES_TRAIT_INROU_KAKU "�w�i����x�����ꂽ�B"
#define MES_TRAIT_BR_SOUN_JAIAN "�u�{�H�G�`�`�`�`�`�`�v"
#define MES_TRAIT_BR_SHAR_BOTEI "�u�{��r���J�b�^�[�I�I�I�v"
#define MES_TRAIT_BO_FIRE_RORENTO_BLIND(CASTER) "%s�������𓊂����B", (CASTER)->name
#define MES_TRAIT_BO_FIRE_RORENTO(CASTER) "%s�͎�֒e�𓊂����B", (CASTER)->name
#define MES_TRAIT_BLINK_DONE(CASTER) "%^s���u���ɏ������B", (CASTER)->name
#define MES_TRAIT_TELEPORT_DONE(CASTER) "%^s���e���|�[�g�����B", (CASTER)->name
#define MES_TRAIT_TELEPORT_BLOCK(CASTER) "���@�̃o���A��%^s�̃e���|�[�g���ז������B", (CASTER)->name
#define MES_TRAIT_SPLIT_TWO_MEN(CASTER) "%^s�����􂵂��I", (CASTER)->name
#define MES_TRAIT_COMBINE "�w�o�[�m�[���x�Ɓw���p�[�g�x�����̂����I"
#define MES_TRAIT_TELEPORT_BACK_DONE(TARGET) "%s�������߂��ꂽ�B", (TARGET)->name
#define MES_TRAIT_WATER_FLOW "�n�ʂ��琅�������o�����I"
#define MES_TRAIT_S_CYBER_BLIND "�d���ȑ������߂��ŕ�������B"
#define MES_TRAIT_RUSH1 "���[���������������������������������������������I�I�I"
#define MES_TRAIT_RUSH2 "�I���I���I���I���I���I���I���I���I���I���I���I���I�I�I"
#define MES_TRAIT_RUSH3 "���ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʖ��ʁI�I�I"
#define MES_TRAIT_CONCENTRATION "���_���W�����ċC�����𗭂߂��B"
#define MES_TRAIT_SWORD_DANCING_MISS(CASTER) "$^s�̍U��������������B", (CASTER)->name
#define MES_TRAIT_EAT_ROCK_HATE_TREE "�؂̖��͍D������Ȃ��I"
#define MES_TRAIT_EAT_ROCK_HATE_GLASS "�K���X�̖��͍D������Ȃ��I"
#define MES_TRAIT_EAT_ROCK_DONE(FEATURE) "����%s�͂ƂĂ����������I", (FEATURE)
#define MES_TRAIT_RECALL_UP "��Ɉ������肠�����銴��������I"
#define MES_TRAIT_RECALL_DOWN "���Ɉ�������~�낳��銴��������I"
#define MES_TRAIT_ALTER_REAL_DONE "���E���ς�����I"
#define MES_TRAIT_ALTER_REAL_FAILED "���E�������̊ԕω������悤���B"
#else
#define MES_TRAIT_DEFAULT_ACTIVATE(CASTER, TITLE) "%s invoked '%s'", (CASTER), (TITLE)
#define MES_TRAIT_CRITICAL_STUN "A vicious blow hits your head."
#define MES_TRAIT_CUT_PENALTY(CREATURE, PLAYER) "%s %s been horribly scarred.", (CREATURE), (PLAYER) ? "have" : "has"
#define MES_TRAIT_RUINATION "Your nerves and muscles feel weak and lifeless!"
#define MES_TRAIT_KNOWLEDGE "You begin to know yourself a little better..."
#define MES_TRAIT_OKURE_NIISAN "Brother OKURE!"
#define MES_TRAIT_SELF_DETONATION "Massive explosions rupture your body!"
#define MES_TRAIT_CLOUDY_MIND "Your mind feels cloudy!"
#define MES_TRAIT_EXPAND_MIND "Your mind expands!"
#define MES_TRAIT_WASTING "You can feel yourself wasting away!"
#define MES_TRAIT_ATT_DRAGON "You have attracted a dragon!"
#define MES_TRAIT_ATT_ANIMAL "You have attracted an animal!"
#define MES_TRAIT_NAUSEA "Your stomach roils, and you lose your lunch!"
#define MES_TRAIT_ADD_CURSED(OBJECT) "There is a malignant black aura surrounding your %s...", (OBJECT)->name
#define MES_TRAIT_BERS_RAGE1 "RAAAAGHH!"
#define MES_TRAIT_BERS_RAGE2 "You feel a fit of rage coming over you!"
#define MES_TRAIT_COWARDICE "It's so dark... so scary!"
#define MES_TRAIT_CURSED_TELEPORT "Your position suddenly seems very uncertain..."
#define MES_TRAIT_BANISH_NO_TARGET "You sense no evil there!"
#define MES_TRAIT_BANISH_DONE "The evil creature vanishes in a puff of sulfurous smoke!"
#define MES_TRAIT_BANISH_UNAFFECTED "The invocation is ineffectual."
#define MES_TRAIT_FLATULENT "BRRAAAP! Oops."
#define MES_TRAIT_PROD_MANA "Magical energy flows through you! You must release it!"
#define MES_TRAIT_VAMPIRISM_NO_TARGET "You bite into thin air!"
#define MES_TRAIT_VAMPIRISM_DONE "You grin and bare your fangs..."
#define MES_TRAIT_VAMPIRISM_FAILED "Yechh. That tastes foul."
#define MES_TRAIT_VAMPIRISM_NO_HUNGER "You were not hungry."
#define MES_TRAIT_EAT_ROCK_CANNOT "You cannot eat this feature."
#define MES_TRAIT_EAT_ROCK_PERMANENT(FEATURE_NAME) "Ouch!  This %s is harder than your teeth!", (FEATURE_NAME)
#define MES_TRAIT_ACROBAT(CREATURE) "%^s suddenly go out of your sight!", (CREATURE)->name
#define MES_TRAIT_ACROBAT_EFFECT(CREATURE) "%^s holds you, and drops from the sky.", (CREATURE)->name
#define MES_TRAIT_ACROBAT_FLOAT_GENTLY(CREATURE) "You floated gently down to the ground.", (CREATURE)->name
#define MES_TRAIT_ACROBAT_CRUSH(CREATURE) "You crashed into the ground.", (CREATURE)->name
#define MES_TRAIT_INROU_SUKE "Suke-san apperars."
#define MES_TRAIT_INROU_KAKU "Kaku-san appears."
#define MES_TRAIT_BR_SOUN_JAIAN "'Booooeeeeee'"
#define MES_TRAIT_BR_SHAR_BOTEI "'Boty-Build cutter!!!'"
#define MES_TRAIT_BO_FIRE_RORENTO_BLIND(CASTER) "%^s throws something.", (CASTER)->name
#define MES_TRAIT_BO_FIRE_RORENTO(CASTER) "%^s throws a hand grenade.", (CASTER)->name
#define MES_TRAIT_BLINK_DONE(CASTER) "%^s blinks", (CASTER)->name
#define MES_TRAIT_TELEPORT_DONE(CASTER) "%^s teleports away.", (CASTER)->name
#define MES_TRAIT_TELEPORT_BLOCK(CASTER) "Magic barrier obstructs teleporting of %^s.", (CASTER)->name
#define MES_TRAIT_SPLIT_TWO_MEN(CASTER) "%^s splits in two person!", (CASTER)->name
#define MES_TRAIT_COMBINE "Banor and Rupart combine into one!"
#define MES_TRAIT_TELEPORT_BACK_DONE(TARGET) "You command %s to return.", (TARGET)->name
#define MES_TRAIT_WATER_FLOW "Water blew off from the ground!"
#define MES_TRAIT_S_CYBER_BLIND "You hear heavy steps nearby."
#define MES_TRAIT_RUSH1 "Ahhhtatatatatatatatatatatatatatataatatatatattaaaaa!!!!"
#define MES_TRAIT_RUSH2 "Oraoraoraoraoraoraoraoraoraoraoraoraoraoraoraoraora!!!!"
#define MES_TRAIT_RUSH3 "Mudamudamudamudamudamudamudamudamudamudamudamudamudamudamudarrrr!!!!"
#define MES_TRAIT_CONCENTRATION "You concentrate to charge your power."
#define MES_TRAIT_SWORD_DANCING_MISS(CASTER) "$^s attack the empty air.", (CASTER)->name
#define MES_TRAIT_EAT_ROCK_HATE_TREE "You don't like the woody taste!"
#define MES_TRAIT_EAT_ROCK_HATE_GLASS "You don't like the glassy taste!"
#define MES_TRAIT_EAT_ROCK_DONE(FEATURE) "This %s is very filling!", (FEATURE)
#define MES_TRAIT_RECALL_UP "You feel yourself yanked upwards!"
#define MES_TRAIT_RECALL_DOWN "You feel yourself yanked downwards!"
#define MES_TRAIT_ALTER_REAL_DONE "The world changes!"
#define MES_TRAIT_ALTER_REAL_FAILED "The world seems to change for a moment!"
#endif

#ifdef JP
#define MES_POSTURE_NO_POSTURE "���Ƃ��ƍ\���Ă��Ȃ��B"
#define MES_POSTURE_S_STEALTH_LITE "�e�̕��������ꂽ�C������B"
#define MES_POSTURE_S_STEALTH_DARK "�e�̕������Z���Ȃ����I"
#define MES_POSTURE_REASSUME "�\���������B"
#define MES_POSTURE_ASSUMED(POSTURE) "%s�̍\�����Ƃ����B", (POSTURE)
#else
#define MES_POSTURE_NO_POSTURE "You are not assuming a posture."
#define MES_POSTURE_S_STEALTH_LITE "Your mantle of shadow become thin."
#define MES_POSTURE_S_STEALTH_DARK "Your mantle of shadow restored its original darkness."
#define MES_POSTURE_REASSUME "You reassume a posture."
#define MES_POSTURE_ASSUMED(POSTURE) "You assume a posture of %s form.", (POSTURE)
#endif

#ifdef JP
#define MES_SNIPE_CONCENTRATE(LEVEL) "�W�������B(�W���x %d)", (LEVEL)
#define MES_SNIPE_RESET_CONS "�W���͂��r�؂ꂽ�B"
#define MES_SNIPE_STRAIN "�ˌ��̔������̂��P�����B"
#else
#define MES_SNIPE_CONCENTRATE(LEVEL) "You concentrate deeply. (level %d)", (LEVEL)
#define MES_SNIPE_RESET_CONS "Stop concentrating."
#define MES_SNIPE_STRAIN "A reactionary of shooting attacked you. "
#endif

#ifdef JP
#define MES_ENCHANT_WHICH_ITEM "�ǂ̃A�C�e�����������܂���? "
#define MES_ENCHANT_NO_ITEM "�����ł���A�C�e�����Ȃ��B"
#define MES_ENCHANT_WHICH_WEAPON "�ǂ̕�����������܂���? "
#define MES_ENCHANT_NO_WEAPON "�����ł��镐�킪�Ȃ��B"
#define MES_ENCHANT_FAILED "�����t���Ɏ��s�����B"
#define MES_ENCHANT_SUCCESS(ITEM, POS, NUM) "%s�͖��邭�P�����I", (ITEM)
#define MES_ENCHANT_BECOME_ARTIFACT(ITEM, POS, NUM) "%s��ῂ����𔭂����I", (ITEM)
#define MES_ENCHANT_RANGE(LIMIT) "���݂̂��Ȃ��̋Z�ʂ��ƁA+%d �܂ŉ��ǂł��܂��B", (LIMIT)
#define MES_ENCHANT_SHORTAGE_GOLD(ITEM) "%s�����ǂ��邾���̃S�[���h������܂���I", (ITEM)
#define MES_ENCHANT_COST(COST) " ���ǂ̗����͈�ɂ���%d �ł��B", (COST)
#define MES_ENCHANT_DONE(COST, OBJECT) "��%d��%s�ɉ��ǂ��܂����B", (COST), (OBJECT)->name
#define MES_RESIZE_RANGE(LIMIT) "���݂̂��Ȃ��̋Z�ʂ��ƁA-%d����+%d �܂ŏC���ł��܂��B", (LIMIT), (LIMIT)
#define MES_RESIZE_DEPEND "�C���̗����̓A�C�e���̉��l�Ɉˑ����܂��B"
#define MES_RESIZE_COST(OBJECT, COST) "%s�̉��ǂɂ� $%d ������܂��I", (OBJECT)->name, (COST)
#define MES_RESIZE_COST_ASK(OBJECT, COST) "%s�̉��ǂɂ� $%d ������܂��A��낵���ł����H", (OBJECT)->name, (COST)
#define MES_RESIZE_NO_NEED "���ǂ̕K�v�͂���܂���B"
#define MES_RESIZE_DONE(OBJECT) "%s�̑傫���𒲐������B", (OBJECT)->name
#else
#define MES_ENCHANT_WHICH_ITEM "Enchant which item? "
#define MES_ENCHANT_NO_ITEM "You have nothing to enchant."
#define MES_ENCHANT_WHICH_WEAPON "Enchant which weapon? "
#define MES_ENCHANT_NO_WEAPON "You have nothing to enchant."
#define MES_ENCHANT_FAILED "The Branding failed."
#define MES_ENCHANT_SUCCESS(ITEM, POS, NUM) "%s %s glow%s brightly!", (((POS) >= 0) ? "Your" : "The"), (ITEM), (((NUM) > 1) ? "" : "s")
#define MES_ENCHANT_BECOME_ARTIFACT(ITEM, POS, NUM)) "%s %s radiate%s a blinding light!", (((POS) >= 0) ? "Your" : "The"), (ITEM), (((NUM) > 1) ? "" : "s")
#define MES_ENCHANT_RANGE(LIMIT) "  Based on your skill, we can improve up to +%d.", (LIMIT)
#define MES_ENCHANT_SHORTAGE_GOLD(ITEM) "You do not have the gold to improve %s!", (ITEM)
#define MES_ENCHANT_COST(COST) "  The price for the service is %d gold per item.", (COST)
#define MES_ENCHANT_DONE(COST, OBJECT) "Improved into %s for %d gold.", (OBJECT)->name, (COST)
#define MES_RESIZE_RANGE(LIMIT) "  Based on your skill, we can resize from -%d to +%d.", (LIMIT), (LIMIT)
#define MES_RESIZE_DEPEND "  The price for the service depend on value of the item."
#define MES_RESIZE_COST(OBJECT, COST) "To improve %s cost $%d!", (OBJECT)->name, (COST)
#define MES_RESIZE_COST_ASK(OBJECT, COST) "To improve %s cost $%d, all right?", (OBJECT)->name, (COST)
#define MES_RESIZE_NO_NEED "No improvement is required."
#define MES_RESIZE_DONE(OBJECT) "%s was made over.", (OBJECT)->name
#endif

#ifdef JP
#define MES_POKER_ROYAL_FLUSH "���C�����X�g���[�g�t���b�V��"
#define MES_POKER_STRAIGHT_FLUSH "�X�g���[�g�t���b�V��"
#define MES_POKER_STRAIGHT "�X�g���[�g"
#define MES_POKER_FLUSH "�t���b�V��"
#define MES_POKER_ONE_PAIR "�����y�A"
#define MES_POKER_TWO_PAIR "�c�[�y�A"
#define MES_POKER_THREE_OF_A_KIND "�X���[�J�[�h"
#define MES_POKER_FULL_HOUSE "�t���n�E�X"
#define MES_POKER_FOUR_OF_A_KIND "�t�H�[�J�[�h"
#define MES_POKER_FIVE_ACE "�t�@�C�u�G�[�X"
#define MES_POKER_FIVE_OF_A_KIND "�t�@�C�u�J�[�h"
#else
#define MES_POKER_ROYAL_FLUSH "Royal Flush"
#define MES_POKER_STRAIGHT_FLUSH "Straight Flush"
#define MES_POKER_STRAIGHT "Straight"
#define MES_POKER_FLUSH "Flush"
#define MES_POKER_ONE_PAIR "One pair"
#define MES_POKER_TWO_PAIR "Two pair"
#define MES_POKER_THREE_OF_A_KIND "Three of a kind"
#define MES_POKER_FULL_HOUSE "Full house"
#define MES_POKER_FOUR_OF_A_KIND "Four of a kind"
#define MES_POKER_FIVE_ACE "Five ace"
#define MES_POKER_FIVE_OF_A_KIND "Five of a kind"
#endif

#ifdef JP
#define MES_CURSE_PREVENT_TAKE_OFF "�Ӂ[�ށA�ǂ�������Ă���悤���B"
#define MES_CURSE_PREVENT_TAKE_OFF2 "�Ȃ�Ă������I���Ȃ��͐_��̗͂Ɏ���Ă���I"
#define MES_CURSE_PREVENT_TAKE_OFF3 "�������O���Ȃ������B"
#define MES_CURSE_PREVENT_TAKE_OFF4(OBJECT) "%s�͎���Ă���悤���B", (OBJECT)->name
#define MES_CURSE_RESIST_FORCE "���ꂽ������͂Â��Ŕ��������I"
#define MES_CURSE_RESIST_DIVINE "���Ȃ��̐_��̗͎͂􂢂𗽉킵�Ă���B���Ȃ��͕��R�Ǝ􂢂̑������O�����B"
#else
#define MES_CURSE_PREVENT_TAKE_OFF "Hmmm, it seems to be cursed."
#define MES_CURSE_PREVENT_TAKE_OFF2 "What the hell! You are cursed by divine power!"
#define MES_CURSE_PREVENT_TAKE_OFF3 "You couldn't remove the equipment."
#define MES_CURSE_PREVENT_TAKE_OFF4(OBJECT) "The %s appears to be cursed.", (OBJECT)->name
#define MES_CURSE_RESIST_FORCE "You teared a cursed equipment off by sheer strength!"
#define MES_CURSE_RESIST_DIVINE "Your divine power is exceeding curse. You teared a cursed equipment off calmly."
#endif

#ifdef JP
#define MES_FILE_SAVED(FILENAME) "%s�ɏ������݂܂����B", (FILENAME)
#define MES_FILE_SAVED_FAIL(FILENAME) "%s�ւ̏������݂Ɏ��s���܂����B", (FILENAME)
#define MES_FILE_LOADED(FILENAME) "%s��ǂݍ��݂܂����B", (FILENAME)
#define MES_FILE_LOADED_FAIL(FILENAME) "%s�̓ǂݍ��݂Ɏ��s���܂����B", (FILENAME)
#else
#define MES_FILE_SAVED(FILENAME) "Saved '%s'.", (FILENAME)
#define MES_FILE_SAVED_FAIL(FILENAME) "Failed to save '%s'.", (FILENAME)
#define MES_FILE_LOADED(FILENAME) "Loaded '%s'.", (FILENAME)
#define MES_FILE_LOADED_FAIL(FILENAME) "Failed to load '%s'.", (FILENAME)
#endif

#ifdef JP
#define MES_INSCRIPTION_VIEW(FEATURE) "%s�Ƀ��b�Z�[�W�����܂�Ă���:", (FEATURE)
#else
#define MES_INSCRIPTION_VIEW(FEATURE) "You find the following inscription on %s.", (FEATURE)
#endif

#ifdef JP
#define MES_RACIAL_RISK_ASK "�{���ɍ��̐��サ����Ԃł��̔\�͂��g���܂����H"
#define MES_RACIAL_FAILED "�[���ɏW���ł��Ȃ������B"
#else
#define MES_RACIAL_RISK_ASK "Really use the power in your weakened state? "
#define MES_RACIAL_FAILED "You've failed to concentrate hard enough."
#endif

#ifdef JP
#define MES_MACRO_ADDED "�}�N����ǉ����܂����B"
#define MES_MACRO_NOT_FOUND "���̃L�[�ɂ̓}�N���͒�`����Ă��܂���B"
#define MES_MACRO_FOUND "�}�N�����m�F���܂����B"
#define MES_MACRO_DELETE "�}�N�����폜���܂����B"
#else
#define MES_MACRO_ADDED "Appended macros."
#define MES_MACRO_NOT_FOUND "Found no macro."
#define MES_MACRO_FOUND "Found a macro"
#define MES_MACRO_DELETE "Removed a macro."
#endif

#ifdef JP
#define MES_INSCRIPTION_DONE "�����̐^���Ƀ��b�Z�[�W������:"
#define MES_INSCRIPTION_VANISHED "���b�Z�[�W�͏����������B"
#else
#define MES_INSCRIPTION_DONE "Inscribe messages on your floor."
#define MES_INSCRIPTION_VANISHED "Messages vanished."
#endif

#ifdef JP
#define MES_LITE_FAINT "�����肪�����ɂȂ��Ă��Ă���B"
#define MES_LITE_OUT "�����肪�����Ă��܂����I"
#define MES_LITE_NONE "�����𑕔����Ă��Ȃ��B"
#define MES_LITE_NO_REFILL "���̌����͎��������΂��Ȃ��B"
#define MES_LITE_FUEL_LAMP "�����v�ɖ��𒍂����B"
#define MES_LITE_FUEL_GONE "�����v�������Ă��܂����I"
#define MES_LITE_FUEL_DARKEN "�����������v�͑S������Ȃ��B"
#define MES_LITE_FUEL_FULL "�����v�̖��͈�t���B"
#define MES_LITE_TORCH_COMBINE "���������������B"
#define MES_LITE_TORCH_GONE "�����������Ă��܂����I"
#define MES_LITE_TORCH_DARKEN "�����������͑S������Ȃ�"
#define MES_LITE_TORCH_FULL "�����̎����͏\�����B"
#define MES_LITE_TORCH_GLOW "�����͂����������邭�P�����B"
#define MES_LITE_PHLOGISTON_NONE "�R�f�������A�C�e���𑕔����Ă��܂���B"
#define MES_LITE_PHLOGISTON_NO_MORE "���̃A�C�e���ɂ͂���ȏ�R�f���[�ł��܂���B"
#define MES_LITE_PHLOGISTON_REFUEL "�Ɩ��p�A�C�e���ɔR�f���[�����B"
#define MES_LITE_PHLOGISTON_FULL "�Ɩ��p�A�C�e���͖��^���ɂȂ����B"
#else
#define MES_LITE_FAINT "Your light is growing faint."
#define MES_LITE_OUT "Your light has gone out!"
#define MES_LITE_NONE "You are not wielding a light."
#define MES_LITE_NO_REFILL "Your light cannot be refilled."
#define MES_LITE_FUEL_LAMP "You fuel your lamp."
#define MES_LITE_FUEL_GONE "Your lamp has gone out!"
#define MES_LITE_FUEL_DARKEN "Curiously, your lamp doesn't light."
#define MES_LITE_FUEL_FULL "Your lamp is full."
#define MES_LITE_TORCH_COMBINE "You combine the torches."
#define MES_LITE_TORCH_GONE "Your torch has gone out!"
#define MES_LITE_TORCH_DARKEN "Curiously, your torche don't light."
#define MES_LITE_TORCH_FULL "Your torch is fully fueled."
#define MES_LITE_TORCH_GLOW "Your torch glows more brightly."
#define MES_LITE_PHLOGISTON_NONE "You are not wielding anything which uses phlogiston
#define MES_LITE_PHLOGISTON_NO_MORE "No more phlogiston can be put in this item."
#define MES_LITE_PHLOGISTON_REFUEL "You add phlogiston to your light item."
#define MES_LITE_PHLOGISTON_FULL "Your light item is full."
#endif

#ifdef JP
#define MES_PREVENT_BY_RIDING "��n���ɂ͏o���Ȃ��B"
#define MES_PREVENT_BY_CONFUSION "������Ԃŏ�肭�o���Ȃ��B"
#define MES_PREVENT_BY_BLIND "�Ⴊ�����Ȃ��ďo���Ȃ��B"
#define MES_PREVENT_BY_NO_LITE "�����肪�Ȃ��ďo���Ȃ��B"
#define MES_PREVENT_BY_HALLUCINATION "���o�ɑj�܂�Ăł��Ȃ��B"
#define MES_PREVENT_BY_STUNED "�����N�O�Ƃ��Ă��ďW���ł��Ȃ��B"
#define MES_PREVENT_BY_AFRAID "���|�ɋ����Ăł��Ȃ�"
#define MES_PREVENT_BY_PET "���̓y�b�g�𑀂邱�ƂɏW�����Ă��Ȃ��ƁB"
#define MES_PREVENT_BY_NO_MIRROR "���̏�łȂ��ƏW���ł��Ȃ��B"
#define MES_PREVENT_BY_NO_BAREBAND "�f�肶��Ȃ��Ƃł��܂���B"
#define MES_PREVENT_BY_NO_SHOOT "�ˌ��p�̕���������Ă��Ȃ��B"
#define MES_PREVENT_BY_NO_BOW "�|�𑕔����Ă��Ȃ��B"
#define MES_PREVENT_ACTIVATE_ONLY "���̕���͔������Ďg�����̂̂悤���B"
#define MES_PREVENT_BY_HP "HP������܂���."
#define MES_PREVENT_BY_MP "MP������܂���B"
#define MES_PREVENT_BY_LEVEL "���̔\�͂��g�p����ɂ̓��x�� %d �ɒB���Ă��Ȃ���΂Ȃ�܂���B"
#define MES_PREVENT_BY_ANTIMAGIC "�����@�o���A�����@���ז������I"
#define MES_PREVENT_BY_TIMESTOP "�~�܂������̒��ł͂��܂������Ȃ��悤���B"
#define MES_PREVENT_BY_HUNGER "���̑O�ɐH�����Ƃ�Ȃ��ƁB"
#define MES_PREVENT_BY_POSTURE "���͍\���ɏW�����Ă���B"
#define MES_PREVENT_BY_CREATURE(NAME) "%s���ז����B", (NAME)
#define MES_PREVENT_FISHING_BY_FEATURE "�����͐��ӂł͂Ȃ��B"
#define MES_PREVENT_MAGIC_BY_DUNGEON "�_���W���������@���z�������I"
#define MES_PREVENT_LITE_BY_DUNGEON "�_���W�����������z�������B"
#define MES_PREVENT_TELEPORT_BY_CREATURE "�e���|�[�g���ז����ꂽ�I"
#define MES_PREVENT_QUAFF_BY_TIMESTOP "�r����t�̂�����o�Ă��Ȃ��I"
#else
#define MES_PREVENT_BY_RIDING "You cannot do it when riding."
#define MES_PREVENT_BY_CONFUSION "You are too confused."
#define MES_PREVENT_BY_BLIND "You are blind!"
#define MES_PREVENT_BY_NO_LITE "You have no light to read by."
#define MES_PREVENT_BY_HALLUCINATION "You are too hallucinated!"
#define MES_PREVENT_BY_STUNED "You are too stuned!"
#define MES_PREVENT_BY_AFRAID "You are trembling with fear."
#define MES_PREVENT_BY_PET "You need concentration on the pets now."
#define MES_PREVENT_BY_NO_MIRROR "Here are not any mirrors!"
#define MES_PREVENT_BY_NO_BAREBAND "You need to be bare hand."
#define MES_PREVENT_BY_NO_SHOOT "You have nothing to fire with."
#define MES_PREVENT_BY_NO_BOW "You wield no bow!"
#define MES_PREVENT_ACTIVATE_ONLY "Do activate."
#define MES_PREVENT_BY_HP "You do not have enough hit-point to use this power."
#define MES_PREVENT_BY_MP "You do not have enough mana to use this power."
#define MES_PREVENT_BY_LEVEL "You need to attain level %d to use this power."
#define MES_PREVENT_BY_ANTIMAGIC "An anti-magic shell disrupts your magic!"
#define MES_PREVENT_BY_TIMESTOP "It shows no reaction."
#define MES_PREVENT_BY_HUNGER "You must eat something here."
#define MES_PREVENT_BY_POSTURE "You need concentration on your form."
#define MES_PREVENT_BY_CREATURE(NAME) "%^s is stand in your way.", (NAME)
#define MES_PREVENT_MAGIC_BY_DUNGEON "This spell can only be used on the surface!"
#define MES_PREVENT_FISHING_BY_FEATURE "There is no fishing place."
#define MES_PREVENT_LITE_BY_DUNGEON "The darkness of this dungeon absorb your light."
#define MES_PREVENT_TELEPORT_BY_CREATURE "Your teleportation is blocked!"
#define MES_PREVENT_QUAFF_BY_TIMESTOP "The potion doesn't flow out from a bottle."
#endif

#ifdef JP
#define MES_CLOSE_NO_TARGET "�����ɂ͕�����̂���������Ȃ��B"
#define MES_DOOR_STUCK "�������������ĕ܂�Ȃ��B"
#define MES_DOOR_BROKEN "���͉��Ă��܂��Ă���B"
#else
#define MES_CLOSE_NO_TARGET "You see nothing there to close."
#define MES_DOOR_STUCK "There seems stuck."
#define MES_DOOR_BROKEN "The door appears to be broken."
#endif

#ifdef JP
#define MES_GAINMAGIC_LIST "[A] ��, [B] ���@�_, [C] ���b�h:"
#define MES_GAINMAGIC_NO_TYPE "���̎�ނ̖��@�͊o���Ă��Ȃ��B"
#define MES_GAINMAGIC_CHARGING "���̖��@�͂܂��[�U���Ă���Œ����B"
#define MES_GAINMAGIC_NO_LEFT "���̖��@�͎g�p�񐔂��؂�Ă���B"
#define MES_GAINMAGIC_WHICH_ITEM "�ǂ̃A�C�e���̖��͂���荞�݂܂���? "
#define MES_GAINMAGIC_NO_ITEM "���͂���荞�߂�A�C�e�����Ȃ��B"
#define MES_GAINMAGIC_NO_MAGIC "���̏�ɂ͔����ׂ̈̔\�͉͂���������Ă��Ȃ��悤���B"
#define MES_GAINMAGIC_NEED_IDENTIFY "�Ӓ肳��Ă��Ȃ��Ǝ�荞�߂Ȃ��B"
#define MES_GAINMAGIC_STILL_CHARGING "�[�U���̃A�C�e���͎�荞�߂Ȃ��B"
#define MES_GAINMAGIC_ABSORBED(OBJECT) "%s�̖��͂���荞�񂾁B", (OBJECT)
#define MES_GAINMAGIC_NONE "���@���o���Ă��Ȃ��I"
#else
#define MES_GAINMAGIC_LIST "[A] staff, [B] wand, [C] rod:"
#define MES_GAINMAGIC_NO_TYPE "You don't have that type of magic."
#define MES_GAINMAGIC_CHARGING "The magic are still charging."
#define MES_GAINMAGIC_NO_LEFT "The magic has no charges left."
#define MES_GAINMAGIC_WHICH_ITEM "Gain power of which item? "
#define MES_GAINMAGIC_NO_ITEM "You have nothing to gain power."
#define MES_GAINMAGIC_NO_MAGIC "This staff doesn't have any magical ability."
#define MES_GAINMAGIC_NEED_IDENTIFY "You need to identify before absorbing."
#define MES_GAINMAGIC_STILL_CHARGING "This item is still charging."
#define MES_GAINMAGIC_ABSORBED(OBJECT) "You absorb magic of %s.", (OBJECT)
#define MES_GAINMAGIC_NONE "You don't have any magic!"
#endif

#ifdef JP
#define MES_VISUAL_COL_SYM(COL, SYM) "���ݒl  �F / ���� = %3d / %3d", (COL), (SYM)
#define MES_VISUAL_RESET "��ʏ��[�F/����]�������l�Ƀ��Z�b�g���܂����B"
#else
#define MES_VISUAL_COL_SYM(COL, SYM) "Current attr/char = %3d / %3d", (COL), (SYM)
#define MES_VISUAL_RESET "Visual attr/char tables reset."
#endif

#ifdef JP
#define MES_CR_DESC_RIDING "(��n��)"
#define MES_CR_DESC_CHAMELEON "(�J�����I��)"
#define MES_CR_DESC_CHAMELEON_LORD "(�J�����I���̉�)"
#else
#define MES_CR_DESC_RIDING "(Riding)"
#define MES_CR_DESC_CHAMELEON "(Chameleon)"
#define MES_CR_DESC_CHAMELEON_LORD "(Chameleon Lord)"
#endif

#ifdef JP
#define MES_OBJ_DESC_TO_SPEED "����"
#define MES_OBJ_DESC_ATTACK "�U��"
#define MES_OBJ_DESC_STEALTH "�B��"
#define MES_OBJ_DESC_SEARCH "�T��"
#define MES_OBJ_DESC_INFRA "�ԊO������"
#else
#define MES_OBJ_DESC_TO_SPEED "to speed"
#define MES_OBJ_DESC_ATTACK " attack"
#define MES_OBJ_DESC_STEALTH " to stealth"
#define MES_OBJ_DESC_SEARCH " to searching"
#define MES_OBJ_DESC_INFRA " to infravision"
#endif

#ifdef JP
#define MES_INN_FULL "���͖������B"
#define MES_INN_FOOD "�o�[�e���͂����炩�̐H�ו��ƃr�[�������ꂽ�B"
#define MES_INN_CANCEL1 "���Ȃ��ɕK�v�Ȃ͕̂����ł͂Ȃ��A���Î҂ł��B"
#define MES_INN_CANCEL2 "���݂܂���A�ł������ŒN���Ɏ��Ȃꂿ�።��܂���ŁB"
#define MES_INN_NIGHTMARE "���Ȃ��͐⋩���Ėڂ��o�܂����B"
#else
#define MES_INN_FULL "You are full now."
#define MES_INN_FOOD "The barkeep gives you some gruel and a beer."
#define MES_INN_CANCEL1 "You need a healer, not a room."
#define MES_INN_CANCEL2 "Sorry, but don't want anyone dying in here."
#define MES_INN_NIGHTMARE "You awake screaming."
#endif

#ifdef JP
#define MES_RESEARCH_FULL_LIST "�S�N���[�`���[�̃��X�g"
#define MES_RESEARCH_UNIQUE_LIST "���j�[�N�E�N���[�`���[�̃��X�g"
#define MES_RESEARCH_NON_UNIQUE_LIST "�񃆃j�[�N�E�N���[�`���[�̃��X�g"
#else
#define MES_RESEARCH_FULL_LIST "Full creature list."
#define MES_RESEARCH_UNIQUE_LIST "Unique creature list."
#define MES_RESEARCH_NON_UNIQUE_LIST "Non-unique creature list."
#endif

#ifdef JP
#define MES_HEX_WHICH_CURSE "�ǂ���􂢂܂����H"
#define MES_HEX_WHICH_NO_WEAPON "����𑕔����Ă��Ȃ��B"
#define MES_HEX_WHICH_NO_ARMOUR "�h��𑕔����Ă��Ȃ��B"
#define MES_HEX_START_PATIENT "�����Ƒς��邱�Ƃɂ����B"
#define MES_HEX_ALREADY_PATIENT "���łɉ䖝�����Ă���B"
#define MES_HEX_REVENGE_DONE "�䖝�������ꂽ�I"
#define MES_HEX_WIZ_REVENGE(POWER) "%d�_�̃_���[�W��Ԃ����B", (POWER)
#else
#define MES_HEX_WHICH_CURSE "Which weapon do you curse?"
#define MES_HEX_WHICH_NO_WEAPON "You wield no weapons."
#define MES_HEX_WHICH_NO_ARMOUR "You wield no piece of armours."
#define MES_HEX_ALREADY_PATIENT "You are already patienting."
#define MES_HEX_START_PATIENT "You decide to patient all damages."
#define MES_HEX_REVENGE_DONE "Time for end of patioence!"
#define MES_HEX_WIZ_REVENGE(POWER) "You return %d damages.", (POWER)
#endif

#ifdef JP
#define MES_AVTIVATE_FAILED "���܂��n�������邱�Ƃ��ł��Ȃ������B"
#define MES_AVTIVATE_TIMEOUT "����͔����ɉ��𗧂āA�P���A������..."
#define MES_AVTIVATE_DONE "�n��������..."
#else
#define MES_AVTIVATE_FAILED "You failed to activate it properly."
#define MES_AVTIVATE_TIMEOUT "It whines, glows and fades..."
#define MES_AVTIVATE_DONE "You activate it..."
#endif

#ifdef JP
#define MES_TUNNEL_NONE "�����ɂ͉�����������Ȃ��B"
#define MES_TUNNEL_NO_TARGET "�����ɂ͌@����̂���������Ȃ��B"
#else
#define MES_TUNNEL_NONE "You see nothing there."
#define MES_TUNNEL_NO_TARGET "You see nothing there to tunnel."
#endif

#ifdef JP
#define MES_EAT_LESS_EFFECT_BY_VAMPIRE "���Ȃ��̂悤�Ȏ҂ɂƂ��ĐH�ƂȂǋ͂��ȉh�{�ɂ����Ȃ�Ȃ��B"
#define MES_EAT_LESS_EFFECT_BY_VAMPIRE2 "���Ȃ��̋Q���͐V�N�Ȍ��ɂ���Ă̂ݖ��������I"
#define MES_EAT_LESS_EFFECT_MORTAL_FOOD "���҂̐H���͂��Ȃ��ɂƂ��ĂقƂ�ǉh�{�ɂȂ�Ȃ��B"
#else
#define MES_EAT_LESS_EFFECT_BY_VAMPIRE "Mere victuals hold scant sustenance for a being such as yourself."
#define MES_EAT_LESS_EFFECT_BY_VAMPIRE2 "Your hunger can only be satisfied with fresh blood!"
#define MES_EAT_LESS_EFFECT_MORTAL_FOOD "The food of mortals is poor sustenance for you."
#endif

#ifdef JP
#define MES_COMBATOP_WEAPON_ONE_HANDED "�����Ў�Ŏ���"
#define MES_COMBATOP_WEAPON_TWO_HANDED "����𗼎�Ŏ���"
#define MES_COMBATOP_BAREHAND_ONE_HANDED "�Ў�Ŋi������"
#define MES_COMBATOP_BAREHAND_TWO_HANDED "����Ŋi������"
#define MES_COMBATOP_BAREHAND_ON "�i�����s��"
#define MES_COMBATOP_BAREHAND_OFF "�i�����s��Ȃ�"
#else
#define MES_COMBATOP_WEAPON_ONE_HANDED "use one hand to control a riding pet"
#define MES_COMBATOP_WEAPON_TWO_HANDED "use both hands for a weapon"
#define MES_COMBATOP_BAREHAND_ONE_HANDED "use one hand to control a riding pet"
#define MES_COMBATOP_BAREHAND_TWO_HANDED "use both hands for melee"
#define MES_COMBATOP_BAREHAND_ON "use one hand for melee"
#define MES_COMBATOP_BAREHAND_OFF "use one hand to control a riding pet"
#endif

#ifdef JP
#define MES_FOOD_WHICH_EAT "�ǂ��H�ׂ܂���? "
#define MES_FOOD_NO_EAT "�H�ו����Ȃ��B"
#define MES_FOOD_BISCUIT "�Â��ăT�N�T�N���ĂƂĂ����������B"
#define MES_FOOD_RATION "����͂��������B"
#define MES_FOOD_JERKY "���������������Ă��������B"
#define MES_FOOD_SLIME_MOLD "����͂Ȃ�Ƃ��`�e�������������B"
#define MES_FOOD_WAYBREAD "����͂Ђ��傤�ɔ������B"
#define MES_FOOD_DRINK "�̂ǂ����u�₩���B"
#define MES_FOOD_SKELETON_FAILED1 "�H�ו����A�S��f�ʂ肵�ė������I"
#define MES_FOOD_SKELETON_FAILED2 "�H�ו����A�S��f�ʂ肵�ė����A�������I"
#else
#define MES_FOOD_WHICH_EAT "Eat which item? "
#define MES_FOOD_NO_EAT "You have nothing to eat."
#define MES_FOOD_BISCUIT "That tastes good."
#define MES_FOOD_RATION "That tastes good."
#define MES_FOOD_JERKY "That tastes good."
#define MES_FOOD_SLIME_MOLD "That tastes good."
#define MES_FOOD_WAYBREAD "That tastes good."
#define MES_FOOD_DRINK "That tastes good."
#define MES_FOOD_SKELETON_FAILED1 "The food falls through your jaws!"
#define MES_FOOD_SKELETON_FAILED2 "The food falls through your jaws and vanishes!"
#endif

#ifdef JP
#define MES_QUAFF_WHICH_POTION "�ǂ̖�����݂܂���? "
#define MES_QUAFF_NO_POTION "���߂�򂪂Ȃ��B"
#define MES_QUAFF_WATER "���̒��������ς肵���B"
#define MES_QUAFF_APPLE_JUICE "�Â��ăT�b�p���Ƃ��Ă��āA�ƂĂ����������B"
#define MES_QUAFF_LESS_THIRSTY "�̂ǂ̊��������������܂����B"
#define MES_QUAFF_DRUNKER1 "�m��Ȃ��ꏊ�Ŗڂ����߂��B���ɂ�����B"
#define MES_QUAFF_DRUNKER2 "�����v���o���Ȃ��B�ǂ�����Ă����֗����̂���������Ȃ��I"
#define MES_EFFECT_LOST_MEMORY "�ߋ��̋L��������Ă����C������B"
#define MES_QUAFF_SKELETON_EFFECT "�t�̂̈ꕔ�͂��Ȃ��̃A�S��f�ʂ肵�ė������I"
#define MES_QUAFF_FLASK "�I�C����⋋�����B"
#define MES_QUAFF_MOIST "��������荞�񂾁B"
#else
#define MES_QUAFF_WHICH_POTION "Quaff which potion? "
#define MES_QUAFF_NO_POTION "You have no potions to quaff."
#define MES_QUAFF_WATER ""
#define MES_QUAFF_APPLE_JUICE ""
#define MES_QUAFF_LESS_THIRSTY "You feel less thirsty."
#define MES_QUAFF_DRUNKER1 "You wake up somewhere with a sore head..."
#define MES_QUAFF_DRUNKER2 "You can't remember a thing, or how you got here!"
#define MES_EFFECT_LOST_MEMORY "You feel your memories fade."
#define MES_QUAFF_SKELETON_EFFECT "Some of the fluid falls through your jaws!"
#define MES_QUAFF_FLASK "You replenish yourself with the oil."
#define MES_QUAFF_MOIST "You are moistened."
#endif

#ifdef JP
#define MES_SCROLL_WHICH_READ "�ǂ̊�����ǂ݂܂���? "
#define MES_SCROLL_NO_READ "�ǂ߂銪�����Ȃ��B"
#define MES_SCROLL_RUMOR_DEFAULT "�R�̉\������B"
#define MES_SCROLL_DISABLE "�����Ȃ�ēǂ߂Ȃ��B"
#define MES_SCROLL_USED "�����͉��𗧂Ăď����������I"
#define MES_SCROLL_MESSAGE "�����ɂ̓��b�Z�[�W��������Ă���:"
#else
#define MES_SCROLL_WHICH_READ "Read which scroll? "
#define MES_SCROLL_NO_READ "You have no scrolls to read."
#define MES_SCROLL_RUMOR_DEFAULT "Some rumors are wrong."
#define MES_SCROLL_DISABLE "You cannot read."
#define MES_SCROLL_USED "The scroll disappears in a puff of smoke!"
#define MES_SCROLL_MESSAGE "There is message on the scroll. It says:"
#endif

#ifdef JP
#define MES_GAMBLE_BETWEEN "�C���E�r�g�C�[��"
#define MES_GAMBLE_CRAPS "�N���b�v�X"
#define MES_GAMBLE_WHEEL "���[���b�g"
#define MES_GAMBLE_DICESLOTS "�_�C�X�E�X���b�g"
#define MES_GAMBLE_CRAPS_FIRST(ROLL1, ROLL2, ROLL3) "�P�U���: %d %d      Total: %d", (ROLL1), (ROLL2), (ROLL3)
#define MES_GAMBLE_CRAPS_RESULT(ROLL1, ROLL2, ROLL3) "�o��: %d %d          ���v:      %d", (ROLL1), (ROLL2), (ROLL3)
#define MES_GAMBLE_ARENA_WHICH "�ǂ�ɓq���܂���:"
#define MES_GAMBLE_ARENA_ODDS_LIST "�N���[�`���[                                                    �{��"
#define MES_GAMBLE_ARENA_ODDS_LIST2(NUM, SPECIES, ODDS) "%d) %-58s  %4ld.%02ld�{", (NUM)+1, format("%s%s",(SPECIES)->name + species_name, has_trait_species((SPECIES), TRAIT_UNIQUE) ? "���ǂ�" : "      "), (ODDS)/100, (ODDS)%100
#define MES_GAMBLE_WON "���Ȃ��̏���"
#define MES_GAMBLE_LOST "���Ȃ��̕���"
#define MES_GAMBLE_TOTAL_WON "�u����ׂ͖����ȁI�ł����͂������������Ă�邩��ȁA��΂ɁI�v"
#define MES_GAMBLE_TOTAL_LOST "�u�����X�b�Ă��܂����ȁA��͂́I�����ɋA��������������"
#define MES_GAMBLE_PAYOFF(ODDS) "�{��: %d", (ODDS)
#define MES_GAMBLE_AGAIN "������x(Y/N)�H"
#define MES_GAMBLE_BEFORE_MONEY(MONEY) "�Q�[���O�̏�����: %9ld", (MONEY)
#define MES_GAMBLE_CURRENT_MONEY(MONEY) "���݂̏�����:     %9ld", (MONEY)
#define MES_GAMBLE_CURRENT_WAGER(MONEY) "���݂̊|����:     %9ld", (MONEY)
#define MES_GAMBLE_ROLE_AGAIN "�Ȃɂ��L�[�������Ƃ������U��܂��B"
#define MES_GAMBLE_BET(BET) "�q���� (1-%ld)�H", (BET)
#define MES_GAMBLE_BET_LIMIT(MONEY) "%ld�S�[���h�����󂯂悤�B�c��͎���Ƃ��ȁB", (MONEY)
#define MES_GAMBLE_BET_UNDERLINE "�n�j�A1�S�[���h�ł������B"
#define MES_GAMBLE_BET_OUT_NUM1 "0�Ԃɂ��Ƃ����B"
#define MES_GAMBLE_BET_OUT_NUM2 "�n�j�A9�Ԃɂ��Ƃ����B"
#define MES_GAMBLE_WHEEL_WHICH_NUM "���ԁH (0-9)"
#define MES_GAMBLE_ROULETTE_RESULT(NUM) "���[���b�g�͉��A�~�܂����B���҂� %d�Ԃ��B", (NUM)
#else
#define MES_GAMBLE_BETWEEN "In Between"
#define MES_GAMBLE_CRAPS "Craps"
#define MES_GAMBLE_WHEEL "Wheel"
#define MES_GAMBLE_DICESLOTS "Dice Slot"
#define MES_GAMBLE_CRAPS_FIRST(ROLL1, ROLL2, ROLL3) "First roll: %d %d    Total: %d", (ROLL1), (ROLL2), (ROLL3)
#define MES_GAMBLE_CRAPS_RESULT(ROLL1, ROLL2, ROLL3) "Roll result: %d %d   Total:     %d", (ROLL1), (ROLL2), (ROLL3)
#define MES_GAMBLE_ARENA_WHICH "Which creature: "
#define MES_GAMBLE_ARENA_ODDS_LIST "Creatures                                                       Odds"
#define MES_GAMBLE_ARENA_ODDS_LIST2(NUM, SPECIES, ODDS) "%d) "%d) %-58s  %4ld.%02ld", (NUM)+1, format("%s%s",(SPECIES)->name + species_name, has_trait_species(species_ptr, TRAIT_UNIQUE) ? "Fake " : ""), (ODDS)/100, (ODDS)%100
#define MES_GAMBLE_WON "YOU WON"
#define MES_GAMBLE_LOST "You Lost"
#define MES_GAMBLE_TOTAL_WON "You came out a winner! We'll win next time, I'm sure."
#define MES_GAMBLE_TOTAL_LOST "You lost gold! Haha, better head home."
#define MES_GAMBLE_PAYOFF(ODDS) "Payoff: %d", (ODDS)
#define MES_GAMBLE_AGAIN "Again(Y/N)?"
#define MES_GAMBLE_BEFORE_MONEY(MONEY) "Gold before game: %9ld", (MONEY)
#define MES_GAMBLE_CURRENT_MONEY(MONEY) "Current Gold:     %9ld", (MONEY)
#define MES_GAMBLE_CURRENT_WAGER(MONEY) "Current Wager:    %9ld", (MONEY)
#define MES_GAMBLE_ROLE_AGAIN "Hit any key to roll again"
#define MES_GAMBLE_BET(BET) "Your wager (1-%ld) ? ", (BET)
#define MES_GAMBLE_BET_LIMIT(MONEY) "I'll take %ld gold of that. Keep the rest.", (MONEY)
#define MES_GAMBLE_BET_UNDERLINE "Ok, we'll start with 1 gold."
#define MES_GAMBLE_BET_OUT_NUM1 "I'll put you down for 0."
#define MES_GAMBLE_BET_OUT_NUM2 "Ok, I'll put you down for 9."
#define MES_GAMBLE_WHEEL_WHICH_NUM "Pick a number (0-9)"
#define MES_GAMBLE_ROULETTE_RESULT(NUM) "The wheel spins to a stop and the winner is %d", (NUM)
#endif

#ifdef JP
#define MES_INFO_UNKNOWN_ARTIFACT "���m�̃A�[�e�B�t�@�N�g"
#define MES_INFO_KNOWN_ARTIFACT "���m�̃A�[�e�B�t�@�N�g"
#define MES_INFO_KILL_COUNT "�E�����N���[�`���[�̐�"
#define MES_INFO_BOUNTY "�܋���̈ꗗ"
#define MES_INFO_BOUNTY_NONE "�܋���͂����c���Ă��܂���B"
#define MES_INFO_DUNGEON "���܂łɓ������_���W����"
#define MES_INFO_SELF "�����Ɋւ�����"
#define MES_INFO_QUEST "�N�G�X�g�B����"
#define MES_INFO_HOME "�䂪�Ƃ̃A�C�e��"
#define MES_INFO_AUTOPICK "�����E��/�j�� �ݒ胊�X�g"
#define MES_INFO_AUTOPICK_NO_LINE "�����j��/�E���ɂ͉����o�^����Ă��܂���B"
#define MES_INFO_AUTOPICK_LINE(NUM) "   �����E��/�j��ɂ͌��� %d�s�o�^����Ă��܂��B\n\n", (NUM)
#else
#define MES_INFO_UNKNOWN_ARTIFACT "Unknown Artifact"
#define MES_INFO_KNOWN_ARTIFACT "Artifacts Seen"
#define MES_INFO_KILL_COUNT "Kill Count"
#define MES_INFO_BOUNTY "Wanted creatures"
#define MES_INFO_BOUNTY_NONE "There is no more wanted creature."
#define MES_INFO_DUNGEON "Dungeon"
#define MES_INFO_SELF "HP-rate & Max stat"
#define MES_INFO_QUEST "Quest status"
#define MES_INFO_HOME "Home inventory"
#define MES_INFO_AUTOPICK "Auto-picker/Destroyer"
#define MES_INFO_AUTOPICK_NO_LINE "No preference for auto picker/destroyer."
#define MES_INFO_AUTOPICK_LINE(NUM) "   There are %d registered lines for auto picker/destroyer.\n\n", (NUM)
#endif

#ifdef JP
#define MES_QUERY_ALL_CREATURE "�S�N���[�`���[�̃��X�g"
#define MES_QUERY_UNIQUE "���j�[�N�E�N���[�`���[�̃��X�g"
#define MES_QUERY_NON_UNIQUE "���j�[�N�O�N���[�`���[�̃��X�g"
#define MES_QUERY_STEED "��n�\�N���[�`���[�̃��X�g"
#define MES_QUERY_INPUT_SYM "�m�肽����������͂��ĉ�����(�L�� or ^A�S,^U��,^N��,^R��n,^M���O): "
#define MES_QUERY_INPUT_ENTER_NAME "���O(�p��̏ꍇ�������ŉ�)"
#define MES_QUERY_MARCH(KEYWORD) "���O:%s�Ƀ}�b�`", (KEYWORD)
#define MES_QUERY_ASK_RECALL "�v���o�����܂���? (k:�E�Q��/y/n): "
#define MES_QUERY_INTERFACE_RECALL "['r'�v���o, ESC]"
#else
#define MES_QUERY_ALL_CREATURE "Full creature list."
#define MES_QUERY_UNIQUE "Unique creature list."
#define MES_QUERY_NON_UNIQUE "Non-unique creature list."
#define MES_QUERY_STEED "Ridable creature list."
#define MES_QUERY_INPUT_SYM "Enter character to be identified(^A:All,^U:Uniqs,^N:Non uniqs,^M:Name): "
#define MES_QUERY_INPUT_ENTER_NAME "Enter name:"
#define MES_QUERY_MARCH(KEYWORD) "Creatures with a name \"%s\"", (KEYWORD)
#define MES_QUERY_ASK_RECALL "Recall details? (k/y/n): "
#define MES_QUERY_INTERFACE_RECALL "[(r)ecall, ESC]"
#endif

#ifdef JP
#define MES_BECOME_BRAND_SHARPNESS "�͉s���𑝂����I"
#define MES_BECOME_BRAND_EARTHQUAKE "�͔j��͂𑝂����I"
#define MES_BECOME_BRAND_SLAYHUMAN "�͐l�Ԃ̌������߂Ă���I"
#define MES_BECOME_BRAND_ELEC "�͓d���ɕ���ꂽ�I"
#define MES_BECOME_BRAND_ACID "�͎_�ɕ���ꂽ�I"
#define MES_BECOME_BRAND_SLAYEVIL "�͎׈��Ȏ҂ւ̍ق������߂Ă���I"
#define MES_BECOME_BRAND_SLAYDEMON "�ِ͈��E�̏Z�l�̓��̂����߂Ă���I"
#define MES_BECOME_BRAND_SLAYUNDEAD "�͕s���҂����߂Ă���I"
#define MES_BECOME_BRAND_SLAYANIMAL "�͓����̌������߂Ă���I"
#define MES_BECOME_BRAND_SLAYDRAGON "�̓h���S���̌������߂Ă���I"
#define MES_BECOME_BRAND_SLAYTROLL "�̓g�����̌������߂Ă���I"
#define MES_BECOME_BRAND_SLAYORC "�̓I�[�N�̌������߂Ă���I"
#define MES_BECOME_BRAND_SLAYGIANT "�͋��l�̌������߂Ă���I"
#define MES_BECOME_BRAND_TRUMP "�͔��ɕs����ɂȂ����悤���B"
#define MES_BECOME_BRAND_VAMPIRIC "�͌������߂Ă���I"
#define MES_BECOME_BRAND_POIS "�͓łɕ���ꂽ�B"
#define MES_BECOME_BRAND_CHAOS "�͏����O���X�Ɉ��ݍ��܂ꂽ�B"
#define MES_BECOME_BRAND_FIRE "�͉��̃V�[���h�ɕ���ꂽ�I"
#define MES_BECOME_BRAND_COLD "�͐[���₽���u���[�ɋP�����I"
#define MES_BECOME_BRAND_FORMAT(OBJECT, BRAND) "���Ȃ���%s%s", (OBJECT), (BRAND)
#else
#define MES_BECOME_BRAND_SHARPNESS "becomes very sharp!"
#define MES_BECOME_BRAND_EARTHQUAKE "seems very powerful."
#define MES_BECOME_BRAND_SLAYHUMAN "seems to be looking for humans!"
#define MES_BECOME_BRAND_ELEC "covered with lightning!"
#define MES_BECOME_BRAND_ACID "coated with acid!"
#define MES_BECOME_BRAND_SLAYEVIL "seems to be looking for evil creatures!"
#define MES_BECOME_BRAND_SLAYDEMON "seems to be looking for demons!"
#define MES_BECOME_BRAND_SLAYUNDEAD "seems to be looking for undead!"
#define MES_BECOME_BRAND_SLAYANIMAL "seems to be looking for animals!"
#define MES_BECOME_BRAND_SLAYDRAGON "seems to be looking for dragons!"
#define MES_BECOME_BRAND_SLAYTROLL "seems to be looking for troll!s"
#define MES_BECOME_BRAND_SLAYORC "seems to be looking for orcs!"
#define MES_BECOME_BRAND_SLAYGIANT "seems to be looking for giants!"
#define MES_BECOME_BRAND_TRUMP "seems very unstable now."
#define MES_BECOME_BRAND_VAMPIRIC "thirsts for blood!"
#define MES_BECOME_BRAND_POIS "is coated with poison."
#define MES_BECOME_BRAND_CHAOS "is engulfed in raw Logrus!"
#define MES_BECOME_BRAND_FIRE "is covered in a fiery shield!"
#define MES_BECOME_BRAND_COLD "glows deep, icy blue!"
#define MES_BECOME_BRAND_FORMAT(OBJECT, BRAND) "Your %s %s", (OBJECT), (BRAND)
#endif

#ifdef JP
#define MES_FETCH_STAND "�����̑��̉��ɂ��镨�͎��܂���B"
#define MES_FETCH_TOO_FAR "����Ȃɉ����ɂ��镨�͎��܂���I"
#define MES_FETCH_NO_OBJECT "�����ɂ͉�������܂���B"
#define MES_FETCH_OUT_CONTROL "�A�C�e�����R���g���[�����O��ė������B"
#define MES_FETCH_OUT_SIGHT "�����͂��Ȃ��̎��E�ɓ����Ă��܂���B"
#define MES_FETCH_WALL "�����͕ǂ̌������ł��B"
#define MES_FETCH_TOO_HEAVY "���̃A�C�e���͏d�߂��܂��B"
#define MES_FETCH_DONE(OBJECT) "%^s�����Ȃ��̑����ɔ��ł����B", (OBJECT)->name
#else
#define MES_FETCH_STAND "You can't fetch when you're already standing on something."
#define MES_FETCH_TOO_FAR "You can't fetch something that far away!"
#define MES_FETCH_NO_OBJECT "There is no object at this place."
#define MES_FETCH_OUT_CONTROL "The item slips from your control."
#define MES_FETCH_OUT_SIGHT "You have no direct line of sight to that location."
#define MES_FETCH_WALL "You have no direct line of sight to that location."
#define MES_FETCH_TOO_HEAVY "The object is too heavy."
#define MES_FETCH_DONE(OBJECT) "%^s flies through the air to your feet.", (OBJECT)->name
#endif

#ifdef JP
#define MES_ALT_REAL_START "���̌i�F���ς��n�߂�..."
#define MES_ALT_REAL_CANCEL "�i�F�����ɖ߂���..."
#else
#define MES_ALT_REAL_START "The view around you begins to change..."
#define MES_ALT_REAL_CANCEL "The view around you got back..."
#endif

#ifdef JP
#define MES_DISENCHANT_NO_EFFECT(OBJECT) "������%s�ɂ͌��ʂ��Ȃ������I", (OBJECT)->name
#define MES_DISENCHANT_DAMAGED(OBJECT) "%s���_���[�W���󂯂��I", (OBJECT)->name
#else
#define MES_DISENCHANT_NO_EFFECT(OBJECT) "Your %s is unaffected!", (OBJECT)->name
#define MES_DISENCHANT_DAMAGED(OBJECT) "Your %s is damaged!", (OBJECT)->name
#endif

#ifdef JP
#define MES_RUSTPROOF_WHICH_OBJECT "�ǂ̖h��ɎK�~�߂����܂����H"
#define MES_RUSTPROOF_NO_OBJECT "�K�~�߂ł�����̂�����܂���B"
#define MES_RUSTPROOF_AS_NEW(OBJECT) "%s�͐V�i���l�ɂȂ����I", (OBJECT)->name
#define MES_RUSTPROOF_PROTECTED(OBJECT) "%s�͕��H���Ȃ��Ȃ����B", (OBJECT)->name
#else
#define MES_RUSTPROOF_WHICH_OBJECT "Rustproof which piece of armour? "
#define MES_RUSTPROOF_NO_OBJECT "You have nothing to rustproof."
#define MES_RUSTPROOF_AS_NEW(OBJECT) "The %s look%s as good as new!", (OBJECT)->name, (((OBJECT)->number > 1) ? "" : "s")
#define MES_RUSTPROOF_PROTECTED(OBJECT) "The %s %s now protected against corrosion.", (OBJECT)->name, (((OBJECT)->number > 1) ? "are" : "is")
#endif

#ifdef JP
#define MES_DDOOR_FAILED "����E���畨���E�ɖ߂鎞���܂������Ȃ������I"
#define MES_DMIRROR_FAILED "���̐��E�����܂��ʂ�Ȃ������I"
#else
#define MES_DDOOR_FAILED "You fail to exit the astral plane correctly!"
#define MES_DMIRROR_FAILED "You fail to pass the mirror plane correctly!"
#endif

#ifdef JP
#define MES_EATMAGIC_WHICH_ITEM "�ǂ̃A�C�e�����疂�͂��z�����܂����H"
#define MES_EATMAGIC_NO_ITEM "���͂��z���ł���A�C�e��������܂���B"
#define MES_EATMAGIC_DISCHARGED_ROD "�[�U���̃��b�h���疂�͂��z�����邱�Ƃ͂ł��܂���B"
#define MES_EATMAGIC_EMPTY "�z���ł��閂�͂�����܂���I"
#define MES_EATMAGIC_BACKFIRE(OBJECT) "���͂��t�������I%s�͊��S�ɖ��͂��������B", (OBJECT)->name
#define MES_EATMAGIC_LOST_MANA(OBJECT) "%s�͔j����Ƃꂽ���A���͂��S�Ď���ꂽ�B", (OBJECT)->name
#else
#define MES_EATMAGIC_WHICH_ITEM "Drain which item? "
#define MES_EATMAGIC_NO_ITEM "You have nothing to drain."
#define MES_EATMAGIC_DISCHARGED_ROD "You can't absorb energy from a discharged rod."
#define MES_EATMAGIC_EMPTY "There's no energy there to absorb!"
#define MES_EATMAGIC_BACKFIRE(OBJECT) "The recharging backfires - %s is completely drained!", (OBJECT)->name
#define MES_EATMAGIC_LOST_MANA(OBJECT) "You save your %s from destruction, but all charges are lost.", (OBEJCT)->name
#endif

#ifdef JP
#define MES_PLEFFILE_TOO_MACRO "�}�N���g���K�[�̐ݒ肪�������܂�!"
#define MES_PLEFFILE_INVALID_OPTION(NAME) "�I�v�V�����̖��O������������܂���F %s", (NAME)
#else
#define MES_PLEFFILE_TOO_MACRO "Too many macro triggers!"
#define MES_PLEFFILE_INVALID_OPTION(NAME) "Ignored invalid option: %s", (NAME)
#endif

#ifdef JP
#define MES_CREATURE_LIFE_COND1 "����"
#define MES_CREATURE_LIFE_COND2 "�y��"
#define MES_CREATURE_LIFE_COND3 "����"
#define MES_CREATURE_LIFE_COND4 "�d��"
#define MES_CREATURE_LIFE_COND5 "��������"
#define MES_CREATURE_NO_LIFE_COND1 "���_���[�W"
#define MES_CREATURE_NO_LIFE_COND2 "���_���[�W"
#define MES_CREATURE_NO_LIFE_COND3 "���_���[�W"
#define MES_CREATURE_NO_LIFE_COND4 "��_���[�W"
#define MES_CREATURE_NO_LIFE_COND5 "�|�ꂩ��"
#define MES_CREATURE_LIFERATE_NOW(NUM) "���݂̗̑̓����N�� %d/100 �ł��B", (NUM)
#define MES_CREATURE_LIFERATE_CHANGED "�̗̓����N���ς�����B"
#else
#define MES_CREATURE_LIFE_COND1 "unhurt"
#define MES_CREATURE_LIFE_COND2 "somewhat wounded"
#define MES_CREATURE_LIFE_COND3 "wounded"
#define MES_CREATURE_LIFE_COND4 "badly wounded"
#define MES_CREATURE_LIFE_COND5 "almost dead"
#define MES_CREATURE_NO_LIFE_COND1 "undamaged"
#define MES_CREATURE_NO_LIFE_COND2 "somewhat damaged"
#define MES_CREATURE_NO_LIFE_COND3 "damaged"
#define MES_CREATURE_NO_LIFE_COND4 "badly damaged"
#define MES_CREATURE_NO_LIFE_COND5 "almost destroyed"
#define MES_CREATURE_LIFERATE_NOW(NUM) "Your life rate is %d/100 now.", (NUM)
#define MES_CREATURE_LIFERATE_CHANGED "Life rate is changed."
#endif

#ifdef JP
#define MES_SCORE_PUSHKEY "[ ESC�Œ��f, ���̑��̃L�[�ő����܂� ]"
#define MES_SCORE_ASK_SEND_SCORE "�X�R�A���X�R�A�E�T�[�o�ɓo�^���܂���? "
#define MES_SCORE_SENDING "���M���D�D"
#define MES_SCORE_SEND "�����B�����L�[�������Ă��������B"
#define MES_SCORE_DISABLE_WIZARD "�E�B�U�[�h�E���[�h�ł̓X�R�A���L�^����܂���B"
#define MES_SCORE_DISABLE_UNIQUE "���j�[�N�E���[�h�ł̓X�R�A���L�^����܂���B"
#define MES_SCORE_DISABLE_CHEAT "���\��������l�̓X�R�A���L�^����܂���B"
#define MES_SCORE_DISABLE_INTERUPT "�����I���̂��߃X�R�A���L�^����܂���B"
#define MES_SCORE_DISABLE_QUIT "�r���I���̂��߃X�R�A���L�^����܂���B"
#define MES_SCORE_SAVE_DEATH "���񂾃f�[�^���Z�[�u���܂����H "
#define MES_SCORE_STAND_BY "��ŃX�R�A��o�^���邽�߂ɑҋ@���܂����H"
#define MES_SCORE_REGISTER_NOW "�ҋ@���Ă����X�R�A�o�^�����s�Ȃ��܂����H"
#define MES_SCORE_REGISTER_CEASE "�X�R�A�o�^����߂܂����H"
#define MES_SCORE_STAND_BY_NEXT "���������ҋ@���܂��B"
#else
#define MES_SCORE_PUSHKEY "[Press ESC to quit, any other key to continue.]"
#define MES_SCORE_ASK_SEND_SCORE "Do you send score to the world score sever? "
#define MES_SCORE_SENDING "Sending..."
#define MES_SCORE_SEND "Completed.  Hit any key."
#define MES_SCORE_DISABLE_WIZARD "Score not registered for wizards."
#define MES_SCORE_DISABLE_UNIQUE "Score not registered for uniques."
#define MES_SCORE_DISABLE_CHEAT "Score not registered for cheaters."
#define MES_SCORE_DISABLE_INTERUPT "Score not registered due to interruption."
#define MES_SCORE_DISABLE_QUIT "Score not registered due to quitting."
#define MES_SCORE_SAVE_DEATH "Save death? "
#define MES_SCORE_STAND_BY "Stand by for later score registration? "
#define MES_SCORE_REGISTER_NOW "Do you register score now? "
#define MES_SCORE_REGISTER_CEASE "Do you give up score registration? "
#define MES_SCORE_STAND_BY_NEXT "standing by for future registration..."
#endif

#ifdef JP
#define MES_WINNER_WON1 "*** ���߂łƂ� ***"
#define MES_WINNER_WON2 "���Ȃ��̓L�����y�[�����R���v���[�g���܂����B"
#define MES_WINNER_WON3 "���ł�����(���E�R�}���h)���邱�Ƃ��ł��܂��B"
#else
#define MES_WINNER_WON1 "*** CONGRATULATIONS ***"
#define MES_WINNER_WON2 "You have won the game!"
#define MES_WINNER_WON3 "You may retire (commit suicide) when you are ready."
#endif

#ifdef JP
#define MES_CHEAT_DEATH "�E�B�U�[�h���[�h�ɔO�𑗂�A�����\�����B"
#else
#define MES_CHEAT_DEATH "You invoke wizard mode and cheat death."
#endif

#ifdef JP
#define MES_QUICKSTART_DESC "�N�C�b�N�E�X�^�[�g���g���ƈȑO�ƑS�������L�����N�^�[�Ŏn�߂��܂��B"
#define MES_QUICKSTART_ASK "�N�C�b�N�E�X�^�[�g���g���܂����H[y/N]"
#else
#define MES_QUICKSTART_DESC "Do you want to use the quick start function(same character as your last one)."
#define MES_QUICKSTART_ASK "Use quick start? [y/N]" 
#endif

#ifdef JP
#define MES_TELEPORT_PREVENTED "�s�v�c�ȗ͂��e���|�[�g��h�����I"
#define MES_TELEPORT_NO_DUNGEON "�I�ׂ�_���W�������Ȃ��B"
#define MES_TELEPORT_WHICH(NOTE) "�ǂ̃_���W����%s���܂���:", (NOTE)
#else
#define MES_TELEPORT_PREVENTED "A mysterious force prevents you from teleporting!"
#define MES_TELEPORT_NO_DUNGEON "No dungeon is available."
#define MES_TELEPORT_WHICH(NOTE) "Which dungeon do you %s?: ", (NOTE)
#endif

#ifdef JP
#define MES_CHEST_STAT_EMPTY "(��)"
#define MES_CHEST_STAT_DISARMED "(������)"
#define MES_CHEST_STAT_UNLOCKED "(��{��)"
#define MES_CHEST_STAT_LOCKED "(�{��)"
#define MES_CHEST_STAT_POISON_NEEDLE "(�Őj)"
#define MES_CHEST_STAT_GAS_TRAP "(�K�X�E�g���b�v)"
#define MES_CHEST_STAT_BOMB "(�������u)"
#define MES_CHEST_STAT_SUMMON "(�����̃��[��)"
#define MES_CHEST_STAT_EVIL "(�׈��ȃ��[��)"
#define MES_CHEST_STAT_ALARM "(�x�񑕒u)"
#define MES_CHEST_STAT_MULTI "(�}���`�E�g���b�v)"
#else
#define MES_CHEST_STAT_EMPTY "(empty)"
#define MES_CHEST_STAT_DISARMED "(disarmed)"
#define MES_CHEST_STAT_UNLOCKED "(unlocked)"
#define MES_CHEST_STAT_LOCKED "(locked)"
#define MES_CHEST_STAT_POISON_NEEDLE "(poison needle)"
#define MES_CHEST_STAT_GAS_TRAP "(Gas Trap)"
#define MES_CHEST_STAT_BOMB "(Explosion Device)"
#define MES_CHEST_STAT_SUMMON "(Summoning Runes)"
#define MES_CHEST_STAT_EVIL "(Gleaming Black Runes)"
#define MES_CHEST_STAT_ALARM "(Alarm)"
#define MES_CHEST_STAT_MULTI "(Multiple Traps)"
#endif

#ifdef JP
#define MES_WARN_COL_BLACK "����"
#define MES_WARN_COL_PURPLE "���F��"
#define MES_WARN_COL_DEEPRED "���F��"
#define MES_WARN_COL_RED "�ԐF��"
#define MES_WARN_COL_PINK "�s���N�F��"
#define MES_WARN_COL_WHITE "����"
#define MES_WARN_GLOW1(OBJECT, COLOR) "%s��%s�������B", (OBJECT)->name, (COLOR)
#define MES_WARN_GLOW2(COLOR) "%s���镨�����ɕ����񂾁B", (COLOR)
#else
#define MES_WARN_COL_BLACK "black"
#define MES_WARN_COL_PURPLE "purple"
#define MES_WARN_COL_DEEPRED "deep red"
#define MES_WARN_COL_RED "red"
#define MES_WARN_COL_PINK "pink"
#define MES_WARN_COL_WHITE "white"
#define MES_WARN_GLOW1(OBJECT, COLOR) "%s glows %s.", (OBJECT)->name, (COLOR)
#define MES_WARN_GLOW2(COLOR) "An %s image forms in your mind.", (COLOR)
#endif

