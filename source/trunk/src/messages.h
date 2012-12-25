
// Keywords
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
#define GAME_MESSAGE_BLACK_AURA_TO_OBJECT "���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I"
#define GAME_MESSAGE_REMOVED_OBJECT_CURSE "�N���Ɍ�����Ă���悤�ȋC������B"
#define GAME_MESSAGE_COMBAT_TALK_STAIR "�u�Ȃ񂾂��̊K�i�́I�H�v"
#define GAME_MESSAGE_PERILOUS_SECRET "�΂𐧌�ł��Ȃ��B"
#define GAME_MESSAGE_RESISTED_FEAR "���������|�ɐN����Ȃ������B"
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
#define GAME_MESSAGE_BLACK_AURA_TO_OBJECT "A terrible black aura blasts your %s!"
#define GAME_MESSAGE_REMOVED_OBJECT_CURSE "You feel as if someone is watching over you."
#define GAME_MESSAGE_COMBAT_TALK_STAIR "'What's a fucking stair!?'"
#define GAME_MESSAGE_PERILOUS_SECRET "You are too weak to control the stone!"
#define GAME_MESSAGE_RESISTED_FEAR "You stand your ground!"
#endif

// Cause of Death
#if JP
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
#else
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
