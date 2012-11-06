
// Keywords

#if JP
static const cptr game_messages[] =
{
	"�N�G�X�g��B�������B",
	"���ǂɎ��s�����B"
	"���������͂𒵂˕Ԃ����B",
	"�ɂ͊��S�ȑϐ�������B",
	"�ɂ͑ϐ�������B",
	"�����ʂ��Ȃ������B",
	"�͍������Ă���B",
	"�N���[�`���[�������ӂ������Ă���B",
	"�����͂������B",
	"�����͂����Ȃ������B",
	"���|�̈Í��I�[�������Ȃ���%s���ݍ��񂾁I",
	"�N���Ɍ�����Ă���悤�ȋC������B",
	"�u�Ȃ񂾂��̊K�i�́I�H�v",
	"�댯�Ȕ閧",
	"�΂𐧌�ł��Ȃ��I",
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
	"There is a creature in the way.",
	"You have picked the lock.",
	"You failed to pick the lock.",
	"A terrible black aura blasts your %s!",
	"You feel as if someone is watching over you.",
	"'What's a fucking stair!?'",
	"You are too weak to control the stone!",
	"perilous secrets",
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
	GAME_MESSAGE_CREATURE_IN_THE_WAY,
	GAME_MESSAGE_SUCCESS_PICKING,
	GAME_MESSAGE_FAILED_PICKING,
	GAME_MESSAGE_BLACK_AURA_TO_OBJECT,
	GAME_MESSAGE_REMOVED_OBJECT_CURSE,
	GAME_MESSAGE_COMBAT_TALK_STAIR,
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
#else
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
#endif
};

enum MESSAGE_FILES
{
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
};
