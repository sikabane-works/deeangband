
// Keywords

#if JP
static const cptr game_messages[] =
{
	"���������͂𒵂˕Ԃ����B",
	"���������ʂ��Ȃ������B",
};
#else
static const cptr game_messages[] =
{
	"You resist the effects",
	"You are unaffected.",
};
#endif

enum GAME_MESSAGE
{
	GAME_MESSAGE_RESIST_THE_EFFECT,
	GAME_MESSAGE_IS_UNAFFECTED,
};
