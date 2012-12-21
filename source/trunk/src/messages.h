
// Keywords

#if JP
static const cptr game_messages[] =
{
	"�N�G�X�g��B�������B",
	"���ǂɎ��s�����B"
	"���������͂𒵂˕Ԃ����B",
	"�ɂ͊��S�ȑϐ�������B",
	"�ɂ͑ϐ�������B",
	"�ɂ͌��ʂ��Ȃ������B",
	"�͖ڂ������Ȃ�",
	"�͍������Ă���B",
	"�N���[�`���[�������ӂ������Ă���B",
	"�����n�b�L���Ƃ����B",
	"�����͂������B",
	"�����͂����Ȃ������B",
	"���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I",
	"�N���Ɍ�����Ă���悤�ȋC������B",
	"�u�Ȃ񂾂��̊K�i�́I�H�v",
	"�_�[�c���",
	"�댯�Ȕ閧",
	"�΂𐧌�ł��Ȃ��B",
};
#else
static const cptr game_messages[] =
{
	"You completed the quest.",
	"The improvement failed.",
	"You resist the effects.",
	"is immune.",
	"resists.",
	"You are unaffected.",
	"is confused",
	"is blind!",
	"There is a creature in the way.",
	"You feel your head clear.",
	"You have picked the lock.",
	"You failed to pick the lock.",
	"A terrible black aura blasts your %s!",
	"You feel as if someone is watching over you.",
	"'What's a fucking stair!?'",
	"a dart trap",
	"perilous secrets",
	"You are too weak to control the stone!",
};
#endif

enum GAME_MESSAGE
{
	GAME_MESSAGE_COMPLETE_QUEST,
	GAME_MESSAGE_IMPROVEMENT_FAILED,
	GAME_MESSAGE_RESIST_THE_EFFECT,
	GAME_MESSAGE_IS_IMMUNE,
	GAME_MESSAGE_RESISTED,
	GAME_MESSAGE_IS_UNAFFECTED,
	GAME_MESSAGE_IS_CONFUSED,
	GAME_MESSAGE_IS_BLIND,
	GAME_MESSAGE_CREATURE_IN_THE_WAY,
	GAME_MESSAGE_MANA_RECOVERLY,
	GAME_MESSAGE_SUCCESS_PICKING,
	GAME_MESSAGE_FAILED_PICKING,
	GAME_MESSAGE_BLACK_AURA_TO_OBJECT,
	GAME_MESSAGE_REMOVED_OBJECT_CURSE,
	GAME_MESSAGE_COMBAT_TALK_STAIR,
	GAME_MESSAGE_DART_TRAP,
	GAME_MESSAGE_FAILED_PERILOUS_IDENTIFY,
	GAME_MESSAGE_PERILOUS_SECRET,
};

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

static const cptr message_files[]=
{
#ifdef JP
	"news_j.txt",
	"a_cursed_j.txt",
	"a_low_j.txt",
	"a_med_j.txt",
	"a_high_j.txt",

	"w_cursed_j.txt",
	"w_low_j.txt",
	"w_med_j.txt",
	"w_high_j.txt",
	"chainswd_j.txt",

	"error_j.txt",
	"death_j.txt",
	"seppuku_j.txt",
	"monfear_j.txt",
	"monpet_j.txt",

	"monfrien_j.txt",
	"monspeak_j.txt",
	"rumors_j.txt",
	"news_j.txt",
#else
	"news.txt",
	"a_cursed.txt",
	"a_low.txt",
	"a_med.txt",
	"a_high.txt",

	"w_cursed.txt",
	"w_low.txt",
	"w_med.txt",
	"w_high.txt",
	"chainswd.txt",

	"error.txt",
	"death.txt",
	"seppuku.txt",
	"monfear.txt",
	"monpet.txt",

	"monfrien.txt",
	"monspeak.txt",
	"rumors.txt",
	"news.txt",
#endif
};

enum MESSAGE_FILES
{
	MESSAGE_FILES_GAME_NEWS,
	MESSAGE_FILES_ARMOUR_CURSED_ARTIFACT,
	MESSAGE_FILES_ARMOUR_LOW_ARTIFACT,
	MESSAGE_FILES_ARMOUR_MED_ARTIFACT,
	MESSAGE_FILES_ARMOUR_HIGH_ARTIFACT,
	MESSAGE_FILES_WEAPON_CURSED_ARTIFACT,

	MESSAGE_FILES_WEAPON_LOW_ARTIFACT,
	MESSAGE_FILES_WEAPON_MED_ARTIFACT,
	MESSAGE_FILES_WEAPON_HIGH_ARTIFACT,
	MESSAGE_FILES_CHAINSWORD,
	MESSAGE_FILES_ERROR,

	MESSAGE_FILES_DEATH,
	MESSAGE_FILES_SEPPUKU,
	MESSAGE_FILES_CREATURE_FEAR,
	MESSAGE_FILES_CREATURE_PET,
	MESSAGE_FILES_CREATURE_FRIENDLY,

	MESSAGE_FILES_CREATURE_SPEAK,
	MESSAGE_FILES_RUMOR,
	MESSAGE_FILES_NEWS,
};

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
