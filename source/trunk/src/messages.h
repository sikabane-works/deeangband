
// Keywords

#if JP
static const cptr game_messages[] =
{
	"���������͂𒵂˕Ԃ����B",
	"���������ʂ��Ȃ������B",
	"�N���[�`���[�������ӂ������Ă���B",
};
#else
static const cptr game_messages[] =
{
	"You resist the effects.",
	"You are unaffected.",
	"There is a creature in the way.",
};
#endif

enum GAME_MESSAGE
{
	GAME_MESSAGE_RESIST_THE_EFFECT,
	GAME_MESSAGE_IS_UNAFFECTED,
	GAME_MESSAGE_CREATURE_IN_THE_WAY,
};
