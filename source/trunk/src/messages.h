
// Keywords

#if JP
static const cptr game_messages[] =
{
	"���������͂𒵂˕Ԃ����B",
	"���������ʂ��Ȃ������B",
	"�N���[�`���[�������ӂ������Ă���B",
	"�����͂����Ȃ������B",
	"�u�Ȃ񂾂��̊K�i�́I�H�v",
};
#else
static const cptr game_messages[] =
{
	"You resist the effects.",
	"You are unaffected.",
	"There is a creature in the way.",
	"You failed to pick the lock.",
	"What's a fucking stair!?",
};
#endif

enum GAME_MESSAGE
{
	GAME_MESSAGE_RESIST_THE_EFFECT,
	GAME_MESSAGE_IS_UNAFFECTED,
	GAME_MESSAGE_CREATURE_IN_THE_WAY,
	GAME_MESSAGE_FAILED_PICKING,
	GAME_MESSAGE_COMBAT_TALK_STAIR,
};
